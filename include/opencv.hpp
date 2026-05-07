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


// cv::Point2d += cv::Point
inline cv::Point2d& operator+=(cv::Point2d& lhs, const cv::Point& rhs)
{
    lhs.x += static_cast<double>(rhs.x);
    lhs.y += static_cast<double>(rhs.y);
    return lhs;
}

// cv::Point2d -= cv::Point
inline cv::Point2d& operator-=(cv::Point2d& lhs, const cv::Point& rhs)
{
    lhs.x -= static_cast<double>(rhs.x);
    lhs.y -= static_cast<double>(rhs.y);
    return lhs;
}

// cv::Point2d + cv::Point
inline cv::Point2d operator+(cv::Point2d lhs, const cv::Point& rhs)
{
    lhs += rhs;
    return lhs;
}

// cv::Point + cv::Point2d
inline cv::Point2d operator+(const cv::Point& lhs, cv::Point2d rhs)
{
    rhs += lhs;
    return rhs;
}

// cv::Point2d - cv::Point
inline cv::Point2d operator-(cv::Point2d lhs, const cv::Point& rhs)
{
    lhs -= rhs;
    return lhs;
}

// cv::Point - cv::Point2d
inline cv::Point2d operator-(const cv::Point& lhs, const cv::Point2d& rhs)
{
    return cv::Point2d(
        static_cast<double>(lhs.x) - rhs.x,
        static_cast<double>(lhs.y) - rhs.y
    );
}

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
  
#define CV_KEY_MINUS       45
#define CV_KEY_PLUS        43

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
    // Draw a 5x3 digit glyph at the given position.
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
  
  
//*****************************************************************************
// drawRotatedText()
//*****************************************************************************
void drawRotatedText(cv::Mat & image, const std::string & text, const cv::Point & center, double angleDeg, double fontScale, const cv::Scalar & color, int thickness = 2) {
    
  int baseline = 0;
    
  cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, &baseline);

  int margin = 20;
  int w = textSize.width + 2 * margin;
  int h = textSize.height + baseline + 2 * margin;

  // Canvas BGRA completamente trasparente
  cv::Mat textImg(h, w, CV_8UC4, cv::Scalar(0, 0, 0, 0));
  cv::Mat alpha(h, w, CV_8UC1, cv::Scalar(0));

  cv::Point org(margin, margin + textSize.height);

  // Disegno il testo nella maschera alpha
  cv::putText(alpha, text, org, cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(255), thickness, cv::LINE_AA);

  // Copio il colore del testo e il suo alpha nel canvas BGRA
  for(int y = 0; y < h; ++y) {
    
    for(int x = 0; x < w; ++x) {
      
      uchar a = alpha.at<uchar>(y, x);
      
      cv::Vec4b& px = textImg.at<cv::Vec4b>(y, x);

      px[0] = static_cast<uchar>(color[0]); // B
      px[1] = static_cast<uchar>(color[1]); // G
      px[2] = static_cast<uchar>(color[2]); // R
      px[3] = a;                            // A
      
    }
    
  }

  // Inverto il segno per avere l'inclinazione visiva corretta in coordinate immagine
  double correctedAngle = -angleDeg;

  cv::Point2f rotCenter(textImg.cols / 2.0f, textImg.rows / 2.0f);
  cv::Mat rotMat = cv::getRotationMatrix2D(rotCenter, correctedAngle, 1.0);

  cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), textImg.size(), correctedAngle).boundingRect2f();

  rotMat.at<double>(0, 2) += bbox.width / 2.0 - textImg.cols / 2.0;
  rotMat.at<double>(1, 2) += bbox.height / 2.0 - textImg.rows / 2.0;

  cv::Mat rotated;
  cv::warpAffine(textImg, rotated, rotMat,  bbox.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,0,0,0));

  int x0 = center.x - rotated.cols / 2;
  int y0 = center.y - rotated.rows / 2;

  // Alpha blending sul target BGR
  for(int y=0; y<rotated.rows; ++y) {
    
    int dstY = y0 + y;
    
    if(dstY < 0 || dstY >= image.rows)
      continue;

    for(int x = 0; x < rotated.cols; ++x) {
      
      int dstX = x0 + x;
      if(dstX < 0 || dstX >= image.cols)
        continue;

      const cv::Vec4b& src = rotated.at<cv::Vec4b>(y, x);
      uchar alphaVal = src[3];
      if(alphaVal == 0)
        continue;

      double a = alphaVal / 255.0;
      cv::Vec3b& dst = image.at<cv::Vec3b>(dstY, dstX);

      dst[0] = cv::saturate_cast<uchar>(src[0] * a + dst[0] * (1.0 - a));
      dst[1] = cv::saturate_cast<uchar>(src[1] * a + dst[1] * (1.0 - a));
      dst[2] = cv::saturate_cast<uchar>(src[2] * a + dst[2] * (1.0 - a));
      
    }
    
  }
  
}

