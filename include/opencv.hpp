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

#ifndef _H_CVL_OPENCV_H_
#define _H_CVL_OPENCV_H_

#include <cstdlib>
#include <cstdio>

#include <string>
#include <ostream>

#include <opencv2/opencv.hpp>

#include <mpl/stdio.hpp>
#include <mpl/stdlib.hpp>


namespace cvl {
  
//  template<typename _Tp1, typename _Tp2>
//  static inline Point_<_Tp1> operator - (const Point_<_Tp1> & a, const Point_<_Tp2> & b) {
//    return Point_<_Tp1>( saturate_cast<_Tp1>(a.x - b.x), saturate_cast<_Tp1>(a.y - b.y));
//  }
//  
//  
//  template<typename _Tp1, typename _Tp2>
//  static inline void operator /= (Point_<_Tp1> & a, const _Tp2 & b) {
//    
//    a.x /= b;
//    a.y /= b;
//
//  }
//
//  template<typename _Tp1, typename _Tp2>
//  static inline void operator /= (Point3_<_Tp1> & a, const _Tp2 & b) {
//    
//    a.x /= b;
//    a.y /= b;
//    a.z /= b;
//
//  }
//  
//  template<typename _TpA, typename _TpB> static inline
//  Point_<_TpA>& operator += (Point_<_TpA>& a, const Point_<_TpB>& b)
//  {
//    a.x += b.x;
//    a.y += b.y;
//    return a;
//  }
//
// 
//  template<typename _TpA, typename _TpB> static inline
//  Point3_<_TpA>& operator += (Point3_<_TpA> & a, const Point3_<_TpB> & b)
//  {
//    a.x += b.x;
//    a.y += b.y;
//    a.z += b.z;
//    return a;
//  }
//
//  template<typename TA, typename TB>
//  static inline Point_<TA> operator + (const Point_<TA> & a, const TB & b) {
//
//    Point_<TA> point = a;
//
//    point.x += b;
//    point.y += b;
//
//    return point;
//
//  }
  
  
//  template <class T1, class T2>
//  inline double norm(const cv::Point_<T1> & a, const cv::Point_<T2> & b) {
//    
//    return std::sqrt(((a.x-b.x) * (a.x-b.x)) + ((a.y-b.y) * (a.y-b.y)));
//    
//  }
//  
//  template <class T1, class T2>
//  inline double norm(const cv::Point3_<T1> & a, const cv::Point3_<T2> & b) {
//    
//    return std::sqrt(((a.x-b.x) * (a.x-b.x)) + ((a.y-b.y) * (a.y-b.y)) + ((a.z-b.z) * (a.z-b.z)));
//    
//  }
  

  // NOTE: in opencv the color are in the BGR order
  const cv::Vec3b Red(0, 0, 255);
  const cv::Vec3b Blue(255, 0, 0);
  const cv::Vec3b Green(0, 255, 0);
  const cv::Vec3b Yellow(0, 255, 255);
  const cv::Vec3b Magenta(255, 0, 255);
  const cv::Vec3b White(255, 255, 255);
  const cv::Vec3b Black(0, 0, 0);
  const cv::Vec3b LightGrey(211, 211, 211);

