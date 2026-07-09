#include "input_state.h"

#include <iostream>

int main()
{
    InputState passive_input;
    passive_input.BeginFrame();
    passive_input.SetMousePosition(1, 2);
    passive_input.SetMousePosition(4, 6);

    if(!passive_input.IsMouseMoved() ||
       passive_input.GetMouseDeltaX() != 3 ||
       passive_input.GetMouseDeltaY() != 4 ||
       !passive_input.HasActivityThisFrame() ||
       passive_input.IsAnyKeyDown() ||
       passive_input.IsMouseButtonDown(InputState::MouseRight))
    {
        std::cout << "input state test Failed" << std::endl;
        return 1;
    }

    InputState input;
    input.BeginFrame();
    input.SetMousePosition(10, 20);
    input.SetMousePosition(15, 18);
    input.SetMousePosition(18, 25);
    input.SetMouseButtonDown(InputState::MouseRight, true);
    input.SetKeyDown('W', true);

    if(!input.IsMouseMoved() ||
       input.GetMouseX() != 18 ||
       input.GetMouseY() != 25 ||
       input.GetMouseDeltaX() != 8 ||
       input.GetMouseDeltaY() != 5 ||
       !input.IsMouseButtonDown(InputState::MouseRight) ||
       !input.IsMouseButtonPressed(InputState::MouseRight) ||
       !input.IsKeyDown('W') ||
       !input.IsKeyPressed('W') ||
       !input.IsAnyKeyDown() ||
       !input.HasActivityThisFrame())
    {
        std::cout << "input state test Failed" << std::endl;
        return 1;
    }

    input.BeginFrame();

    if(input.IsMouseMoved() ||
       input.GetMouseDeltaX() != 0 ||
       input.GetMouseDeltaY() != 0 ||
       !input.IsMouseButtonDown(InputState::MouseRight) ||
       input.IsMouseButtonPressed(InputState::MouseRight) ||
       !input.IsKeyDown('W') ||
       input.IsKeyPressed('W') ||
       !input.IsAnyKeyDown() ||
       input.HasActivityThisFrame())
    {
        std::cout << "input state test Failed" << std::endl;
        return 1;
    }

    input.SetMouseButtonDown(InputState::MouseRight, false);
    input.SetKeyDown('W', false);

    if(input.IsMouseButtonDown(InputState::MouseRight) ||
       !input.IsMouseButtonReleased(InputState::MouseRight) ||
       input.IsKeyDown('W') ||
       !input.IsKeyReleased('W') ||
       input.IsAnyKeyDown() ||
       !input.HasActivityThisFrame())
    {
        std::cout << "input state test Failed" << std::endl;
        return 1;
    }

    std::cout << "input state test Pass" << std::endl;
    return 0;
}
