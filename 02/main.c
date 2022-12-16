#include <stdio.h>
#include <stdlib.h>

#define PART_2

#define MROCK 0
#define MPAPER 1
#define MSCISSORS 2

int main(int argc, char **argv) {
	//read file
	FILE *fp = fopen("input.txt", "rb");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *bf = calloc(size + 1, 1);
	fread(bf, size, 1, fp);
	fclose(fp);
	
	//parse char by char
	int opponentMove =0, myMove = 0;
	int score = 0;
	for (int i = 0; i < size; i++) {
		char c = bf[i];
		
		if (c >= 'A' && c <= 'C') {
			opponentMove = c - 'A';
			continue;
		}
		
		if (c >= 'X' && c <= 'Z') {
#ifndef PART_2
			myMove = c - 'X';
#else
			if (c == 'Y') myMove = opponentMove;
			else if (c == 'Z') {
				//win
				if (opponentMove == MPAPER) myMove = MSCISSORS;
				if (opponentMove == MROCK) myMove = MPAPER;
				if (opponentMove == MSCISSORS) myMove = MROCK;
			} else {
				if (opponentMove == MPAPER) myMove = MROCK;
				if (opponentMove == MROCK) myMove = MSCISSORS;
				if (opponentMove == MSCISSORS) myMove = MPAPER;
			}
#endif
			continue;
		}
		
		if (c == '\n') {
			//score up
			score += myMove + 1;
			if ((myMove == MROCK && opponentMove == MSCISSORS)
				|| (myMove == MSCISSORS && opponentMove == MPAPER)
				|| (myMove == MPAPER && opponentMove == MROCK)) {
				score += 6;
			} else if (myMove == opponentMove) {
				score += 3;
			}
		}
	}
	
	printf("Score: %d\n", score);
	
	return 0;
}