//Spoilers beware, don't expand
//if
//you
//don't
//want
//this
//spoiled


#include <stdio.h>
#include <stdlib.h>


//undefine for part 1
#define PART_2

typedef struct POINT_ {
	int x;
	int y;
} POINT;

typedef struct STRIP_ {
	int nPoints;
	POINT *points;
} STRIP;

typedef struct STRIP_LIST_ {
	int nStrips;
	STRIP *strips;
} STRIP_LIST;

void stripStartLine(STRIP_LIST *list) {
	if (list->strips == NULL) {
		list->strips = (STRIP *) calloc(1, sizeof(STRIP));
		list->nStrips = 1;
		return;
	}
	list->nStrips++;
	list->strips = realloc(list->strips, list->nStrips * sizeof(STRIP)); //cry about it
	list->strips[list->nStrips - 1].nPoints = 0;
	list->strips[list->nStrips - 1].points = NULL;
}

void stripAddPoint(STRIP_LIST *list, int x, int y) {
	STRIP *strip = list->strips + list->nStrips - 1;
	if (strip->nPoints == 0) {
		strip->nPoints = 1;
		strip->points = (POINT *) malloc(sizeof(POINT));
		strip->points[0].x = x;
		strip->points[0].y = y;
		return;
	}
	strip->nPoints++;
	strip->points = (POINT *) realloc(strip->points, strip->nPoints * sizeof(POINT)); //seethe
	strip->points[strip->nPoints - 1].x = x;
	strip->points[strip->nPoints - 1].y = y;
}

void stripFindMin(STRIP_LIST *list, int *x, int *y) {
	int minX = 0x7FFF, minY = 0x7FFF;
	for (int i = 0; i < list->nStrips; i++) {
		STRIP *strip = list->strips + i;
		for (int j = 0; j < strip->nPoints; j++) {
			POINT *p = strip->points + j;
			if (p->x < minX) minX = p->x;
			if (p->y < minY) minY = p->y;
		}
	}
	*x = minX;
	*y = minY;
}

void stripFindMax(STRIP_LIST *list, int *x, int *y) {
	int maxX = -0x7FFF, maxY = -0x7FFF;
	for (int i = 0; i < list->nStrips; i++) {
		STRIP *strip = list->strips + i;
		for (int j = 0; j < strip->nPoints; j++) {
			POINT *p = strip->points + j;
			if (p->x > maxX) maxX = p->x;
			if (p->y > maxY) maxY = p->y;
		}
	}
	*x = maxX;
	*y = maxY;
}

void stripDraw(char *field, STRIP *strip, int width, int height, int minX, int minY) {
	int nLines = strip->nPoints - 1;
	for (int i = 0; i < nLines; i++) {
		POINT *p1 = strip->points + i;
		POINT *p2 = strip->points + i + 1;
		
		//horz or vert?
		if (p1->x == p2->x) {
			int lminY = p1->y < p2->y ? p1->y : p2->y;
			int lmaxY = p1->y < p2->y ? p2->y : p1->y;
			for (int y = lminY; y <= lmaxY; y++) {
				field[(p1->x - minX) + (y - minY) * width] = '#';
			}
		} else {
			int lminX = p1->x < p2->x ? p1->x : p2->x;
			int lmaxX = p1->x < p2->x ? p2->x : p1->x;
			for (int x = lminX; x <= lmaxX; x++) {
				field[(x - minX) + (p1->y - minY) * width] = '#';
			}
		}
	}
}

//returns 0 for out of bounds
#define GET(x,y) ((x)<0?0:((y)<0?0:((x)>=(width)?0:((y)>=(height)?0:field[(x)+(y)*(width)]))))

//return 1 for out of bounds, 0 for moved, 2 for stopped.
int moveOnce(char *field, int width, int height, int *x, int *y) {
	char down = GET(*x, *y + 1);
	char downLeft = GET(*x - 1, *y + 1);
	char downRight = GET(*x + 1, *y + 1);
	
	//check down, down left, then down right
	if (down == 0) {
		(*y)++;
		return 1;
	}
	if (down == '.') {
		(*y)++;
		return 0;
	}
	if (downLeft == 0) {
		(*x)--;
		(*y)++;
		return 1;
	}
	if (downLeft == '.') {
		(*x)--;
		(*y)++;
		return 0;
	}
	if (downRight == 0) {
		(*x)++;
		(*y)++;
		return 1;
	}
	if (downRight == '.') {
		(*x)++;
		(*y)++;
		return 0;
	}
	
	//no viable paths, stop.
	return 2;
}

