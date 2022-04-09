#include "ConsoleFormat.h"
#include <iostream>
#include <ctime>
#include <thread>
#include <string>
#include <fstream>
#include <Windows.h>
#include <conio.h>

#include <vector>

using namespace std;


#define VK_P 80 // Phím P
#define VK_L 76 // Phím L
#define VK_T 84 // Phím T
#define VK_A 65
#define VK_S 83
#define VK_D 68
#define VK_W 87


#define LEFT_WALL 5 // Khoảng cách của tường trái so với cạnh trái console
#define RIGHT_WALL 85 // Khoảng cách của tường phải so với cạnh trái console
#define TOP_WALL 2 // Khoảng cách của tường trên so với cạnh trên console
#define BOTTOM_WALL 27 // Khoảng cách của tường dưới so với cạnh trên console

#define INFO_FRAME_WIDTH 25 // Chiều rộng khung thông tin người chơi
#define INFO_FRAME_HEIGHT 12 // Chiều cao khung thông tin người chơi

#define BOARD_COLOR 112 // Màu của tường
#define FRAME_COLOR 94 // Màu của khung thông tin người chơi

struct Point
{
	int x;
	int y;
};
//
Point a;

Point FOOD[4];

struct HIGHLENGTH
{
	string name;
	int length;
};

HIGHLENGTH HighLength[5];
HIGHLENGTH NewLength;
//
Point oldTail;
int foodColor; // Màu thức ăn
int snakeColor; // Màu rắn
char MSSV[] = "2112042021120418211204212112043721120440";
Point snake[30];
int snakeLength = 6; // Chiều dài rắn
int oldLength = 6; // Lưu chiều dài của rắn ở ván trước

Point food;
int foodIndex; // Thức ăn in ra là kí tự nào trong chuỗi MSSV, foodIndex = 3 thì thức ăn in ra là MSSV[3]

Point gate[5];

int direction; // hướng di chuyển của rắn do người chơi nhấn
int curDirection; // hướng di chuyển hiện tại của rắn
int score = 0;
int speed = 1;
int curMap = 0; // ván chơi hiện tại
char* playerName = new char[255]; // tên người chơi
int key = VK_RIGHT; // lưu phím người dùng nhấn
int STATE; // trạng thái có đang chơi không
int foodPerLevel; // số thức ăn ở mỗi ván rắn cần ăn để qua ván mới
int eatenFood; // số lượng thức ăn đã ăn trong ván hiện tại
bool gateState; // cho biết đã mở cổng hay chưa
bool isPassLevel; // qua ván mới
/// 
int INDEX_ID;
int CHAR_LOCK;
int MOVING;
int SPEED;
int HEIGH_CONSOLE;
int WIDTH_CONSOLE;
int FOOD_INDEX;
int SIZE_SNAKE;
int SIZE_PLUS;
int ROUND;
bool GATE_EXIST;
char newgame[] = "NEW GAME";
char highlength[] = "HIGH LENGTH";
char exitgame[] = "EXIT GAME";
char yes[] = "Yes";
char no[] = "No";
char ok[] = "OK";
int temp;
int win;
/// 
typedef vector<Point> MAP;
vector<MAP> mapList; // danh sách các map

void FormatConsoleWindow();
void ResetData();
void InitializeSnake();
void DrawSnake(Point oldTail);
Point ControlSnake(const int& direction, int& curDirection);
void CatchKey(const int curDirection);
void DrawBoard();
bool IsGameOver(int curMap);
void InitializeFood(int curMap);
void DrawFood();
void CheckSnakeEatFood(int& score);
void InitializeMap();
void DrawMap(int mapIndex);
void map1(vector<Point>& map);
void map2(vector<Point>& map);
void map3(vector<Point>& map);
void DisplayPlayerInfo();

void InitializeGate();
void DrawGate();
bool CollideGate();

void StartGame();
void BlinkSnake();
void ThreadFunc();
void showStartMenu();
void PauseGame(HANDLE t);

void ExitGame(HANDLE& t);

void PrintLogo();