//*****************************************************************************
// shrinkLine()
//*****************************************************************************
void shrinkLine(cv::Point & p1, cv::Point & p2, int cutFromP1, int cutFromP2) {
    
  cv::Point2d dir = p2 - p1;

  double length = std::sqrt(dir.x * dir.x + dir.y * dir.y);

  if(length <= 1e-9) {
    fprintf(stderr, "shrinkLine() warning: line too short\n");
    return;
  }
  
  dir.x /= length;
  dir.y /= length;

  // Evita di accorciare più della lunghezza totale
  double totalCut = cutFromP1 + cutFromP2;

  if(totalCut >= length) {
    fprintf(stderr, "shrinkLine() warning: cut too big\n");
    return;
  }

  p1.x = p1.x + dir.x * cutFromP1;
  p1.y = p1.y + dir.y * cutFromP1;

  p2.x = p2.x - dir.x * cutFromP2;
  p2.y = p2.y - dir.y * cutFromP2;

}

void shrinkLine(cv::Point & p1, cv::Point & p2, int cut) {
  shrinkLine(p1, p2, cut, cut);
}

void scaleLine(cv::Point & p1, cv::Point & p2, double scale) {
  cv::Point2d center = (p1 + p2) * 0.5;
  p1.x = center.x + (p1.x - center.x) * scale;
  p1.y = center.y + (p1.y - center.y) * scale;
  p2.x = center.x + (p2.x - center.x) * scale;
  p2.y = center.y + (p2.y - center.y) * scale;
}