//return 1 if it fell out
int dropSand(char *field, int width, int height, int startX, int startY) {
	//trace down
	int x = startX, y = startY;
	
	while (1) {
		int result = moveOnce(field, width, height, &x, &y);
		
		//have we stoppd? return if so, setting sand position
		if (result == 2) {
			if (y >= 0)
				field[x + y * width] = 'O';
			return 0;
		}
		
		//are we out of bounds? don't set sand
		if (result == 1) {
			return 1;
		}
		
		//else, keep going
	}
}

int main(int argc, char **argv) {
	//read input
	FILE *fp = fopen("input.txt", "rb");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *bf = (char *) calloc(size + 1, 1); //let's null terminate
	fread(bf, 1, size, fp);
	fclose(fp);
	
	//initialize strip list
	STRIP_LIST list = { 0 };
	
	//parse the file
	int lastCoord = 0;
	int coord = 0;
	stripStartLine(&list);
	for (int i = 0; i < size; i++) {
		char c = bf[i];
		
		//if is a digit, accumulate
		if (c >= '0' && c <= '9') {
			coord = coord * 10 + (c - '0');
			continue;
		}
		
		//if a space, ignore
		if (c == ' ') {
			continue;
		}
		
		//if comma, swap coordinate buffers
		if (c == ',') {
			lastCoord = coord;
			coord = 0;
			continue;
		}
		
		//if dash, check following character for right angle bracket
		if (c == '-' && (i + 1) < size && bf[i + 1] == '>') {
			i++;
			
			//we will follow with another coordinate
			stripAddPoint(&list, lastCoord, coord);
			coord = 0;
			lastCoord = 0;
			continue;
		}
		
		//check for CR, LF, or CRLF
		if (c == '\r' || c == '\n') {
			
			//start new strip
			stripAddPoint(&list, lastCoord, coord);
			stripStartLine(&list);
			coord = 0;
			lastCoord = 0;
			continue;
		}
		
		//unknown character?
		printf("Unknown char %c\n", c);
	}
	
	//construct output
	int minX, minY, maxX, maxY;
	stripFindMin(&list, &minX, &minY);
	stripFindMax(&list, &maxX, &maxY);
	printf("%d, %d\n", minX, minY);
	printf("%d, %d\n", maxX, maxY);
	
	//initialize sand (world space)
	int sandStartX = 500;
	int sandStartY = 0;
	
#ifdef PART_2
	//part 2: infinitely wide floor
	//we can pretend we have an infinite floor, it doesn't really need to be
	int floorY = maxY + 2;
	maxY = floorY;
	
	//subtract floor Y from the X, and add to the X of the sand drop
	int floorLeft = sandStartX - floorY;
	int floorRight = sandStartX + floorY;
	if (floorLeft < minX) minX = floorLeft;
	if (floorRight > maxX) maxX = floorRight;
	
	//add
	stripAddPoint(&list, floorLeft, floorY);
	stripAddPoint(&list, floorRight, floorY);
#endif
	
	//hack: force y min to 0
	minY = 0;
	
	//create "game" field
	int width = maxX - minX + 1;
	int height = maxY - minY + 1;
	char *field = (char *) malloc(width * height);
	memset(field, '.', width * height);
	
	//draw lines
	for (int i = 0; i < list.nStrips; i++) {
		stripDraw(field, list.strips + i, width, height, minX, minY);
	}
	
	//simulate a grain of sand
	printf("Dropping from %d, %d\n", sandStartX - minX, sandStartY - minY);
	
	int nDropped = 0;
	while (1) {
		int result = dropSand(field, width, height, sandStartX - minX, sandStartY - minY);
#ifndef PART_2
		//check if out of bounds for part 1
		if (result) {
			break; //yay
		}
#else
		//check if origin obstructed for part 2
		char origin = field[sandStartX - minX + (sandStartY - minY) * width];
		if (origin == 'O') {
			nDropped++; //the last one we'll count
			break;
		}
#endif
		nDropped++; //don't cound the one that fell
	}
	
	printf("Dropped: %d\n", nDropped);
	
	//print out
#ifndef PART_2
	for (int row = 0; row < height; row++) {
		fwrite(field + row * width, width, 1, stdout);
		putchar('\n');
	}
#endif
	
	return 0;
}