void TextColor(int color, char* OutputContent);
void CreateNewHighLength();
void SortHighLength();
void SaveData();
void LoadData();
int main()
{
	strcpy_s(playerName, 255, "Cuong");
	srand((unsigned int)time(NULL));
	FormatConsoleWindow();
	InitializeMap();

menu:
	showStartMenu();
	if (STATE == 1)
	{
		thread t1(ThreadFunc);
		HANDLE handle_t1 = t1.native_handle();

		while (STATE == 1)
		{
			CatchKey(curDirection);
			if (key == VK_LEFT || key == VK_RIGHT || key == VK_UP || key == VK_DOWN ||
				key == VK_A || key == VK_D || key == VK_W || key == VK_S)
			{
				direction = key;
				ResumeThread(handle_t1);
			}
			else if (key == VK_P)
			{
				PauseGame(handle_t1);
			}
			else if (key == VK_L)
			{
				PauseGame(handle_t1);
				SaveData();
				ExitGame(handle_t1);
				return 0;
			}
			else if (key == VK_T)
			{
				PauseGame(handle_t1);
				LoadData();
				ResumeThread(handle_t1);
			}
			else if (key == VK_ESCAPE)
			{
				ClearScreen();
				ExitGame(handle_t1);
				STATE = 0;
			}
		}

		t1.join();
		goto menu;
	}

	return 0;
}

void ThreadFunc()
{

	while (true)
	{
		Point oldPoint = ControlSnake(direction, curDirection);
		Sleep(150 / speed);
		DrawSnake(oldPoint);
		CheckSnakeEatFood(score);
		if (IsGameOver(curMap))
		{
			STATE = 0;
		}
		if (eatenFood == foodPerLevel)
		{
			InitializeGate();
			DrawGate();
			eatenFood = -1;
		}
		if (gateState == true)
		{
			GotoXY(food.x, food.y);
			cout << " ";
			food.x = -1;
			food.y = -1;
			if (CollideGate())
			{
				STATE = 0;
			}
		}
		if (isPassLevel)
		{
			curMap++;
			speed++;

			key = VK_RIGHT;
			StartGame();
			isPassLevel = false;
		}
		if (STATE == 0)
		{
			break;
		}
	}
}

void StartGame()
{
	ClearScreen();
	DrawBoard();
	ResetData();
	DisplayPlayerInfo();
	STATE = 1;
}

void FormatConsoleWindow()
{
	SetWindowSize(115, 30);
	FixConsoleWindow();
	ShowCur(0);
}

void ResetData()
{
	key = VK_RIGHT;
	if (STATE == 1)
	{
		snakeLength = oldLength;
	}
	else
	{
		snakeLength = 6;
		curMap = 0;
	}
	direction = VK_RIGHT, curDirection = VK_RIGHT;
	snakeColor = 3;
	foodPerLevel = 1 + rand() % (2 - 1 + 1);
	eatenFood = 0;
	InitializeSnake();
	foodIndex = snakeLength;
	InitializeFood(curMap);
	DrawMap(curMap);
	DrawFood();
	gateState = false;
}

void InitializeSnake()
{
	snake[0].x = LEFT_WALL + snakeLength;
	snake[0].y = TOP_WALL + 1;
	for (int i = 1; i < snakeLength; i++)
	{
		snake[i].x = snake[i - 1].x - 1;
		snake[i].y = snake[i - 1].y;
	}
}

void DrawSnake(Point oldTail)
{

	SetColor(snakeColor);
	for (int i = 0; i < snakeLength; i++)
	{
		GotoXY(snake[i].x, snake[i].y);
		cout << MSSV[i];
	}
	GotoXY(oldTail.x, oldTail.y);
	cout << " ";
	SetColor(15);

}

Point ControlSnake(const int& direction, int& curDirection)
{
	oldTail = snake[snakeLength - 1];
	for (int i = snakeLength - 1; i > 0; i--)
	{
		snake[i].x = snake[i - 1].x;
		snake[i].y = snake[i - 1].y;
	}
	switch (direction)
	{
	case VK_A:
		snake[0].x--;
		curDirection = VK_A;
		break;
	case VK_LEFT:
		snake[0].x--;
		curDirection = VK_LEFT;
		break;
	case VK_D:
		snake[0].x++;
		curDirection = VK_D;
		break;
	case VK_RIGHT:
		snake[0].x++;
		curDirection = VK_RIGHT;
		break;
	case VK_W:
		snake[0].y--;
		curDirection = VK_W;
		break;
	case VK_UP:
		snake[0].y--;
		curDirection = VK_UP;
		break;
	case VK_S:
		snake[0].y++;
		curDirection = VK_S;
		break;
	case VK_DOWN:
		snake[0].y++;
		curDirection = VK_DOWN;
		break;
	}
	return oldTail;
}

