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

const unsigned int kMainWindowWidth = 800;
const unsigned int kMainWindowHeight = 600;

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine,
                    int nCmdShow)
{
    if(!Application::GetInstance()->InitMainWindow(hInstance, kMainWindowWidth, kMainWindowHeight))
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