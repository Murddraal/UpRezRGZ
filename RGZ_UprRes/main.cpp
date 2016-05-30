#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <string>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("РГЗ по Управлению ресурсами в вычислительных системах.");
HINSTANCE hInst;
HWND hWindow;
char info[256], info_1[256], str[256];
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
		lf.lfCharSet = DEFAULT_CHARSET; //значение по умолчанию
		lf.lfPitchAndFamily = DEFAULT_PITCH; //значения по умолчанию
		strcpy_s(lf.lfFaceName, "Times New Roman"); //копируем в строку название шрифта 
		lf.lfHeight = 16; //высота
		lf.lfWidth = 7.5; //ширина
		lf.lfWeight = 40; //толщина
		lf.lfUnderline = 0;//без подчеркивания
		lf.lfItalic = 0;//без курсива
		lf.lfStrikeOut = 0;//без перечеркивания
		lf.lfEscapement = 0; //шрифт без поворота
		colorText = RGB(0, 0, 0);//цвет текста
		hFont = CreateFontIndirect(&lf); //Cоздали шрифт
		SelectObject(hdc, hFont); //установка шрифта
		SetTextColor(hdc, colorText); //установка цвета текста
		SetBkMode(hdc, TRANSPARENT);//установка закраски за текстом
		//вывод текста
		TextOut(hdc, 5, 5, str, _tcslen(str));
		TextOut(hdc, 5, 30, info, _tcslen(info));
		TextOut(hdc, 5, 60, info_1, _tcslen(info_1));
		DeleteObject(hFont); //выгрузим из памяти объект шрифта
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY://закрытие окна
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
	WNDCLASSEX wcex;//класс окна
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;//стиль окна(перерисовка при изменении размеров окна)
	wcex.lpfnWndProc = WndProc;//функция-обработчик сообщений
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;//класс окна
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));//пиктограмма
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//курсор окна
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 128, 256));//цвет фона
	wcex.lpszMenuName = NULL;//меню в окне не предусмотрено
	wcex.lpszClassName = szWindowClass;//название класса окна приложения
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	if (!RegisterClassEx(&wcex))//регистрация класса окна
	{
		MessageBox(NULL,_T("Call to RegisterClassEx failed!"), _T("Win32 Guided Tour"),NULL);
		return 1;
	}
	hInst = hInstance;
	// Параметры для CreateWindow:
	// szWindowClass: название приложения
	// szTitle: текст, отображаемый в строке заголовка
	//WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX: тип окна для создания
	// CW_USEDEFAULT, CW_USEDEFAULT: начальное положение (x, y)
	// 800, 200: начальный размер (ширина, длина)
	// NULL:родительское окно
	// NULL: наличие меню
	// hInstance: первый параметр WinMain	
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		850, 200,
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
	DWORD WINAPI ThreadFunc(void *);//прототип функции потока
	DWORD IDThread;//идентификатор потока
	//создание потока
	//возвращает дескриптор потока
	HANDLE hThread = CreateThread(NULL, 0,
		ThreadFunc, NULL, 0, &IDThread);
	//ожидание завершения потока
	//INFINITE-бесконечное время ожидания
	WaitForSingleObject(hThread, INFINITE);
	//удаление дескриптора потока
	CloseHandle(hThread);
	// параметры, для отображения окна:
	// hWnd: возвращаемое значение из CreateWindow
	// nCmdShow: 4 параметр из WinMain
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	MSG msg;//тип сообщений
	//цикл обработки сообщений
	while (GetMessage(&msg, NULL, 0, 0))//получение сообщения
	{
		//преобразование код клавиш в ASCI
		TranslateMessage(&msg);
		//отправка сообщений в нужную процедуру
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

DWORD WINAPI ThreadFunc(void*)
{

	HINSTANCE hLib = LoadLibrary(TEXT("lib_DLL.dll"));
	if (hLib)
	{

		typedef std::string(*GetKeyboard)();//тип указателя на функцию
		GetKeyboard DLLInfo = (GetKeyboard)GetProcAddress(hLib, "GetInformation");

		typedef int(*GetCacheAssociative)();
		GetCacheAssociative CacheAssociative = (GetCacheAssociative)GetProcAddress(hLib, "GetCache");

		std::string KeyboarType = DLLInfo();
		int Cache = CacheAssociative();

		sprintf_s(str, "Выполнил студент группы ПМИ-31 Савицкий Ю.Р.\n");
		sprintf_s(info, "Тип клавиатуры: %s\n", KeyboarType);
		sprintf_s(info_1, "Размер страницы буфера ассоциативной трансляции команд: %d", Cache);
		FreeLibrary(hLib);
		return 0;
	}
	else
	{
		MessageBox(hWindow, _T("Dll was not loaded!"), _T("Error!"), MB_OK);
		return 1;
	}
}