void CatchKey(const int curDirection)
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		if (curDirection != VK_RIGHT)
		{
			key = VK_LEFT;
		}
	}
	else if (GetAsyncKeyState(VK_A))
	{
		if (curDirection != VK_D)
		{
			key = VK_A;
		}
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		if (curDirection != VK_LEFT)
		{
			key = VK_RIGHT;
		}
	}
	else if (GetAsyncKeyState(VK_D))
	{
		if (curDirection != VK_A)
		{
			key = VK_D;
		}
	}
	else if (GetAsyncKeyState(VK_UP))
	{
		if (curDirection != VK_DOWN)
		{
			key = VK_UP;
		}
	}
	else if (GetAsyncKeyState(VK_W))
	{
		if (curDirection != VK_S)
		{
			key = VK_W;
		}
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		if (curDirection != VK_UP)
		{
			key = VK_DOWN;
		}
	}
	else if (GetAsyncKeyState(VK_S))
	{
		if (curDirection != VK_W)
		{
			key = VK_S;
		}
	}
	else if (GetAsyncKeyState(VK_P))
	{
		key = VK_P;
	}
	else if (GetAsyncKeyState(VK_L))
	{
		key = VK_L;
	}
	else if (GetAsyncKeyState(VK_T))
	{
		key = VK_T;
	}
	else if (GetAsyncKeyState(VK_ESCAPE))
	{
		key = VK_ESCAPE;
	}
}

void DrawBoard()
{
	SetColor(BOARD_COLOR);
	for (int i = LEFT_WALL; i < RIGHT_WALL; i++)
	{
		GotoXY(i, TOP_WALL);
		cout << " ";
		GotoXY(i, BOTTOM_WALL);
		cout << " ";
	}
	for (int i = TOP_WALL; i <= BOTTOM_WALL; i++)
	{
		GotoXY(LEFT_WALL, i);
		cout << " ";
		GotoXY(RIGHT_WALL, i);
		cout << " ";
		Sleep(1);
	}
	SetColor(15);
}

bool IsGameOver(int curMap)
{
	if (snake[0].x == LEFT_WALL || snake[0].x == RIGHT_WALL ||
		snake[0].y == TOP_WALL || snake[0].y == BOTTOM_WALL)
	{
		return true;
	}
	for (int i = 1; i < snakeLength; i++)
	{
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			return true;
		}
	}
	int mapPixels = (int)mapList[curMap].size();
	for (int i = 0; i < mapPixels; i++)
	{
		if (snake[0].x == mapList[curMap][i].x && snake[0].y == mapList[curMap][i].y)
		{
			return true;
		}
	}
	return false;
}

void InitializeFood(int curMap)
{
	int x, y;
	bool isSameCoordinates = false;
	int a = LEFT_WALL + 1;
	int b = RIGHT_WALL - 1;
	int c = TOP_WALL + 1;
	int d = BOTTOM_WALL - 1;
	do
	{
		// random từ a đến b: a + rand() % (b - a + 1)
		x = a + rand() % (b - a + 1);
		y = c + rand() % (d - c + 1);
		for (int j = 0; j < snakeLength; j++)
		{
			if (snake[j].x == x && snake[j].y == y)
			{
				isSameCoordinates = true;
				break;
			}
		}
		int mapPixels = (int)mapList[curMap].size();
		for (int j = 0; j < mapPixels; j++)
		{
			if (mapList[curMap][j].x == x && mapList[curMap][j].y == y)
			{
				isSameCoordinates = true;
				break;
			}
		}
	} while (isSameCoordinates);
	food.x = x;
	food.y = y;
	foodColor = 1 + rand() % 15;
}

void DrawFood()
{
	SetColor(foodColor);
	GotoXY(food.x, food.y);
	cout << MSSV[foodIndex];
	SetColor(15);
}

void CheckSnakeEatFood(int& score)
{
	if (snake[0].x == food.x && snake[0].y == food.y)
	{
		food.x = -1;
		food.y = -1;
		snakeLength++;
		snake[snakeLength - 1] = oldTail;
		score += 10;
		snakeColor = foodColor;
		foodIndex++;
		DisplayPlayerInfo();
		InitializeFood(curMap);
		DrawFood();
		eatenFood++;
	}
}

