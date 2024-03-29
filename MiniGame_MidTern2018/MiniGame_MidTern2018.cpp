// MiniGame_MidTern2018.cpp : Defines the entry point for the application.
// 1512054 - Tran Minh Cuong
#include "stdafx.h"
#include "MiniGame_MidTern2018.h"
#include <windowsx.h>
#include <cstdlib>
#include <ctime>
#include "windows.h"
#include <gdiplus.h>
#pragma comment (lib, "gdiplus")
using namespace Gdiplus;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

#pragma region MyVariables
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnDestroy(HWND hwnd);
void OnPaint(HWND hwnd);

//Mảng lưu giá trị chuỗi của hình ảnh
const wchar_t* imgs[10] = { L"Flags/Brazil.jpg",
							L"Flags/Cambodia.jpg",
							L"Flags/Duc.jpg",
							L"Flags/England.jpg",
							L"Flags/France.jpg",
							L"Flags/Japan.jpg",
							L"Flags/Korea.jpg",
							L"Flags/Thailand.jpg",
							L"Flags/VietNam.jpg",
							L"Flags/Y.jpg" };
//Mảng lưu giá trị câu hỏi tương ứng
const wchar_t* questions[10] = { L"Which flag belongs to Brazil?",
									L"Which flag belongs to Cambodia?",
									L"Which flag belongs to Germany?",
									L"Which flag belongs to England?",
									L"Which flag belongs to France?",
									L"Which flag belongs to Japan?",
									L"Which flag belongs to Korea?",
									L"Which flag belongs to Thailand?",
									L"Which flag belongs to Vietnam?",
									L"Which flag belongs to Italia?" };
//Label hiển thị câu hỏi
HWND lblQuestion;
//Label hiển thị điểm của người chơi
HWND lblScore;
//Label hien thi thoi gian dem nguoc
HWND lblTimer;
//Giá trị thời gian đếm ngược là 10s
int seconds = 10;
// Button chọn hình 1
HWND btnImg1;
// Button chọn hình 2
HWND btnImg2;

//Đối tượng đồ họa vẽ ảnh
ULONG_PTR gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

//Biến lưu giữ giá trị được chọn đúng.
int iTrueButton;
//Biến lưu giữ giá trị điểm của người chơi
int score;

//Lưu thông tin cấu hình
const int BUFFERSIZE = 260;
WCHAR buffer[BUFFERSIZE];
WCHAR curPath[BUFFERSIZE];
WCHAR configPath[BUFFERSIZE];
#pragma endregion

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MINIGAMEMIDTERN2018, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINIGAMEMIDTERN2018));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



