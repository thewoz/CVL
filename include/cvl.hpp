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


#ifndef _H_CVL_H_
#define _H_CVL_H_

#define CVL_LIB_VERSION "Cvl Lib v3.2.6"

//****************************************************************************/
// General third-party includes (centralized here so the single headers do
// not need to repeat them).
//****************************************************************************/
#include <opencv2/opencv.hpp>

#ifdef _DEBUG_CVL_LIBS_

  #include "utils.hpp"
  #include "colors.hpp"
  #include "opencv.hpp"
  #include "background.hpp"
  #include "baricenter.hpp"
  #include "contours.hpp"
  #include "blob.hpp"
  #include "blobs.hpp"
  #include "labeling.hpp"
  #include "blobSplitting.hpp"
  #include "chessboard.hpp"
  #include "corners.hpp"
  #include "clustering.hpp"
  #include "deflickering.hpp"
  #include "denoising.hpp"
  #include "enhance.hpp"
  #include "subpixel.hpp"
  #include "threshold.hpp"
  #include "window.hpp"
  #include "pose.hpp"
  #include "draw.hpp"
  #include "convert.hpp"

#else

  #include <cvl/utils.hpp>
  #include <cvl/colors.hpp>
  #include <cvl/opencv.hpp>
  #include <cvl/background.hpp>
  #include <cvl/baricenter.hpp>
  #include <cvl/contours.hpp>
  #include <cvl/blob.hpp>
  #include <cvl/blobs.hpp>
  #include <cvl/labeling.hpp>
  #include <cvl/blobSplitting.hpp>
  #include <cvl/chessboard.hpp>
  #include <cvl/corners.hpp>
  #include <cvl/clustering.hpp>
  #include <cvl/deflickering.hpp>
  #include <cvl/denoising.hpp>
  #include <cvl/enhance.hpp>
  #include <cvl/subpixel.hpp>
  #include <cvl/threshold.hpp>
  #include <cvl/window.hpp>
  #include <cvl/pose.hpp>
  #include <cvl/draw.hpp>
  #include <cvl/convert.hpp>

#endif

#endif /* _H_CVL_H_ */