void InitializeMap()
{
	vector<Point> map;
	map1(map);
	mapList.push_back(map);
	map2(map);
	mapList.push_back(map);
	map3(map);
	mapList.push_back(map);
}

void map1(vector<Point>& map)
{
	map.resize(0);
}

void map2(vector<Point>& map)
{
	map.resize(0);
	int k = (RIGHT_WALL - LEFT_WALL) * 3 / 7;
	int l = (BOTTOM_WALL - TOP_WALL) * 2 / 3;
	for (int i = 1; i < k; i++)
	{
		Point p{ LEFT_WALL + i, l };
		map.push_back(p);
	}
	for (int i = 1; i < l * 2 / 3; i++)
	{
		Point p{ k, TOP_WALL + i };
		map.push_back(p);
	}
	int mid = (RIGHT_WALL - LEFT_WALL) * 2 / 3;
	int m = (BOTTOM_WALL - TOP_WALL) * 4 / 5;
	for (int i = 1; i < m; i++)
	{
		Point p{ mid, BOTTOM_WALL - i };
		map.push_back(p);
	}
}

void map3(vector<Point>& map)
{
	map.resize(0);
	int k = (RIGHT_WALL - LEFT_WALL) * 1 / 2;
	int l = (BOTTOM_WALL - TOP_WALL) * 1 / 3;
	for (int i = 1; i < k; i++)
	{
		Point p{ LEFT_WALL + i, l };
		map.push_back(p);
	}
	for (int i = RIGHT_WALL - 1; i > k; i--)
	{
		Point p{ i, 2 * l };
		map.push_back(p);
	}
}

void DrawMap(int mapIndex)
{
	int mapPixels = (int)mapList[curMap].size();
	SetColor(BOARD_COLOR);
	Sleep(15);
	for (int i = 0; i < mapPixels; i++)
	{
		GotoXY(mapList[mapIndex][i].x, mapList[mapIndex][i].y);
		cout << " ";
		Sleep(5);
	}
	SetColor(15);
}

void DisplayPlayerInfo()
{
	SetColor(FRAME_COLOR);
	GotoXY(RIGHT_WALL + 2, TOP_WALL + 1);
	for (int i = 0; i < INFO_FRAME_WIDTH; i++)
	{
		cout << " ";
	}
	GotoXY(RIGHT_WALL + 2, TOP_WALL + INFO_FRAME_HEIGHT + 1);
	for (int i = 0; i < INFO_FRAME_WIDTH; i++)
	{
		cout << " ";
	}

	for (int i = 0; i <= INFO_FRAME_HEIGHT; i++)
	{
		GotoXY(RIGHT_WALL + 2, TOP_WALL + 1 + i);
		cout << " ";
	}
	for (int i = 0; i <= INFO_FRAME_HEIGHT; i++)
	{
		GotoXY(RIGHT_WALL + 2 + INFO_FRAME_WIDTH, TOP_WALL + 1 + i);
		cout << " ";
	}
	SetColor(15);
	GotoXY(RIGHT_WALL + 4, TOP_WALL + 2);
	cout << "Player name: " << playerName;
	GotoXY(RIGHT_WALL + 4, TOP_WALL + 3);
	cout << "Score: " << score;
	GotoXY(RIGHT_WALL + 4, TOP_WALL + 4);
	cout << "Level: " << curMap + 1;
	GotoXY(RIGHT_WALL + 4, TOP_WALL + 5);
	cout << "Snake size: " << snakeLength;
}

