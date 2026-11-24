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

#ifndef _H_CVL_CONVERT_H_
#define _H_CVL_CONVERT_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>
#include <utility>

//*****************************************************************************
// namespace cvl::convert
//*****************************************************************************
namespace cvl::convert {

  enum { LINEAR, LINEAR_MINMAX, LINEAR_CUTWINGS, LINEAR_CUTEXTREMAL, UPPER, LOWER };

  //*****************************************************************************
  // from16to8
  //*****************************************************************************
  // Convert a 16-bit grayscale image to 8-bit using explicit scale/offset.
  inline void from16to8(const cv::Mat & src, cv::Mat & dst, double alpha, double beta) {

    if(src.empty() || src.type() != CV_16UC1) {
      cvl::log::error("cvl::convert::from16to8() error: the input image must be a non-empty CV_16UC1 image\n");
    }

    src.convertTo(dst, CV_8U, alpha, beta);

  }

  //*****************************************************************************
  // from16to8
  //*****************************************************************************
  // Convert a 16-bit grayscale image to 8-bit using the chosen mode.
  // Returns the (alpha, beta) pair actually used; for the non-linear
  // UPPER/LOWER modes returns (INFINITY, INFINITY).
  // For LINEAR_CUTWINGS 'value' is the FRACTION (0..1, not a percent) of
  // pixels to clip on each histogram wing.
  inline std::pair<double,double> from16to8(const cv::Mat & src, cv::Mat & dst, int mode, double value = 0) {

    if(src.empty() || src.type() != CV_16UC1) {
      cvl::log::error("cvl::convert::from16to8() error: the input image must be a non-empty CV_16UC1 image\n");
    }

    if(mode == LINEAR) {
      double alpha = 255 / 65535.0;
      double beta  = 0;
      src.convertTo(dst, CV_8U, alpha, beta);
      return std::pair<double,double>(alpha, beta);
    }


    if(mode == LINEAR_MINMAX) {

      double min, max;

      cv::Mat histogram;
      cvl::histogram16bit(src, histogram, min, max);

      //printf("LINEAR_MINMAX min %f max %f\n", min, max);

      if(max <= min) max = min + 1; // evita la divisione per zero su immagini piatte
      double alpha = 255 / (max-min);
      double beta  = - (255*min) / (max-min);

      src.convertTo(dst, CV_8U, alpha, beta);

      return std::pair<double,double>(alpha, beta);

    }

    if(mode == LINEAR_CUTWINGS) {

      cv::Mat histogram;
      cvl::histogram16bit(src, histogram);

      double min = 0;
      double counterMin = 0;

      double max = 65535;
      double counterMax = 0;

      double target = (src.rows * src.cols) * value;

      for(int i=0; i<histogram.rows; ++i) {
        min = i;
        counterMin += histogram.at<float>(i);
        if(counterMin >= target) break;
      }

      for(int i=histogram.rows-1; i>=0; --i) {
        max = i;
        counterMax += histogram.at<float>(i);
        if(counterMax >= target) break;
      }

      //printf("LINEAR_CUTWINGS min %f max %f\n", min, max);

      if(max <= min) max = min + 1; // evita la divisione per zero su immagini piatte
      double alpha = 255 / (max-min);
      double beta  = - (255*min) / (max-min);

      src.convertTo(dst, CV_8U, alpha, beta);

      return std::pair<double,double>(alpha, beta);

    }

    if(mode == LINEAR_CUTEXTREMAL) {

      cv::Mat histogram;
      cvl::histogram16bit(src, histogram);

      double min = 0;
      double max = 65535;

      int skip = 0;

      for(int i=0; i<histogram.rows; ++i) {
        min = i;
        if(histogram.at<float>(i) != 0) skip++;
        if(skip == 2) break;
      }

      skip = 0;

      for(int i=histogram.rows-1; i>=0; --i) {
        max = i;
        if(histogram.at<float>(i) != 0) skip++;
        if(skip == 2) break;
      }

      //printf("LINEAR_CUTEXTREMAL min %f max %f\n", min, max);

      if(max <= min) max = min + 1; // evita la divisione per zero su immagini piatte
      double alpha = 255 / (max-min);
      double beta  = - (255*min) / (max-min);

      src.convertTo(dst, CV_8U, alpha, beta);

      return std::pair<double,double>(alpha, beta);

    }

    if(mode == LOWER) {

      // Estrae gli 8 bit meno significativi usando un'operazione di mascheramento
      dst = src & 0x00FF;

      dst.convertTo(dst, CV_8U);

      return std::pair<double,double>(INFINITY, INFINITY);

    }

    if(mode == UPPER) {

      // Estrae gli 8 bit più significativi: dopo la maschera il valore è un multiplo
      // esatto di 256, quindi la divisione equivale a uno shift (nessun arrotondamento)
      dst = src & 0xFF00;

      dst.convertTo(dst, CV_8U, 1.0 / 256.0);

      return std::pair<double,double>(INFINITY, INFINITY);

    }

    cvl::log::error("cvl::convert::from16to8() error: mode not recognized\n");

    return {};

  }

} /* namespace cvl::convert */

#endif /* _H_CVL_CONVERT_H_ */
