#pragma once

#include "core/common.h"

#include "core/InputSystem.h"

struct GLFWwindow;

namespace platform
{
    class Inputs : public hex::Inputs
    {
    public:
        Inputs(GLFWwindow* windowHandle);
        ~Inputs() override;

        bool isAnyPressed() const override;
        bool isPressed(Digital di) const override;
        bool isFirstPressed(Digital di) const override;
        bool isReleased(Digital di) const override;
        bool isFirstReleased(Digital di) const override;
        float getDurationPressed(Digital di) const override;
        float getAnalogInput(Analog ai) const override;
        float getTimeCorrectedAnalogInput(Analog ai) const override;
        void update(float frameDelta) override;

    private:
        static constexpr size_t NUM_DIGITAL_INPUTS = static_cast<size_t>(Digital::Mouse7) + 1;
        static constexpr size_t NUM_ANALOG_INPUTS = static_cast<size_t>(Analog::Analog_MouseScroll) + 1;
        static constexpr size_t NUM_KEYS = static_cast<size_t>(Digital::Key_apps) + 1;
        static constexpr size_t NUM_MOUSE_BUTTONS = static_cast<size_t>(Digital::Mouse7) - static_cast<size_t>(Digital::Mouse0) + 1;

        void buildKeyMapping();
        void registerCallbacks();
        void unregisterCallbacks();

        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

        void pollGamepad();
        void pollKeyboardAndMouse();

        inline float filterAnalogInput(float val, float deadZone) const;

        std::array<std::array<bool, NUM_DIGITAL_INPUTS>, 2> mButtons;
        std::array<float, NUM_DIGITAL_INPUTS> mHoldDuration;
        std::array<float, NUM_ANALOG_INPUTS> mAnalogs;
        std::array<float, NUM_ANALOG_INPUTS> mAnalogsTC;

        GLFWwindow* const mWindowHandle;
        std::array<int, NUM_KEYS> mGlfwKeyMapping;
        double mLastMouseX;
        double mLastMouseY;
        float mScrollAccumulator;

        static Inputs* sActiveInstance;
    };
}