void InitializeGate()
{
	srand((unsigned int)time(NULL));
	int x, y;
	bool isSameCoordinates = false;
	int a = LEFT_WALL + 2;
	int b = RIGHT_WALL - 2;
	int c = TOP_WALL + 1;
	int d = BOTTOM_WALL - 3;
	int mapPixels = (int)mapList[curMap].size();
	do
	{
		// random từ a đến b: a + rand() % (b - a + 1)
		x = a + rand() % (b - a + 1);
		y = c + rand() % (d - c + 1);
		for (int j = 0; j < snakeLength; j++)
		{
			if ((snake[j].x == x && snake[j].y == y) ||
				(snake[j].x == (x - 1) && snake[j].y == y) ||
				(snake[j].x == (x + 1) && snake[j].y == y) ||
				(snake[j].x == (x + 1) && snake[j].y == (y + 1)) ||
				(snake[j].x == (x - 1) && snake[j].y == (y + 1))
				)
			{
				isSameCoordinates = true;
				break;
			}
		}
		for (int j = 0; j < mapPixels; j++)
		{
			if ((mapList[curMap][j].x == x && mapList[curMap][j].y == y) ||
				(mapList[curMap][j].x == (x - 1) && mapList[curMap][j].y == y) ||
				(mapList[curMap][j].x == (x + 1) && mapList[curMap][j].y == y) ||
				(mapList[curMap][j].x == (x - 1) && mapList[curMap][j].y == (y + 1)) ||
				(mapList[curMap][j].x == (x + 1) && mapList[curMap][j].y == (y + 1)) ||
				(mapList[curMap][j].x == x && mapList[curMap][j].y == (y + 2))
				)
			{
				isSameCoordinates = true;
				break;                        // ***
			}                                 // * *
		}
	} while (isSameCoordinates);
	gate[2].x = x;
	gate[2].y = y;
	gate[1].x = x - 1;
	gate[1].y = y;
	gate[3].x = x + 1;
	gate[3].y = y;
	gate[4].x = x + 1;
	gate[4].y = y + 1;
	gate[0].x = x - 1;
	gate[0].y = y + 1;

	gateState = true;
}

void DrawGate()
{
	SetColor(182);
	for (int i = 0; i < 5; i++)
	{
		GotoXY(gate[i].x, gate[i].y);
		cout << " ";
	}
	SetColor(15);
}

bool CollideGate()
{
	if ((snake[0].x == gate[0].x && snake[0].y == gate[0].y) ||
		(snake[0].x == gate[1].x && snake[0].y == gate[1].y) ||
		(snake[0].x == gate[3].x && snake[0].y == gate[3].y) ||
		(snake[0].x == gate[4].x && snake[0].y == gate[4].y))
	{
		return true;
	}
	else if (snake[0].x == gate[2].x && snake[0].y == gate[2].y && curDirection != VK_UP)
	{
		return true;
	}
	else if (snake[0].x == gate[2].x && snake[0].y == gate[2].y && curDirection == VK_UP)
	{
		oldLength = snakeLength;
		while (snakeLength > 0)
		{
			DrawGate();
			for (int i = 0; i < snakeLength - 1; i++)
			{
				snake[i].x = snake[i + 1].x;
				snake[i].y = snake[i + 1].y;
			}
			snakeLength--;
			Point oldTail = ControlSnake(direction, curDirection);
			Sleep(80);
			DrawSnake(oldTail);
		}
		isPassLevel = true;
	}
	return false;
}

void BlinkSnake()
{
	SetColor(4);
	for (int j = 0; j < 10; j++)
	{
		Sleep(150);
		for (int i = 0; i < snakeLength; i++)
		{
			GotoXY(snake[i].x, snake[i].y);
			cout << MSSV[i];
		}
		Sleep(100);
		for (int i = 0; i < snakeLength; i++)
		{
			GotoXY(snake[i].x, snake[i].y);
			cout << " ";
		}
	}
	SetColor(15);
}

void PauseGame(HANDLE t)
{
	SuspendThread(t);
}

void ExitGame(HANDLE& t)
{
	ClearScreen();
	CloseHandle(t);
	terminate();
}

