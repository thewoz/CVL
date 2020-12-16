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

#ifndef _H_CVL_FEATURES_H_
#define _H_CVL_FEATURES_H_

#include <cstdio>
#include <cstdlib>

#include <vector>

#include <opencv2/opencv.hpp>


//****************************************************************************/
// cvl::features
//****************************************************************************/
namespace cvl::features {
  
  //****************************************************************************/
  // findCircles
  //****************************************************************************/
  std::vector<cv::Vec3f> findCircles(cv::Mat & image, double minDist, double higherThreshold = 100, double accumulatorThreshold = 100, double minRadius = 0, double maxRadius = 0) {
    
    std::vector<cv::Vec3f> circles;
    
    cv::HoughCircles(image, circles, cv::HOUGH_GRADIENT, 1, minDist, higherThreshold, accumulatorThreshold, minRadius, maxRadius);
    
    return circles;
    
  }
  
} /* namespace cvl::features */

#endif /* _H_CVL_FEATURES_H_ */
