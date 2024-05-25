#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <conio.h> // _getch �Լ� ����
#include <windows.h> // Windows API ����

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
void init(); // init �Լ� ���� �߰�
void drawMap();
void setColor(int, int);
void gLoop(int);
#endif