void PrintLogo()
{
	SetColor(10);
	GotoXY(0, 5);
	cout << "\t\t    sssss      nnnnn      nnn       aaa       Kkk     kkkkk   eeeeeeeeeeee       \n";
	cout << "\t\tssss     ssss  nnnnnn     nnn     aaa aaa     Kkk    kkkk     Eeeeeeeeeeee       \n";
	cout << "\t\t ssss          nnn  nnn   nnn    aaa   aaa    Kkk   kkkk      Eee                \n";
	cout << "\t\t  sssss        nnn   nnn  nnn   aaa     aaa   Kkkkkk          Eeeeeeeeeeee       \n";
	cout << "\t\t     sssss     nnn    nnn nnn  aaaaaaaaaaaaa  Kkk   kkkk      Eee                \n";
	cout << "\t\tssss    ssss   nnn     nnnnnn  aaa       aaa  Kkk     kkk     Eeeeeeeeeeee       \n";
	cout << "\t\t   sssss       nnn      nnnnn  aaa       aaa  Kkk      kkkkk  eeeeeeeeeeee       \n";
	cout << endl;
	cout << endl;
	cout << endl;
	SetColor(15);
}
void showStartMenu()
{
	ClearScreen();
	cout << endl;
	cout << endl;
	cout << endl;
	SetColor(10);
	cout << "\t\t\t\t\tWelcome"; Sleep(500);
	cout << " to"; Sleep(500);
	cout << " snake"; Sleep(500);
	cout << " game!" << endl; Sleep(200);
	SetColor(15);
	ClearScreen();

	int option = 1;
	while (true)
	{
		Sleep(92);
		if (GetAsyncKeyState(VK_UP))
		{
			option--;
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			option++;
		}
		if (GetAsyncKeyState(VK_RETURN))
		{
			switch (option)
			{
			case 1:
				StartGame();
				break;
			case 2:
				LoadData();
				break;
			case 3:
				break;
			}
			break;
		}

		if (option == 0)
		{
			option = 3;
		}
		if (option > 3)
		{
			option = 1;
		}
		if (option == 1)
		{
			PrintLogo();
			GotoXY(47, 15);
			cout << "OPTIONS:" << endl;
			SetColor(122);
			GotoXY(47, 17);
			cout << "1. Start" << endl;
			SetColor(15);
			GotoXY(47, 18);
			cout << "2. Load game" << endl;
			GotoXY(47, 19);
			cout << "3. Quit" << endl;
		}
		else if (option == 2)
		{
			PrintLogo();
			GotoXY(47, 15);
			cout << "OPTIONS:" << endl;
			GotoXY(47, 17);
			cout << "1. Start" << endl;
			SetColor(122);
			GotoXY(47, 18);
			cout << "2. Load game" << endl;
			SetColor(15);
			GotoXY(47, 19);
			cout << "3. Quit" << endl;
		}
		else if (option == 3)
		{
			PrintLogo();
			GotoXY(47, 15);
			cout << "OPTIONS:" << endl;
			GotoXY(47, 17);
			cout << "1. Start" << endl;
			GotoXY(47, 18);
			cout << "2. Load game" << endl;
			SetColor(122);
			GotoXY(47, 19);
			cout << "3. Quit" << endl;
			SetColor(15);
		}
	}
}



