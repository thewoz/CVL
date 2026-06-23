/*
 * GNU GENERAL PUBLIC LICENSE
 *
 * Copyright (C) 2021-2026
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

#include <cstdlib>
#include <cstdio>

#include <map>
#include <string>
#include <random>

#include <opencv2/opencv.hpp>

#include "utils.hpp"

//*****************************************************************************
// namespace cvl
//*****************************************************************************
namespace cvl {

  //*****************************************************************************/
  // lista dei colori
  //*****************************************************************************/
  // https://htmlcolorcodes.com/color-names/
  //NOTE: i colori sono in BGR e non RGB
  const cv::Vec3b aliceblue = cv::Vec3b(255, 248, 240);
  const cv::Vec3b antiquewhite = cv::Vec3b(215, 235, 250);
  const cv::Vec3b aqua = cv::Vec3b(255, 255, 0);
  const cv::Vec3b aquamarine = cv::Vec3b(212, 255, 127);
  const cv::Vec3b azure = cv::Vec3b(255, 255, 240);
  const cv::Vec3b beige = cv::Vec3b(220, 245, 245);
  const cv::Vec3b bisque = cv::Vec3b(196, 228, 255);
  const cv::Vec3b black = cv::Vec3b(0, 0, 0);
  const cv::Vec3b blanchedalmond = cv::Vec3b(205, 235, 255);
  const cv::Vec3b blue = cv::Vec3b(255, 0, 0);
  const cv::Vec3b blueviolet = cv::Vec3b(226, 43, 138);
  const cv::Vec3b brown = cv::Vec3b(42, 42, 165);
  const cv::Vec3b burlywood = cv::Vec3b(135, 184, 222);
  const cv::Vec3b cadetblue = cv::Vec3b(160, 158, 95);
  const cv::Vec3b chartreuse = cv::Vec3b(0, 255, 127);
  const cv::Vec3b chocolate = cv::Vec3b(30, 105, 210);
  const cv::Vec3b coral = cv::Vec3b(80, 127, 255);
  const cv::Vec3b cornflowerblue = cv::Vec3b(237, 149, 100);
  const cv::Vec3b cornsilk = cv::Vec3b(220, 248, 255);
  const cv::Vec3b crimson = cv::Vec3b(60, 20, 220);
  const cv::Vec3b cyan = cv::Vec3b(255, 255, 0);
  const cv::Vec3b darkblue = cv::Vec3b(139, 0, 0);
  const cv::Vec3b darkcyan = cv::Vec3b(139, 139, 0);
  const cv::Vec3b darkgoldenrod = cv::Vec3b(11, 134, 184);
  const cv::Vec3b darkgray = cv::Vec3b(169, 169, 169);
  const cv::Vec3b darkgreen = cv::Vec3b(0, 100, 0);
  const cv::Vec3b darkgrey = cv::Vec3b(169, 169, 169);
  const cv::Vec3b darkkhaki = cv::Vec3b(107, 183, 189);
  const cv::Vec3b darkmagenta = cv::Vec3b(139, 0, 139);
  const cv::Vec3b darkolivegreen = cv::Vec3b(47, 107, 85);
  const cv::Vec3b darkorange = cv::Vec3b(0, 140, 255);
  const cv::Vec3b darkorchid = cv::Vec3b(204, 50, 153);
  const cv::Vec3b darkred = cv::Vec3b(0, 0, 139);
  const cv::Vec3b darksalmon = cv::Vec3b(122, 150, 233);
  const cv::Vec3b darkseagreen = cv::Vec3b(143, 188, 143);
  const cv::Vec3b darkslateblue = cv::Vec3b(139, 61, 72);
  const cv::Vec3b darkslategray = cv::Vec3b(79, 79, 47);
  const cv::Vec3b darkslategrey = cv::Vec3b(79, 79, 47);
  const cv::Vec3b darkturquoise = cv::Vec3b(209, 206, 0);
  const cv::Vec3b darkviolet = cv::Vec3b(211, 0, 148);
  const cv::Vec3b deeppink = cv::Vec3b(147, 20, 255);
  const cv::Vec3b deepskyblue = cv::Vec3b(255, 191, 0);
  const cv::Vec3b dimgray = cv::Vec3b(105, 105, 105);
  const cv::Vec3b dimgrey = cv::Vec3b(105, 105, 105);
  const cv::Vec3b dodgerblue = cv::Vec3b(255, 144, 30);
  const cv::Vec3b firebrick = cv::Vec3b(34, 34, 178);
  const cv::Vec3b floralwhite = cv::Vec3b(240, 250, 255);
  const cv::Vec3b forestgreen = cv::Vec3b(34, 139, 34);
  const cv::Vec3b fuchsia = cv::Vec3b(255, 0, 255);
  const cv::Vec3b gainsboro = cv::Vec3b(220, 220, 220);
  const cv::Vec3b ghostwhite = cv::Vec3b(255, 248, 248);
  const cv::Vec3b gold = cv::Vec3b(0, 215, 255);
  const cv::Vec3b goldenrod = cv::Vec3b(32, 165, 218);
  const cv::Vec3b gray = cv::Vec3b(128, 128, 128);
  const cv::Vec3b green = cv::Vec3b(0, 128, 0);
  const cv::Vec3b greenyellow = cv::Vec3b(47, 255, 173);
  const cv::Vec3b grey = cv::Vec3b(128, 128, 128);
  const cv::Vec3b honeydew = cv::Vec3b(240, 255, 240);
  const cv::Vec3b hotpink = cv::Vec3b(180, 105, 255);
  const cv::Vec3b indianred = cv::Vec3b(92, 92, 205);
  const cv::Vec3b indigo = cv::Vec3b(130, 0, 75);
  const cv::Vec3b ivory = cv::Vec3b(240, 255, 255);
  const cv::Vec3b khaki = cv::Vec3b(140, 230, 240);
  const cv::Vec3b lavender = cv::Vec3b(250, 230, 230);
  const cv::Vec3b lavenderblush = cv::Vec3b(245, 240, 255);
  const cv::Vec3b lawngreen = cv::Vec3b(0, 252, 124);
  const cv::Vec3b lemonchiffon = cv::Vec3b(205, 250, 255);
  const cv::Vec3b lightblue = cv::Vec3b(230, 216, 173);
  const cv::Vec3b lightcoral = cv::Vec3b(128, 128, 240);
  const cv::Vec3b lightcyan = cv::Vec3b(255, 255, 224);
  const cv::Vec3b lightgoldenrodyellow = cv::Vec3b(210, 250, 250);
  const cv::Vec3b lightgray = cv::Vec3b(211, 211, 211);
  const cv::Vec3b lightgreen = cv::Vec3b(144, 238, 144);
  const cv::Vec3b lightgrey = cv::Vec3b(211, 211, 211);
  const cv::Vec3b lightpink = cv::Vec3b(193, 182, 255);
  const cv::Vec3b lightsalmon = cv::Vec3b(122, 160, 255);
  const cv::Vec3b lightseagreen = cv::Vec3b(170, 178, 32);
  const cv::Vec3b lightskyblue = cv::Vec3b(250, 206, 135);
  const cv::Vec3b lightslategray = cv::Vec3b(153, 136, 119);
  const cv::Vec3b lightslategrey = cv::Vec3b(153, 136, 119);
  const cv::Vec3b lightsteelblue = cv::Vec3b(222, 196, 176);
  const cv::Vec3b lightyellow = cv::Vec3b(224, 255, 255);
  const cv::Vec3b lime = cv::Vec3b(0, 255, 0);
  const cv::Vec3b limegreen = cv::Vec3b(50, 205, 50);
  const cv::Vec3b linen = cv::Vec3b(230, 240, 250);
  const cv::Vec3b magenta = cv::Vec3b(255, 0, 255);
  const cv::Vec3b maroon = cv::Vec3b(0, 0, 128);
  const cv::Vec3b mediumaquamarine = cv::Vec3b(170, 205, 102);
  const cv::Vec3b mediumblue = cv::Vec3b(205, 0, 0);
  const cv::Vec3b mediumorchid = cv::Vec3b(211, 85, 186);
  const cv::Vec3b mediumpurple = cv::Vec3b(219, 112, 147);
  const cv::Vec3b mediumseagreen = cv::Vec3b(113, 179, 60);
  const cv::Vec3b mediumslateblue = cv::Vec3b(238, 104, 123);
  const cv::Vec3b mediumspringgreen = cv::Vec3b(154, 250, 0);
  const cv::Vec3b mediumturquoise = cv::Vec3b(204, 209, 72);
  const cv::Vec3b mediumvioletred = cv::Vec3b(133, 21, 199);
  const cv::Vec3b midnightblue = cv::Vec3b(112, 25, 25);
  const cv::Vec3b mintcream = cv::Vec3b(250, 255, 245);
  const cv::Vec3b mistyrose = cv::Vec3b(225, 228, 255);
  const cv::Vec3b moccasin = cv::Vec3b(181, 228, 255);
  const cv::Vec3b navajowhite = cv::Vec3b(173, 222, 255);
  const cv::Vec3b navy = cv::Vec3b(128, 0, 0);
  const cv::Vec3b oldlace = cv::Vec3b(230, 245, 253);
  const cv::Vec3b olive = cv::Vec3b(0, 128, 128);
  const cv::Vec3b olivedrab = cv::Vec3b(35, 142, 107);
  const cv::Vec3b orange = cv::Vec3b(0, 165, 255);
  const cv::Vec3b orangered = cv::Vec3b(0, 69, 255);
  const cv::Vec3b orchid = cv::Vec3b(214, 112, 218);
  const cv::Vec3b palegoldenrod = cv::Vec3b(170, 232, 238);
  const cv::Vec3b palegreen = cv::Vec3b(152, 251, 152);
  const cv::Vec3b paleturquoise = cv::Vec3b(238, 238, 175);
  const cv::Vec3b palevioletred = cv::Vec3b(147, 112, 219);
  const cv::Vec3b papayawhip = cv::Vec3b(213, 239, 255);
  const cv::Vec3b peachpuff = cv::Vec3b(185, 218, 255);
  const cv::Vec3b peru = cv::Vec3b(63, 133, 205);
  const cv::Vec3b pink = cv::Vec3b(203, 192, 255);
  const cv::Vec3b plum = cv::Vec3b(221, 160, 221);
  const cv::Vec3b powderblue = cv::Vec3b(230, 224, 176);
  const cv::Vec3b purple = cv::Vec3b(128, 0, 128);
  const cv::Vec3b red = cv::Vec3b(0, 0, 255);
  const cv::Vec3b rosybrown = cv::Vec3b(143, 143, 188);
  const cv::Vec3b royalblue = cv::Vec3b(225, 105, 65);
  const cv::Vec3b saddlebrown = cv::Vec3b(19, 69, 139);
  const cv::Vec3b salmon = cv::Vec3b(114, 128, 250);
  const cv::Vec3b sandybrown = cv::Vec3b(96, 164, 244);
  const cv::Vec3b seagreen = cv::Vec3b(87, 139, 46);
  const cv::Vec3b seashell = cv::Vec3b(238, 245, 255);
  const cv::Vec3b sienna = cv::Vec3b(45, 82, 160);
  const cv::Vec3b silver = cv::Vec3b(192, 192, 192);
  const cv::Vec3b skyblue = cv::Vec3b(235, 206, 135);
  const cv::Vec3b slateblue = cv::Vec3b(205, 90, 106);
  const cv::Vec3b slategray = cv::Vec3b(144, 128, 112);
  const cv::Vec3b slategrey = cv::Vec3b(144, 128, 112);
  const cv::Vec3b snow = cv::Vec3b(250, 250, 255);
  const cv::Vec3b springgreen = cv::Vec3b(127, 255, 0);
  const cv::Vec3b steelblue = cv::Vec3b(180, 130, 70);
  const cv::Vec3b tan = cv::Vec3b(140, 180, 210);
  const cv::Vec3b teal = cv::Vec3b(128, 128, 0);
  const cv::Vec3b thistle = cv::Vec3b(216, 191, 216);
  const cv::Vec3b tomato = cv::Vec3b(71, 99, 255);
  const cv::Vec3b turquoise = cv::Vec3b(208, 224, 64);
  const cv::Vec3b violet = cv::Vec3b(238, 130, 238);
  const cv::Vec3b wheat = cv::Vec3b(179, 222, 245);
  const cv::Vec3b white = cv::Vec3b(255, 255, 255);
  const cv::Vec3b whitesmoke = cv::Vec3b(245, 245, 245);
  const cv::Vec3b yellow = cv::Vec3b(0, 255, 255);
  const cv::Vec3b yellowgreen = cv::Vec3b(50, 205, 154);

  const cv::Scalar Zero = cv::Scalar(0, 0, 0);
  const cv::Scalar One = cv::Scalar(255, 255, 255);


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
    // loadPalette()
    //****************************************************************************//
    // Fill the name->color map with the built-in palette.
    // https://htmlcolorcodes.com/color-names/
    static void loadPalette() {
      
      _colors.clear();
      _colors["aliceblue"] = aliceblue;
      _colors["antiquewhite"] = antiquewhite;
      _colors["aqua"] = aqua;
      _colors["aquamarine"] = aquamarine;
      _colors["azure"] = azure;
      _colors["beige"] = beige;
      _colors["bisque"] = bisque;
      _colors["black"] = black;
      _colors["blanchedalmond"] = blanchedalmond;
      _colors["blue"] = blue;
      _colors["blueviolet"] = blueviolet;
      _colors["brown"] = brown;
      _colors["burlywood"] = burlywood;
      _colors["cadetblue"] = cadetblue;
      _colors["chartreuse"] = chartreuse;
      _colors["chocolate"] = chocolate;
      _colors["coral"] = coral;
      _colors["cornflowerblue"] = cornflowerblue;
      _colors["cornsilk"] = cornsilk;
      _colors["crimson"] = crimson;
      _colors["cyan"] = cyan;
      _colors["darkblue"] = darkblue;
      _colors["darkcyan"] = darkcyan;
      _colors["darkgoldenrod"] = darkgoldenrod;
      _colors["darkgray"] = darkgray;
      _colors["darkgreen"] = darkgreen;
      _colors["darkgrey"] = darkgrey;
      _colors["darkkhaki"] = darkkhaki;
      _colors["darkmagenta"] = darkmagenta;
      _colors["darkolivegreen"] = darkolivegreen;
      _colors["darkorange"] = darkorange;
      _colors["darkorchid"] = darkorchid;
      _colors["darkred"] = darkred;
      _colors["darksalmon"] = darksalmon;
      _colors["darkseagreen"] = darkseagreen;
      _colors["darkslateblue"] = darkslateblue;
      _colors["darkslategray"] = darkslategray;
      _colors["darkslategrey"] = darkslategrey;
      _colors["darkturquoise"] = darkturquoise;
      _colors["darkviolet"] = darkviolet;
      _colors["deeppink"] = deeppink;
      _colors["deepskyblue"] = deepskyblue;
      _colors["dimgray"] = dimgray;
      _colors["dimgrey"] = dimgrey;
      _colors["dodgerblue"] = dodgerblue;
      _colors["firebrick"] = firebrick;
      _colors["floralwhite"] = floralwhite;
      _colors["forestgreen"] = forestgreen;
      _colors["fuchsia"] = fuchsia;
      _colors["gainsboro"] = gainsboro;
      _colors["ghostwhite"] = ghostwhite;
      _colors["gold"] = gold;
      _colors["goldenrod"] = goldenrod;
      _colors["gray"] = gray;
      _colors["green"] = green;
      _colors["greenyellow"] = greenyellow;
      _colors["grey"] = grey;
      _colors["honeydew"] = honeydew;
      _colors["hotpink"] = hotpink;
      _colors["indianred"] = indianred;
      _colors["indigo"] = indigo;
      _colors["ivory"] = ivory;
      _colors["khaki"] = khaki;
      _colors["lavender"] = lavender;
      _colors["lavenderblush"] = lavenderblush;
      _colors["lawngreen"] = lawngreen;
      _colors["lemonchiffon"] = lemonchiffon;
      _colors["lightblue"] = lightblue;
      _colors["lightcoral"] = lightcoral;
      _colors["lightcyan"] = lightcyan;
      _colors["lightgoldenrodyellow"] = lightgoldenrodyellow;
      _colors["lightgray"] = lightgray;
      _colors["lightgreen"] = lightgreen;
      _colors["lightgrey"] = lightgrey;
      _colors["lightpink"] = lightpink;
      _colors["lightsalmon"] = lightsalmon;
      _colors["lightseagreen"] = lightseagreen;
      _colors["lightskyblue"] = lightskyblue;
      _colors["lightslategray"] = lightslategray;
      _colors["lightslategrey"] = lightslategrey;
      _colors["lightsteelblue"] = lightsteelblue;
      _colors["lightyellow"] = lightyellow;
      _colors["lime"] = lime;
      _colors["limegreen"] = limegreen;
      _colors["linen"] = linen;
      _colors["magenta"] = magenta;
      _colors["maroon"] = maroon;
      _colors["mediumaquamarine"] = mediumaquamarine;
      _colors["mediumblue"] = mediumblue;
      _colors["mediumorchid"] = mediumorchid;
      _colors["mediumpurple"] = mediumpurple;
      _colors["mediumseagreen"] = mediumseagreen;
      _colors["mediumslateblue"] = mediumslateblue;
      _colors["mediumspringgreen"] = mediumspringgreen;
      _colors["mediumturquoise"] = mediumturquoise;
      _colors["mediumvioletred"] = mediumvioletred;
      _colors["midnightblue"] = midnightblue;
      _colors["mintcream"] = mintcream;
      _colors["mistyrose"] = mistyrose;
      _colors["moccasin"] = moccasin;
      _colors["navajowhite"] = navajowhite;
      _colors["navy"] = navy;
      _colors["oldlace"] = oldlace;
      _colors["olive"] = olive;
      _colors["olivedrab"] = olivedrab;
      _colors["orange"] = orange;
      _colors["orangered"] = orangered;
      _colors["orchid"] = orchid;
      _colors["palegoldenrod"] = palegoldenrod;
      _colors["palegreen"] = palegreen;
      _colors["paleturquoise"] = paleturquoise;
      _colors["palevioletred"] = palevioletred;
      _colors["papayawhip"] = papayawhip;
      _colors["peachpuff"] = peachpuff;
      _colors["peru"] = peru;
      _colors["pink"] = pink;
      _colors["plum"] = plum;
      _colors["powderblue"] = powderblue;
      _colors["purple"] = purple;
      _colors["red"] = red;
      _colors["rosybrown"] = rosybrown;
      _colors["royalblue"] = royalblue;
      _colors["saddlebrown"] = saddlebrown;
      _colors["salmon"] = salmon;
      _colors["sandybrown"] = sandybrown;
      _colors["seagreen"] = seagreen;
      _colors["seashell"] = seashell;
      _colors["sienna"] = sienna;
      _colors["silver"] = silver;
      _colors["skyblue"] = skyblue;
      _colors["slateblue"] = slateblue;
      _colors["slategray"] = slategray;
      _colors["slategrey"] = slategrey;
      _colors["snow"] = snow;
      _colors["springgreen"] = springgreen;
      _colors["steelblue"] = steelblue;
      _colors["tan"] = tan;
      _colors["teal"] = teal;
      _colors["thistle"] = thistle;
      _colors["tomato"] = tomato;
      _colors["turquoise"] = turquoise;
      _colors["violet"] = violet;
      _colors["wheat"] = wheat;
      _colors["white"] = white;
      _colors["whitesmoke"] = whitesmoke;
      _colors["yellow"] = yellow;
      _colors["yellowgreen"] = yellowgreen;
          
    }
    
    //*****************************************************************************/
    // RGBtoHex:
    // Function to convert the RGB code to Hex color code
    // Convert RGB values to a hex color string (e.g., #RRGGBB).
    //*****************************************************************************/
    static std::string RGBtoHex(int R, int G, int B) {
      
      if((R >= 0 && R <= 255) && (G >= 0 && G <= 255) && (B >= 0 && B <= 255)) {
     
        std::string hexCode = "#";
                    hexCode += cvl::utils::decToHex(R);
                    hexCode += cvl::utils::decToHex(G);
                    hexCode += cvl::utils::decToHex(B);
     
        return hexCode;
        
      } else {
        cvl::log::error("cvl::colors::RGBtoHex() error: RGB values out of range\n");
        return "";
      }
      
    }
    
    static std::string RGBtoHex(const cv::Vec3b & color) { return RGBtoHex(color[0], color[1], color[2]); }

    
    //*****************************************************************************/
    // BGRtoHex()
    // Function to convert the BGR code to Hex color code
    // Convert BGR values to a hex color string (e.g., #RRGGBB).
    //*****************************************************************************/
    static std::string BGRtoHex(int B, int G, int R) {
      
      if((B >= 0 && B <= 255) && (G >= 0 && G <= 255) && (R >= 0 && R <= 255)) {

        std::string hexCode = "#";
                    hexCode += cvl::utils::decToHex(R);
                    hexCode += cvl::utils::decToHex(G);
                    hexCode += cvl::utils::decToHex(B);
     
        return hexCode;
        
      } else  {
        cvl::log::error("cvl::colors::BGRtoHex() error: BGR values out of range\n");
        return "";
      }
      
    }
    
    static std::string BGRtoHex(const cv::Vec3b & color) { return BGRtoHex(color[0], color[1], color[2]); }
    
    //****************************************************************************//
    // get()
    //****************************************************************************//
    // Look up a color by name; with create=true a missing name is added as random.
    static const cv::Vec3b & get(const std::string & str, bool create = false) {
   
      auto colorMap = _colors.find(normalize(str));
      
      if(colorMap == _colors.end()) {
        
        if(!create) {
          cvl::log::error("cvl::colors::get() error: color '%s' not found\n", str.c_str());
        }
        
        add(str, getRandom());
        
        return _colors[normalize(str)];
        
      }
      
      return colorMap->second;
            
    }
    
    //****************************************************************************//
    // getRGBHex()
    //****************************************************************************//
    // Return the named color as an #RRGGBB hex string.
    static const std::string getRGBHex(const std::string & str, bool create = false) { return RGBtoHex(get(str,create)); }
    

    //****************************************************************************//
    // add()
    //****************************************************************************//
    // Register a color in the palette (RGB args are stored as BGR).
    static void add(const std::string & name, int r, int g, int b) {
      _colors[normalize(name)] = cv::Vec3b(b, g, r);
    }
    
    static void add(const std::string & name, const cv::Vec3b & color) {
      _colors[normalize(name)] = color;
    }

    //****************************************************************************//
    // getRandom()
    //****************************************************************************//
    // Return a random BGR color.
    static const cv::Vec3b getRandom() {

      static std::default_random_engine generator;
      static std::uniform_int_distribution<int> distribution(0,255);

      return cv::Vec3b(distribution(generator), distribution(generator), distribution(generator));

    }
        
  private:
    
    //****************************************************************************//
    // normalize()
    //****************************************************************************//
    // Lower-case a color name for case-insensitive lookup.
    static std::string normalize(const std::string & value) {
      
      std::string lowered;
      
      lowered.reserve(value.size());
      
      for(unsigned char ch : value) {
        lowered.push_back((char)(std::tolower(ch)));
      }
      
      return lowered;
      
    }
    
  }; /* class glColors */

  inline std::map<std::string, cv::Vec3b> cvl::colors::_colors = std::map<std::string, cv::Vec3b>();

} /* namespace cvl */

#endif
