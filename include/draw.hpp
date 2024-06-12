/*
 * GNU GENERAL PUBLIC LICENSE
 *
 * Copyright (C) 2019
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

#ifndef _H_CVL_DRAW_H_
#define _H_CVL_DRAW_H_

#include <cstdlib>
#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>

//****************************************************************************/
// namespace
//****************************************************************************/
namespace cv {

  //****************************************************************************/
  // dashedCircle() - ChatGPT
  //****************************************************************************/
  void dashedCircle(cv::Mat & image, cv::Point center, int radius, int dashWidth, int dashSpacing, const cv::Scalar & color, int thickness = 1) {

    const double PI = 3.14159265358979323846;
    double circumference = 2 * PI * radius;
    int numSegments = static_cast<int>(circumference / (dashWidth + dashSpacing));
    
    double angleStep = 2 * PI / numSegments;
    double dashAngle = angleStep * dashWidth / (dashWidth + dashSpacing);
    
    for (int i = 0; i < numSegments; ++i) {
      
      double theta1 = i * angleStep;
      double theta2 = theta1 + dashAngle;
      
      cv::Point pt1(center.x + radius * cos(theta1), center.y + radius * sin(theta1));
      cv::Point pt2(center.x + radius * cos(theta2), center.y + radius * sin(theta2));
      
      cv::line(image, pt1, pt2, color, thickness, cv::LINE_AA);
      
    }
    
  }
  
} /* namespace */

#endif /* _H_CVL_DRAW_H_ */

