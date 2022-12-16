#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_2

typedef struct POINT_ {
	int x;
	int y;
} POINT;

typedef struct SENSOR_ {
	POINT sensor;
	POINT beacon;
} SENSOR;

char *readInt(char *str, int *val) {
	int n = 0, neg = 0;
	if (*str == '-') {
		neg = 1;
		str++;
	}
	while (1) {
		char c = *str;
		if (c >= '0' && c <= '9') {
			n = n * 10 + c - '0';
		} else break;
		str++;
	}
	if (neg) n = -n;
	*val = n;
	return str;
}

//expects: "x=nnn, y=nnn"
char *readCoordinates(char *str, int *x, int *y) {
	str += 2;
	str = readInt(str, x);
	str += 4;
	str = readInt(str, y);
	return str;
}

int distance(POINT *p1, POINT *p2) {
	int dx = p1->x - p2->x;
	if (dx < 0) dx = -dx;
	int dy = p1->y - p2->y;
	if (dy < 0) dy = -dy;
	return dx + dy;
}

int isPointWithinSensors(SENSOR *sensors, int nSensors, int x, int y) {
	POINT p = { x, y};
	
	for (int i = 0; i < nSensors; i++) {
		SENSOR *s = sensors + i;
		int d = distance(&s->sensor, &p);
		int dMax = distance(&s->sensor, &s->beacon);
		if (d <= dMax) return 1;
	}
	
	return 0;
}

int findSensor(SENSOR *sensors, int nSensors, int x, int y, int *pmx) {
	POINT pt = { x, y };
	
	//find rightmost-coordinate-having sensor that contains this
	int sensor = -1, rightmost = -1;
	for (int i = 0; i < nSensors; i++) {
		SENSOR *s = sensors + i;
		int md = distance(&s->sensor, &s->beacon);
		int dd = distance(&s->sensor, &pt);
		if (dd <= md) {
			//|x - sensor.x| + |y - sensor.y| = md
			//|x - sensor.x| = md - |y - sensor.y|
			int dy = y - s->sensor.y;
			if (dy < 0) dy = -dy;
			int width = md - dy;
			int mx = s->sensor.x + width;
			if (mx > rightmost) {
				rightmost = mx;
				sensor = i;
			}
		}
	}
	*pmx = rightmost;
	return sensor;
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
	
	int nSensors = 0;
	SENSOR *sensors = (SENSOR *) calloc(0, sizeof(SENSOR));
	
	//parse file
	char *ptr = bf;
	while (ptr < bf + size) {
		int sensorX, sensorY, beaconX, beaconY;
		ptr += 10; //Sensor at 
		ptr = readCoordinates(ptr, &sensorX, &sensorY);
		ptr += 23;
		ptr = readCoordinates(ptr, &beaconX, &beaconY);
		
		nSensors++;
		sensors = (SENSOR *) realloc(sensors, nSensors * sizeof(SENSOR));
		sensors[nSensors - 1].sensor.x = sensorX;
		sensors[nSensors - 1].sensor.y = sensorY;
		sensors[nSensors - 1].beacon.x = beaconX;
		sensors[nSensors - 1].beacon.y = beaconY;
		
		while (*ptr == '\n') ptr++;
	}
	
	//find min coordinate x and max coordinate x
	int minX = 0, maxX = 0, maxY = 0;
	for (int i = 0; i < nSensors; i++) {
		SENSOR *sensor = sensors + i;
		int d = distance(&sensor->sensor, &sensor->beacon);
		int tmx = sensor->sensor.x - d;
		int tMx = sensor->sensor.x + d;
		int tMy = sensor->sensor.y + d;
		if (tmx < minX) minX = tmx;
		if (tMx > maxX) maxX = tMx;
		if (tMy > maxY) maxY = tMy;
	}
	
	//I'm paranoid about off by 1 errors >~<
	minX--;
	maxX++;
	
#ifndef PART_2
	
	//scan
	int y = 2000000;
	int nHits = 0;
	for (int x = minX; x <= maxX; x++) {
		nHits += isPointWithinSensors(sensors, nSensors, x, y);
	}
	
	printf("hits: %d\n", nHits - 1); //?
#else
	
	//part 2 bitches
	if (maxX > 4000000) maxX = 4000000;
	if (maxY > 4000000) maxY = 4000000;
	printf("max: %d, %d\n", maxX, maxY);
	
	for (int y = 0; y < maxY; y++) {
		for (int x = 0; x < maxX; x++) {
			
			//are we in a sensor range? Find the one with the farthest rightmost coordinate for this row
			int rightmost = 0;
			int foundSensor = findSensor(sensors, nSensors, x, y, &rightmost);
			if (foundSensor > -1) {
				x = rightmost;
			} else {
				unsigned long long sig = ((unsigned long long) x) * 4000000ull + ((unsigned long long) y);
				puts("found");
				printf("%d, %d\n", x, y);
				printf("sig: %llu %llu\n", sig, 0);
				return 0;
			}
			
		}
	}

#endif
	
	return 0;
}