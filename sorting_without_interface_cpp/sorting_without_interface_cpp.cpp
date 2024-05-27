#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <commctrl.h>
#include <iomanip>

#pragma comment(lib, "comctl32.lib")

#define IDC_INPUT_ARRAY_EDIT 1001
#define IDC_SORT_TYPE_COMBO 1002
#define IDC_SORT_BUTTON 1003
#define IDC_SHOW_STEPS_BUTTON 1004
#define IDC_SHOW_TIME_BUTTON 1005
#define IDC_VIEW_ARRAY_BUTTON 1006

using namespace std;

// Вспомогательные функции
vector<int> parse_array(const  wstring& input) 
{
    vector<int> arr;
    wstringstream ss(input);
    int num;
    while (ss >> num) {
        arr.push_back(num);
    }
    return arr;
}

// Реализации сортировок
void bubble_sort(vector<int>& arr, vector<vector<int>>& steps) 
{
    steps.clear();
    steps.push_back(arr);
    for (size_t i = 0; i < arr.size(); ++i) {
        for (size_t j = 0; j < arr.size() - i - 1; ++j) 
        {
            if (arr[j] > arr[j + 1]) 
            {
                 swap(arr[j], arr[j + 1]);
                steps.push_back(arr);
            }
        }
    }
}

void quick_sort(vector<int>& arr, int left, int right, vector<vector<int>>& steps) 
{
    if (left < right) 
    {
        int pivot = arr[right];
        int i = left - 1;
        for (int j = left; j < right; ++j) 
        {
            if (arr[j] < pivot) 
            {
                ++i;
                 swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[right]);
        steps.push_back(arr);
        int pi = i + 1;
        quick_sort(arr, left, pi - 1, steps);
        quick_sort(arr, pi + 1, right, steps);
    }
}

void selection_sort(vector<int>& arr,  vector< vector<int>>& steps) 
{
    steps.clear();
    steps.push_back(arr);
    for (size_t i = 0; i < arr.size(); ++i) 
    {
        size_t min_index = i;
        for (size_t j = i + 1; j < arr.size(); ++j) 
        {
            if (arr[j] < arr[min_index]) 
            {
                min_index = j;
            }
        }
        if (min_index != i) 
        {
             swap(arr[i], arr[min_index]);
            steps.push_back(arr);
        }
    }
}

void insertion_sort( vector<int>& arr,  vector< vector<int>>& steps)
{
    steps.clear();
    steps.push_back(arr);
    for (size_t i = 1; i < arr.size(); ++i) 
    {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) 
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
        steps.push_back(arr);
    }
}

void merge( vector<int>& arr, int left, int middle, int right,  vector< vector<int>>& steps) 
{
    int n1 = middle - left + 1;
    int n2 = right - middle;
     vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; ++i) 
    {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; ++j) 
    {
        R[j] = arr[middle + 1 + j];
    }

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) 
    {
        if (L[i] <= R[j]) 
        {
            arr[k] = L[i];
            ++i;
        } else 
        {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) 
    {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) 
    {
        arr[k] = R[j];
        ++j;
        ++k;
    }
    steps.push_back(arr);
}

void merge_sort( vector<int>& arr, int left, int right,  vector< vector<int>>& steps)
{
    if (left < right) 
    {
        int middle = left + (right - left) / 2;
        merge_sort(arr, left, middle, steps);
        merge_sort(arr, middle + 1, right, steps);
        merge(arr, left, middle, right, steps);
    }
}

// Вспомогательные структуры и функции для работы с окнами
struct SortData {
     vector<int> original_array;
     vector<int> sorted_array;
     vector< vector<int>> steps;
    double sort_time;
    HWND hStepWnd;
    HWND hTimeWnd;
    HWND hViewArrayWnd;
};

SortData* create_sort_data()
{
    SortData* data = new SortData();
    data->sort_time = 0;
    data->hStepWnd = NULL;
    data->hTimeWnd = NULL;
    data->hViewArrayWnd = NULL;
    return data;
}

