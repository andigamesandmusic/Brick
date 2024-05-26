/*
  ==============================================================================

   This file is part of the Belle, Bonne, Sage library
   Copyright 2007-2010 by Andi

  ------------------------------------------------------------------------------

   Belle, Bonne, Sage can be redistributed and/or modified under the terms of
   the GNU Lesser General Public License, as published by the
   Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Belle, Bonne, Sage is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with Belle, Bonne, Sage; if not, visit www.gnu.org/licenses or write:
   
   Free Software Foundation, Inc.
   59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ==============================================================================
*/

#ifndef primColor
#define primColor

#include "primTypes.h"
#include "primMath.h"

namespace prim
{
  namespace colors
  {
    /**\brief An RGB color specified in 24-bit hex format with blue in the
    lower eight bits, green in the middle eight, and red in the upper
    eight.*/
    typedef uint32 RGB;

    ///A structure for separating colors into their components.
    struct Component
    {
      ///Returns the red component of an RGB color.
      static number r(RGB Hex)
      {
        return (number)math::Mod((Hex >> 16),256)/(number)255.0;
      }

      ///Returns the green component of an RGB color.
      static number g(RGB Hex)
      {
        return (number)math::Mod((Hex >> 8),256)/(number)255.0;
      }

      ///Returns the blue component of an RGB color.
      static number b(RGB Hex)
      {
        return (number)math::Mod(Hex,256)/(number)255.0;
      }

      //Returns the cyan component of an RGB color.
      static number c(RGB Hex)
      {
        using namespace math;
        number C = (number)1.0 - Component::r(Hex);
        number M = (number)1.0 - Component::g(Hex);
        number Y = (number)1.0 - Component::b(Hex);
        number K = Min(Min(C,M),Y);
        if(K != (number)1.0)
          return (C - K) / ((number)1.0 - K);
        else
          return (number)1.0;
      }

      //Returns the magenta component of an RGB color.
      static number m(RGB Hex)
      {
        using namespace math;
        number C = (number)1.0 - Component::r(Hex);
        number M = (number)1.0 - Component::g(Hex);
        number Y = (number)1.0 - Component::b(Hex);
        number K = Min(Min(C,M),Y);
        if(K != (number)1.0)
          return (M - K) / ((number)1.0 - K);
        else
          return (number)1.0;
      }

      //Returns the yellow component of an RGB color.
      static number y(RGB Hex)
      {
        using namespace math;
        number C = (number)1.0 - Component::r(Hex);
        number M = (number)1.0 - Component::g(Hex);
        number Y = (number)1.0 - Component::b(Hex);
        number K = Min(Min(C,M),Y);
        if(K != (number)1.0)
          return (Y - K) / ((number)1.0 - K);
        else
          return (number)1.0;
      }

      //Returns the black component of an RGB color.
      static number k(RGB Hex)
      {
        using namespace math;
        number C = (number)1.0 - Component::r(Hex);
        number M = (number)1.0 - Component::g(Hex);
        number Y = (number)1.0 - Component::b(Hex);
        return Min(Min(C,M),Y);
      }
    };

    //Taken from the HTML Color Names.
    //See: http://w3schools.com/tags/ref_colornames.asp

