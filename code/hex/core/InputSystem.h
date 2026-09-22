#pragma once

#include "common.h"

namespace hex
{
    class Inputs
    {
    public:
        enum class Digital
        {
            Key_escape = 0,
            Key_1,
            Key_2,
            Key_3,
            Key_4,
            Key_5,
            Key_6,
            Key_7,
            Key_8,
            Key_9,
            Key_0,
            Key_minus,
            Key_equals,
            Key_back,
            Key_tab,
            Key_q,
            Key_w,
            Key_e,
            Key_r,
            Key_t,
            Key_y,
            Key_u,
            Key_i,
            Key_o,
            Key_p,
            Key_lbracket,
            Key_rbracket,
            Key_return,
            Key_lcontrol,
            Key_a,
            Key_s,
            Key_d,
            Key_f,
            Key_g,
            Key_h,
            Key_j,
            Key_k,
            Key_l,
            Key_semicolon,
            Key_apostrophe,
            Key_grave,
            Key_lshift,
            Key_backslash,
            Key_z,
            Key_x,
            Key_c,
            Key_v,
            Key_b,
            Key_n,
            Key_m,
            Key_comma,
            Key_period,
            Key_slash,
            Key_rshift,
            Key_multiply,
            Key_lalt,
            Key_space,
            Key_capital,
            Key_f1,
            Key_f2,
            Key_f3,
            Key_f4,
            Key_f5,
            Key_f6,
            Key_f7,
            Key_f8,
            Key_f9,
            Key_f10,
            Key_numlock,
            Key_scroll,
            Key_numpad7,
            Key_numpad8,
            Key_numpad9,
            Key_subtract,
            Key_numpad4,
            Key_numpad5,
            Key_numpad6,
            Key_add,
            Key_numpad1,
            Key_numpad2,
            Key_numpad3,
            Key_numpad0,
            Key_decimal,
            Key_f11,
            Key_f12,
            Key_numpadenter,
            Key_rcontrol,
            Key_divide,
            Key_sysrq,
            Key_ralt,
            Key_pause,
            Key_home,
            Key_up,
            Key_pgup,
            Key_left,
            Key_right,
            Key_end,
            Key_down,
            Key_pgdn,
            Key_insert,
            Key_delete,
            Key_lwin,
            Key_rwin,
            Key_apps,

            kNumKeys,

            DPad_Up = kNumKeys,
            DPad_Down,
            DPad_Left,
            DPad_Right,
            DPad_StartButton,
            DPad_BackButton,
            DPad_L3,
            DPad_R3,
            DPad_L1,
            DPad_R1,
            DPad_AButton,
            DPad_BButton,
            DPad_XButton,
            DPad_YButton,

            Mouse0,
            Mouse1,
            Mouse2,
            Mouse3,
            Mouse4,
            Mouse5,
            Mouse6,
            Mouse7
        };

        enum class Analog
        {
            Analog_L2,
            Analog_R2,
            Analog_LeftStickX,
            Analog_LeftStickY,
            Analog_RightStickX,
            Analog_RightStickY,

            Analog_MouseX,
            Analog_MouseY,
            Analog_MouseScroll
        };

        virtual ~Inputs() = default;

        virtual bool isAnyPressed() const = 0;
        virtual bool isPressed(Digital di) const = 0;
        virtual bool isFirstPressed(Digital di) const = 0;
        virtual bool isReleased(Digital di) const = 0;
        virtual bool isFirstReleased(Digital di) const = 0;
        virtual float getDurationPressed(Digital di) const = 0;
        virtual float getAnalogInput(Analog ai) const = 0;
        virtual float getTimeCorrectedAnalogInput(Analog ai) const = 0;
        virtual void update(float frameDelta) = 0;
    };
}
