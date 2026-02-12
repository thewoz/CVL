/*
 * GNU GENERAL PUBLIC LICENSE
 *
 * Copyright (C) 2026
 * Created by Leonardo Parisi (leonardo.parisi[at]gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _H_CVL_COLORS_H_
#define _H_CVL_COLORS_H_

#include <map>
#include <string>

#include <opencv2/opencv.hpp>

//*****************************************************************************
// namespace cvl
//*****************************************************************************
namespace cvl {

  //*****************************************************************************
  // class colors
  //*****************************************************************************
  class colors {
    
  private:
    
    colors() { }
    
    
    // mappa che mi contiene i colori
    static std::map<std::string, cv::Vec3b> _colors;
    
  public:
    
    //****************************************************************************//
    // get()
    //****************************************************************************//
    // https://htmlcolorcodes.com/color-names/
    void loadPalette() {
      
    }
    
    //*****************************************************************************/
    // RGBtoHex:
    // Function to convert the RGB code to Hex color code
    // Convert RGB values to a hex color string (e.g., #RRGGBB).
    //*****************************************************************************/
    static std::string RGBtoHex(int R, int G, int B) {
      
      if((R >= 0 && R <= 255) && (G >= 0 && G <= 255) && (B >= 0 && B <= 255)) {
     
        std::string hexCode = "#";
                    hexCode += std::decToHex(R);
                    hexCode += std::decToHex(G);
                    hexCode += std::decToHex(B);
     
        return hexCode;
        
      } else {
        fprintf(stderr, "error: in RGBtoHex()\n");
        abort();
      }
      
    }
    
    //*****************************************************************************/
    // BGRtoHex()
    // Function to convert the BGR code to Hex color code
    // Convert BGR values to a hex color string (e.g., #RRGGBB).
    //*****************************************************************************/
    static std::string BGRtoHex(const cv::Vec3b & color) {
      
      if((color[0] >= 0 && color[0] <= 255) && (color[1] >= 0 && color[1] <= 255) && (color[2] >= 0 && color[2] <= 255)) {
     
        std::string hexCode = "#";
                    hexCode += std::decToHex(color[2]);
                    hexCode += std::decToHex(color[1]);
                    hexCode += std::decToHex(color[0]);
     
        return hexCode;
        
      } else  {
        fprintf(stderr, "error: in BGRtoHex()\n");
        abort();
      }
      
    }
    
    //****************************************************************************//
    // get()
    //****************************************************************************//
    static const cv::Vec3b & get(const std::string & str, bool create = false) {
   
      auto colorMap = _colors.find(normalize(str));
      
      if(colorMap == _colors.end()) {
        
        if(!create) {
          fprintf(stderr, "error: color \"%s\" not found cvl::colors\n", str.c_str());
          abort();
        }
        
        add(str, getRandom());
        
        return _colors[str];
        
      }
      
      return colorMap->second;
            
    }
    
    //****************************************************************************//
    // getHex()
    //****************************************************************************//
    static const std::string getHex(const std::string & str, bool create = false) { return BGRtoHex(get(str,create)); }
    

    //****************************************************************************//
    // add()
    //****************************************************************************//
    static void add(const std::string & name, int r, int g, int b) {
      _colors[name] = cv::Vec3b(b, g, r);
    }
    
    static void add(const std::string & name, const cv::Vec3b & color) {
      _colors[name] = color;
    }

    //****************************************************************************//
    // getRandom()
    //****************************************************************************//
    static const cv::Vec3b getRandom() {
      
      static std::default_random_engine generator;
      static std::uniform_real_distribution<float> distribution(0.0,1.0);
      
      return cv::Vec3b(distribution(generator), distribution(generator), distribution(generator));
      
    }
    
    //*****************************************************************************/
    // lista dei colori
    //*****************************************************************************/
    // https://htmlcolorcodes.com/color-names/
    //NOTE: i colori sono in BGR e non RGB
    constexpr static const cv::Vec3b aliceblue(255, 248, 240);
    constexpr static const cv::Vec3b antiquewhite(215, 235, 250);
    constexpr static const cv::Vec3b aqua(255, 255, 0);
    constexpr static const cv::Vec3b aquamarine(212, 255, 127);
    constexpr static const cv::Vec3b azure(255, 255, 240);
    constexpr static const cv::Vec3b beige(220, 245, 245);
    constexpr static const cv::Vec3b bisque(196, 228, 255);
    constexpr static const cv::Vec3b black(0, 0, 0);
    constexpr static const cv::Vec3b blanchedalmond(205, 235, 255);
    constexpr static const cv::Vec3b blue(255, 0, 0);
    constexpr static const cv::Vec3b blueviolet(226, 43, 138);
    constexpr static const cv::Vec3b brown(42, 42, 165);
    constexpr static const cv::Vec3b burlywood(135, 184, 222);
    constexpr static const cv::Vec3b cadetblue(160, 158, 95);
    constexpr static const cv::Vec3b chartreuse(0, 255, 127);
    constexpr static const cv::Vec3b chocolate(30, 105, 210);
    constexpr static const cv::Vec3b coral(80, 127, 255);
    constexpr static const cv::Vec3b cornflowerblue(237, 149, 100);
    constexpr static const cv::Vec3b cornsilk(220, 248, 255);
    constexpr static const cv::Vec3b crimson(60, 20, 220);
    constexpr static const cv::Vec3b cyan(255, 255, 0);
    constexpr static const cv::Vec3b darkblue(139, 0, 0);
    constexpr static const cv::Vec3b darkcyan(139, 139, 0);
    constexpr static const cv::Vec3b darkgoldenrod(11, 134, 184);
    constexpr static const cv::Vec3b darkgray(169, 169, 169);
    constexpr static const cv::Vec3b darkgreen(0, 100, 0);
    constexpr static const cv::Vec3b darkgrey(169, 169, 169);
    constexpr static const cv::Vec3b darkkhaki(107, 183, 189);
    constexpr static const cv::Vec3b darkmagenta(139, 0, 139);
    constexpr static const cv::Vec3b darkolivegreen(47, 107, 85);
    constexpr static const cv::Vec3b darkorange(0, 140, 255);
    constexpr static const cv::Vec3b darkorchid(204, 50, 153);
    constexpr static const cv::Vec3b darkred(0, 0, 139);
    constexpr static const cv::Vec3b darksalmon(122, 150, 233);
    constexpr static const cv::Vec3b darkseagreen(143, 188, 143);
    constexpr static const cv::Vec3b darkslateblue(139, 61, 72);
    constexpr static const cv::Vec3b darkslategray(79, 79, 47);
    constexpr static const cv::Vec3b darkslategrey(79, 79, 47);
    constexpr static const cv::Vec3b darkturquoise(209, 206, 0);
    constexpr static const cv::Vec3b darkviolet(211, 0, 148);
    constexpr static const cv::Vec3b deeppink(147, 20, 255);
    constexpr static const cv::Vec3b deepskyblue(255, 191, 0);
    constexpr static const cv::Vec3b dimgray(105, 105, 105);
    constexpr static const cv::Vec3b dimgrey(105, 105, 105);
    constexpr static const cv::Vec3b dodgerblue(255, 144, 30);
    constexpr static const cv::Vec3b firebrick(34, 34, 178);
    constexpr static const cv::Vec3b floralwhite(240, 250, 255);
    constexpr static const cv::Vec3b forestgreen(34, 139, 34);
    constexpr static const cv::Vec3b fuchsia(255, 0, 255);
    constexpr static const cv::Vec3b gainsboro(220, 220, 220);
    constexpr static const cv::Vec3b ghostwhite(255, 248, 248);
    constexpr static const cv::Vec3b gold(0, 215, 255);
    constexpr static const cv::Vec3b goldenrod(32, 165, 218);
    constexpr static const cv::Vec3b gray(128, 128, 128);
    constexpr static const cv::Vec3b green(0, 128, 0);
    constexpr static const cv::Vec3b greenyellow(47, 255, 173);
    constexpr static const cv::Vec3b grey(128, 128, 128);
    constexpr static const cv::Vec3b honeydew(240, 255, 240);
    constexpr static const cv::Vec3b hotpink(180, 105, 255);
    constexpr static const cv::Vec3b indianred(92, 92, 205);
    constexpr static const cv::Vec3b indigo(130, 0, 75);
    constexpr static const cv::Vec3b ivory(240, 255, 255);
    constexpr static const cv::Vec3b khaki(140, 230, 240);
    constexpr static const cv::Vec3b lavender(250, 230, 230);
    constexpr static const cv::Vec3b lavenderblush(245, 240, 255);
    constexpr static const cv::Vec3b lawngreen(0, 252, 124);
    constexpr static const cv::Vec3b lemonchiffon(205, 250, 255);
    constexpr static const cv::Vec3b lightblue(230, 216, 173);
    constexpr static const cv::Vec3b lightcoral(128, 128, 240);
    constexpr static const cv::Vec3b lightcyan(255, 255, 224);
    constexpr static const cv::Vec3b lightgoldenrodyellow(210, 250, 250);
    constexpr static const cv::Vec3b lightgray(211, 211, 211);
    constexpr static const cv::Vec3b lightgreen(144, 238, 144);
    constexpr static const cv::Vec3b lightgrey(211, 211, 211);
    constexpr static const cv::Vec3b lightpink(193, 182, 255);
    constexpr static const cv::Vec3b lightsalmon(122, 160, 255);
    constexpr static const cv::Vec3b lightseagreen(170, 178, 32);
    constexpr static const cv::Vec3b lightskyblue(250, 206, 135);
    constexpr static const cv::Vec3b lightslategray(153, 136, 119);
    constexpr static const cv::Vec3b lightslategrey(153, 136, 119);
    constexpr static const cv::Vec3b lightsteelblue(222, 196, 176);
    constexpr static const cv::Vec3b lightyellow(224, 255, 255);
    constexpr static const cv::Vec3b lime(0, 255, 0);
    constexpr static const cv::Vec3b limegreen(50, 205, 50);
    constexpr static const cv::Vec3b linen(230, 240, 250);
    constexpr static const cv::Vec3b magenta(255, 0, 255);
    constexpr static const cv::Vec3b maroon(0, 0, 128);
    constexpr static const cv::Vec3b mediumaquamarine(170, 205, 102);
    constexpr static const cv::Vec3b mediumblue(205, 0, 0);
    constexpr static const cv::Vec3b mediumorchid(211, 85, 186);
    constexpr static const cv::Vec3b mediumpurple(219, 112, 147);
    constexpr static const cv::Vec3b mediumseagreen(113, 179, 60);
    constexpr static const cv::Vec3b mediumslateblue(238, 104, 123);
    constexpr static const cv::Vec3b mediumspringgreen(154, 250, 0);
    constexpr static const cv::Vec3b mediumturquoise(204, 209, 72);
    constexpr static const cv::Vec3b mediumvioletred(133, 21, 199);
    constexpr static const cv::Vec3b midnightblue(112, 25, 25);
    constexpr static const cv::Vec3b mintcream(250, 255, 245);
    constexpr static const cv::Vec3b mistyrose(225, 228, 255);
    constexpr static const cv::Vec3b moccasin(181, 228, 255);
    constexpr static const cv::Vec3b navajowhite(173, 222, 255);
    constexpr static const cv::Vec3b navy(128, 0, 0);
    constexpr static const cv::Vec3b oldlace(230, 245, 253);
    constexpr static const cv::Vec3b olive(0, 128, 128);
    constexpr static const cv::Vec3b olivedrab(35, 142, 107);
    constexpr static const cv::Vec3b orange(0, 165, 255);
    constexpr static const cv::Vec3b orangered(0, 69, 255);
    constexpr static const cv::Vec3b orchid(214, 112, 218);
    constexpr static const cv::Vec3b palegoldenrod(170, 232, 238);
    constexpr static const cv::Vec3b palegreen(152, 251, 152);
    constexpr static const cv::Vec3b paleturquoise(238, 238, 175);
    constexpr static const cv::Vec3b palevioletred(147, 112, 219);
    constexpr static const cv::Vec3b papayawhip(213, 239, 255);
    constexpr static const cv::Vec3b peachpuff(185, 218, 255);
    constexpr static const cv::Vec3b peru(63, 133, 205);
    constexpr static const cv::Vec3b pink(203, 192, 255);
    constexpr static const cv::Vec3b plum(221, 160, 221);
    constexpr static const cv::Vec3b powderblue(230, 224, 176);
    constexpr static const cv::Vec3b purple(128, 0, 128);
    constexpr static const cv::Vec3b red(0, 0, 255);
    constexpr static const cv::Vec3b rosybrown(143, 143, 188);
    constexpr static const cv::Vec3b royalblue(225, 105, 65);
    constexpr static const cv::Vec3b saddlebrown(19, 69, 139);
    constexpr static const cv::Vec3b salmon(114, 128, 250);
    constexpr static const cv::Vec3b sandybrown(96, 164, 244);
    constexpr static const cv::Vec3b seagreen(87, 139, 46);
    constexpr static const cv::Vec3b seashell(238, 245, 255);
    constexpr static const cv::Vec3b sienna(45, 82, 160);
    constexpr static const cv::Vec3b silver(192, 192, 192);
    constexpr static const cv::Vec3b skyblue(235, 206, 135);
    constexpr static const cv::Vec3b slateblue(205, 90, 106);
    constexpr static const cv::Vec3b slategray(144, 128, 112);
    constexpr static const cv::Vec3b slategrey(144, 128, 112);
    constexpr static const cv::Vec3b snow(250, 250, 255);
    constexpr static const cv::Vec3b springgreen(127, 255, 0);
    constexpr static const cv::Vec3b steelblue(180, 130, 70);
    constexpr static const cv::Vec3b tan(140, 180, 210);
    constexpr static const cv::Vec3b teal(128, 128, 0);
    constexpr static const cv::Vec3b thistle(216, 191, 216);
    constexpr static const cv::Vec3b tomato(71, 99, 255);
    constexpr static const cv::Vec3b turquoise(208, 224, 64);
    constexpr static const cv::Vec3b violet(238, 130, 238);
    constexpr static const cv::Vec3b wheat(179, 222, 245);
    constexpr static const cv::Vec3b white(255, 255, 255);
    constexpr static const cv::Vec3b whitesmoke(245, 245, 245);
    constexpr static const cv::Vec3b yellow(0, 255, 255);
    constexpr static const cv::Vec3b yellowgreen(50, 205, 154);
    
    constexpr static const cv::Scalar Zero(0, 0, 0);
    constexpr static const cv::Scalar One(255, 255, 255);
    
  private:
    
    //****************************************************************************//
    // normalize()
    //****************************************************************************//
    static std::string normalize(const std::string & value) {
      
      std::string lowered;
      
      lowered.reserve(value.size());
      
      for (unsigned char ch : value) {
        lowered.push_back(static_cast<char>(std::tolower(ch)));
      }
      
      return lowered;
      
    }
    
  }; /* class glColors */

  std::map<std::string, glm::vec4> cvl::_colors = std::map<std::string, cv::Vec3b>();

} /* namespace cvl */

#endif
