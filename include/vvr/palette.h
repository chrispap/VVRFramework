#pragma once

#include "vvrframework_DLL.h"
#include <string>
#include <algorithm>

namespace vvr
{
    struct VVRFramework_API Colour
    {
        Colour()
            : r(0)
            , g(0)
            , b(0)
            , a(255)
        { }

        Colour(unsigned val)
            : r((unsigned char)((val & 0xFF0000) >> 16))
            , g((unsigned char)((val & 0x00FF00) >>  8))
            , b((unsigned char)((val & 0x0000FF) >>  0))
            , a(255)
        { }

        Colour(int r, int g, int b)
            : r((unsigned char)r)
            , g((unsigned char)g)
            , b((unsigned char)b)
            , a(255)
        { }

        Colour(float r, float g, float b)
            : r((unsigned char)(r * 0xFF))
            , g((unsigned char)(g * 0xFF))
            , b((unsigned char)(b * 0xFF))
            , a(255)
        { }

        Colour(std::string hex_str)
            : r(strtol(hex_str.substr(0, 2).c_str(), 0, 16))
            , g(strtol(hex_str.substr(2, 2).c_str(), 0, 16))
            , b(strtol(hex_str.substr(4, 2).c_str(), 0, 16))
            , a(255)
        { }

        void add(unsigned char val)
        {
            r = std::min(0xFF, (int)r + val);
            g = std::min(0xFF, (int)g + val);
            b = std::min(0xFF, (int)b + val);
        }

        void sub(unsigned char val)
        {
            r = std::max(0x00, (int)r - val);
            g = std::max(0x00, (int)g - val);
            b = std::max(0x00, (int)b - val);
        }

        void mul(float c)
        {
            r = std::min((int)(c * r), 0xFF);
            g = std::min((int)(c * g), 0xFF);
            b = std::min((int)(c * b), 0xFF);
        }

        void lighter()
        {
            add(55);
        }

        void darker()
        {
            sub(55);
        }

        union
        {
            struct { unsigned char r, g, b, a; };
            unsigned char data[4];
        };
    };
}

namespace vvr
{
    extern const VVRFramework_API Colour white;
    extern const VVRFramework_API Colour red;
    extern const VVRFramework_API Colour green;
    extern const VVRFramework_API Colour blue;
    extern const VVRFramework_API Colour black;
    extern const VVRFramework_API Colour yellow;
    extern const VVRFramework_API Colour grey;
    extern const VVRFramework_API Colour orange;
    extern const VVRFramework_API Colour cyan;
    extern const VVRFramework_API Colour magenta;
    extern const VVRFramework_API Colour darkOrange;
    extern const VVRFramework_API Colour darkRed;
    extern const VVRFramework_API Colour darkGreen;
    extern const VVRFramework_API Colour yellowGreen;
    extern const VVRFramework_API Colour lilac;
}

