#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <conio.h> // _getch 함수 포함
#include <windows.h> // Windows API 포함

enum Direction { UP, DOWN, LEFT, RIGHT, SUBMIT };
enum {
	black,
	blue,
	green,
	cyan,
	red,
	purple,
	brown,
	lightgray,
	darkgray,
	lightblue,
	lightgreen,
	lightcyan,
	lightred,
	lightpurple,
	yellow,
	white
};

int keyControl();
void gotoxy(int x, int y);
void titleDraw();
void infoDraw();
int menuDraw();
void init(); // init 함수 선언 추가
void drawMap();
void setColor(int, int);
void gLoop(int);
#endif
