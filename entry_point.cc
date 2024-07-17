/*
    Author: 航火火
    Path: main.cpp
    Description:  Application Exe Main Entry Point File
*/
#include <Windows.h>

#include <iostream>

#include "application.h"

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPTSTR lpCmdLine,
                    int nCmdShow)
{
    const int window_width = 1200;
    const int window_height = 900;
    const TCHAR* window_title = TEXT("DragonSoftRenderer");

    if(!APP->Init(hInstance, window_title, window_width, window_height))
    {
        std::cout << "Application Init Failed!" << std::endl;
        return -1;
    }

	APP->Run();

    std::cout << "Application will exit!" << std::endl;

    return 0;
}