namespace vvr
{
    extern const VVRFramework_API Colour AliceBlue;
    extern const VVRFramework_API Colour AntiqueWhite;
    extern const VVRFramework_API Colour Aqua;
    extern const VVRFramework_API Colour Aquamarine;
    extern const VVRFramework_API Colour Azure;
    extern const VVRFramework_API Colour Beige;
    extern const VVRFramework_API Colour Bisque;
    extern const VVRFramework_API Colour Black;
    extern const VVRFramework_API Colour BlanchedAlmond;
    extern const VVRFramework_API Colour Blue;
    extern const VVRFramework_API Colour BlueViolet;
    extern const VVRFramework_API Colour Brown;
    extern const VVRFramework_API Colour BurlyWood;
    extern const VVRFramework_API Colour CadetBlue;
    extern const VVRFramework_API Colour Chartreuse;
    extern const VVRFramework_API Colour Chocolate;
    extern const VVRFramework_API Colour Coral;
    extern const VVRFramework_API Colour CornflowerBlue;
    extern const VVRFramework_API Colour Cornsilk;
    extern const VVRFramework_API Colour Crimson;
    extern const VVRFramework_API Colour Cyan;
    extern const VVRFramework_API Colour DarkBlue;
    extern const VVRFramework_API Colour DarkCyan;
    extern const VVRFramework_API Colour DarkGoldenrod;
    extern const VVRFramework_API Colour DarkGray;
    extern const VVRFramework_API Colour DarkGreen;
    extern const VVRFramework_API Colour DarkKhaki;
    extern const VVRFramework_API Colour DarkMagenta;
    extern const VVRFramework_API Colour DarkOliveGreen;
    extern const VVRFramework_API Colour DarkOrange;
    extern const VVRFramework_API Colour DarkOrchid;
    extern const VVRFramework_API Colour DarkRed;
    extern const VVRFramework_API Colour DarkSalmon;
    extern const VVRFramework_API Colour DarkSeaGreen;
    extern const VVRFramework_API Colour DarkSlateBlue;
    extern const VVRFramework_API Colour DarkSlateGray;
    extern const VVRFramework_API Colour DarkTurquoise;
    extern const VVRFramework_API Colour DarkViolet;
    extern const VVRFramework_API Colour DeepPink;
    extern const VVRFramework_API Colour DeepSkyBlue;
    extern const VVRFramework_API Colour DimGray;
    extern const VVRFramework_API Colour DodgerBlue;
    extern const VVRFramework_API Colour FireBrick;
    extern const VVRFramework_API Colour FloralWhite;
    extern const VVRFramework_API Colour ForestGreen;
    extern const VVRFramework_API Colour Fuchsia;
    extern const VVRFramework_API Colour Gainsboro;
    extern const VVRFramework_API Colour GhostWhite;
    extern const VVRFramework_API Colour Gold;
    extern const VVRFramework_API Colour Goldenrod;
    extern const VVRFramework_API Colour Gray;
    extern const VVRFramework_API Colour Green;
    extern const VVRFramework_API Colour GreenYellow;
    extern const VVRFramework_API Colour Honeydew;
    extern const VVRFramework_API Colour HotPink;
    extern const VVRFramework_API Colour IndianRed;
    extern const VVRFramework_API Colour Indigo;
    extern const VVRFramework_API Colour Ivory;
    extern const VVRFramework_API Colour Khaki;
    extern const VVRFramework_API Colour Lavender;
    extern const VVRFramework_API Colour LavenderBlush;
    extern const VVRFramework_API Colour LawnGreen;
    extern const VVRFramework_API Colour LemonChiffon;
    extern const VVRFramework_API Colour LightBlue;
    extern const VVRFramework_API Colour LightCoral;
    extern const VVRFramework_API Colour LightCyan;
    extern const VVRFramework_API Colour LightGoldenrodYellow;
    extern const VVRFramework_API Colour LightGreen;
    extern const VVRFramework_API Colour LightGrey;
    extern const VVRFramework_API Colour LightPink;
    extern const VVRFramework_API Colour LightSalmon;
    extern const VVRFramework_API Colour LightSeaGreen;
    extern const VVRFramework_API Colour LightSkyBlue;
    extern const VVRFramework_API Colour LightSlateGray;
    extern const VVRFramework_API Colour LightSteelBlue;
    extern const VVRFramework_API Colour LightYellow;
    extern const VVRFramework_API Colour Lime;
    extern const VVRFramework_API Colour LimeGreen;
    extern const VVRFramework_API Colour Linen;
    extern const VVRFramework_API Colour Magenta;
    extern const VVRFramework_API Colour Maroon;
    extern const VVRFramework_API Colour MediumAquamarine;
    extern const VVRFramework_API Colour MediumBlue;
    extern const VVRFramework_API Colour MediumOrchid;
    extern const VVRFramework_API Colour MediumPurple;
    extern const VVRFramework_API Colour MediumSeaGreen;
    extern const VVRFramework_API Colour MediumSlateBlue;
    extern const VVRFramework_API Colour MediumSpringGreen;
    extern const VVRFramework_API Colour MediumTurquoise;
    extern const VVRFramework_API Colour MediumVioletRed;
    extern const VVRFramework_API Colour MidnightBlue;
    extern const VVRFramework_API Colour MintCream;
    extern const VVRFramework_API Colour MistyRose;
    extern const VVRFramework_API Colour Moccasin;
    extern const VVRFramework_API Colour NavajoWhite;
    extern const VVRFramework_API Colour Navy;
    extern const VVRFramework_API Colour OldLace;
    extern const VVRFramework_API Colour Olive;
    extern const VVRFramework_API Colour OliveDrab;
    extern const VVRFramework_API Colour Orange;
    extern const VVRFramework_API Colour OrangeRed;
    extern const VVRFramework_API Colour Orchid;
    extern const VVRFramework_API Colour PaleGoldenrod;
    extern const VVRFramework_API Colour PaleGreen;
    extern const VVRFramework_API Colour PaleTurquoise;
    extern const VVRFramework_API Colour PaleVioletRed;
    extern const VVRFramework_API Colour PapayaWhip;
    extern const VVRFramework_API Colour PeachPuff;
    extern const VVRFramework_API Colour Peru;
    extern const VVRFramework_API Colour Pink;
    extern const VVRFramework_API Colour Plum;
    extern const VVRFramework_API Colour PowderBlue;
    extern const VVRFramework_API Colour Purple;
    extern const VVRFramework_API Colour Red;
    extern const VVRFramework_API Colour RosyBrown;
    extern const VVRFramework_API Colour RoyalBlue;
    extern const VVRFramework_API Colour SaddleBrown;
    extern const VVRFramework_API Colour Salmon;
    extern const VVRFramework_API Colour SandyBrown;
    extern const VVRFramework_API Colour SeaGreen;
    extern const VVRFramework_API Colour Seashell;
    extern const VVRFramework_API Colour Sienna;
    extern const VVRFramework_API Colour Silver;
    extern const VVRFramework_API Colour SkyBlue;
    extern const VVRFramework_API Colour SlateBlue;
    extern const VVRFramework_API Colour SlateGray;
    extern const VVRFramework_API Colour Snow;
    extern const VVRFramework_API Colour SpringGreen;
    extern const VVRFramework_API Colour SteelBlue;
    extern const VVRFramework_API Colour Tan;
    extern const VVRFramework_API Colour Teal;
    extern const VVRFramework_API Colour Thistle;
    extern const VVRFramework_API Colour Tomato;
    extern const VVRFramework_API Colour Turquoise;
    extern const VVRFramework_API Colour Violet;
    extern const VVRFramework_API Colour Wheat;
    extern const VVRFramework_API Colour White;
    extern const VVRFramework_API Colour WhiteSmoke;
    extern const VVRFramework_API Colour Yellow;
    extern const VVRFramework_API Colour YellowGreen;
}