//*****************************************************************************
// drawDistanceMarker()
//*****************************************************************************
void drawDistanceMarker(cv::Mat & image, cv::Point p1, cv::Point p2, const std::string & label, const cv::Scalar & color, int thickness = 2, int shrink = 0, int endMarkSize = 10, int textOffset = 20, double fontScale = 0.8) {
  
  if(shrink != 0) shrinkLine(p1, p2, shrink);
      
  // Linea principale
  cv::line(image, p1, p2, color, thickness, cv::LINE_AA);

  // Direzione della linea
  cv::Point2f dir((p2.x - p1.x), (p2.y - p1.y));

  float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
  if(len < 1.0f) return;

  dir.x /= len;
  dir.y /= len;

  // Perpendicolare
  cv::Point2f perp(-dir.y, dir.x);

   // Tacche alle estremità
  cv::Point p1a(cvRound(p1.x + perp.x * endMarkSize), cvRound(p1.y + perp.y * endMarkSize));
  cv::Point p1b(cvRound(p1.x - perp.x * endMarkSize), cvRound(p1.y - perp.y * endMarkSize));

  cv::Point p2a(cvRound(p2.x + perp.x * endMarkSize), cvRound(p2.y + perp.y * endMarkSize));
  cv::Point p2b(cvRound(p2.x - perp.x * endMarkSize), cvRound(p2.y - perp.y * endMarkSize));

  cv::line(image, p1a, p1b, color, thickness, cv::LINE_AA);
  cv::line(image, p2a, p2b, color, thickness, cv::LINE_AA);

  // Punto medio
  cv::Point mid((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);

  // Posizione del testo, spostata in direzione perpendicolare
  cv::Point textCenter(cvRound(mid.x + perp.x * textOffset), cvRound(mid.y + perp.y * textOffset));

  // Angolo della linea
  double angleRad = std::atan2((p2.y - p1.y), (p2.x - p1.x));
  
  double angleDeg = angleRad * 180.0 / CV_PI;

  // Mantiene il testo leggibile
       if(angleDeg >  90.0) angleDeg -= 180.0;
  else if(angleDeg < -90.0) angleDeg += 180.0;

  drawRotatedText(image, label, textCenter, angleDeg, fontScale, color, thickness);
  
}



  //*****************************************************************************/
  // drawString
  //*****************************************************************************/
  // Draw a numeric string using the built-in 5x3 glyphs.
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
  // Draw an integer number using built-in digit glyphs.
  void drawNumber(cv::Mat & dst, int number, const cv::Point & pos, const cv::Vec3b & color){
    
    char str[PATH_MAX];
    
    std::itoa(number, str);
    
    drawString(dst, str, pos, color);
    
  }

  //*****************************************************************************/
  // drawParabola
  //*****************************************************************************/
  // Draw a parabola defined by quadratic coefficients.
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
  // Draw a small cross marker.
  void cross(cv::Mat & dst, const cv::Point & pos, const cv::Vec3b & color) {
    cv::circle(dst, pos, 1, color);
  }

  //*****************************************************************************/
  // minEnclosureRoi
  //*****************************************************************************/
  // Compute the minimal ROI enclosing two rectangles.
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
  // Generate a random BGR color optionally above a brightness threshold.
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
  // save
  //*****************************************************************************/
  // Save an image to disk, expanding paths and validating the write.
  bool save(cv::Mat & image, std::string str) {
    
    mpl::io::expandPath(str);

    int status = cv::imwrite(str, image);
    
    if(!status) {
      fprintf(stderr, "error in opening the file '%s'\n", str.c_str());
      abort();
    }
    
    return true;
    
  }
  
  //*****************************************************************************/
  // resize()
  //*****************************************************************************/
  // INTER_AREA  downscale  meno aliasing, default per <100%
  // INTER_LINEAR  upscale veloce  buon compromesso qualità/velocità
  // INTER_CUBIC  upscale qualità  più nitido, 4× più lento di LINEAR
  // INTER_LANCZOS4  upscale max qualità  il migliore, usalo per immagini scientifiche
  // INTER_NEAREST  pixel art / debug  velocissimo, nessun blending
  cv::Mat resize(const cv::Mat & src, double percent, int interp = -1) {
    
    if(src.empty())
      throw std::invalid_argument("resizeByPercent: immagine vuota");
    if(percent <= 0.0)
      throw std::invalid_argument("resizeByPercent: percentuale deve essere > 0");

    double scale = percent / 100.0;

    // Scelta automatic interpolazione se non specificata
    if(interp == -1) {
      interp = (scale < 1.0)
              ? cv::INTER_AREA    // downscale: riduce aliasing
              : cv::INTER_LINEAR; // upscale:   buon compromesso
    }

    cv::Mat dst;
    cv::resize(src, dst, cv::Size(), scale, scale, interp);
    
    return dst;
    
  }

  //*****************************************************************************/
  // open
  //*****************************************************************************/
  // Open an image from disk with the requested imread mode.
  cv::Mat open(std::string str, int mode = cv::IMREAD_UNCHANGED) {
    
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
  // Open an image from disk with the requested imread mode.
  cv::Mat open(std::string str, double percent, int mode = cv::IMREAD_UNCHANGED, int interp = -1) {
    
    cv::Mat image = open(str, mode);
    
    return resize(image, percent, interp);
    
  }

  //*****************************************************************************/
  // open
  //*****************************************************************************/
  // Open a 16-bit single-channel image from disk.
  cv::Mat open16bitC1(std::string str){
    
    mpl::io::expandPath(str);
    
    cv::Mat image = cv::imread(str, cv::IMREAD_UNCHANGED);
    
    // controllo che l'immagine sia buona
    if(image.empty()) {
      fprintf(stderr, "error: in opening the image: '%s'\n", str.c_str());
      exit(EXIT_FAILURE);
    }
    
    // controllo che l'immagine sia buona
    if(image.type() != CV_16UC1) {
      fprintf(stderr, "warning: is not a CV_16UC1 image: '%s'\n", str.c_str());
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
  void histogram8bit(const cv::Mat & src, cv::Mat & histo, cv::Mat & mask, int valueToSkip = -1) {
    
    if(src.empty() || src.type() != CV_8UC1) {
      fprintf(stderr, "error in cvl::histogram8bit(): the input image must be a non empty CV_8UC1 image\n");
      abort();
    }
    
    if(!mask.empty() || mask.type() != CV_8UC1) {
      fprintf(stderr, "error in cvl::histogram8bit(): the mask image must be a CV_8UC1 image\n");
      abort();
    }
    
    //FIXME: devo controllare che sia ad 8bit
    
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
  // histogram8bit
  //****************************************************************************/
  void histogram8bit(const cv::Mat & src, cv::Mat & histo, int valueToSkip = -1) {
    
    cv::Mat mask = cv::Mat();
    
    histogram8bit(src, histo, mask, valueToSkip);
    
  }

  //****************************************************************************/
  // histogram16bit
  //****************************************************************************/
  void histogram16bit(const cv::Mat & src, cv::Mat & histogram) {
    
    if(src.empty() || src.type() != CV_16UC1) {
      fprintf(stderr, "error in cvl::histogram16bit(): the input image must be a non empty CV_16UC1 image\n");
      abort();
    }
      
    int histSize = 65536;
    float range[] = { 0, 65536 };
    const float * histRange = { range };
    
    cv::calcHist(&src, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);

  }

  //****************************************************************************/
  // histogram16bit
  //****************************************************************************/
  template <typename T>
  void histogram16bit(const cv::Mat & src, cv::Mat & histogram, T & min, T & max) {

    histogram16bit(src, histogram);
    
    min = 0;
    max = 65535;

    for(int i=0; i<histogram.rows; ++i) {
      min = i;
      if(histogram.at<float>(i) != 0) break;
    }

    for(int i=histogram.rows-1; i>=0; --i) {
      max = i;
      if(histogram.at<float>(i) != 0) break;
    }
    
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