  const cv::Scalar Zero(0, 0, 0);
  const cv::Scalar One(255, 255, 255);

#ifdef __APPLE__

#define CV_KEY_ARROW_UP    126
#define CV_KEY_ARROW_RIGHT 124
#define CV_KEY_ARROW_DOWN  125
#define CV_KEY_ARROW_LEFT  123
#define CV_KEY_RETURN       13
#define CV_KEY_ESC          27
#define CV_KEY_SPACE        32
#define CV_KEY_DELETE        8

#else
  
#define CV_KEY_ARROW_UP    82
#define CV_KEY_ARROW_RIGHT 83
#define CV_KEY_ARROW_DOWN  84
#define CV_KEY_ARROW_LEFT  81
#define CV_KEY_RETURN      13
#define CV_KEY_ESC         27
#define CV_KEY_SPACE       32
#define CV_KEY_DELETE       8

#endif
  
#define CV_KEY_0           48
#define CV_KEY_1           49
#define CV_KEY_2           50
#define CV_KEY_3           51
#define CV_KEY_4           52
#define CV_KEY_5           53
#define CV_KEY_6           54
#define CV_KEY_7           55
#define CV_KEY_8           56
#define CV_KEY_9           57

#define CV_KEY_A           97
#define CV_KEY_B           98
#define CV_KEY_C           99
#define CV_KEY_D          100
#define CV_KEY_E          101
#define CV_KEY_F          102
#define CV_KEY_G          103
#define CV_KEY_H          104
#define CV_KEY_I          105
#define CV_KEY_J          106
#define CV_KEY_K          107
#define CV_KEY_L          108
#define CV_KEY_M          109
#define CV_KEY_N          110
#define CV_KEY_O          111
#define CV_KEY_P          112
#define CV_KEY_Q          113
#define CV_KEY_R          114
#define CV_KEY_S          115
#define CV_KEY_T          116
#define CV_KEY_U          117
#define CV_KEY_V          118
#define CV_KEY_W          119
#define CV_KEY_X          120
#define CV_KEY_Y          121
#define CV_KEY_Z          122

  //*****************************************************************************/
  // util
  //*****************************************************************************/
  namespace util {
    
    typedef int digit_t[5][3];
    
    const digit_t zero = {
      {1, 1, 1},
      {1, 0, 1},
      {1, 0, 1},
      {1, 0, 1},
      {1, 1, 1}
    };
    
    const digit_t uno = {
      {0, 1, 0},
      {0, 1, 0},
      {0, 1, 0},
      {0, 1, 0},
      {0, 1, 0}
    };
    
    const digit_t due = {
      {1, 1, 1},
      {0, 0, 1},
      {1, 1, 1},
      {1, 0, 0},
      {1, 1, 1}
    };
    
    const digit_t tre = {
      {1, 1, 1},
      {0, 0, 1},
      {0, 1, 1},
      {0, 0, 1},
      {1, 1, 1}
    };
    
    const digit_t quattro = {
      {1, 0, 0},
      {1, 0, 0},
      {1, 0, 1},
      {1, 1, 1},
      {0, 0, 1}
    };
    
    const digit_t cinque = {
      {1, 1, 1},
      {1, 0, 0},
      {1, 1, 1},
      {0, 0, 1},
      {1, 1, 1}
    };
    
    const digit_t sei = {
      {1, 0, 0},
      {1, 0, 0},
      {1, 1, 1},
      {1, 0, 1},
      {1, 1, 1}
    };
    
    const digit_t sette = {
      {1, 1, 1},
      {0, 0, 1},
      {0, 0, 1},
      {0, 0, 1},
      {0, 0, 1}
    };
    
    const digit_t otto = {
      {1, 1, 1},
      {1, 0, 1},
      {1, 1, 1},
      {1, 0, 1},
      {1, 1, 1}
    };
    
    const digit_t nove = {
      {1, 1, 1},
      {1, 0, 1},
      {1, 1, 1},
      {0, 0, 1},
      {0, 0, 1}
    };
    
    const digit_t * digits[10] = { &zero, &uno, &due, &tre, &quattro, &cinque, &sei, &sette, &otto, &nove };
    
    //*****************************************************************************/
    // drawDigit
    //*****************************************************************************/
    void drawDigit(cv::Mat & dst, int number, const cv::Point & pos, const cv::Vec3b & color) {

      const digit_t & digit = *digits[number];
      
      int hTo = pos.y - 5;
      int hFrom = pos.y;

      if(hTo < 0) hTo = 0;
      if(hFrom > dst.rows) hFrom = dst.rows;

      int wTo = pos.x + 3;
      if(wTo >= dst.cols) wTo = dst.cols;
      
      for(int h=hTo, i=0; h<hFrom; ++h, ++i) {
        
        for(int w=pos.x, j=0; w<wTo; ++w, ++j) {
          
          if(h < 0 || h > dst.rows) continue;
          if(w < 0 || w > dst.cols) continue;

          if(digit[i][j] == 1) dst.at<cv::Vec3b>(h, w) = color;
          
        }
        
      }
      
    }
    
  } // namespace util
  
  
  //*****************************************************************************/
  // drawString
  //*****************************************************************************/
  void drawString(cv::Mat & dst, const char * str, cv::Point pos, const cv::Vec3b & color){
    
    if(dst.channels() != 3){
      fprintf(stderr, "The destination image is not a three channel image.\n");
      exit(EXIT_FAILURE);
    }
    
    size_t size = (size_t)strlen(str);
    
    //printf("DDDD %s (%d,%d)\n", str, pos.x, pos.y);
    
    for(size_t i=0; i<size; ++i){
      
      util::drawDigit(dst, str[i]-48, pos, color);
      
      pos.x += 4;
      
    }
    
  }
  
