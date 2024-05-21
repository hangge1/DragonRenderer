/**
 * Author: 航火火
 * Path: main.cpp
 * Description: 
 *      Application Exe Main Entry Point File
*/

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )


#include <Windows.h>

#include <iostream>

#include "application.h"



int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine,
                    int nCmdShow)
{
    if(!Application::GetInstance()->InitMainWindow(hInstance))
    {
        std::cout << "Application Init Failed!" << std::endl;
        return -1;
    }

    while(!Application::GetInstance()->HasMainWindowDestoryed())
    {
        Application::GetInstance()->DispatchMessageLoop();
    }

    std::cout << "Application will exit!" << std::endl;

    return 0;
}


// 控制台入口函数
// int main()
// {
//     std::cout << "Hello Dragon Renderer" << std::endl;
//     return 0;
// }