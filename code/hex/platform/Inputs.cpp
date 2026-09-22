#include "Inputs.h"

#include "win.h"

namespace platform
{
    Inputs* Inputs::sActiveInstance = nullptr;

    namespace
    {
        constexpr float kLeftStickDeadZone = 0.2395f;
        constexpr float kRightStickDeadZone = 0.2652f;
        constexpr float kMouseSensitivity = 0.0018f;
    }

    Inputs::Inputs(GLFWwindow* windowHandle)
        : mWindowHandle(windowHandle)
        , mLastMouseX(0.0)
        , mLastMouseY(0.0)
        , mScrollAccumulator(0.0f)
    {
        std::memset(mButtons.data(), 0, sizeof(mButtons));
        std::memset(mHoldDuration.data(), 0, sizeof(mHoldDuration));
        std::memset(mAnalogs.data(), 0, sizeof(mAnalogs));
        std::memset(mAnalogsTC.data(), 0, sizeof(mAnalogsTC));

        buildKeyMapping();
        glfwGetCursorPos(mWindowHandle, &mLastMouseX, &mLastMouseY);
        registerCallbacks();
    }

    Inputs::~Inputs()
    {
        unregisterCallbacks();
    }

    bool Inputs::isAnyPressed() const
    {
        for (size_t i = 0; i < NUM_DIGITAL_INPUTS; ++i)
        {
            if (mButtons[0][i])
            {
                return true;
            }
        }
        return false;
    }

    bool Inputs::isPressed(Digital di) const
    {
        return mButtons[0][static_cast<size_t>(enumCast(di))];
    }

    bool Inputs::isFirstPressed(Digital di) const
    {
        size_t idx = static_cast<size_t>(enumCast(di));
        return mButtons[0][idx] && !mButtons[1][idx];
    }

    bool Inputs::isReleased(Digital di) const
    {
        return !mButtons[0][static_cast<size_t>(enumCast(di))];
    }

    bool Inputs::isFirstReleased(Digital di) const
    {
        size_t idx = static_cast<size_t>(enumCast(di));
        return !mButtons[0][idx] && mButtons[1][idx];
    }

    float Inputs::getDurationPressed(Digital di) const
    {
        return mHoldDuration[static_cast<size_t>(enumCast(di))];
    }

    float Inputs::getAnalogInput(Analog ai) const
    {
        return mAnalogs[static_cast<size_t>(enumCast(ai))];
    }

    float Inputs::getTimeCorrectedAnalogInput(Analog ai) const
    {
        return mAnalogsTC[static_cast<size_t>(enumCast(ai))];
    }

    void Inputs::update(float frameDelta)
    {
        std::memcpy(mButtons[1].data(), mButtons[0].data(), sizeof(mButtons[0]));
        std::memset(mButtons[0].data(), 0, sizeof(mButtons[0]));
        std::memset(mAnalogs.data(), 0, sizeof(mAnalogs));

        pollGamepad();
        pollKeyboardAndMouse();

        for (size_t i = 0; i < NUM_DIGITAL_INPUTS; ++i)
        {
            if (mButtons[0][i])
            {
                mHoldDuration[i] = mButtons[1][i] ? (mHoldDuration[i] + frameDelta) : 0.0f;
            }
            else
            {
                mHoldDuration[i] = 0.0f;
            }
        }

        for (size_t i = 0; i < NUM_ANALOG_INPUTS; ++i)
        {
            mAnalogsTC[i] = mAnalogs[i] * frameDelta;
        }
    }