void destroy_sort_data(SortData* data) { delete data; }

void init_ui(HWND hWnd) 
{
    CreateWindowEx(0, L"STATIC", L"Введите массив (числа через пробел):", WS_CHILD | WS_VISIBLE,
        10, 10, 300, 20, hWnd, NULL, NULL, NULL);

    CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
        10, 40, 300, 60, hWnd, (HMENU)IDC_INPUT_ARRAY_EDIT, NULL, NULL);

    CreateWindowEx(0, L"STATIC", L"Выберите тип сортировки:", WS_CHILD | WS_VISIBLE,
        10, 110, 200, 20, hWnd, NULL, NULL, NULL);

    HWND hCombo = CreateWindowEx(0, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,
        10, 140, 200, 100, hWnd, (HMENU)IDC_SORT_TYPE_COMBO, NULL, NULL);

    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Сортировка пузырьком");
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Быстрая сортировка");
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Сортировка выбором");
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Сортировка вставками");
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Сортировка слиянием");

    CreateWindowEx(0, L"BUTTON", L"Сортировать", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        10, 180, 100, 30, hWnd, (HMENU)IDC_SORT_BUTTON, NULL, NULL);

    CreateWindowEx(0, L"BUTTON", L"Показать этапы", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        120, 180, 120, 30, hWnd, (HMENU)IDC_SHOW_STEPS_BUTTON, NULL, NULL);

    CreateWindowEx(0, L"BUTTON", L"Показать время", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        250, 180, 120, 30, hWnd, (HMENU)IDC_SHOW_TIME_BUTTON, NULL, NULL);

    CreateWindowEx(0, L"BUTTON", L"Показать массивы", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        10, 220, 120, 30, hWnd, (HMENU)IDC_VIEW_ARRAY_BUTTON, NULL, NULL);
}

