// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <utility>

#include "./Core.h"

namespace Borek {

// Uses GLFW keymap
enum class KeyCode {
        kSPACE         =    32,
        kAPOSTROPHE    =    39,  /* ' */
        kCOMMA         =    44,  /* , */
        kMINUS         =    45,  /* - */
        kPERIOD        =    46,  /* . */
        kSLASH         =    47,  /* / */
        k0             =    48,
        k1             =    49,
        k2             =    50,
        k3             =    51,
        k4             =    52,
        k5             =    53,
        k6             =    54,
        k7             =    55,
        k8             =    56,
        k9             =    57,
        kSEMICOLON     =    59,  /* ; */
        kEQUAL         =    61,  /* = */
        kA             =    65,
        kB             =    66,
        kC             =    67,
        kD             =    68,
        kE             =    69,
        kF             =    70,
        kG             =    71,
        kH             =    72,
        kI             =    73,
        kJ             =    74,
        kK             =    75,
        kL             =    76,
        kM             =    77,
        kN             =    78,
        kO             =    79,
        kP             =    80,
        kQ             =    81,
        kR             =    82,
        kS             =    83,
        kT             =    84,
        kU             =    85,
        kV             =    86,
        kW             =    87,
        kX             =    88,
        kY             =    89,
        kZ             =    90,
        kLEFT_BRACKET  =    91,  /* [ */
        kBACKSLASH     =    92,  /* \ */
        kRIGHT_BRACKET =    93,  /* ] */
        kGRAVE_ACCENT  =    96,  /* ` */
        kWORLD_1       =    161, /* non-US #1 */
        kWORLD_2       =    162, /* non-US #2 */

        /* Function keys */
        kESCAPE        =    256,
        kENTER         =    257,
        kTAB           =    258,
        kBACKSPACE     =    259,
        kINSERT        =    260,
        kDELETE        =    261,
        kRIGHT         =    262,
        kLEFT          =    263,
        kDOWN          =    264,
        kUP            =    265,
        kPAGE_UP       =    266,
        kPAGE_DOWN     =    267,
        kHOME          =    268,
        kEND           =    269,
        kCAPS_LOCK     =    280,
        kSCROLL_LOCK   =    281,
        kNUM_LOCK      =    282,
        kPRINT_SCREEN  =    283,
        kPAUSE         =    284,
        kF1            =    290,
        kF2            =    291,
        kF3            =    292,
        kF4            =    293,
        kF5            =    294,
        kF6            =    295,
        kF7            =    296,
        kF8            =    297,
        kF9            =    298,
        kF10           =    299,
        kF11           =    300,
        kF12           =    301,
        kF13           =    302,
        kF14           =    303,
        kF15           =    304,
        kF16           =    305,
        kF17           =    306,
        kF18           =    307,
        kF19           =    308,
        kF20           =    309,
        kF21           =    310,
        kF22           =    311,
        kF23           =    312,
        kF24           =    313,
        kF25           =    314,
        kKP_0          =    320,
        kKP_1          =    321,
        kKP_2          =    322,
        kKP_3          =    323,
        kKP_4          =    324,
        kKP_5          =    325,
        kKP_6          =    326,
        kKP_7          =    327,
        kKP_8          =    328,
        kKP_9          =    329,
        kKP_DECIMAL    =    330,
        kKP_DIVIDE     =    331,
        kKP_MULTIPLY   =    332,
        kKP_SUBTRACT   =    333,
        kKP_ADD        =    334,
        kKP_ENTER      =    335,
        kKP_EQUAL      =    336,
        kLEFT_SHIFT    =    340,
        kLEFT_CONTROL  =    341,
        kLEFT_ALT      =    342,
        kLEFT_SUPER    =    343,
        kRIGHT_SHIFT   =    344,
        kRIGHT_CONTROL =    345,
        kRIGHT_ALT     =    346,
        kRIGHT_SUPER   =    347,
        kMENU          =    348,
};

enum class MouseButton {
        kBUTTON_1      = 0,
        kBUTTON_2      = 1,
        kBUTTON_3      = 2,
        kBUTTON_4      = 3,
        kBUTTON_5      = 4,
        kBUTTON_6      = 5,
        kBUTTON_7      = 6,
        kBUTTON_8      = 7,
        kBUTTON_LEFT   = kBUTTON_1,
        kBUTTON_RIGHT  = kBUTTON_2,
        kBUTTON_MIDDLE = kBUTTON_3,
};

class Input {
public:
        static bool IsKeyPressed(KeyCode key);
        static bool IsMouseButtonPressed(MouseButton button);
        static inline double GetMouseX()
        {
                return GetMousePos().first;
        }
        static inline double GetMouseY()
        {
                return GetMousePos().second;
        }

        static std::pair<double, double> GetMousePos();
};


}  // namespace Borek
