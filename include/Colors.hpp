/*
 * GNU GENERAL PUBLIC LICENSE
 *
 * Copyright (C) 2017
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

#include <opencv2/core/types.hpp>

namespace cvl {

  // NOTE: in OpenCV the colors are in BGR order.
  const cv::Vec3b Red(0, 0, 255);
  const cv::Vec3b Blue(255, 0, 0);
  const cv::Vec3b Green(0, 255, 0);
  const cv::Vec3b Yellow(0, 255, 255);
  const cv::Vec3b Magenta(255, 0, 255);
  const cv::Vec3b White(255, 255, 255);
  const cv::Vec3b Black(0, 0, 0);
  const cv::Vec3b LightGrey(211, 211, 211);
  const cv::Vec3b darkslategray4(59, 122, 121);

  // OGL glColors palette converted from RGB [0..1] to OpenCV BGR [0..255].
  const cv::Vec3b aliceblue(255, 248, 240);
  const cv::Vec3b antiquewhite(215, 235, 250);
  const cv::Vec3b aqua(255, 255, 0);
  const cv::Vec3b aquamarine(212, 255, 127);
  const cv::Vec3b azure(255, 255, 240);
  const cv::Vec3b beige(220, 245, 245);
  const cv::Vec3b bisque(196, 228, 255);
  const cv::Vec3b black(0, 0, 0);
  const cv::Vec3b blanchedalmond(205, 235, 255);
  const cv::Vec3b blue(255, 0, 0);
  const cv::Vec3b blueviolet(226, 43, 138);
  const cv::Vec3b brown(42, 42, 165);
  const cv::Vec3b burlywood(135, 184, 222);
  const cv::Vec3b cadetblue(160, 158, 95);
  const cv::Vec3b chartreuse(0, 255, 127);
  const cv::Vec3b chocolate(30, 105, 210);
  const cv::Vec3b coral(80, 127, 255);
  const cv::Vec3b cornflowerblue(237, 149, 100);
  const cv::Vec3b cornsilk(220, 248, 255);
  const cv::Vec3b crimson(60, 20, 220);
  const cv::Vec3b cyan(255, 255, 0);
  const cv::Vec3b darkblue(139, 0, 0);
  const cv::Vec3b darkcyan(139, 139, 0);
  const cv::Vec3b darkgoldenrod(11, 134, 184);
  const cv::Vec3b darkgray(169, 169, 169);
  const cv::Vec3b darkgreen(0, 100, 0);
  const cv::Vec3b darkgrey(169, 169, 169);
  const cv::Vec3b darkkhaki(107, 183, 189);
  const cv::Vec3b darkmagenta(139, 0, 139);
  const cv::Vec3b darkolivegreen(47, 107, 85);
  const cv::Vec3b darkorange(0, 140, 255);
  const cv::Vec3b darkorchid(204, 50, 153);
  const cv::Vec3b darkred(0, 0, 139);
  const cv::Vec3b darksalmon(122, 150, 233);
  const cv::Vec3b darkseagreen(143, 188, 143);
  const cv::Vec3b darkslateblue(139, 61, 72);
  const cv::Vec3b darkslategray(79, 79, 47);
  const cv::Vec3b darkslategrey(79, 79, 47);
  const cv::Vec3b darkturquoise(209, 206, 0);
  const cv::Vec3b darkviolet(211, 0, 148);
  const cv::Vec3b deeppink(147, 20, 255);
  const cv::Vec3b deepskyblue(255, 191, 0);
  const cv::Vec3b dimgray(105, 105, 105);
  const cv::Vec3b dimgrey(105, 105, 105);
  const cv::Vec3b dodgerblue(255, 144, 30);
  const cv::Vec3b firebrick(34, 34, 178);
  const cv::Vec3b floralwhite(240, 250, 255);
  const cv::Vec3b forestgreen(34, 139, 34);
  const cv::Vec3b fuchsia(255, 0, 255);
  const cv::Vec3b gainsboro(220, 220, 220);
  const cv::Vec3b ghostwhite(255, 248, 248);
  const cv::Vec3b gold(0, 215, 255);
  const cv::Vec3b goldenrod(32, 165, 218);
  const cv::Vec3b gray(128, 128, 128);
  const cv::Vec3b green(0, 128, 0);
  const cv::Vec3b greenyellow(47, 255, 173);
  const cv::Vec3b grey(128, 128, 128);
  const cv::Vec3b honeydew(240, 255, 240);
  const cv::Vec3b hotpink(180, 105, 255);
  const cv::Vec3b indianred(92, 92, 205);
  const cv::Vec3b indigo(130, 0, 75);
  const cv::Vec3b ivory(240, 255, 255);
  const cv::Vec3b khaki(140, 230, 240);
  const cv::Vec3b lavender(250, 230, 230);
  const cv::Vec3b lavenderblush(245, 240, 255);
  const cv::Vec3b lawngreen(0, 252, 124);
  const cv::Vec3b lemonchiffon(205, 250, 255);
  const cv::Vec3b lightblue(230, 216, 173);
  const cv::Vec3b lightcoral(128, 128, 240);
  const cv::Vec3b lightcyan(255, 255, 224);
  const cv::Vec3b lightgoldenrodyellow(210, 250, 250);
  const cv::Vec3b lightgray(211, 211, 211);
  const cv::Vec3b lightgreen(144, 238, 144);
  const cv::Vec3b lightgrey(211, 211, 211);
  const cv::Vec3b lightpink(193, 182, 255);
  const cv::Vec3b lightsalmon(122, 160, 255);
  const cv::Vec3b lightseagreen(170, 178, 32);
  const cv::Vec3b lightskyblue(250, 206, 135);
  const cv::Vec3b lightslategray(153, 136, 119);
  const cv::Vec3b lightslategrey(153, 136, 119);
  const cv::Vec3b lightsteelblue(222, 196, 176);
  const cv::Vec3b lightyellow(224, 255, 255);
  const cv::Vec3b lime(0, 255, 0);
  const cv::Vec3b limegreen(50, 205, 50);
  const cv::Vec3b linen(230, 240, 250);
  const cv::Vec3b magenta(255, 0, 255);
  const cv::Vec3b maroon(0, 0, 128);
  const cv::Vec3b mediumaquamarine(170, 205, 102);
  const cv::Vec3b mediumblue(205, 0, 0);
  const cv::Vec3b mediumorchid(211, 85, 186);
  const cv::Vec3b mediumpurple(219, 112, 147);
  const cv::Vec3b mediumseagreen(113, 179, 60);
  const cv::Vec3b mediumslateblue(238, 104, 123);
  const cv::Vec3b mediumspringgreen(154, 250, 0);
  const cv::Vec3b mediumturquoise(204, 209, 72);
  const cv::Vec3b mediumvioletred(133, 21, 199);
  const cv::Vec3b midnightblue(112, 25, 25);
  const cv::Vec3b mintcream(250, 255, 245);
  const cv::Vec3b mistyrose(225, 228, 255);
  const cv::Vec3b moccasin(181, 228, 255);
  const cv::Vec3b navajowhite(173, 222, 255);
  const cv::Vec3b navy(128, 0, 0);
  const cv::Vec3b oldlace(230, 245, 253);
  const cv::Vec3b olive(0, 128, 128);
  const cv::Vec3b olivedrab(35, 142, 107);
  const cv::Vec3b orange(0, 165, 255);
  const cv::Vec3b orangered(0, 69, 255);
  const cv::Vec3b orchid(214, 112, 218);
  const cv::Vec3b palegoldenrod(170, 232, 238);
  const cv::Vec3b palegreen(152, 251, 152);
  const cv::Vec3b paleturquoise(238, 238, 175);
  const cv::Vec3b palevioletred(147, 112, 219);
  const cv::Vec3b papayawhip(213, 239, 255);
  const cv::Vec3b peachpuff(185, 218, 255);
  const cv::Vec3b peru(63, 133, 205);
  const cv::Vec3b pink(203, 192, 255);
  const cv::Vec3b plum(221, 160, 221);
  const cv::Vec3b powderblue(230, 224, 176);
  const cv::Vec3b purple(128, 0, 128);
  const cv::Vec3b red(0, 0, 255);
  const cv::Vec3b rosybrown(143, 143, 188);
  const cv::Vec3b royalblue(225, 105, 65);
  const cv::Vec3b saddlebrown(19, 69, 139);
  const cv::Vec3b salmon(114, 128, 250);
  const cv::Vec3b sandybrown(96, 164, 244);
  const cv::Vec3b seagreen(87, 139, 46);
  const cv::Vec3b seashell(238, 245, 255);
  const cv::Vec3b sienna(45, 82, 160);
  const cv::Vec3b silver(192, 192, 192);
  const cv::Vec3b skyblue(235, 206, 135);
  const cv::Vec3b slateblue(205, 90, 106);
  const cv::Vec3b slategray(144, 128, 112);
  const cv::Vec3b slategrey(144, 128, 112);
  const cv::Vec3b snow(250, 250, 255);
  const cv::Vec3b springgreen(127, 255, 0);
  const cv::Vec3b steelblue(180, 130, 70);
  const cv::Vec3b tan(140, 180, 210);
  const cv::Vec3b teal(128, 128, 0);
  const cv::Vec3b thistle(216, 191, 216);
  const cv::Vec3b tomato(71, 99, 255);
  const cv::Vec3b turquoise(208, 224, 64);
  const cv::Vec3b violet(238, 130, 238);
  const cv::Vec3b wheat(179, 222, 245);
  const cv::Vec3b white(255, 255, 255);
  const cv::Vec3b whitesmoke(245, 245, 245);
  const cv::Vec3b yellow(0, 255, 255);
  const cv::Vec3b yellowgreen(50, 205, 154);

  const cv::Scalar Zero(0, 0, 0);
  const cv::Scalar One(255, 255, 255);

}

#endif
