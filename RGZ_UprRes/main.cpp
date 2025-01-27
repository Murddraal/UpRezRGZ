#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <string>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("��� �� ���������� ��������� � �������������� ��������.");
HINSTANCE hInst;
HWND hWindow;
char info[256], info_1[256], str[256], brandInfo[256];

DWORD WINAPI ThreadFunc(void*)
{

	HINSTANCE hLib = LoadLibrary(TEXT("lib.dll"));
	if (hLib)
	{

		typedef char*(*GetKeyboard)();//��� ��������� �� �������
		GetKeyboard KeyboardType = (GetKeyboard)GetProcAddress(hLib, "GetInformation");

		typedef char*(*GetCacheAssociative)(int*);
		GetCacheAssociative CacheAssociative = (GetCacheAssociative)GetProcAddress(hLib, "GetCache");

		char* keyboarType = KeyboardType();
		int Cache = 0;

		char* Brand = CacheAssociative(&Cache);

		sprintf_s(str, "�������� ������� ������ ���-31 �������� �.�.\n");
		sprintf_s(info, "��� ����������: %s\n", keyboarType);
		sprintf_s(brandInfo, "����� ����������: %s\n", Brand);

		if (Cache > 1)
			sprintf_s(info_1, "��������������� L3 ����: %d-way associative", Cache);
		else
			if (Cache == -1)
				sprintf_s(info_1, "L3 ��� ��������� ������������");
			else
				sprintf_s(info_1, "��������� ���������� ��������������� L3 ����");


		FreeLibrary(hLib);
		return 0;
	}
	else
	{
		MessageBox(hWindow, _T("Dll was not loaded!"), _T("Error!"), MB_OK);
		return 1;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	COLORREF colorText;
	LOGFONT lf;
	HFONT hFont;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		lf.lfCharSet = DEFAULT_CHARSET; //�������� �� ���������
		lf.lfPitchAndFamily = DEFAULT_PITCH; //�������� �� ���������
		strcpy_s(lf.lfFaceName, "Times New Roman"); //�������� � ������ �������� ������ 
		lf.lfHeight = 16; //������
		lf.lfWidth = 7.5; //������
		lf.lfWeight = 40; //�������
		lf.lfUnderline = 0;//��� �������������
		lf.lfItalic = 0;//��� �������
		lf.lfStrikeOut = 0;//��� ��������������
		lf.lfEscapement = 0; //����� ��� ��������
		colorText = RGB(0, 0, 0);//���� ������
		hFont = CreateFontIndirect(&lf); //C������ �����
		SelectObject(hdc, hFont); //��������� ������
		SetTextColor(hdc, colorText); //��������� ����� ������
		SetBkMode(hdc, TRANSPARENT);//��������� �������� �� �������
		//����� ������
		TextOut(hdc, 5, 5, str, _tcslen(str));
		TextOut(hdc, 5, 30, info, _tcslen(info));
		TextOut(hdc, 5, 60, brandInfo, _tcslen(brandInfo));
		TextOut(hdc, 5, 90, info_1, _tcslen(info_1));
		
		DeleteObject(hFont); //�������� �� ������ ������ ������
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY://�������� ����
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;//����� ����
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;//����� ����(����������� ��� ��������� �������� ����)
	wcex.lpfnWndProc = WndProc;//�������-���������� ���������
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;//����� ����
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));//�����������
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//������ ����
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 128));//���� ����
	wcex.lpszMenuName = NULL;//���� � ���� �� �������������
	wcex.lpszClassName = szWindowClass;//�������� ������ ���� ����������
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	if (!RegisterClassEx(&wcex))//����������� ������ ����
	{
		MessageBox(NULL,_T("Call to RegisterClassEx failed!"), _T("Win32 Guided Tour"),NULL);
		return 1;
	}
	hInst = hInstance;
	// ��������� ��� CreateWindow:
	// szWindowClass: �������� ����������
	// szTitle: �����, ������������ � ������ ���������
	//WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX: ��� ���� ��� ��������
	// CW_USEDEFAULT, CW_USEDEFAULT: ��������� ��������� (x, y)
	// 450, 200: ��������� ������ (������, �����)
	// NULL:������������ ����
	// NULL: ������� ����
	// hInstance: ������ �������� WinMain	
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		450, 200,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}
	DWORD WINAPI ThreadFunc(void *);//�������� ������� ������
	DWORD IDThread;//������������� ������
	//�������� ������
	//���������� ���������� ������
	HANDLE hThread = CreateThread(NULL, 0,
		ThreadFunc, NULL, 0, &IDThread);
	//�������� ���������� ������
	//INFINITE-����������� ����� ��������
	WaitForSingleObject(hThread, INFINITE);
	//�������� ����������� ������
	CloseHandle(hThread);
	// ���������, ��� ����������� ����:
	// hWnd: ������������ �������� �� CreateWindow
	// nCmdShow: 4 �������� �� WinMain
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	MSG msg;//��� ���������
	//���� ��������� ���������
	while (GetMessage(&msg, NULL, 0, 0))//��������� ���������
	{
		//�������������� ��� ������ � ASCI
		TranslateMessage(&msg);
		//�������� ��������� � ������ ���������
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

