#ifndef _RENDER_EVENT_H_
#define _RENDER_EVENT_H_

#include <cstdint>
#include <string>

class Event
{
public:
    Event() = default;
    virtual ~Event() = default;

    virtual std::string Name() = 0;
};

//=========================================键盘

class KeyEvent : public Event
{
public:
    KeyEvent(uint32_t key)
    {
        keycode = key;
    }

    virtual std::string Name() override { return "KeyEvent";}

    uint32_t keycode;
};

class KeyDownEvent : public KeyEvent
{
public:
    KeyDownEvent(uint32_t key)
        : KeyEvent(key)
    {
        
    }

    virtual std::string Name() override { return "KeyDownEvent";}
};

class KeyUpEvent : public KeyEvent
{
public:
    KeyUpEvent(uint32_t key)
        : KeyEvent(key)
    {
        
    }

    virtual std::string Name() override { return "KeyUpEvent";}
};

//===============================鼠标

class MouseEvent : public Event
{
public:
    //0 左 1 中 2 右
    MouseEvent(int x, int y)
    {
        posx = x;
        posy = y;
    }

    virtual std::string Name() override { return "MouseEvent";}

    int posx;
    int posy;
};

class MouseMoveEvent : public MouseEvent
{
public:
    //0 左 1 中 2 右
    MouseMoveEvent(int x, int y)
        : MouseEvent(x, y)
    {
        
    }

    virtual std::string Name() override { return "MouseMoveEvent";}
};

class MouseButtonEvent : public MouseEvent
{
public:
    //0 左 1 中 2 右
    MouseButtonEvent(int x, int y, uint32_t btn)
        : MouseEvent(x, y)
    {
        button = btn;
    }

    virtual std::string Name() override { return "MouseButtonEvent";}
    
    uint32_t button;
};

class MouseDownEvent : public MouseButtonEvent
{
public:
    MouseDownEvent(int x, int y, uint32_t button)
        : MouseButtonEvent(x, y, button)
    {
        
    }

    virtual std::string Name() override { return "MouseDownEvent";}
};

class MouseUpEvent : public MouseButtonEvent
{
public:
    MouseUpEvent(int x, int y, uint32_t button)
        : MouseButtonEvent(x, y, button)
    {
        
    }

    virtual std::string Name() override { return "MouseUpEvent";}
};

//==============================



#endif