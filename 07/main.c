//contained within
//is
//the ugliest parser
//and most unsafe code
//you might see today

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_2

typedef struct CFILE_ {
	int size;
	char name[32];
} CFILE;

typedef struct CDIR_ {
	CFILE *files;
	int nFiles;
	struct CDIR_ *dirs;
	int nDirs;
	char name[32];
} CDIR;

CDIR *fsFindDir(CDIR *dir, char *name) {
	//scan through my listing
	char firstLevel[32] = { 0 };
	if (*name == '/') name++;
	int len = strlen(name);
	
	for (int i = 0; i < len; i++) {
		if (name[i] == '/') break;
		firstLevel[i] = name[i];
	}
	
	//scan for this directory
	CDIR *nextLevel = dir;
	for (int i = 0; i < dir->nDirs; i++) {
		CDIR *d = dir->dirs + i;
		if (strcmp(d->name, firstLevel) == 0) {
			nextLevel = d;
			break;
		}
	}
	
	name += strlen(firstLevel);
	if (*name == '/') name++;
	if (*name == '\0') return nextLevel;
	return fsFindDir(nextLevel, name);
}

void fsAddDir(CDIR *dir, char *rootName, char *dirname) {
	dir = fsFindDir(dir, rootName);
	
	if (dir->dirs == NULL) {
		dir->dirs = (CDIR *) calloc(1, sizeof(CDIR));
		dir->nDirs = 1;
		memcpy(dir->dirs[0].name, dirname, strlen(dirname) + 1);
		return;
	}
	
	dir->nDirs++;
	dir->dirs = (CDIR *) realloc(dir->dirs, dir->nDirs * sizeof(CDIR));
	
	CDIR *dest = dir->dirs + dir->nDirs - 1;
	memset(dest, 0, sizeof(CDIR));
	memcpy(dest->name, dirname, strlen(dirname) + 1);
}

void fsAddFile(CDIR *dir, char *rootName, char *fname, int size) {
	dir = fsFindDir(dir, rootName);
	
	if (dir->files == NULL) {
		dir->nFiles = 1;
		dir->files = (CFILE *) calloc(1, sizeof(CFILE));
		memcpy(dir->files[0].name, fname, strlen(fname) + 1);
		dir->files[0].size = size;
		return;
	}
	
	dir->nFiles++;
	dir->files = (CFILE *) realloc(dir->files, dir->nFiles * sizeof(CFILE));
	
	CFILE *dest = dir->files + dir->nFiles - 1;
	memcpy(dest->name, fname, strlen(fname) - 1);
	dest->size = size;
}

void fsEnumDirs(CDIR *fs, void (*callback) (CDIR *)) {
	for (int i = 0; i < fs->nDirs; i++) {
		callback(fs->dirs + i);
		fsEnumDirs(fs->dirs + i, callback);
	}
}

int fsMeasureSize(CDIR *dir) {
	int size = 0;
	for (int i = 0; i < dir->nFiles; i++) {
		size += dir->files[i].size;
	}
	for (int i = 0; i < dir->nDirs; i++) {
		size += fsMeasureSize(dir->dirs + i);
	}
	return size;
}

int g_size = 0;
int g_maxDirSize = 0;
int g_smallestDirFound = 0x7FFFFFFF;

void printDirSize(CDIR *dir) {
	//puts(dir->name);
	int size = fsMeasureSize(dir);
	if (size <= 100000) {
		g_size += size;
	}
}

void findSmallestSizeLessThan(CDIR *dir) {
	int size = fsMeasureSize(dir);
	if (size < g_maxDirSize) return;
	if (size < g_smallestDirFound) {
		g_smallestDirFound = size;
	}
}

int main(int argc, char **argv) {
	//read file
	FILE *fp = fopen("input.txt", "rb");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *bf = calloc(size + 1, 1);
	fread(bf, size, 1, fp);
	fclose(fp);
	
	CDIR fs = { 0 };
	
	char directory[256];
	for (int i = 0; i < size; i++) {
		char c = bf[i];
		
		//are we running a command?
		if (c == '$') {
			i++;
			i++; //skip space
			c = bf[i];
			
			//can run cd or ls
			if (c == 'c') {
				i += 3;
				
				//followed by the path to switch to
				c = bf[i];
				if (c == '/') {
					directory[0] = '/';
					directory[1] = '\0';
					i += 2; // / and newline
					continue;
				} else if (c == '.') {
					//up a directory
					
					int dirlen = strlen(directory);
					int lastSlash = -1;
					for (int j = 0; j < dirlen - 1; j++) {
						char dc = directory[j];
						if (dc == '/') {
							lastSlash = j;
						}
					}
					directory[lastSlash + 1] = '\0';
					
					i++; //.
					i++; //\n
					continue;
				} else {
					//append directory
					int dirlen = strlen(directory);
					while (1) {
						c = bf[i];
						i++;
						if (c == '\n') {
							directory[dirlen] = '/';
							directory[dirlen + 1] = '\0';
							break;
						}
						directory[dirlen] = c;
						directory[dirlen + 1] = '\0';
						dirlen++;
					}
					continue;
				}
			} else if (c == 'l') {
				i += 2; //ls newline
			}
			continue;
		}
		
		//else, we're a directory listing.
		//if we start with 'd', it's a directory
		//if we start with a digit, it's a file.
		if (c == 'd') {
			i++; //i
			i++; //r
			i++; // 
			i++;
			
			int dirlen = 0;
			char dirname[32] = { 0 };
			while (1) {
				c = bf[i];
				i++;
				if (c == '\0' || c == '\n') {
					break;
				}
				dirname[dirlen] = c;
				dirname[dirlen + 1] = '\0';
				dirlen++;
			}
			
			//we've got a directory name. 
			fsAddDir(&fs, directory, dirname);
			
			i--; //gets re-added
			continue;
		}
		
		//file
		if (c >= '0' && c <= '9') {
			int size = 0;
			while (1) {
				c = bf[i];
				i++;
				if (c >= '0' && c <= '9') {
					size = size * 10 + c - '0';
					continue;
				}
				if (c == ' ') break;
			}
			
			//get name
			int flen = 0;
			char fname[32] = { 0 };
			while (1) {
				c = bf[i];
				i++;
				if (c == '\0' || c == '\n') {
					break;
				}
				fname[flen] = c;
				fname[flen + 1] = '\0';
				flen++;
			}
			
			
			//we've got a file name. 
			fsAddFile(&fs, directory, fname, size);
			
			i--; //gets re-added
			continue;
		}
	}
	
#ifndef PART_2
	//enumerate directories
	fsEnumDirs(&fs, printDirSize);
	
	printf("\n\n%d\n", g_size);
#else
	
	//measure total FS size
	int totalSize = fsMeasureSize(&fs);
	int diskSize = 70000000;
	int freeSpace = diskSize - totalSize;
	int neededSpace = 30000000;
	printf("Total FS size: %d\n", totalSize);
	printf("Free space: %d\n", freeSpace);
	printf("Need to remove: %d\n", neededSpace - freeSpace);
	
	g_maxDirSize = neededSpace - freeSpace;
	fsEnumDirs(&fs, findSmallestSizeLessThan);

	printf("Smallest dir that yields the correct size: %d\n", g_smallestDirFound);

#endif
	
	return 0;
}