bool IsExistedFileName(string FileName)
{
	ifstream f_user(".\\Data\\username.txt");
	string tmp;

	while (f_user >> tmp)
		if (tmp == FileName)
		{
			f_user.close();
			return true;
		}
	f_user.close();
	return false;
}
bool IsValidFileName(string FileName)
{
	for (int i = 0; i < FileName.length(); i++)
		if (FileName[i] == '>' || FileName[i] == '<' || FileName[i] == ':'
			|| FileName[i] == '"' || FileName[i] == '/' || FileName[i] == '\\'
			|| FileName[i] == '|' || FileName[i] == '?' || FileName[i] == '*')
			return false;
	return true;
}
void DeleteBox()
{
	int column = 30;
	int row = 8;
	int xgame = (WIDTH_CONSOLE / 2) - 15;
	int ygame = (HEIGH_CONSOLE / 2) - 3;

	for (int i = 0; i < row; i++)
	{
		GotoXY(xgame, ygame + i);
		for (int j = 0; j < column; j++)
		{
			if (i == 0)
				cout << " ";
			else if (i == row - 1)
				cout << " ";
			else if (j == 0 || j == column - 1)
				cout << " ";
			else
				cout << " ";
		}
	}
}
void SaveData()
{
	string FileName;

	int column = 30;
	int row = 8;
	int xgame = (WIDTH_CONSOLE / 2) - 15;
	int ygame = (HEIGH_CONSOLE / 2) - 3;

	for (int i = 0; i < row; i++)
	{
		GotoXY(xgame, ygame + i);
		for (int j = 0; j < column; j++)
		{
			if (i == 0)
				cout << (unsigned char)220;
			else if (i == row - 1)
				cout << (unsigned char)223;
			else if (j == 0 || j == column - 1)
				cout << (unsigned char)219;
			else
				cout << " ";
		}
	}

	GotoXY(xgame + 9, ygame + 2);
	cout << "Save and Exit";
	GotoXY(xgame + 14, ygame + 5);
	TextColor(3, ok);
	GotoXY(xgame + 3, ygame + 3);
	cout << "Name: ";

	do
	{
		GotoXY(xgame + 9, ygame + 3);
		cin >> FileName;
		GotoXY(xgame + 3, ygame + 4);
		if (IsExistedFileName(FileName))
			cout << "File existed, re-type!";
		if (!IsValidFileName(FileName))
			cout << "Invalid char, re-type!";
		if (FileName.length() > 18)
			cout << "Too long, re-type!";

		if (IsExistedFileName(FileName) || !IsValidFileName(FileName) || FileName.length() > 18)
		{
			GotoXY(xgame + 9, ygame + 3);
			for (int i = 0; i < 18; i++)
				cout << " ";
		}
	} while (IsExistedFileName(FileName) || !IsValidFileName(FileName) || FileName.length() > 18);

	DeleteBox();

	ofstream fo(".\\Data\\" + FileName);

	ofstream f_user;
	f_user.open(".\\Data\\username.txt", ios::app);
	f_user << FileName << endl;
	f_user.close();

	fo << SIZE_SNAKE << " " << SIZE_PLUS << endl;

	for (int i = 0; i < SIZE_SNAKE; i++)
		fo << snake[i].x << " " << snake[i].y << endl;

	fo << FOOD_INDEX << endl;

	fo << FOOD[FOOD_INDEX].x << " " << FOOD[FOOD_INDEX].y << endl;

	if (GATE_EXIST)
		fo << a.x << " " << a.y << endl;
	else
		fo << -1 << " " << -1 << endl;

	fo << INDEX_ID << endl;

	fo << SPEED << endl;

	fo << ROUND << endl;

	fo << MOVING << endl;

	fo << CHAR_LOCK << endl;

	fo << win;

	fo.close();

	NewLength.name = FileName;
	NewLength.length = SIZE_SNAKE + SIZE_PLUS;

	CreateNewHighLength();
	SortHighLength();
}
void LoadData()
{
	string FileName;
	int column = 30;
	int row = 8;
	int xgame = (WIDTH_CONSOLE / 2) - 15;
	int ygame = (HEIGH_CONSOLE / 2) - 3;

	for (int i = 0; i < row; i++)
	{
		GotoXY(xgame, ygame + i);
		for (int j = 0; j < column; j++)
		{
			if (i == 0)
				cout << (unsigned char)220;
			else if (i == row - 1)
				cout << (unsigned char)223;
			else if (j == 0 || j == column - 1)
				cout << (unsigned char)219;
			else
				cout << " ";
		}
	}

	GotoXY(xgame + 11, ygame + 2);
	cout << "Load data";
	GotoXY(xgame + 14, ygame + 5);
	TextColor(3, ok);
	GotoXY(xgame + 3, ygame + 3);
	cout << "Name: ";

	do
	{
		GotoXY(xgame + 9, ygame + 3);
		cin >> FileName;
		if (!IsExistedFileName(FileName))
		{
			GotoXY(xgame + 3, ygame + 4);
			cout << "Not existed user!";
			GotoXY(xgame + 9, ygame + 3);
			for (int i = 0; i < 18; i++)
				cout << " ";
		}
	} while (!IsExistedFileName(FileName));

	DeleteBox();

	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		GotoXY(snake[i].x, snake[i].y);
		cout << " ";
	}

	ifstream fi(".\\Data\\" + FileName);

	fi >> SIZE_SNAKE >> SIZE_PLUS;

	GotoXY(8, HEIGH_CONSOLE);
	cout << SIZE_SNAKE + SIZE_PLUS;

	for (int i = 0; i < SIZE_SNAKE; i++)
		fi >> snake[i].x >> snake[i].y;

	GotoXY(FOOD[FOOD_INDEX].x, FOOD[FOOD_INDEX].y);

	fi >> FOOD_INDEX;
	cout << " ";
	fi >> FOOD[FOOD_INDEX].x >> FOOD[FOOD_INDEX].y;

	fi >> a.x >> a.y;
	if (a.x != -1 && a.y != -1)
	{
		GotoXY(a.x + 1, a.y);
		cout << (unsigned char)223;
		GotoXY(a.x - 1, a.y - 1);
		cout << (unsigned char)219;
		GotoXY(a.x, a.y);
		cout << (unsigned char)223;
		GotoXY(a.x + 1, a.y - 1);
		cout << (unsigned char)219;
		GotoXY(a.x - 1, a.y);
		cout << (unsigned char)223;
		GATE_EXIST = true;
	}

	fi >> INDEX_ID;

	fi >> SPEED;

	fi >> ROUND;

	fi >> MOVING;

	fi >> CHAR_LOCK;

	fi >> win;

	fi.close();
}
bool IsEmptyHighLengthFile()
{
	ifstream fi;
	string name;
	int length;

	fi.open(".\\Data\\highlength.txt");
	if (fi >> name >> length)
	{
		fi.close();
		return false;
	}

	fi.close();
	return true;
}
void SaveHighLength()
{
	remove(".\\Data\\highlength.txt");

	ofstream fo;
	fo.open(".\\Data\\highlength.txt");

	for (int i = 0; i < 4; i++)
		fo << HighLength[i].name << " " << HighLength[i].length << endl;
	fo << HighLength[4].name << " " << HighLength[4].length;

	fo.close();
}
void ResetHighLength()
{
	for (int i = 0; i < 5; i++)
	{
		HighLength[i].name = "[NONE]";
		HighLength[i].length = 4;
	}
	SaveHighLength();
}
void InitializeHighLength()
{
	if (!IsEmptyHighLengthFile())
	{
		string name;
		int length;

		ifstream fi;
		fi.open(".\\Data\\highlength.txt");

		int i = 0;

		while (fi >> name >> length)
		{
			HighLength[i].name = name;
			HighLength[i].length = length;
			i++;
		}

		fi.close();
	}
	else
		ResetHighLength();
}
void CreateNewHighLength()
{
	int minLength = HighLength[0].length;
	int index = 0;
	for (int i = 0; i < 5; i++)
	{
		if (HighLength[i].name == "[NONE]")
		{
			minLength = HighLength[i].length;
			index = i;
			break;
		}
		else if (HighLength[i].length < minLength)
		{
			minLength = HighLength[i].length;
			index = i;
		}
	}

	if (NewLength.length > minLength || (NewLength.length == 4 && HighLength[index].name == "[NONE]"))
	{
		HighLength[index].name = NewLength.name;
		HighLength[index].length = NewLength.length;
	}
}
void SortHighLength()
{
	for (int i = 0; i < 4; i++)
		for (int j = i + 1; j < 5; j++)
			if (HighLength[i].length < HighLength[j].length)
			{
				string name = HighLength[i].name;
				HighLength[i].name = HighLength[j].name;
				HighLength[j].name = name;

				int length = HighLength[i].length;
				HighLength[i].length = HighLength[j].length;
				HighLength[j].length = length;
			}
	SaveHighLength();
}
void ShowHighLength()
{
	ifstream fi;
	fi.open(".\\Data\\highlength.txt");

	string name;
	int length;

	system("cls");

	int column = 31;
	int row = 9;
	int xHighLength = (WIDTH_CONSOLE / 2) - 15;
	int yHighLength = (HEIGH_CONSOLE / 2) - 4;

	for (int j = 0; j < row; j++)
	{
		GotoXY(xHighLength, yHighLength + j);
		for (int k = 0; k < column; k++)
		{
			if (j == 0)
				cout << (unsigned char)220;
			else if (j == 2 && k != 0 && k != column - 1)
				cout << "*";
			else if (j == row - 1)
				cout << (unsigned char)223;
			else if (k == 0 || k == column - 1)
				cout << (unsigned char)219;
			else
				cout << " ";
		}
	}

	while (true)
	{
		GotoXY(xHighLength + 10, yHighLength + 1);
		cout << "HIGH LENGTH";

		int i = 0;

		while (fi >> name >> length)
		{
			GotoXY(xHighLength + 4, yHighLength + i + 3);
			cout << "#" << i + 1 << ". ";
			GotoXY(xHighLength + 8, yHighLength + i + 3);
			cout << name;
			GotoXY(xHighLength + 27, yHighLength + i + 3);
			cout << length;
			i++;
		}

		if (_kbhit())
			break;
	}
	fi.close();
}
void TextColor(int color, char* OutputContent)
{
	static int __BACKGROUND;

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(h, &csbiInfo);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color + (__BACKGROUND << 4));

	cout << OutputContent;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 + (__BACKGROUND << 4));
}