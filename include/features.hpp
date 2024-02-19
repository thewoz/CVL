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

#include <cvl/subpixelNew.hpp>


//****************************************************************************/
// cvl::features
//****************************************************************************/
namespace cvl::features {
  
  //****************************************************************************/
  // findCircles
  //****************************************************************************/
  std::vector<cv::Vec3f> findCircles(cv::Mat & image, double minDist, double higherThreshold = 100, double accumulatorThreshold = 100, double minRadius = 0, double maxRadius = 0) {
    
    cv::namedWindow("detected circles", cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL);

    std::vector<cv::Vec3f> circles;
    //std::vector<cv::Vec3f> circles = cvl::features::findCircles(image, image.rows/20, 100, 23, 10, 30);

    minDist = 100;
    minRadius = 10; minRadius = 100;
    maxRadius = 30; maxRadius = 200;
    higherThreshold = 100; higherThreshold = 650;
    accumulatorThreshold = 23; accumulatorThreshold = 50;
    
    //cv::imshow("detected circles", image); cv::waitKey();

    //cv::threshold(image, image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    
    //cv::Canny(image, image, 0, 650, 3, false);

    cv::imshow("detected circles", image); cv::waitKey();

    
    cv::HoughCircles(image, circles, cv::HOUGH_GRADIENT, 1, minDist, higherThreshold, accumulatorThreshold, minRadius, maxRadius);
    
    printf("circles.size() %lu\n", circles.size()); fflush(stdout);
    cvtColor(image,image,cv::COLOR_GRAY2RGB);

    for( size_t i = 0; i < circles.size(); i++ )
     {
         cv::Vec3i c = circles[i];
       cv::Point center = cv::Point(c[0], c[1]);
         // circle center
       cv::circle( image, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
         // circle outline
         int radius = c[2];
       cv::circle( image, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);
     }
    cv::imshow("detected circles", image); cv::waitKey();
    
    return circles;
    
  }


  // //****************************************************************************/
  // // subpixelNew
  // //****************************************************************************/
  void cornerSubPixNew(const cv::Mat image, std::vector<cv::Point2f> & corners, int roiSize = 60){

    for(auto & corner : corners){

      int x = corner.x - roiSize/2;  // Centro della maschera      
      int y = corner.y - roiSize/2;

      cv::Rect roi(x, y, roiSize, roiSize);

      cv::Mat subImage = image(roi);

      // cv::namedWindow("check");
      // cv::imshow("check",subImage);
      // cv::waitKey(0);

      corner = cornerSubPixNewImpl(subImage);

      if(corner.x != 0 || corner.y != 0){
        corner.x += roi.x; // mi riporto nel sistema di riferimento di image
        corner.y += roi.y;
      }

    } 
  }
  
} /* namespace cvl::features */

#endif /* _H_CVL_FEATURES_H_ */
