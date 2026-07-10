#ifndef _RENDER_LAYER_INPUT_STATE_H_
#define _RENDER_LAYER_INPUT_STATE_H_

#include <array>
#include <cstdint>

class InputState
{
public:
    static constexpr uint32_t kMaxKeyCode = 256;
    static constexpr uint32_t kMouseButtonCount = 3;

    enum MouseButton : uint32_t
    {
        MouseLeft = 0,
        MouseMiddle = 1,
        MouseRight = 2
    };

    void BeginFrame()
    {
        mouse_delta_x_ = 0;
        mouse_delta_y_ = 0;
        mouse_moved_ = false;
        has_activity_this_frame_ = false;
        key_pressed_.fill(false);
        key_released_.fill(false);
        mouse_pressed_.fill(false);
        mouse_released_.fill(false);
    }

    void ResetHeldState()
    {
        key_down_.fill(false);
        key_pressed_.fill(false);
        key_released_.fill(false);
        mouse_down_.fill(false);
        mouse_pressed_.fill(false);
        mouse_released_.fill(false);
    }

    void SetKeyDown(uint32_t key, bool is_down)
    {
        if(key >= kMaxKeyCode)
        {
            return;
        }

        if(key_down_[key] != is_down)
        {
            key_pressed_[key] = is_down;
            key_released_[key] = !is_down;
            has_activity_this_frame_ = true;
        }

        key_down_[key] = is_down;
    }

    bool IsKeyDown(uint32_t key) const
    {
        return key < kMaxKeyCode && key_down_[key];
    }

    bool IsKeyPressed(uint32_t key) const
    {
        return key < kMaxKeyCode && key_pressed_[key];
    }

    bool IsKeyReleased(uint32_t key) const
    {
        return key < kMaxKeyCode && key_released_[key];
    }

    bool IsAnyKeyDown() const
    {
        for(bool is_down : key_down_)
        {
            if(is_down)
            {
                return true;
            }
        }

        return false;
    }

    void SetMousePosition(int32_t x, int32_t y)
    {
        if(has_mouse_position_)
        {
            mouse_delta_x_ += x - mouse_x_;
            mouse_delta_y_ += y - mouse_y_;
        }

        mouse_x_ = x;
        mouse_y_ = y;
        has_mouse_position_ = true;
        mouse_moved_ = true;
        has_activity_this_frame_ = true;
    }

    int32_t GetMouseX() const { return mouse_x_; }
    int32_t GetMouseY() const { return mouse_y_; }
    int32_t GetMouseDeltaX() const { return mouse_delta_x_; }
    int32_t GetMouseDeltaY() const { return mouse_delta_y_; }
    bool HasMousePosition() const { return has_mouse_position_; }
    bool IsMouseMoved() const { return mouse_moved_; }

    void SetMouseButtonDown(uint32_t button, bool is_down)
    {
        if(button >= kMouseButtonCount)
        {
            return;
        }

        if(mouse_down_[button] != is_down)
        {
            mouse_pressed_[button] = is_down;
            mouse_released_[button] = !is_down;
            has_activity_this_frame_ = true;
        }

        mouse_down_[button] = is_down;
    }

    bool IsMouseButtonDown(uint32_t button) const
    {
        return button < kMouseButtonCount && mouse_down_[button];
    }

    bool IsMouseButtonPressed(uint32_t button) const
    {
        return button < kMouseButtonCount && mouse_pressed_[button];
    }

    bool IsMouseButtonReleased(uint32_t button) const
    {
        return button < kMouseButtonCount && mouse_released_[button];
    }

    bool HasActivityThisFrame() const { return has_activity_this_frame_; }

private:
    std::array<bool, kMaxKeyCode> key_down_ {};
    std::array<bool, kMaxKeyCode> key_pressed_ {};
    std::array<bool, kMaxKeyCode> key_released_ {};

    std::array<bool, kMouseButtonCount> mouse_down_ {};
    std::array<bool, kMouseButtonCount> mouse_pressed_ {};
    std::array<bool, kMouseButtonCount> mouse_released_ {};

    int32_t mouse_x_ { 0 };
    int32_t mouse_y_ { 0 };
    int32_t mouse_delta_x_ { 0 };
    int32_t mouse_delta_y_ { 0 };
    bool has_mouse_position_ { false };
    bool mouse_moved_ { false };
    bool has_activity_this_frame_ { false };
};

#endif