    static const RGB AliceBlue = 0xF0F8FF;
    static const RGB AntiqueWhite = 0xFAEBD7;
    static const RGB Aqua = 0x00FFFF;
    static const RGB Aquamarine = 0x7FFFD4;
    static const RGB Azure = 0xF0FFFF;
    static const RGB Beige = 0xF5F5DC;
    static const RGB Bisque = 0xFFE4C4;
    static const RGB Black = 0x000000;
    static const RGB BlanchedAlmond = 0xFFEBCD;
    static const RGB Blue = 0x0000FF;
    static const RGB BlueViolet = 0x8A2BE2;
    static const RGB Brown = 0xA52A2A;
    static const RGB BurlyWood = 0xDEB887;
    static const RGB CadetBlue = 0x5F9EA0;
    static const RGB Chartreuse = 0x7FFF00;
    static const RGB Chocolate = 0xD2691E;
    static const RGB Coral = 0xFF7F50;
    static const RGB CornflowerBlue = 0x6495ED;
    static const RGB Cornsilk = 0xFFF8DC;
    static const RGB Crimson = 0xDC143C;
    static const RGB Cyan = 0x00FFFF;
    static const RGB DarkBlue = 0x00008B;
    static const RGB DarkCyan = 0x008B8B;
    static const RGB DarkGoldenRod = 0xB8860B;
    static const RGB DarkGray = 0xA9A9A9;
    static const RGB DarkGrey = 0xA9A9A9;
    static const RGB DarkGreen = 0x006400;
    static const RGB DarkKhaki = 0xBDB76B;
    static const RGB DarkMagenta = 0x8B008B;
    static const RGB DarkOliveGreen = 0x556B2F;
    static const RGB DarkOrange = 0xFF8C00;
    static const RGB DarkOrchid = 0x9932CC;
    static const RGB DarkRed = 0x8B0000;
    static const RGB DarkSalmon = 0xE9967A;
    static const RGB DarkSeaGreen = 0x8FBC8F;
    static const RGB DarkSlateBlue = 0x483D8B;
    static const RGB DarkSlateGray = 0x2F4F4F;
    static const RGB DarkSlateGrey = 0x2F4F4F;
    static const RGB DarkTurquoise = 0x00CED1;
    static const RGB DarkViolet = 0x9400D3;
    static const RGB DeepPink = 0xFF1493;
    static const RGB DeepSkyBlue = 0x00BFFF;
    static const RGB DimGray = 0x696969;
    static const RGB DimGrey = 0x696969;
    static const RGB DodgerBlue = 0x1E90FF;
    static const RGB FireBrick = 0xB22222;
    static const RGB FloralWhite = 0xFFFAF0;
    static const RGB ForestGreen = 0x228B22;
    static const RGB Fuchsia = 0xFF00FF;
    static const RGB Gainsboro = 0xDCDCDC;
    static const RGB GhostWhite = 0xF8F8FF;
    static const RGB Gold = 0xFFD700;
    static const RGB GoldenRod = 0xDAA520;
    static const RGB Gray = 0x808080;
    static const RGB Grey = 0x808080;
    static const RGB Green = 0x008000;
    static const RGB GreenYellow = 0xADFF2F;
    static const RGB HoneyDew = 0xF0FFF0;
    static const RGB HotPink = 0xFF69B4;
    static const RGB IndianRed = 0xCD5C5C;
    static const RGB Indigo = 0x4B0082;
    static const RGB Ivory = 0xFFFFF0;
    static const RGB Khaki = 0xF0E68C;
    static const RGB Lavender = 0xE6E6FA;
    static const RGB LavenderBlush = 0xFFF0F5;
    static const RGB LawnGreen = 0x7CFC00;
    static const RGB LemonChiffon = 0xFFFACD;
    static const RGB LightBlue = 0xADD8E6;
    static const RGB LightCoral = 0xF08080;
    static const RGB LightCyan = 0xE0FFFF;
    static const RGB LightGoldenRodYellow = 0xFAFAD2;
    static const RGB LightGray = 0xD3D3D3;
    static const RGB LightGrey = 0xD3D3D3;
    static const RGB LightGreen = 0x90EE90;
    static const RGB LightPink = 0xFFB6C1;
    static const RGB LightSalmon = 0xFFA07A;
    static const RGB LightSeaGreen = 0x20B2AA;
    static const RGB LightSkyBlue = 0x87CEFA;
    static const RGB LightSlateGray = 0x778899;
    static const RGB LightSlateGrey = 0x778899;
    static const RGB LightSteelBlue = 0xB0C4DE;
    static const RGB LightYellow = 0xFFFFE0;
    static const RGB Lime = 0x00FF00;
    static const RGB LimeGreen = 0x32CD32;
    static const RGB Linen = 0xFAF0E6;
    static const RGB Magenta = 0xFF00FF;
    static const RGB Maroon = 0x800000;
    static const RGB MediumAquaMarine = 0x66CDAA;
    static const RGB MediumBlue = 0x0000CD;
    static const RGB MediumOrchid = 0xBA55D3;
    static const RGB MediumPurple = 0x9370D8;
    static const RGB MediumSeaGreen = 0x3CB371;
    static const RGB MediumSlateBlue = 0x7B68EE;
    static const RGB MediumSpringGreen = 0x00FA9A;
    static const RGB MediumTurquoise = 0x48D1CC;
    static const RGB MediumVioletRed = 0xC71585;
    static const RGB MidnightBlue = 0x191970;
    static const RGB MintCream = 0xF5FFFA;
    static const RGB MistyRose = 0xFFE4E1;
    static const RGB Moccasin = 0xFFE4B5;
    static const RGB NavajoWhite = 0xFFDEAD;
    static const RGB Navy = 0x000080;
    static const RGB OldLace = 0xFDF5E6;
    static const RGB Olive = 0x808000;
    static const RGB OliveDrab = 0x6B8E23;
    static const RGB Orange = 0xFFA500;
    static const RGB OrangeRed = 0xFF4500;
    static const RGB Orchid = 0xDA70D6;
    static const RGB PaleGoldenRod = 0xEEE8AA;
    static const RGB PaleGreen = 0x98FB98;
    static const RGB PaleTurquoise = 0xAFEEEE;
    static const RGB PaleVioletRed = 0xD87093;
    static const RGB PapayaWhip = 0xFFEFD5;
    static const RGB PeachPuff = 0xFFDAB9;
    static const RGB Peru = 0xCD853F;
    static const RGB Pink = 0xFFC0CB;
    static const RGB Plum = 0xDDA0DD;
    static const RGB PowderBlue = 0xB0E0E6;
    static const RGB Purple = 0x800080;
    static const RGB Red = 0xFF0000;
    static const RGB RosyBrown = 0xBC8F8F;
    static const RGB RoyalBlue = 0x4169E1;
    static const RGB SaddleBrown = 0x8B4513;
    static const RGB Salmon = 0xFA8072;
    static const RGB SandyBrown = 0xF4A460;
    static const RGB SeaGreen = 0x2E8B57;
    static const RGB SeaShell = 0xFFF5EE;
    static const RGB Sienna = 0xA0522D;
    static const RGB Silver = 0xC0C0C0;
    static const RGB SkyBlue = 0x87CEEB;
    static const RGB SlateBlue = 0x6A5ACD;
    static const RGB SlateGray = 0x708090;
    static const RGB SlateGrey = 0x708090;
    static const RGB Snow = 0xFFFAFA;
    static const RGB SpringGreen = 0x00FF7F;
    static const RGB SteelBlue = 0x4682B4;
    static const RGB Tan = 0xD2B48C;
    static const RGB Teal = 0x008080;
    static const RGB Thistle = 0xD8BFD8;
    static const RGB Tomato = 0xFF6347;
    static const RGB Turquoise = 0x40E0D0;
    static const RGB Violet = 0xEE82EE;
    static const RGB Wheat = 0xF5DEB3;
    static const RGB White = 0xFFFFFF;
    static const RGB WhiteSmoke = 0xF5F5F5;
    static const RGB Yellow = 0xFFFF00;
    static const RGB YellowGreen = 0x9ACD32;
  }
}

#endif
