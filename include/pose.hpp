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

#ifndef _H_CVL_POSE_H_
#define _H_CVL_POSE_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdio>
#include <cstdlib>

#include <vector>

//*****************************************************************************
// namespace cvl::pose
//*****************************************************************************
namespace cvl::pose {

  //*****************************************************************************
  // recoverPose
  //*****************************************************************************
  // Recover relative camera pose from matched corners and camera intrinsics.
  inline void recoverPose(const std::vector<cv::Point2d> & cornersLeft, const std::vector<cv::Point2d> & cornersRight, const cv::Mat & leftK, const std::vector<double> & leftDistCoef, const cv::Mat & rightK, const std::vector<double> & rightDistCoef, double camDistance, cv::Mat & RLeft, cv::Mat & TLeft, cv::Mat & RRight, cv::Mat & TRight) {

    cv::Mat E;

    cv::recoverPose(cornersRight, cornersLeft, rightK, rightDistCoef, leftK, leftDistCoef, E, RLeft, TLeft);

    TLeft *= camDistance;

    RRight = cv::Mat::eye(3, 3, CV_64FC1);

    TRight = cv::Mat::zeros(3, 1, CV_64FC1); // 3x1 come TLeft

  }

  //*****************************************************************************
  // solvePnP
  //*****************************************************************************
  // Solve PnP for left/right views and align right camera to left.
  inline void solvePnP(const std::vector<cv::Point3d> & objectPoints, const std::vector<cv::Point2d> & cornersLeft, const std::vector<cv::Point2d> & cornersRight, const cv::Mat & leftK, const std::vector<double> & leftDistCoef, const cv::Mat & rightK, const std::vector<double> & rightDistCoef, cv::Mat & RLeft, cv::Mat & TLeft, cv::Mat & RRight, cv::Mat & TRight) {

    // risolvo la camera destra
    cv::Mat Rv_right, R_right, T_right;
    cv::solvePnP(objectPoints, cornersRight, rightK, rightDistCoef, Rv_right, T_right);

    // risolvo la camera sinistra
    cv::Mat Rv_left, R_left, T_left;
    cv::solvePnP(objectPoints, cornersLeft, leftK, leftDistCoef, Rv_left, T_left);

    cv::Rodrigues(Rv_left,  R_left);
    cv::Rodrigues(Rv_right, R_right);

    RLeft = R_left * R_right.t();
    TLeft = T_left - RLeft * T_right;

    RRight = cv::Mat::eye(3, 3, CV_64FC1);
    TRight = cv::Mat::zeros(3, 1, CV_64FC1); // 3x1 come TLeft

  }

  //*****************************************************************************
  // stereoCalibrate
  //*****************************************************************************
  // Run stereo calibration with fixed intrinsics.
  inline void stereoCalibrate(const std::vector<cv::Point3d> & _objectPoints, const std::vector<cv::Point2d> & _cornersLeft, const std::vector<cv::Point2d> & _cornersRight, const cv::Mat & leftK, const std::vector<double> & leftDistCoef, const cv::Mat & rightK, const std::vector<double> & rightDistCoef, cv::Size imageSize, cv::Mat & RLeft, cv::Mat & TLeft, cv::Mat & RRight, cv::Mat & TRight) {

    cv::Mat E, F, perViewErrors;

    std::vector<std::vector<cv::Point3d>> objectPoints; objectPoints.push_back(_objectPoints);
    std::vector<std::vector<cv::Point2d>> imagePointsRight; imagePointsRight.push_back(_cornersRight);
    std::vector<std::vector<cv::Point2d>> imagePointsLeft; imagePointsLeft.push_back(_cornersLeft);

    cv::stereoCalibrate(objectPoints,
                        imagePointsRight, imagePointsLeft,
                        rightK, rightDistCoef,
                        leftK, leftDistCoef,
                        imageSize,
                        RLeft, TLeft, E, F, perViewErrors,
                        cv::CALIB_FIX_INTRINSIC);
      //cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_K2 | cv::CALIB_FIX_K3);

    RRight = cv::Mat::eye(3, 3, CV_64FC1);

    TRight = cv::Mat::zeros(3, 1, CV_64FC1); // 3x1 come TLeft

  }

} /* namespace cvl::pose */

#endif /* _H_CVL_POSE_H_ */
