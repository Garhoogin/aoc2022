#include <stdio.h>
#include <stdlib.h>

//undefine for part 1
#define PART_2

int comparator(const void *a, const void *b) {
	return -(*(int *) a - *(int *) b);
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
	
	//scan the file
	int nCalories = 0;
	int nBuffer = 0;
	int nCharsScanned = 0;
	
	int *calorieCounts = (int *) calloc(0, sizeof(int));
	int nElves = 0;
	
	for (int i = 0; i < size; i++) {
		char c = bf[i];
		
		//if digit, append
		if (c >= '0' && c <= '9') {
			nCharsScanned++;
			nBuffer = nBuffer * 10 + (c - '0');
			continue;
		}
		
		//if newline, add calories to total
		if (c == '\n' && nCharsScanned) {
			nCalories += nBuffer;
			nBuffer = 0;
			nCharsScanned = 0;
			continue;
		}
		
		//if empty line, append count to list
		if (c == '\n' && nCharsScanned == 0) {
			nElves++;
			calorieCounts = (int *) realloc(calorieCounts, nElves * sizeof(int));
			calorieCounts[nElves - 1] = nCalories;
			nBuffer = 0;
			nCharsScanned = 0;
			nCalories = 0;
			continue;
		}
	}
	
	//if residual, top it off
	if (nCalories) {
		nElves++;
		calorieCounts = (int *) realloc(calorieCounts, nElves * sizeof(int));
		calorieCounts[nElves - 1] = nCalories;
	}
	
	//find index of the most load-bearing elf
	int nMost = 0, bestIndex = 0;
	for (int i = 0; i < nElves; i++) {
		if (calorieCounts[i] > nMost) {
			nMost = calorieCounts[i];
			bestIndex = i;
		}
	}
	
#ifndef PART_2
	//return
	printf("%d by %d\n", nMost, bestIndex + 1);
#else
	//sort
	qsort(calorieCounts, nElves, sizeof(int), comparator);
	
	int sum = calorieCounts[0] + calorieCounts[1] + calorieCounts[2];
	printf("Sum: %d\n", sum);
#endif
	
	return 0;
}