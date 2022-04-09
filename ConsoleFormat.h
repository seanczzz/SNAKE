#pragma once
#include <Windows.h>
#include <conio.h>

// Chỉnh kích thước cửa sổ console
void SetWindowSize(SHORT width, SHORT height);

// Không cho phép người dùng thay đổi kích thước cửa sổ Console
void FixConsoleWindow();

// Đổi màu chữ và nền chữ
void SetColor(int code);

void GotoXY(int x, int y);

// Xóa màn hình
void ClearScreen();

void ShowCur(bool CursorVisibility);