  //*****************************************************************************/
  // drawNumber
  //*****************************************************************************/
  void drawNumber(cv::Mat & dst, int number, const cv::Point & pos, const cv::Vec3b & color){
    
    char str[PATH_MAX];
    
    std::itoa(number, str);
    
    drawString(dst, str, pos, color);
    
  }

  //*****************************************************************************/
  // drawParabola
  //*****************************************************************************/
  void drawParabola(cv::Mat & dst, cv::Vec3d & coeffs, const cv::Vec3b & color, const cv::Point2d & offset = cv::Point2d(0,0)) {
    
     for(int x=0; x<dst.cols; ++x) {
       
       double y = coeffs[0] * x * x + coeffs[1] * x + coeffs[2];
       
       cv::Point pt(x, static_cast<int>(y));
       
       if(pt.y >= 0 && pt.y < dst.rows){
         if(dst.channels() == 1) dst.at<uchar>(pt) = color[0];
         if(dst.channels() == 3) dst.at<cv::Vec3b>(pt) = color;
       }
         
       
     }
    
  }

  //*****************************************************************************/
  // cross
  //*****************************************************************************/
  void cross(cv::Mat & dst, const cv::Point & pos, const cv::Vec3b & color) {
    cv::circle(dst, pos, 1, color);
  }

  //*****************************************************************************/
  // minEnclosureRoi
  //*****************************************************************************/
  cv::Rect minEnclosureRoi(const cv::Rect & roi1, const cv::Rect & roi2) {
    
    cv::Rect roi;
    
    roi.x = (roi1.x < roi2.x) ? roi1.x : roi2.x;
    roi.y = (roi1.y < roi2.y) ? roi1.y : roi2.y;

    cv::Point p1 = cv::Point(roi1.x + roi1.width - roi.x, roi1.y + roi1.height - roi.y);
    cv::Point p2 = cv::Point(roi2.x + roi2.width - roi.x, roi2.y + roi2.height - roi.y);

    roi.width  = (p1.x > p2.x) ? p1.x : p2.x;
    roi.height = (p1.y > p2.y) ? p1.y : p2.y;

    return roi;
    
  }

  //*****************************************************************************/
  // getRandomColor
  //*****************************************************************************/
  cv::Vec3b getRandomColor(double minBright = -1) {

    if(minBright != -1 && minBright < 0) minBright = 0;
    if(minBright > 255) minBright = 255;

    cv::Vec3b color;
    
    for(;;) {
      
      color.val[0] = (uchar)((rand() / (double) RAND_MAX) * 255);
      color.val[1] = (uchar)((rand() / (double) RAND_MAX) * 255);
      color.val[2] = (uchar)((rand() / (double) RAND_MAX) * 255);
      
      if(minBright == -1) break;
      
      double Y = 0.299 * color.val[0] + 0.587 * color.val[1] + 0.114 * color.val[2];

      if(Y >= minBright) break;
      
    }
      
    return color;
        
  }
 
  //*****************************************************************************/
  // RGBtoHex:
  // Function to convert the RGB code to Hex color code
  //*****************************************************************************/
  std::string RGBtoHex(int R, int G, int B) {
    
    if((R >= 0 && R <= 255) && (G >= 0 && G <= 255) && (B >= 0 && B <= 255)) {
   
      std::string hexCode = "#";
                  hexCode += std::decToHex(R);
                  hexCode += std::decToHex(G);
                  hexCode += std::decToHex(B);
   
      return hexCode;
      
    } else return "-1";
    
  }


