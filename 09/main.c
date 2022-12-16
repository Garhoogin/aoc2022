#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_TAILS 9

typedef struct POINT_ {
	int x;
	int y;
} POINT;

int mustMove(POINT *head, POINT *tail) {
	int dx = tail->x - head->x;
	int dy = tail->y - head->y;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	
	return dx > 1 || dy > 1;
}

int doMove(POINT *head, POINT *tail) {
	if (head->x == tail->x) {
		if (tail->y < head->y) {
			tail->y++;
		} else {
			tail->y--;
		}
		return 0;
	} else if (head->y == tail->y) {
		if (tail->x < head->x) {
			tail->x++;
		} else {
			tail->x--;
		}
		return 0;
	}
	
	//move diagonal
	if (tail->y < head->y) tail->y++;
	else if (tail->y > head->y) tail->y--;
	if (tail->x < head->x) tail->x++;
	else if (tail->x > head->x) tail->x--;
	
	return 0;
}

int hasPoint(POINT *visited, int nVisited, int x, int y) {
	for (int i = 0; i < nVisited; i++) {
		if (visited[i].x == x && visited[i].y == y) return 1;
	}
	return 0;
}

void addPoint(POINT **visited, int *nVisited, POINT *tail) {
	int x = tail->x;
	int y = tail->y;
	
	//check that the point doesn't exist
	for (int i = 0; i < *nVisited; i++) {
		POINT *p = (*visited) + i;
		if (p->x == x && p->y == y) return;
	}
	
	//doesn't exist
	(*nVisited)++;
	*visited = (POINT *) realloc(*visited, *nVisited * sizeof(POINT));
	(*visited)[*nVisited - 1].x = x;
	(*visited)[*nVisited - 1].y = y;
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
	
	int nMoves = 0, nVisited = 0;
	char *moves = (char *) calloc(0, 1);
	POINT *visited = (POINT *) calloc(0, sizeof(POINT));
	
	POINT head = { 0 }, tails[N_TAILS] = { 0 };
	
	int amt = 0;
	char dir = '\0';
	for (int i = 0; i < size; i++) {
		char c = bf[i];
		
		//if direction code, do that
		if (c == 'U' || c == 'D' || c == 'L' || c == 'R') {
			amt = 0;
			dir = c;
			i++;
			continue;
		}
		
		//if digit, add it
		if (c >= '0' && c <= '9') {
			amt = amt * 10 + (c - '0');
			continue;
		}
		
		//if newline, push move
		if (c == '\n') {
			//add amt x dir
			nMoves += amt;
			moves = (char *) realloc(moves, nMoves);
			memset(moves + nMoves - amt, dir, amt);
		}
	}
	
	//print
	//fwrite(moves, nMoves, 1, stdout);
	//putchar('\n');
	
	addPoint(&visited, &nVisited, tails + N_TAILS - 1);
	
	for (int i = 0; i < nMoves; i++) {
		char move = moves[i];
		switch (move) {
			case 'U':
				head.y--;
				break;
			case 'D':
				head.y++;
				break;
			case 'L':
				head.x--;
				break;
			case 'R':
				head.x++;
				break;
		}
		
		//move tail 0
		if (mustMove(&head, tails + 0)) {
			doMove(&head, tails + 0);
		}
		
		//move other tails
		for (int i = 0; i < N_TAILS - 1; i++) {
			POINT *dest = tails + i;
			POINT *src = tails + i + 1;
			
			//move
			if (mustMove(dest, src)) {
				doMove(dest, src);
			}
		}
		
		//log 
		addPoint(&visited, &nVisited, tails + N_TAILS - 1);
		
	}
	
	//print board
	/*for (int y = -20; y < 20; y++) {
		for (int x = -20; x < 20; x++) {
			char c = '.';
			if (x == head.x && y == head.y) c = 'H';
			
			else {
				for (int i = 0; i < N_TAILS; i++) {
					if (x == tails[i].x && y == tails[i].y) {
						c = i + '1';
						break;
					}
				}
			}
			//if (hasPoint(visited, nVisited, x, y)) c = '#';
			
			if (c == '.' && x == 0 && y == 0) c = 's';
			
			putchar(c);
		}
		putchar('\n');
	}*/
	
	printf("Head: %d, %d\n", head.x, head.y);
	printf("Visited: %d\n", nVisited);
	
	return 0;
}