void sort_array(HWND hWnd, SortData* data) 
{
    wchar_t input[1024];
    GetDlgItemTextW(hWnd, IDC_INPUT_ARRAY_EDIT, input, 1024);
    data->original_array = parse_array(input);

    int sort_index = SendDlgItemMessageW(hWnd, IDC_SORT_TYPE_COMBO, CB_GETCURSEL, 0, 0);
    if (sort_index == CB_ERR) 
    {
        MessageBoxW(hWnd, L"Пожалуйста, выберите тип сортировки.", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }

    data->sorted_array = data->original_array;
    data->steps.clear();
    auto start =  chrono::high_resolution_clock::now();
    switch (sort_index) 
    {
    case 0:
        bubble_sort(data->sorted_array, data->steps);
        break;
    case 1:
        quick_sort(data->sorted_array, 0, data->sorted_array.size() - 1, data->steps);
        break;
    case 2:
        selection_sort(data->sorted_array, data->steps);
        break;
    case 3:
        insertion_sort(data->sorted_array, data->steps);
        break;
    case 4:
        merge_sort(data->sorted_array, 0, data->sorted_array.size() - 1, data->steps);
        break;
    default:
        MessageBoxW(hWnd, L"Неверный выбор сортировки.", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }
    auto end =  chrono::high_resolution_clock::now();
     chrono::duration<double,  milli> duration = end - start;
    data->sort_time = duration.count();

    MessageBoxW(hWnd, L"Сортировка завершена.", L"Уведомление", MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK StepWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    static SortData* data = nullptr;

    if (message == WM_CREATE) 
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        data = (SortData*)pCreate->lpCreateParams;
    }

    switch (message) 
    {
    case WM_CREATE:
        if (data) 
        {
            int y = 10;
            for (const auto& step : data->steps) 
            {
                 wstringstream ss;
                for (int num : step)
                {
                    ss << num << L" ";
                }
                CreateWindowW(L"STATIC", ss.str().c_str(), WS_CHILD | WS_VISIBLE,
                    10, y, 360, 20, hWnd, NULL, NULL, NULL);
                y += 30;
            }
        }
        break;
    case WM_DESTROY:
        data->hStepWnd = NULL;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK TimeWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    static SortData* data = nullptr;

    if (message == WM_CREATE) 
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        data = (SortData*)pCreate->lpCreateParams;
    }

    switch (message) 
    {
    case WM_CREATE:
        if (data) 
        {
             wstringstream ss;
            ss << L"Время сортировки: " << data->sort_time << L" мс";
            CreateWindowW(L"STATIC", ss.str().c_str(), WS_CHILD | WS_VISIBLE,
                10, 10, 360, 20, hWnd, NULL, NULL, NULL);
        }
        break;
    case WM_DESTROY:
        data->hTimeWnd = NULL;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ViewArrayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    static SortData* data = nullptr;

    if (message == WM_CREATE) 
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        data = (SortData*)pCreate->lpCreateParams;
    }

    switch (message) 
    {
    case WM_CREATE:
        if (data) 
        {
             wstringstream ssOriginal, ssSorted;
            ssOriginal << L"Исходный массив: ";
            for (int num : data->original_array) 
            {
                ssOriginal << num << L" ";
            }
            ssSorted << L"Отсортированный массив: ";
            for (int num : data->sorted_array) 
            {
                ssSorted << num << L" ";
            }

            CreateWindowW(L"STATIC", ssOriginal.str().c_str(), WS_CHILD | WS_VISIBLE,
                10, 10, 360, 20, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"STATIC", ssSorted.str().c_str(), WS_CHILD | WS_VISIBLE,
                10, 40, 360, 20, hWnd, NULL, NULL, NULL);
        }
        break;
    case WM_DESTROY:
        data->hViewArrayWnd = NULL;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void show_steps_window(HWND hWnd, SortData* data) 
{
    if (data->hStepWnd) 
    {
        SetForegroundWindow(data->hStepWnd);
        return;
    }

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = StepWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"StepWindowClass";
    RegisterClassW(&wc);

    data->hStepWnd = CreateWindowW(L"StepWindowClass", L"Этапы сортировки", WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, hWnd, NULL, GetModuleHandle(NULL), data);

    ShowWindow(data->hStepWnd, SW_SHOW);
    UpdateWindow(data->hStepWnd);
}

void show_time_window(HWND hWnd, SortData* data) 
{
    if (data->hTimeWnd) 
    {
        SetForegroundWindow(data->hTimeWnd);
        return;
    }

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = TimeWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"TimeWindowClass";
    RegisterClassW(&wc);

    data->hTimeWnd = CreateWindowW(L"TimeWindowClass", L"Время сортировки", WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, hWnd, NULL, GetModuleHandle(NULL), data);

    ShowWindow(data->hTimeWnd, SW_SHOW);
    UpdateWindow(data->hTimeWnd);
}

void show_view_array_window(HWND hWnd, SortData* data) 
{
    if (data->hViewArrayWnd) 
    {
        SetForegroundWindow(data->hViewArrayWnd);
        return;
    }

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = ViewArrayWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ViewArrayWindowClass";
    RegisterClassW(&wc);

    data->hViewArrayWnd = CreateWindowW(L"ViewArrayWindowClass", L"Просмотр массивов", WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, hWnd, NULL, GetModuleHandle(NULL), data);

    ShowWindow(data->hViewArrayWnd, SW_SHOW);
    UpdateWindow(data->hViewArrayWnd);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    static SortData* data = nullptr;

    switch (message) 
    {
    case WM_CREATE:
        data = create_sort_data();
        init_ui(hWnd);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDC_SORT_BUTTON:
            sort_array(hWnd, data);
            break;
        case IDC_SHOW_STEPS_BUTTON:
            show_steps_window(hWnd, data);
            break;
        case IDC_SHOW_TIME_BUTTON:
            show_time_window(hWnd, data);
            break;
        case IDC_VIEW_ARRAY_BUTTON:
            show_view_array_window(hWnd, data);
            break;
        }
        return 0;
    case WM_DESTROY:
        destroy_sort_data(data);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) 
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainWindowClass";
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(L"MainWindowClass", L"Сортировщик", WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
