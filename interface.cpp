#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <string>

/*
    NOTES:
    - Will need to store manifest name when user inputs into a global variable
*/

std::string getT() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_c);

    char buf[80];
    std::strftime(buf, sizeof(buf), "%r", &now_tm);

    std::stringstream stream;
    stream << buf;
    std::string time_str = stream.str();


    return time_str;
}

std::string getManifestName()
{
    std::string MANIFEST_NAME;
    // TODO: Need to store manifest name to global variable
    return MANIFEST_NAME;
}

std::string moveIndicator()
{
    std::string moveStr;
    // TODO: Need to get Move # and Total Moves and combine
    return moveStr;
}

std::string moveDesc()
{
    std::string moveInfo;
    // TODO: Need to generate move info based on algorithim output
    return moveInfo;
}

void genDropDownMenu()
{
    // TODO: Find Fix for menu problem
    /*
    #define MAX_OPTIONS 5

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "LoadListC";
        RegisterClass(&wc);

        HWND hwnd = CreateWindowEx(0, "WinLoadContainers", "LoadListC", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);
        if (hwnd == NULL) {
            return 0;
        }

        // Create the drop-down check box
        HWND hwndCheckBox = CreateWindowEx(0, "BUTTON", "Select containers to unload", WS_CHILD | WS_VISIBLE | BS_DROPDOWNCHECK | CBS_DROPDOWNLIST,
            10, 10, 200, 30, hwnd, (HMENU)1, hInstance, NULL);

        // Add the options to the drop-down check box
        TCHAR options[MAX_OPTIONS][20] = { "Option 1", "Option 2", "Option 3", "Option 4", "Option 5" };
        for (int i = 0; i < MAX_OPTIONS; i++) {
            SendMessage(hwndCheckBox, CB_ADDSTRING, 0, (LPARAM)options[i]);
        }

        ShowWindow(hwnd, nCmdShow);

        MSG msg = { 0 };
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return msg.wParam;
    }

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    */
}