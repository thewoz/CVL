/*
 * MIT License
 *
 * Copyright © 2019 CoBBS
 * Created by Leonardo Parisi (leonardo.parisi[at]gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