  //*****************************************************************************/
  // save
  //*****************************************************************************/
  bool save(cv::Mat & image, std::string str){
    
    mpl::io::expandPath(str);

    int status = cv::imwrite(str, image);
    
    if(!status) {
      fprintf(stderr, "error in opening the file '%s'\n", str.c_str());
      abort();
    }
    
    return true;
    
  }
    
  //*****************************************************************************/
  // open
  //*****************************************************************************/
  cv::Mat open(std::string str, int mode = cv::IMREAD_UNCHANGED){
    
    mpl::io::expandPath(str);
    
    cv::Mat image = cv::imread(str, mode);
    
    if(image.data==NULL){
      fprintf(stderr, "error in opening the image '%s'\n", str.c_str());
      exit(EXIT_FAILURE);
    }
    
    return image;
    
  }

  //*****************************************************************************/
  // open
  //*****************************************************************************/
//  void open(std::string str, cv::Mat & image, uint32_t mode = cv::IMREAD_UNCHANGED){
//    
//    mpl::io::expandPath(str);
//    
//    cv::imread(str, image, mode);
//    
//    if(image.data==NULL){
//      fprintf(stderr, "error in opening the image '%s'\n", str.c_str());
//      exit(EXIT_FAILURE);
//    }
//      
//  }

  
#if(0)
  /*****************************************************************************/
  // median
  /*****************************************************************************/
  //TODO: testare con minvAlue ecc
  double median(cv::Mat src, int minValue = 0, int maxValue = 256) {
    
    int histSize = 256;
    
    float range[] = { static_cast<float>(minValue), static_cast<float>(maxValue) };
    
    const float* histRange = { range };
    
    cv::Mat hist;
    
    cv::calcHist( &src, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);
    
    double m = (src.rows*src.cols) / 2.0;
    
    int bin = 0;
    
    double med = -1.0;
    
    for(int i = 0; i < histSize && med < 0.0; ++i) {
      
      bin += cvRound(hist.at<float>(i));
      
      if(bin>m && med<0.0) med = i;
      
    }
    
    return med;
    
  }
#endif


  //****************************************************************************/
  // histogram
  //****************************************************************************/
  void histogram(const cv::Mat & src, cv::Mat & histo, cv::Mat & mask, int valueToSkip = -1) {
    
    if(src.channels() > 1){
      fprintf(stderr, "The input image must have one channel.\n");
      exit(EXIT_FAILURE);
    }
    
    int histSize = 256;
    float range[] = { 0, 256 };
    const float * histRange = { range };
    
    if(valueToSkip != 1) {
      
      if(mask.empty()) mask = cv::Mat::ones(src.size(), CV_8UC1);
      
      for(int i=0; i<src.rows; ++i)
        for(int j=0; j<src.cols; ++j)
          if(src.at<uchar>(i,j) == valueToSkip) mask.at<uchar>(i,j) = 0;
      
    }
    
    cv::calcHist(&src, 1, 0, mask, histo, 1, &histSize, &histRange);
    
  }

  //****************************************************************************/
  // histogram
  //****************************************************************************/
  void histogram(const cv::Mat & src, cv::Mat & histo, int valueToSkip = -1) {
    
    cv::Mat mask = cv::Mat();
    
    histogram(src, histo, mask, valueToSkip);
    
  }



//// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
//// Returns a positive value, if OAB makes a counter-clockwise turn,
//// negative for clockwise turn, and zero if the points are collinear.
//template <typename T>
//T cross(const cv::Point_<T> & O, const cv::Point_<T> & A, const cv::Point_<T> & B)
//{
//  return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
//}
//
//template <typename T>
//bool operator < (const cv::Point_<T> & p1, const cv::Point_<T> & p2) {
//  
//    return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
//  
//}
  

} /* namespace opencv */

#endif /* _H_CVL_OPENCV_H_ */