#pragma region MyMethods
//Lưu thông tin cấu hình bao gồm app name và width và height
void saveGameConfig(HWND hwnd)
{
	//Lấy tên chương trình
	WCHAR appName[255];
	GetWindowText(hwnd, appName, BUFFERSIZE);
	//Ghi tên chương trình vào trong file ini
	WritePrivateProfileString(L"app", L"title", appName, configPath);
	//Lấy width và hieght
	RECT rect;
	int width, height;
	if (GetWindowRect(hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	wchar_t *wcharWidth = new wchar_t[32];
	_itow_s(width, wcharWidth, 10, 10);
	wchar_t *wcharHeight = new wchar_t[32];
	_itow_s(width, wcharHeight, 10, 10);
	WritePrivateProfileString(L"app", L"width", wcharWidth, configPath);
	WritePrivateProfileString(L"app", L"height", wcharHeight, configPath);
	delete wcharWidth;
	delete wcharHeight;
}

//Khởi tạo
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	// Lấy font hệ thống và để dành để gán cho các control (để Thầy thưởng cho 0.5đ)
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	//Khởi tạo điểm là 0
	score = 0;

	//Khởi tạo bộ sinh số ngẫu nhiên
	srand(time(NULL));

	//Tạo label câu hỏi
	lblQuestion = CreateWindowEx(NULL, L"STATIC", L"Question:", WS_CHILD | WS_VISIBLE,
		10, 10, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	SetWindowFont(lblQuestion, hFont, TRUE);

	//Tạo Label hiển thị điểm cho người chơi
	lblScore = CreateWindowEx(NULL, L"STATIC", L"0", WS_CHILD | WS_VISIBLE,
		450, 10, 20, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	SetWindowFont(lblScore, hFont, TRUE);

	//Tạo Label hiển thị thời gian đếm ngược
	lblTimer = CreateWindowEx(NULL, L"STATIC", L"10", WS_CHILD | WS_VISIBLE,
		255, 140, 20, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	SetWindowFont(lblTimer, hFont, TRUE);

	//Tạo button chọn đáp án bên trái
	btnImg1 = CreateWindowEx(NULL, L"BUTTON", L"Select",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 130, 140, 50, 25, hwnd,
		(HMENU)IDC_BTNIMG1, lpCreateStruct->hInstance, NULL);
	SetWindowFont(btnImg1, hFont, TRUE);
	//Tạo button chọn đáp án bên phải
	btnImg2 = CreateWindowEx(NULL, L"BUTTON", L"Select",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 330, 140, 50, 25, hwnd,
		(HMENU)IDC_BTNIMG2, lpCreateStruct->hInstance, NULL);
	SetWindowFont(btnImg2, hFont, TRUE);

	return TRUE;
}
//Thực thi
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	//Kiểm tra các ID đầu vào
	switch (id)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hwnd);
		break;
	case IDC_BTNIMG1:
		//Xử lí khi click vào button số 1
		if (iTrueButton == 1)
		{
			//Chọn đúng
			//Tăng 10 điểm
			score += 10;
			InvalidateRect(hwnd, NULL, true);
		}
		else
		{
			//Chọn sai
			MessageBox(hwnd, (LPCWSTR)L"Your answer was wrong :(",
				(LPCWSTR)L"Notification", MB_ICONERROR);
			wchar_t *yourScore = new wchar_t[32];
			_itow_s(score, yourScore, 10, 10);
			MessageBox(hwnd, (LPCWSTR)yourScore,
				(LPCWSTR)L"Your Score:", 0);
			MessageBox(hwnd, (LPCWSTR)L"See you again...",
				(LPCWSTR)L"Good bye!", 0);
			delete yourScore;
			PostQuitMessage(0);
		}
		break;
	case IDC_BTNIMG2:
		//Xử lí khi click vào button số 2
		if (iTrueButton == 2)
		{
			//Chọn đúng
			//Tăng 10 điểm
			score += 10;
			InvalidateRect(hwnd, NULL, true);
		}
		else
		{
			//Chọn sai
			MessageBox(hwnd, (LPCWSTR)L"Your answer was wrong :(",
				(LPCWSTR)L"Notification", MB_ICONERROR);
			wchar_t *yourScore = new wchar_t[32];
			_itow_s(score, yourScore, 10, 10);
			MessageBox(hwnd, (LPCWSTR)yourScore,
				(LPCWSTR)L"Your Score:", 0);
			MessageBox(hwnd, (LPCWSTR)L"See you again...",
				(LPCWSTR)L"Good bye!", 0);
			delete yourScore;
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}
}
//Hủy
void OnDestroy(HWND hwnd)
{
	saveGameConfig(hwnd);
	PostQuitMessage(0);
}
//Xử lí khi timer chạy
void DecreaseSeconds(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	seconds--; // Giảm đi một giây

	WCHAR buffer[5];
	wsprintf(buffer, L"%02d", seconds);
	SetWindowText(lblTimer, buffer);
	if (seconds == 0)
	{
		KillTimer(hwnd, IDT_TIMER1); // Hủy bỏ timer
		MessageBox(0, L"You lost the game because you did not choose any answers", L"Notification", 0);
		wchar_t *yourScore = new wchar_t[32];
		_itow_s(score, yourScore, 10, 10);
		MessageBox(hwnd, (LPCWSTR)yourScore,
			(LPCWSTR)L"Your Score:", 0);
		MessageBox(hwnd, (LPCWSTR)L"See you again...",
			(LPCWSTR)L"Good bye!", 0);
		PostQuitMessage(0);
	}
}
//Vẽ lại
void OnPaint(HWND hwnd)
{
	seconds = 10;
	//Đếm thời gian ngược 10s nếu người chơi không chọn là coi như thua game
	SetTimer(hwnd, IDT_TIMER1, 1000, (TIMERPROC)DecreaseSeconds);

	//Random câu hỏi
	int indexTrueQues = rand() % 10;
	SetWindowText(lblQuestion, questions[indexTrueQues]);
	//Hiển thị điểm lên label
	wchar_t *_score = new wchar_t[32];
	_itow_s(score, _score, 10, 10);
	SetWindowText(lblScore, _score);
	delete _score;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	{	//Dùng ngoặc này để cho đảm bảo cơ chế các đồ họa hình ảnh hủy trước khi mà GdiplusShutdown
		Graphics graphics(hdc);
		int iRandButton = rand() % 2;
		if (iRandButton == 0)
		{
			//Trường hợp lá cờ đúng nằm ở bên trái
			iTrueButton = 1;
			//Set lá cờ đúng ở bên trái
			graphics.DrawImage(&Image(imgs[indexTrueQues]), 100, 50, 120, 80);
			//Random đại một lá cờ mà khác lá cờ đúng để ở bên còn lại
			int b;
			do
			{
				b = rand() % 10;
			} while (b == indexTrueQues);
			graphics.DrawImage(&Image(imgs[b]), 300, 50, 120, 80);
		}
		else
		{
			//Trường hợp lá cờ đúng nằm ở bên phải
			iTrueButton = 2;
			//Set lá cờ đúng cho nó ở bên phải
			graphics.DrawImage(&Image(imgs[indexTrueQues]), 300, 50, 120, 80);
			//Random đại một lá cờ mà khác lá cờ đúng để ở bên còn lại
			int b;
			do
			{
				b = rand() % 10;
			} while (b == indexTrueQues);
			graphics.DrawImage(&Image(imgs[b]), 100, 50, 120, 80);
		}
	}

	GdiplusShutdown(gdiplusToken);

	EndPaint(hwnd, &ps);
}
#pragma endregion


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINIGAMEMIDTERN2018));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MINIGAMEMIDTERN2018);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		100, 100, 500, 300, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Tạo đường dẫn tuyệt đối tới file config
	GetCurrentDirectory(BUFFERSIZE, curPath);
	wsprintf(configPath, L"%s\\config.ini", curPath);

	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