    void Inputs::buildKeyMapping()
    {
        mGlfwKeyMapping[enumCast(Digital::Key_escape)] = GLFW_KEY_ESCAPE;
        mGlfwKeyMapping[enumCast(Digital::Key_1)] = GLFW_KEY_1;
        mGlfwKeyMapping[enumCast(Digital::Key_2)] = GLFW_KEY_2;
        mGlfwKeyMapping[enumCast(Digital::Key_3)] = GLFW_KEY_3;
        mGlfwKeyMapping[enumCast(Digital::Key_4)] = GLFW_KEY_4;
        mGlfwKeyMapping[enumCast(Digital::Key_5)] = GLFW_KEY_5;
        mGlfwKeyMapping[enumCast(Digital::Key_6)] = GLFW_KEY_6;
        mGlfwKeyMapping[enumCast(Digital::Key_7)] = GLFW_KEY_7;
        mGlfwKeyMapping[enumCast(Digital::Key_8)] = GLFW_KEY_8;
        mGlfwKeyMapping[enumCast(Digital::Key_9)] = GLFW_KEY_9;
        mGlfwKeyMapping[enumCast(Digital::Key_0)] = GLFW_KEY_0;
        mGlfwKeyMapping[enumCast(Digital::Key_minus)] = GLFW_KEY_MINUS;
        mGlfwKeyMapping[enumCast(Digital::Key_equals)] = GLFW_KEY_EQUAL;
        mGlfwKeyMapping[enumCast(Digital::Key_back)] = GLFW_KEY_BACKSPACE;
        mGlfwKeyMapping[enumCast(Digital::Key_tab)] = GLFW_KEY_TAB;
        mGlfwKeyMapping[enumCast(Digital::Key_q)] = GLFW_KEY_Q;
        mGlfwKeyMapping[enumCast(Digital::Key_w)] = GLFW_KEY_W;
        mGlfwKeyMapping[enumCast(Digital::Key_e)] = GLFW_KEY_E;
        mGlfwKeyMapping[enumCast(Digital::Key_r)] = GLFW_KEY_R;
        mGlfwKeyMapping[enumCast(Digital::Key_t)] = GLFW_KEY_T;
        mGlfwKeyMapping[enumCast(Digital::Key_y)] = GLFW_KEY_Y;
        mGlfwKeyMapping[enumCast(Digital::Key_u)] = GLFW_KEY_U;
        mGlfwKeyMapping[enumCast(Digital::Key_i)] = GLFW_KEY_I;
        mGlfwKeyMapping[enumCast(Digital::Key_o)] = GLFW_KEY_O;
        mGlfwKeyMapping[enumCast(Digital::Key_p)] = GLFW_KEY_P;
        mGlfwKeyMapping[enumCast(Digital::Key_lbracket)] = GLFW_KEY_LEFT_BRACKET;
        mGlfwKeyMapping[enumCast(Digital::Key_rbracket)] = GLFW_KEY_RIGHT_BRACKET;
        mGlfwKeyMapping[enumCast(Digital::Key_return)] = GLFW_KEY_ENTER;
        mGlfwKeyMapping[enumCast(Digital::Key_lcontrol)] = GLFW_KEY_LEFT_CONTROL;
        mGlfwKeyMapping[enumCast(Digital::Key_a)] = GLFW_KEY_A;
        mGlfwKeyMapping[enumCast(Digital::Key_s)] = GLFW_KEY_S;
        mGlfwKeyMapping[enumCast(Digital::Key_d)] = GLFW_KEY_D;
        mGlfwKeyMapping[enumCast(Digital::Key_f)] = GLFW_KEY_F;
        mGlfwKeyMapping[enumCast(Digital::Key_g)] = GLFW_KEY_G;
        mGlfwKeyMapping[enumCast(Digital::Key_h)] = GLFW_KEY_H;
        mGlfwKeyMapping[enumCast(Digital::Key_j)] = GLFW_KEY_J;
        mGlfwKeyMapping[enumCast(Digital::Key_k)] = GLFW_KEY_K;
        mGlfwKeyMapping[enumCast(Digital::Key_l)] = GLFW_KEY_L;
        mGlfwKeyMapping[enumCast(Digital::Key_semicolon)] = GLFW_KEY_SEMICOLON;
        mGlfwKeyMapping[enumCast(Digital::Key_apostrophe)] = GLFW_KEY_APOSTROPHE;
        mGlfwKeyMapping[enumCast(Digital::Key_grave)] = GLFW_KEY_GRAVE_ACCENT;
        mGlfwKeyMapping[enumCast(Digital::Key_lshift)] = GLFW_KEY_LEFT_SHIFT;
        mGlfwKeyMapping[enumCast(Digital::Key_backslash)] = GLFW_KEY_BACKSLASH;
        mGlfwKeyMapping[enumCast(Digital::Key_z)] = GLFW_KEY_Z;
        mGlfwKeyMapping[enumCast(Digital::Key_x)] = GLFW_KEY_X;
        mGlfwKeyMapping[enumCast(Digital::Key_c)] = GLFW_KEY_C;
        mGlfwKeyMapping[enumCast(Digital::Key_v)] = GLFW_KEY_V;
        mGlfwKeyMapping[enumCast(Digital::Key_b)] = GLFW_KEY_B;
        mGlfwKeyMapping[enumCast(Digital::Key_n)] = GLFW_KEY_N;
        mGlfwKeyMapping[enumCast(Digital::Key_m)] = GLFW_KEY_M;
        mGlfwKeyMapping[enumCast(Digital::Key_comma)] = GLFW_KEY_COMMA;
        mGlfwKeyMapping[enumCast(Digital::Key_period)] = GLFW_KEY_PERIOD;
        mGlfwKeyMapping[enumCast(Digital::Key_slash)] = GLFW_KEY_SLASH;
        mGlfwKeyMapping[enumCast(Digital::Key_rshift)] = GLFW_KEY_RIGHT_SHIFT;
        mGlfwKeyMapping[enumCast(Digital::Key_multiply)] = GLFW_KEY_KP_MULTIPLY;
        mGlfwKeyMapping[enumCast(Digital::Key_lalt)] = GLFW_KEY_LEFT_ALT;
        mGlfwKeyMapping[enumCast(Digital::Key_space)] = GLFW_KEY_SPACE;
        mGlfwKeyMapping[enumCast(Digital::Key_capital)] = GLFW_KEY_CAPS_LOCK;
        mGlfwKeyMapping[enumCast(Digital::Key_f1)] = GLFW_KEY_F1;
        mGlfwKeyMapping[enumCast(Digital::Key_f2)] = GLFW_KEY_F2;
        mGlfwKeyMapping[enumCast(Digital::Key_f3)] = GLFW_KEY_F3;
        mGlfwKeyMapping[enumCast(Digital::Key_f4)] = GLFW_KEY_F4;
        mGlfwKeyMapping[enumCast(Digital::Key_f5)] = GLFW_KEY_F5;
        mGlfwKeyMapping[enumCast(Digital::Key_f6)] = GLFW_KEY_F6;
        mGlfwKeyMapping[enumCast(Digital::Key_f7)] = GLFW_KEY_F7;
        mGlfwKeyMapping[enumCast(Digital::Key_f8)] = GLFW_KEY_F8;
        mGlfwKeyMapping[enumCast(Digital::Key_f9)] = GLFW_KEY_F9;
        mGlfwKeyMapping[enumCast(Digital::Key_f10)] = GLFW_KEY_F10;
        mGlfwKeyMapping[enumCast(Digital::Key_numlock)] = GLFW_KEY_NUM_LOCK;
        mGlfwKeyMapping[enumCast(Digital::Key_scroll)] = GLFW_KEY_SCROLL_LOCK;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad7)] = GLFW_KEY_KP_7;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad8)] = GLFW_KEY_KP_8;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad9)] = GLFW_KEY_KP_9;
        mGlfwKeyMapping[enumCast(Digital::Key_subtract)] = GLFW_KEY_KP_SUBTRACT;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad4)] = GLFW_KEY_KP_4;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad5)] = GLFW_KEY_KP_5;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad6)] = GLFW_KEY_KP_6;
        mGlfwKeyMapping[enumCast(Digital::Key_add)] = GLFW_KEY_KP_ADD;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad1)] = GLFW_KEY_KP_1;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad2)] = GLFW_KEY_KP_2;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad3)] = GLFW_KEY_KP_3;
        mGlfwKeyMapping[enumCast(Digital::Key_numpad0)] = GLFW_KEY_KP_0;
        mGlfwKeyMapping[enumCast(Digital::Key_decimal)] = GLFW_KEY_KP_DECIMAL;
        mGlfwKeyMapping[enumCast(Digital::Key_f11)] = GLFW_KEY_F11;
        mGlfwKeyMapping[enumCast(Digital::Key_f12)] = GLFW_KEY_F12;
        mGlfwKeyMapping[enumCast(Digital::Key_numpadenter)] = GLFW_KEY_KP_ENTER;
        mGlfwKeyMapping[enumCast(Digital::Key_rcontrol)] = GLFW_KEY_RIGHT_CONTROL;
        mGlfwKeyMapping[enumCast(Digital::Key_divide)] = GLFW_KEY_KP_DIVIDE;
        mGlfwKeyMapping[enumCast(Digital::Key_sysrq)] = GLFW_KEY_PRINT_SCREEN;
        mGlfwKeyMapping[enumCast(Digital::Key_ralt)] = GLFW_KEY_RIGHT_ALT;
        mGlfwKeyMapping[enumCast(Digital::Key_pause)] = GLFW_KEY_PAUSE;
        mGlfwKeyMapping[enumCast(Digital::Key_home)] = GLFW_KEY_HOME;
        mGlfwKeyMapping[enumCast(Digital::Key_up)] = GLFW_KEY_UP;
        mGlfwKeyMapping[enumCast(Digital::Key_pgup)] = GLFW_KEY_PAGE_UP;
        mGlfwKeyMapping[enumCast(Digital::Key_left)] = GLFW_KEY_LEFT;
        mGlfwKeyMapping[enumCast(Digital::Key_right)] = GLFW_KEY_RIGHT;
        mGlfwKeyMapping[enumCast(Digital::Key_end)] = GLFW_KEY_END;
        mGlfwKeyMapping[enumCast(Digital::Key_down)] = GLFW_KEY_DOWN;
        mGlfwKeyMapping[enumCast(Digital::Key_pgdn)] = GLFW_KEY_PAGE_DOWN;
        mGlfwKeyMapping[enumCast(Digital::Key_insert)] = GLFW_KEY_INSERT;
        mGlfwKeyMapping[enumCast(Digital::Key_delete)] = GLFW_KEY_DELETE;
        mGlfwKeyMapping[enumCast(Digital::Key_lwin)] = GLFW_KEY_LEFT_SUPER;
        mGlfwKeyMapping[enumCast(Digital::Key_rwin)] = GLFW_KEY_RIGHT_SUPER;
        mGlfwKeyMapping[enumCast(Digital::Key_apps)] = GLFW_KEY_MENU;
    }

    void Inputs::registerCallbacks()
    {
        HEX_ASSERT(sActiveInstance == nullptr, "Only one platform::Inputs instance is supported at a time.");
        sActiveInstance = this;
        glfwSetScrollCallback(mWindowHandle, &Inputs::scrollCallback);
    }

    void Inputs::unregisterCallbacks()
    {
        if (sActiveInstance == this)
        {
            glfwSetScrollCallback(mWindowHandle, nullptr);
            sActiveInstance = nullptr;
        }
    }

    void Inputs::scrollCallback(GLFWwindow* /*window*/, double /*xOffset*/, double yOffset)
    {
        if (sActiveInstance)
        {
            sActiveInstance->mScrollAccumulator += static_cast<float>(yOffset);
        }
    }

    void Inputs::pollGamepad()
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            const std::function<void(Digital, int)> bindButton = [&](Digital d, int button)
                {
                    mButtons[0][static_cast<size_t>(enumCast(d))] = state.buttons[button] == GLFW_PRESS;
                };
            bindButton(Digital::DPad_Up, GLFW_GAMEPAD_BUTTON_DPAD_UP);
            bindButton(Digital::DPad_Down, GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
            bindButton(Digital::DPad_Left, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
            bindButton(Digital::DPad_Right, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
            bindButton(Digital::DPad_StartButton, GLFW_GAMEPAD_BUTTON_START);
            bindButton(Digital::DPad_BackButton, GLFW_GAMEPAD_BUTTON_BACK);
            bindButton(Digital::DPad_L3, GLFW_GAMEPAD_BUTTON_LEFT_THUMB);
            bindButton(Digital::DPad_R3, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);
            bindButton(Digital::DPad_L1, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
            bindButton(Digital::DPad_R1, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);
            bindButton(Digital::DPad_AButton, GLFW_GAMEPAD_BUTTON_A);
            bindButton(Digital::DPad_BButton, GLFW_GAMEPAD_BUTTON_B);
            bindButton(Digital::DPad_XButton, GLFW_GAMEPAD_BUTTON_X);
            bindButton(Digital::DPad_YButton, GLFW_GAMEPAD_BUTTON_Y);

            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_L2))] = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1.0f) * 0.5f;
            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_R2))] = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1.0f) * 0.5f;
            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_LeftStickX))] = filterAnalogInput(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], kLeftStickDeadZone);
            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_LeftStickY))] = -filterAnalogInput(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y], kLeftStickDeadZone);
            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_RightStickX))] = filterAnalogInput(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], kRightStickDeadZone);
            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_RightStickY))] = -filterAnalogInput(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y], kRightStickDeadZone);
        }
    }

    void Inputs::pollKeyboardAndMouse()
    {
        if (glfwGetWindowAttrib(mWindowHandle, GLFW_FOCUSED))
        {
            for (size_t i = 0; i < NUM_KEYS; ++i)
            {
                mButtons[0][i] = glfwGetKey(mWindowHandle, mGlfwKeyMapping[i]) == GLFW_PRESS;
            }

            for (size_t i = 0; i < NUM_MOUSE_BUTTONS; ++i)
            {
                mButtons[0][static_cast<size_t>(enumCast(Digital::Mouse0)) + i] = glfwGetMouseButton(mWindowHandle, GLFW_MOUSE_BUTTON_1 + static_cast<int>(i)) == GLFW_PRESS;
            }

            double x = 0.0, y = 0.0;
            glfwGetCursorPos(mWindowHandle, &x, &y);
            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_MouseX))] = static_cast<float>(x - mLastMouseX) * kMouseSensitivity;
            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_MouseY))] = static_cast<float>(mLastMouseY - y) * kMouseSensitivity;
            mLastMouseX = x;
            mLastMouseY = y;

            mAnalogs[static_cast<size_t>(enumCast(Analog::Analog_MouseScroll))] = (mScrollAccumulator > 0.0f ? 1.0f : (mScrollAccumulator < 0.0f ? -1.0f : 0.0f));
            mScrollAccumulator = 0.0f;
        }
        else
        {
            glfwGetCursorPos(mWindowHandle, &mLastMouseX, &mLastMouseY);
            mScrollAccumulator = 0.0f;
        }
    }

    inline float Inputs::filterAnalogInput(float val, float deadZone) const
    {
        if (val < 0.0f)
        {
            return (val > -deadZone) ? 0.0f : (val + deadZone) / (1.0f - deadZone);
        }
        else
        {
            return (val < deadZone) ? 0.0f : (val - deadZone) / (1.0f - deadZone);
        }
    }
}
