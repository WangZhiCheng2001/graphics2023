#pragma once

#include <SDL.h>

#include "input.h"

class SDLInputSystem : public InputSystem
{
public:
    SDLInputSystem()
    {
#ifdef INPUT_HAS_CAPTURE_AND_GLOBAL_MOUSE
        const char *sdl_backend = SDL_GetCurrentVideoDriver();
        std::array global_mouse_whitelist = {"windows", "cocoa", "x11", "DIVE", "VMAN"};
        for (int n = 0; n < global_mouse_whitelist.size(); n++)
            if (strncmp(sdl_backend, global_mouse_whitelist[n], strlen(global_mouse_whitelist[n])) == 0)
                m_mouseCanUseGlobalState = true;
#endif
        for (auto i = 0; i < m_mouseCursors.size(); ++i)
            m_mouseCursors[i] = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(mouseCursorTranslator(static_cast<eMouseCursor>(i))));
    }

    ~SDLInputSystem()
    {
        for (auto &cursor : m_mouseCursors)
            SDL_FreeCursor(static_cast<SDL_Cursor *>(cursor));
        m_lastMouseCursor = nullptr;
    }

    bool showCursor(bool show = true) override final
    {
        return SDL_ShowCursor(show);
    }

    bool keyDown(eKeyCode keyCode) override final
    {
        const uint8_t *state = SDL_GetKeyboardState(nullptr);
        switch (keyCode)
        {
        case KEY_CODE_Shift:
            return state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
        case KEY_CODE_Alt:
            return state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT];
        case KEY_CODE_Ctrl:
            return state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL];
        default:
            return state[keyCodeTranslator(keyCode)];
        }
    }

    bool mouseKeyDown(eMouseKeyCode mouseKeyCode) override final
    {
        int x, y;
        Uint32 button = m_mouseCanUseGlobalState ? SDL_GetGlobalMouseState(&x, &y) : SDL_GetMouseState(&x, &y);
        if (button & mouseKeyCodeTranslator(mouseKeyCode))
            return true;
        return false;
    }

    std::pair<uint32_t, uint32_t> getCursorPos(eCursorCoordinate coord) const override final
    {
        int x, y;
        if (coord == CURSOR_COORDINATE_WINDOW)
        {
            SDL_GetMouseState(&x, &y);
        }
        else if (coord == CURSOR_COORDINATE_SCREEN)
        {
            SDL_GetGlobalMouseState(&x, &y);
        }
        return {static_cast<uint32_t>(x), static_cast<uint32_t>(y)};
    }

    bool setCursorPos(const std::pair<uint32_t, uint32_t> &pos, void *windowHandle = nullptr) override final
    {
        SDL_WarpMouseInWindow(static_cast<SDL_Window *>(windowHandle), pos.first, pos.second);
        return true;
    }

    bool setCursorType(eMouseCursor cursor) override final
    {
        if (cursor == MOUSE_CURSOR_HIDE)
        {
            SDL_ShowCursor(SDL_FALSE);
            return true;
        }
        else
        {
            if (m_mouseCursors[cursor] == nullptr)
            {
                SDL_Cursor *sdlCursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(mouseCursorTranslator(cursor)));
                if (sdlCursor != nullptr)
                {
                    m_mouseCursors[cursor] = sdlCursor;
                    m_lastMouseCursor = m_mouseCursors[cursor];
                    SDL_SetCursor(sdlCursor);
                    SDL_ShowCursor(SDL_TRUE);
                    return true;
                }
                return false;
            }
            else
            {
                m_lastMouseCursor = m_mouseCursors[cursor];
                SDL_SetCursor(static_cast<SDL_Cursor *>(m_mouseCursors[cursor]));
                SDL_ShowCursor(SDL_TRUE);
                return true;
            }
            // SDL_Cursor *sdlCursor;
            // sdlCursor = SDL_GetCursor();
            // if (sdlCursor)
            //     SDL_FreeCursor(sdlCursor);
            // sdlCursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(mouseCursorTranslator(cursor)));
            // if (sdlCursor)
            // {
            //     SDL_SetCursor(sdlCursor);
            //     return true;
            // }
            // return false;
        }
    }

    static uint32_t keyCodeTranslator(eKeyCode keyCode)
    {
        switch (keyCode)
        {
            KEY_CODE_TRANS(KEY_CODE_Invalid, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS(KEY_CODE_Backspace, SDL_SCANCODE_BACKSPACE)
            KEY_CODE_TRANS(KEY_CODE_Tab, SDL_SCANCODE_TAB)
            KEY_CODE_TRANS(KEY_CODE_Clear, SDL_SCANCODE_CLEAR)
            KEY_CODE_TRANS(KEY_CODE_Enter, SDL_SCANCODE_RETURN)
            KEY_CODE_TRANS(KEY_CODE_NumpadEnter, SDL_SCANCODE_KP_ENTER)
            // combined
            KEY_CODE_TRANS(KEY_CODE_Shift, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS(KEY_CODE_Ctrl, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS(KEY_CODE_Alt, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS(KEY_CODE_Pause, SDL_SCANCODE_PAUSE)
            KEY_CODE_TRANS(KEY_CODE_CapsLock, SDL_SCANCODE_CAPSLOCK)
            KEY_CODE_TRANS(KEY_CODE_Esc, SDL_SCANCODE_ESCAPE)
            KEY_CODE_TRANS(KEY_CODE_SpaceBar, SDL_SCANCODE_SPACE)
            KEY_CODE_TRANS(KEY_CODE_PageUp, SDL_SCANCODE_PAGEUP)
            KEY_CODE_TRANS(KEY_CODE_PageDown, SDL_SCANCODE_PAGEDOWN)
            KEY_CODE_TRANS(KEY_CODE_End, SDL_SCANCODE_END)
            KEY_CODE_TRANS(KEY_CODE_Home, SDL_SCANCODE_HOME)
            KEY_CODE_TRANS(KEY_CODE_Left, SDL_SCANCODE_LEFT)
            KEY_CODE_TRANS(KEY_CODE_Up, SDL_SCANCODE_UP)
            KEY_CODE_TRANS(KEY_CODE_Right, SDL_SCANCODE_RIGHT)
            KEY_CODE_TRANS(KEY_CODE_Down, SDL_SCANCODE_DOWN)
            KEY_CODE_TRANS(KEY_CODE_Select, SDL_SCANCODE_SELECT)
            KEY_CODE_TRANS(KEY_CODE_Print, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS(KEY_CODE_Execute, SDL_SCANCODE_EXECUTE)
            KEY_CODE_TRANS(KEY_CODE_Printscreen, SDL_SCANCODE_PRINTSCREEN)
            KEY_CODE_TRANS(KEY_CODE_Insert, SDL_SCANCODE_INSERT)
            KEY_CODE_TRANS(KEY_CODE_Del, SDL_SCANCODE_DELETE)
            KEY_CODE_TRANS(KEY_CODE_Help, SDL_SCANCODE_HELP)
            KEY_CODE_TRANS(KEY_CODE_0, SDL_SCANCODE_0)
            KEY_CODE_TRANS(KEY_CODE_1, SDL_SCANCODE_1)
            KEY_CODE_TRANS(KEY_CODE_2, SDL_SCANCODE_2)
            KEY_CODE_TRANS(KEY_CODE_3, SDL_SCANCODE_3)
            KEY_CODE_TRANS(KEY_CODE_4, SDL_SCANCODE_4)
            KEY_CODE_TRANS(KEY_CODE_5, SDL_SCANCODE_5)
            KEY_CODE_TRANS(KEY_CODE_6, SDL_SCANCODE_6)
            KEY_CODE_TRANS(KEY_CODE_7, SDL_SCANCODE_7)
            KEY_CODE_TRANS(KEY_CODE_8, SDL_SCANCODE_8)
            KEY_CODE_TRANS(KEY_CODE_9, SDL_SCANCODE_9)
            KEY_CODE_TRANS(KEY_CODE_A, SDL_SCANCODE_A)
            KEY_CODE_TRANS(KEY_CODE_B, SDL_SCANCODE_B)
            KEY_CODE_TRANS(KEY_CODE_C, SDL_SCANCODE_C)
            KEY_CODE_TRANS(KEY_CODE_D, SDL_SCANCODE_D)
            KEY_CODE_TRANS(KEY_CODE_E, SDL_SCANCODE_E)
            KEY_CODE_TRANS(KEY_CODE_F, SDL_SCANCODE_F)
            KEY_CODE_TRANS(KEY_CODE_G, SDL_SCANCODE_G)
            KEY_CODE_TRANS(KEY_CODE_H, SDL_SCANCODE_H)
            KEY_CODE_TRANS(KEY_CODE_I, SDL_SCANCODE_I)
            KEY_CODE_TRANS(KEY_CODE_J, SDL_SCANCODE_J)
            KEY_CODE_TRANS(KEY_CODE_K, SDL_SCANCODE_K)
            KEY_CODE_TRANS(KEY_CODE_L, SDL_SCANCODE_L)
            KEY_CODE_TRANS(KEY_CODE_M, SDL_SCANCODE_M)
            KEY_CODE_TRANS(KEY_CODE_N, SDL_SCANCODE_N)
            KEY_CODE_TRANS(KEY_CODE_O, SDL_SCANCODE_O)
            KEY_CODE_TRANS(KEY_CODE_P, SDL_SCANCODE_P)
            KEY_CODE_TRANS(KEY_CODE_Q, SDL_SCANCODE_Q)
            KEY_CODE_TRANS(KEY_CODE_R, SDL_SCANCODE_R)
            KEY_CODE_TRANS(KEY_CODE_S, SDL_SCANCODE_S)
            KEY_CODE_TRANS(KEY_CODE_T, SDL_SCANCODE_T)
            KEY_CODE_TRANS(KEY_CODE_U, SDL_SCANCODE_U)
            KEY_CODE_TRANS(KEY_CODE_V, SDL_SCANCODE_V)
            KEY_CODE_TRANS(KEY_CODE_W, SDL_SCANCODE_W)
            KEY_CODE_TRANS(KEY_CODE_X, SDL_SCANCODE_X)
            KEY_CODE_TRANS(KEY_CODE_Y, SDL_SCANCODE_Y)
            KEY_CODE_TRANS(KEY_CODE_Z, SDL_SCANCODE_Z)
            KEY_CODE_TRANS(KEY_CODE_LSystem, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS(KEY_CODE_RSystem, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS(KEY_CODE_App, SDL_SCANCODE_APPLICATION)
            KEY_CODE_TRANS(KEY_CODE_Sleep, SDL_SCANCODE_SLEEP)
            KEY_CODE_TRANS(KEY_CODE_Numpad0, SDL_SCANCODE_KP_0)
            KEY_CODE_TRANS(KEY_CODE_Numpad1, SDL_SCANCODE_KP_1)
            KEY_CODE_TRANS(KEY_CODE_Numpad2, SDL_SCANCODE_KP_2)
            KEY_CODE_TRANS(KEY_CODE_Numpad3, SDL_SCANCODE_KP_3)
            KEY_CODE_TRANS(KEY_CODE_Numpad4, SDL_SCANCODE_KP_4)
            KEY_CODE_TRANS(KEY_CODE_Numpad5, SDL_SCANCODE_KP_5)
            KEY_CODE_TRANS(KEY_CODE_Numpad6, SDL_SCANCODE_KP_6)
            KEY_CODE_TRANS(KEY_CODE_Numpad7, SDL_SCANCODE_KP_7)
            KEY_CODE_TRANS(KEY_CODE_Numpad8, SDL_SCANCODE_KP_8)
            KEY_CODE_TRANS(KEY_CODE_Numpad9, SDL_SCANCODE_KP_9)
            KEY_CODE_TRANS(KEY_CODE_Multiply, SDL_SCANCODE_KP_MULTIPLY)
            KEY_CODE_TRANS(KEY_CODE_Add, SDL_SCANCODE_KP_PLUS)
            KEY_CODE_TRANS(KEY_CODE_Separator, SDL_SCANCODE_SEPARATOR)
            KEY_CODE_TRANS(KEY_CODE_Subtract, SDL_SCANCODE_KP_MINUS)
            KEY_CODE_TRANS(KEY_CODE_Decimal, SDL_SCANCODE_KP_DECIMAL)
            KEY_CODE_TRANS(KEY_CODE_Divide, SDL_SCANCODE_KP_DIVIDE)
            KEY_CODE_TRANS(KEY_CODE_F1, SDL_SCANCODE_F1)
            KEY_CODE_TRANS(KEY_CODE_F2, SDL_SCANCODE_F2)
            KEY_CODE_TRANS(KEY_CODE_F3, SDL_SCANCODE_F3)
            KEY_CODE_TRANS(KEY_CODE_F4, SDL_SCANCODE_F4)
            KEY_CODE_TRANS(KEY_CODE_F5, SDL_SCANCODE_F5)
            KEY_CODE_TRANS(KEY_CODE_F6, SDL_SCANCODE_F6)
            KEY_CODE_TRANS(KEY_CODE_F7, SDL_SCANCODE_F7)
            KEY_CODE_TRANS(KEY_CODE_F8, SDL_SCANCODE_F8)
            KEY_CODE_TRANS(KEY_CODE_F9, SDL_SCANCODE_F9)
            KEY_CODE_TRANS(KEY_CODE_F10, SDL_SCANCODE_F10)
            KEY_CODE_TRANS(KEY_CODE_F11, SDL_SCANCODE_F11)
            KEY_CODE_TRANS(KEY_CODE_F12, SDL_SCANCODE_F12)
            KEY_CODE_TRANS(KEY_CODE_F13, SDL_SCANCODE_F13)
            KEY_CODE_TRANS(KEY_CODE_F14, SDL_SCANCODE_F14)
            KEY_CODE_TRANS(KEY_CODE_F15, SDL_SCANCODE_F15)
            KEY_CODE_TRANS(KEY_CODE_F16, SDL_SCANCODE_F16)
            KEY_CODE_TRANS(KEY_CODE_F17, SDL_SCANCODE_F17)
            KEY_CODE_TRANS(KEY_CODE_F18, SDL_SCANCODE_F18)
            KEY_CODE_TRANS(KEY_CODE_F19, SDL_SCANCODE_F19)
            KEY_CODE_TRANS(KEY_CODE_F20, SDL_SCANCODE_F20)
            KEY_CODE_TRANS(KEY_CODE_F21, SDL_SCANCODE_F21)
            KEY_CODE_TRANS(KEY_CODE_F22, SDL_SCANCODE_F22)
            KEY_CODE_TRANS(KEY_CODE_F23, SDL_SCANCODE_F23)
            KEY_CODE_TRANS(KEY_CODE_F24, SDL_SCANCODE_F24)
            KEY_CODE_TRANS(KEY_CODE_Numlock, SDL_SCANCODE_NUMLOCKCLEAR)
            KEY_CODE_TRANS(KEY_CODE_Scrolllock, SDL_SCANCODE_SCROLLLOCK)

            KEY_CODE_TRANS(KEY_CODE_LShift, SDL_SCANCODE_LSHIFT)
            KEY_CODE_TRANS(KEY_CODE_RShift, SDL_SCANCODE_RSHIFT)
            KEY_CODE_TRANS(KEY_CODE_LCtrl, SDL_SCANCODE_LCTRL)
            KEY_CODE_TRANS(KEY_CODE_RCtrl, SDL_SCANCODE_RCTRL)
            KEY_CODE_TRANS(KEY_CODE_LAlt, SDL_SCANCODE_LALT)
            KEY_CODE_TRANS(KEY_CODE_RAlt, SDL_SCANCODE_RALT)
            KEY_CODE_TRANS(KEY_CODE_LGui, SDL_SCANCODE_LGUI)
            KEY_CODE_TRANS(KEY_CODE_RGui, SDL_SCANCODE_RGUI)

            KEY_CODE_TRANS(KEY_CODE_Semicolon, SDL_SCANCODE_SEMICOLON)
            KEY_CODE_TRANS(KEY_CODE_Plus, SDL_SCANCODE_EQUALS)
            KEY_CODE_TRANS(KEY_CODE_NumpadEqual, SDL_SCANCODE_KP_EQUALS)
            KEY_CODE_TRANS(KEY_CODE_Comma, SDL_SCANCODE_COMMA)
            KEY_CODE_TRANS(KEY_CODE_Minus, SDL_SCANCODE_MINUS)
            KEY_CODE_TRANS(KEY_CODE_Dot, SDL_SCANCODE_PERIOD)
            KEY_CODE_TRANS(KEY_CODE_Slash, SDL_SCANCODE_SLASH)
            KEY_CODE_TRANS(KEY_CODE_Wave, SDL_SCANCODE_GRAVE)
            KEY_CODE_TRANS(KEY_CODE_LBracket, SDL_SCANCODE_LEFTBRACKET)
            KEY_CODE_TRANS(KEY_CODE_Backslash, SDL_SCANCODE_BACKSLASH)
            KEY_CODE_TRANS(KEY_CODE_RBracket, SDL_SCANCODE_RIGHTBRACKET)
            KEY_CODE_TRANS(KEY_CODE_Quote, SDL_SCANCODE_APOSTROPHE)

        default:
            return SDL_SCANCODE_UNKNOWN;
        }
    }

    static eKeyCode keyCodeTranslatorBack(uint32_t keyCode)
    {
        switch (keyCode)
        {
            KEY_CODE_TRANS_BACK(KEY_CODE_Invalid, SDL_SCANCODE_UNKNOWN)
            KEY_CODE_TRANS_BACK(KEY_CODE_Backspace, SDL_SCANCODE_BACKSPACE)
            KEY_CODE_TRANS_BACK(KEY_CODE_Tab, SDL_SCANCODE_TAB)
            KEY_CODE_TRANS_BACK(KEY_CODE_Clear, SDL_SCANCODE_CLEAR)
            KEY_CODE_TRANS_BACK(KEY_CODE_Enter, SDL_SCANCODE_RETURN)
            KEY_CODE_TRANS_BACK(KEY_CODE_NumpadEnter, SDL_SCANCODE_KP_ENTER)
            // combined
            KEY_CODE_TRANS_BACK(KEY_CODE_Pause, SDL_SCANCODE_PAUSE)
            KEY_CODE_TRANS_BACK(KEY_CODE_CapsLock, SDL_SCANCODE_CAPSLOCK)
            KEY_CODE_TRANS_BACK(KEY_CODE_Esc, SDL_SCANCODE_ESCAPE)
            KEY_CODE_TRANS_BACK(KEY_CODE_SpaceBar, SDL_SCANCODE_SPACE)
            KEY_CODE_TRANS_BACK(KEY_CODE_PageUp, SDL_SCANCODE_PAGEUP)
            KEY_CODE_TRANS_BACK(KEY_CODE_PageDown, SDL_SCANCODE_PAGEDOWN)
            KEY_CODE_TRANS_BACK(KEY_CODE_End, SDL_SCANCODE_END)
            KEY_CODE_TRANS_BACK(KEY_CODE_Home, SDL_SCANCODE_HOME)
            KEY_CODE_TRANS_BACK(KEY_CODE_Left, SDL_SCANCODE_LEFT)
            KEY_CODE_TRANS_BACK(KEY_CODE_Up, SDL_SCANCODE_UP)
            KEY_CODE_TRANS_BACK(KEY_CODE_Right, SDL_SCANCODE_RIGHT)
            KEY_CODE_TRANS_BACK(KEY_CODE_Down, SDL_SCANCODE_DOWN)
            KEY_CODE_TRANS_BACK(KEY_CODE_Select, SDL_SCANCODE_SELECT)
            KEY_CODE_TRANS_BACK(KEY_CODE_Execute, SDL_SCANCODE_EXECUTE)
            KEY_CODE_TRANS_BACK(KEY_CODE_Printscreen, SDL_SCANCODE_PRINTSCREEN)
            KEY_CODE_TRANS_BACK(KEY_CODE_Insert, SDL_SCANCODE_INSERT)
            KEY_CODE_TRANS_BACK(KEY_CODE_Del, SDL_SCANCODE_DELETE)
            KEY_CODE_TRANS_BACK(KEY_CODE_Help, SDL_SCANCODE_HELP)
            KEY_CODE_TRANS_BACK(KEY_CODE_0, SDL_SCANCODE_0)
            KEY_CODE_TRANS_BACK(KEY_CODE_1, SDL_SCANCODE_1)
            KEY_CODE_TRANS_BACK(KEY_CODE_2, SDL_SCANCODE_2)
            KEY_CODE_TRANS_BACK(KEY_CODE_3, SDL_SCANCODE_3)
            KEY_CODE_TRANS_BACK(KEY_CODE_4, SDL_SCANCODE_4)
            KEY_CODE_TRANS_BACK(KEY_CODE_5, SDL_SCANCODE_5)
            KEY_CODE_TRANS_BACK(KEY_CODE_6, SDL_SCANCODE_6)
            KEY_CODE_TRANS_BACK(KEY_CODE_7, SDL_SCANCODE_7)
            KEY_CODE_TRANS_BACK(KEY_CODE_8, SDL_SCANCODE_8)
            KEY_CODE_TRANS_BACK(KEY_CODE_9, SDL_SCANCODE_9)
            KEY_CODE_TRANS_BACK(KEY_CODE_A, SDL_SCANCODE_A)
            KEY_CODE_TRANS_BACK(KEY_CODE_B, SDL_SCANCODE_B)
            KEY_CODE_TRANS_BACK(KEY_CODE_C, SDL_SCANCODE_C)
            KEY_CODE_TRANS_BACK(KEY_CODE_D, SDL_SCANCODE_D)
            KEY_CODE_TRANS_BACK(KEY_CODE_E, SDL_SCANCODE_E)
            KEY_CODE_TRANS_BACK(KEY_CODE_F, SDL_SCANCODE_F)
            KEY_CODE_TRANS_BACK(KEY_CODE_G, SDL_SCANCODE_G)
            KEY_CODE_TRANS_BACK(KEY_CODE_H, SDL_SCANCODE_H)
            KEY_CODE_TRANS_BACK(KEY_CODE_I, SDL_SCANCODE_I)
            KEY_CODE_TRANS_BACK(KEY_CODE_J, SDL_SCANCODE_J)
            KEY_CODE_TRANS_BACK(KEY_CODE_K, SDL_SCANCODE_K)
            KEY_CODE_TRANS_BACK(KEY_CODE_L, SDL_SCANCODE_L)
            KEY_CODE_TRANS_BACK(KEY_CODE_M, SDL_SCANCODE_M)
            KEY_CODE_TRANS_BACK(KEY_CODE_N, SDL_SCANCODE_N)
            KEY_CODE_TRANS_BACK(KEY_CODE_O, SDL_SCANCODE_O)
            KEY_CODE_TRANS_BACK(KEY_CODE_P, SDL_SCANCODE_P)
            KEY_CODE_TRANS_BACK(KEY_CODE_Q, SDL_SCANCODE_Q)
            KEY_CODE_TRANS_BACK(KEY_CODE_R, SDL_SCANCODE_R)
            KEY_CODE_TRANS_BACK(KEY_CODE_S, SDL_SCANCODE_S)
            KEY_CODE_TRANS_BACK(KEY_CODE_T, SDL_SCANCODE_T)
            KEY_CODE_TRANS_BACK(KEY_CODE_U, SDL_SCANCODE_U)
            KEY_CODE_TRANS_BACK(KEY_CODE_V, SDL_SCANCODE_V)
            KEY_CODE_TRANS_BACK(KEY_CODE_W, SDL_SCANCODE_W)
            KEY_CODE_TRANS_BACK(KEY_CODE_X, SDL_SCANCODE_X)
            KEY_CODE_TRANS_BACK(KEY_CODE_Y, SDL_SCANCODE_Y)
            KEY_CODE_TRANS_BACK(KEY_CODE_Z, SDL_SCANCODE_Z)
            KEY_CODE_TRANS_BACK(KEY_CODE_App, SDL_SCANCODE_APPLICATION)
            KEY_CODE_TRANS_BACK(KEY_CODE_Sleep, SDL_SCANCODE_SLEEP)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad0, SDL_SCANCODE_KP_0)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad1, SDL_SCANCODE_KP_1)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad2, SDL_SCANCODE_KP_2)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad3, SDL_SCANCODE_KP_3)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad4, SDL_SCANCODE_KP_4)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad5, SDL_SCANCODE_KP_5)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad6, SDL_SCANCODE_KP_6)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad7, SDL_SCANCODE_KP_7)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad8, SDL_SCANCODE_KP_8)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numpad9, SDL_SCANCODE_KP_9)
            KEY_CODE_TRANS_BACK(KEY_CODE_Multiply, SDL_SCANCODE_KP_MULTIPLY)
            KEY_CODE_TRANS_BACK(KEY_CODE_Add, SDL_SCANCODE_KP_PLUS)
            KEY_CODE_TRANS_BACK(KEY_CODE_Separator, SDL_SCANCODE_SEPARATOR)
            KEY_CODE_TRANS_BACK(KEY_CODE_Subtract, SDL_SCANCODE_KP_MINUS)
            KEY_CODE_TRANS_BACK(KEY_CODE_Decimal, SDL_SCANCODE_KP_DECIMAL)
            KEY_CODE_TRANS_BACK(KEY_CODE_Divide, SDL_SCANCODE_KP_DIVIDE)
            KEY_CODE_TRANS_BACK(KEY_CODE_F1, SDL_SCANCODE_F1)
            KEY_CODE_TRANS_BACK(KEY_CODE_F2, SDL_SCANCODE_F2)
            KEY_CODE_TRANS_BACK(KEY_CODE_F3, SDL_SCANCODE_F3)
            KEY_CODE_TRANS_BACK(KEY_CODE_F4, SDL_SCANCODE_F4)
            KEY_CODE_TRANS_BACK(KEY_CODE_F5, SDL_SCANCODE_F5)
            KEY_CODE_TRANS_BACK(KEY_CODE_F6, SDL_SCANCODE_F6)
            KEY_CODE_TRANS_BACK(KEY_CODE_F7, SDL_SCANCODE_F7)
            KEY_CODE_TRANS_BACK(KEY_CODE_F8, SDL_SCANCODE_F8)
            KEY_CODE_TRANS_BACK(KEY_CODE_F9, SDL_SCANCODE_F9)
            KEY_CODE_TRANS_BACK(KEY_CODE_F10, SDL_SCANCODE_F10)
            KEY_CODE_TRANS_BACK(KEY_CODE_F11, SDL_SCANCODE_F11)
            KEY_CODE_TRANS_BACK(KEY_CODE_F12, SDL_SCANCODE_F12)
            KEY_CODE_TRANS_BACK(KEY_CODE_F13, SDL_SCANCODE_F13)
            KEY_CODE_TRANS_BACK(KEY_CODE_F14, SDL_SCANCODE_F14)
            KEY_CODE_TRANS_BACK(KEY_CODE_F15, SDL_SCANCODE_F15)
            KEY_CODE_TRANS_BACK(KEY_CODE_F16, SDL_SCANCODE_F16)
            KEY_CODE_TRANS_BACK(KEY_CODE_F17, SDL_SCANCODE_F17)
            KEY_CODE_TRANS_BACK(KEY_CODE_F18, SDL_SCANCODE_F18)
            KEY_CODE_TRANS_BACK(KEY_CODE_F19, SDL_SCANCODE_F19)
            KEY_CODE_TRANS_BACK(KEY_CODE_F20, SDL_SCANCODE_F20)
            KEY_CODE_TRANS_BACK(KEY_CODE_F21, SDL_SCANCODE_F21)
            KEY_CODE_TRANS_BACK(KEY_CODE_F22, SDL_SCANCODE_F22)
            KEY_CODE_TRANS_BACK(KEY_CODE_F23, SDL_SCANCODE_F23)
            KEY_CODE_TRANS_BACK(KEY_CODE_F24, SDL_SCANCODE_F24)
            KEY_CODE_TRANS_BACK(KEY_CODE_Numlock, SDL_SCANCODE_NUMLOCKCLEAR)
            KEY_CODE_TRANS_BACK(KEY_CODE_Scrolllock, SDL_SCANCODE_SCROLLLOCK)

            KEY_CODE_TRANS_BACK(KEY_CODE_LShift, SDL_SCANCODE_LSHIFT)
            KEY_CODE_TRANS_BACK(KEY_CODE_RShift, SDL_SCANCODE_RSHIFT)
            KEY_CODE_TRANS_BACK(KEY_CODE_LCtrl, SDL_SCANCODE_LCTRL)
            KEY_CODE_TRANS_BACK(KEY_CODE_RCtrl, SDL_SCANCODE_RCTRL)
            KEY_CODE_TRANS_BACK(KEY_CODE_LAlt, SDL_SCANCODE_LALT)
            KEY_CODE_TRANS_BACK(KEY_CODE_RAlt, SDL_SCANCODE_RALT)
            KEY_CODE_TRANS_BACK(KEY_CODE_LGui, SDL_SCANCODE_LGUI)
            KEY_CODE_TRANS_BACK(KEY_CODE_RGui, SDL_SCANCODE_RGUI)

            KEY_CODE_TRANS_BACK(KEY_CODE_Semicolon, SDL_SCANCODE_SEMICOLON)
            KEY_CODE_TRANS_BACK(KEY_CODE_Plus, SDL_SCANCODE_EQUALS)
            KEY_CODE_TRANS_BACK(KEY_CODE_NumpadEqual, SDL_SCANCODE_KP_EQUALS)
            KEY_CODE_TRANS_BACK(KEY_CODE_Comma, SDL_SCANCODE_COMMA)
            KEY_CODE_TRANS_BACK(KEY_CODE_Minus, SDL_SCANCODE_MINUS)
            KEY_CODE_TRANS_BACK(KEY_CODE_Dot, SDL_SCANCODE_PERIOD)
            KEY_CODE_TRANS_BACK(KEY_CODE_Slash, SDL_SCANCODE_SLASH)
            KEY_CODE_TRANS_BACK(KEY_CODE_Wave, SDL_SCANCODE_GRAVE)
            KEY_CODE_TRANS_BACK(KEY_CODE_LBracket, SDL_SCANCODE_LEFTBRACKET)
            KEY_CODE_TRANS_BACK(KEY_CODE_Backslash, SDL_SCANCODE_BACKSLASH)
            KEY_CODE_TRANS_BACK(KEY_CODE_RBracket, SDL_SCANCODE_RIGHTBRACKET)
            KEY_CODE_TRANS_BACK(KEY_CODE_Quote, SDL_SCANCODE_APOSTROPHE)

        default:
            return KEY_CODE_Invalid;
        }
    }

    static uint32_t mouseKeyCodeTranslator(eMouseKeyCode mouseKeyCode)
    {
        switch (mouseKeyCode)
        {
            KEY_CODE_TRANS(MOUSE_KEY_CODE_NONE, 0)
            KEY_CODE_TRANS(MOUSE_KEY_CODE_LB, SDL_BUTTON_LMASK)
            KEY_CODE_TRANS(MOUSE_KEY_CODE_RB, SDL_BUTTON_RMASK)
            KEY_CODE_TRANS(MOUSE_KEY_CODE_MB, SDL_BUTTON_MMASK)
            KEY_CODE_TRANS(MOUSE_KEY_CODE_X1B, SDL_BUTTON_X1MASK)
            KEY_CODE_TRANS(MOUSE_KEY_CODE_X2B, SDL_BUTTON_X2MASK)
        default:
            return 0;
        }
    }

    static eMouseKeyCode mouseKeyCodeTranslatorBack(uint32_t mouseKeyCode)
    {
        switch (mouseKeyCode)
        {
            KEY_CODE_TRANS(SDL_BUTTON_LMASK, MOUSE_KEY_CODE_LB)
            KEY_CODE_TRANS(SDL_BUTTON_RMASK, MOUSE_KEY_CODE_RB)
            KEY_CODE_TRANS(SDL_BUTTON_MMASK, MOUSE_KEY_CODE_MB)
            KEY_CODE_TRANS(SDL_BUTTON_X1MASK, MOUSE_KEY_CODE_X1B)
            KEY_CODE_TRANS(SDL_BUTTON_X2MASK, MOUSE_KEY_CODE_X2B)
        default:
            return MOUSE_KEY_CODE_NONE;
        }
    }

    static uint32_t mouseCursorTranslator(eMouseCursor cursor)
    {
        switch (cursor)
        {
            KEY_CODE_TRANS(MOUSE_CURSOR_ARROW, SDL_SYSTEM_CURSOR_ARROW)
            KEY_CODE_TRANS(MOUSE_CURSOR_TEXT_INPUT, SDL_SYSTEM_CURSOR_IBEAM)
            KEY_CODE_TRANS(MOUSE_CURSOR_RESIZE_ALL, SDL_SYSTEM_CURSOR_SIZEALL)
            KEY_CODE_TRANS(MOUSE_CURSOR_RESIZE_EW, SDL_SYSTEM_CURSOR_SIZEWE)
            KEY_CODE_TRANS(MOUSE_CURSOR_RESIZE_NS, SDL_SYSTEM_CURSOR_SIZENS)
            KEY_CODE_TRANS(MOUSE_CURSOR_RESIZE_NESW, SDL_SYSTEM_CURSOR_SIZENESW)
            KEY_CODE_TRANS(MOUSE_CURSOR_RESIZE_NWSE, SDL_SYSTEM_CURSOR_SIZENWSE)
            KEY_CODE_TRANS(MOUSE_CURSOR_HAND, SDL_SYSTEM_CURSOR_HAND)
            KEY_CODE_TRANS(MOUSE_CURSOR_NOT_ALLOWED, SDL_SYSTEM_CURSOR_NO)
            KEY_CODE_TRANS(MOUSE_CURSOR_WAIT, SDL_SYSTEM_CURSOR_WAIT)
            KEY_CODE_TRANS(MOUSE_CURSOR_WAIT_ARROW, SDL_SYSTEM_CURSOR_WAITARROW)
        default:
            return 0;
        }
    }
};