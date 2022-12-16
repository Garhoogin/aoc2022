#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_2

int computeScenicScore(char *map, int width, int height, int x, int y) {
	int refheight = map[x + y * width] - '0';
	
	//trace up down left and right
	int leftDistance = 1, rightDistance = 1, upDistance = 1, downDistance = 1;
	
	//left
	for (int scanX = x - 1; scanX >= 0; scanX--) {
		int h = map[scanX + y * width] - '0';
		leftDistance = x - scanX;
		if (h >= refheight) {
			break;
		}
	}
	
	//up
	for (int scanY = y - 1; scanY >= 0; scanY--) {
		int h = map[x + scanY * width] - '0';
		upDistance = y - scanY;
		if (h >= refheight) {
			break;
		}
	}
	
	//right
	for (int scanX = x + 1; scanX < width; scanX++) {
		int h = map[scanX + y * width] - '0';
		rightDistance = scanX - x;
		if (h >= refheight) {
			break;
		}
	}
	
	//down
	for (int scanY = y + 1; scanY < height; scanY++) {
		int h = map[x + scanY * width] - '0';
		downDistance = scanY - y;
		if (h >= refheight) {
			break;
		}
	}
	
	return leftDistance * rightDistance * upDistance * downDistance;
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
	
	//count rows and columns
	int nCols = 0, nRows = 0;
	for (int i = 0; i < size; i++) {
		char c = bf[i];
		if (c == '\n') {
			if (nCols == 0) nCols = i;
			nRows++;
			continue;
		}
	}
	
	//create map
	int width = nCols, height = nRows;
	char *map = (char *) calloc(width * height, 1);
	
	for (int y = 0; y < height; y++) {
		memcpy(map + y * width, bf + y * (width + 1), width);
	}
	
	//count visible trees; 1 means counted
	char *counted = (char *) calloc(width * height, 1);
	
	//scan from top and bottom
	//following the Do Repeat Yourself principle
	for (int x = 0; x < width; x++) {
		int highest = -1;
		for (int y = 0; y < height; y++) {
			int a = map[x + y * width] - '0';
			if (a > highest) {
				highest = a;
				counted[x + y * width] = 1;
			}
		}
		
		highest = -1;
		for (int y = height - 1; y >= 0; y--) {
			int a = map[x + y * width] - '0';
			if (a > highest) {
				highest = a;
				counted[x + y * width] = 1;
			}
		}
	}
	
	//scan from left and right
	for (int y = 0; y < height; y++) {
		int highest = -1;
		for (int x = 0; x < width; x++) {
			int a = map[x + y * width] - '0';
			if (a > highest) {
				highest = a;
				counted[x + y * width] = 1;
			}
		}
	}
	for (int y = 0; y < height; y++) {
		int highest = -1;
		for (int x = width - 1; x >= 0; x--) {
			int a = map[x + y * width] - '0';
			if (a > highest) {
				highest = a;
				counted[x + y * width] = 1;
			}
		}
	}
	
#ifndef PART_2
	//count hits
	int nVisible = 0;
	for (int i = 0; i < width * height; i++) {
		if (counted[i]) nVisible++;
	}
	
	printf("Visible: %d\n", nVisible);
#else
	//compute scenic scores
	int bestScore = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int score = computeScenicScore(map, width, height, x, y);
			if (score > bestScore) {
				bestScore = score;
			}
		}
	}
	printf("Best score: %d\n", bestScore);
#endif
	
	
	
	return 0;
}