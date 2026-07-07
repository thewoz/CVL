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

#ifndef _H_CVL_SUBPIXEL_H_
#define _H_CVL_SUBPIXEL_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdio>
#include <cstdlib>

#include <cmath>
#include <vector>
#include <string>
#include <iostream>

//****************************************************************************/
// cvl::subpixel
//****************************************************************************/
namespace cvl::subpixel {

  //****************************************************************************/
  // cvl::subpixel::detail
  //****************************************************************************/
  // Implementazione interna del detector di angoli a precisione subpixel
  // (algoritmo "nuovo", di tipo ROCHADE). Usato solo da get().
  namespace detail {

    static const int MAX_SYMMETRY_ERRORS = 5;

    //**************************************************************************/
    // Parameters - parametri del feature detector
    //**************************************************************************/
    struct Parameters {

      float strength;        //!< intensità minima di una giunzione valida in dB
      float resolution;      //!< risoluzione angolare in radianti
      int branches;          //!< numero di rami
      int min_scale;         //!< livello di scala [0..8]
      int max_scale;         //!< livello di scala [0..8]
      bool filter;           //!< filtra la feature map per migliorare la risposta impulsiva
      bool super_resolution; //!< up-sample

      Parameters() {
        strength = 40;
        resolution = float(CV_PI*0.25);
        branches = 2;
        min_scale = 2;
        max_scale = 5;
        super_resolution = true;
        filter = true;
      }

    };

    static Parameters parameters;

    //**************************************************************************/
    // ChessBoardParameters - parametri di rilevamento della scacchiera
    //**************************************************************************/
    struct ChessBoardParameters {

      cv::Size chessboard_size; //!< dimensione della scacchiera
      int min_scale;            //!< livello di scala [0..8]
      int max_scale;            //!< livello di scala [0..8]
      int max_points;           //!< numero massimo di punti considerati
      int max_tests;            //!< numero massimo di ipotesi testate
      bool super_resolution;    //!< usa la super-resolution per il rilevamento
      bool larger;              //!< indica se restituire scacchiere più grandi
      bool marker;              //!< le scacchiere valide devono avere un marker bianco e nero per l'orientamento

      ChessBoardParameters() {
        chessboard_size = cv::Size(9,6);
        min_scale = 3;
        max_scale = 4;
        super_resolution = true;
        max_points = 200;
        max_tests = 50;
        larger = false;
        marker = false;
      }

      ChessBoardParameters(int scale, int _max_points):
        min_scale(scale),
        max_scale(scale),
        max_points(_max_points) {
          chessboard_size = cv::Size(9,6);
      }

    };

    static ChessBoardParameters CBparameters;

    //**************************************************************************/
    // rotate() -
    //**************************************************************************/
    // Ruota un'immagine di un angolo (in radianti) attorno al suo centro.
    inline void rotate(float angle, cv::InputArray img, cv::Size size, cv::OutputArray out) {

      if(angle == 0) {
        img.copyTo(out);
        return;
      } else {
        cv::Matx23d m = cv::getRotationMatrix2D(cv::Point2f(float(img.cols()*0.5),float(img.rows()*0.5)),float(angle/CV_PI*180),1);
        m(0,2) += 0.5*(size.width-img.cols());
        m(1,2) += 0.5*(size.height-img.rows());
        cv::warpAffine(img,out,m,size);
      }

    }

    //**************************************************************************/
    // calcFeatureMap() -
    //**************************************************************************/
    // Calcola la feature map usata per il rilevamento degli angoli della scacchiera.
    inline void calcFeatureMap(const cv::Mat &images, cv::Mat& out) {

      if(images.empty())
        CV_Error(cv::Error::StsBadArg,"no rotation images");
      int type = images.type(), depth = CV_MAT_DEPTH(type);
      CV_CheckType(type,depth == CV_8U,
              "Only 8-bit grayscale or color images are supported");
      if(!images.isContinuous())
        CV_Error(cv::Error::StsBadArg,"image must be continuous");

      float signal,noise,rating;
      int count1;
      unsigned char val1,val2,val3;
      const unsigned char* wrap_around;
      const unsigned char* pend;
      const unsigned char* pimages = images.data;
      const int channels = images.channels();
      if(channels < 4)
        CV_Error(cv::Error::StsBadArg,"images must have at least four channels");

      // for each pixel
      out = cv::Mat::zeros(images.rows,images.cols,CV_32FC1); // istogramma (?)
      const float *pout_end = reinterpret_cast<const float*>(out.dataend);
      for(float *pout=out.ptr<float>(0,0);pout != pout_end;++pout) {
        //reset values
        rating = 0.0; count1 = 0;
        noise = 255; signal = 0;

        //calc rating
        pend = pimages+channels;
        val1 = *(pend-1);                       // wrap around (last value) -- VALORE DELL ULTIMO PIXEL DEI CANALI
        wrap_around = pimages++;                // store for wrap around (first value)
        val2 = *wrap_around;                    // first value -- VALORE DEL PRIMO DEI CANALI
        for(;pimages != pend;++pimages) {       // ciclo for su tutti i canali
          val3 = *pimages;
          if(val1 <= val2) {
            if(val3 < val2) { // maxima
              if(signal < val2)
                signal = val2;
              ++count1; // contatore di massimi o minini LOCALI
            }
          } else if(val1 > val2 && val3 >= val2) { // minima
            if(noise > val2)
              noise = val2;
            ++count1;
          }
          val1 = val2;
          val2 = val3;
        }
        // wrap around
        if(val1 <= val2) { // maxima
          if(*wrap_around < val2) {
            if(signal < val2)
              signal = val2;
            ++count1;
          }
        } else if(val1 > val2 && *wrap_around >= val2) { // minima
          if(noise > val2)
            noise = val2;
          ++count1;
        }

        // store rating
        if(count1 == parameters.branches) {
          rating = signal-noise;
          *pout = rating*rating; //store rating in the feature map
        }
      }

    }

    //**************************************************************************/
    // detectImpl() -
    //**************************************************************************/
    // Costruisce le immagini ruotate e le feature map alle varie scale.
    inline void detectImpl(const cv::Mat& _gray_image,
            std::vector<cv::Mat> &rotated_images,
            std::vector<cv::Mat> &feature_maps,
            const cv::Mat &_mask) {

      if(!_mask.empty())
        CV_Error(cv::Error::StsBadSize, "Mask is not supported");
      CV_CheckTypeEQ(_gray_image.type(), CV_8UC1, "Unsupported image type");

      // up-sample if needed
      cv::UMat gray_image;
      const int super_res = int(parameters.super_resolution);
      if(super_res)
        cv::resize(_gray_image,gray_image,cv::Size(),2,2);
      else
        _gray_image.copyTo(gray_image);

      //for each scale
      const int num_scales = parameters.max_scale-parameters.min_scale+1;
      const int diag = int(sqrt(gray_image.rows*gray_image.rows+gray_image.cols*gray_image.cols));
      const cv::Size size(diag,diag);
      const int num = int(0.5001*CV_PI/parameters.resolution); // numero di rotazioni che lui fa

      rotated_images.resize(num_scales);
      feature_maps.resize(num_scales);

      parallel_for_(cv::Range(parameters.min_scale,parameters.max_scale+1),[&](const cv::Range& range){
        for(int scale=range.start;scale < range.end;++scale) {
          // calc images
          // for each angle step
          int scale_id = scale-parameters.min_scale;
          int scale_size = int(pow(2.0,scale+1+super_res));
          int scale_size2 = int((scale_size/7)*2+1);
          std::vector<cv::UMat> images;
          images.resize(2*num);
          cv::UMat rotated,filtered_h,filtered_v;
          // il vettore images è composto per la prima metà di immagini convolute orizzontalmente, e la seconda metà verticalmente
          cv::boxFilter(gray_image,images[0],-1,cv::Size(scale_size,scale_size2)); // m*n box filter (orizzontale)
          cv::boxFilter(gray_image,images[num],-1,cv::Size(scale_size2,scale_size)); // n*m box filter (verticale)

          //forse le ultime due rotazioni sono ridondanti, quindi num può essere anche 0 (inteso che ruota solo una volta)
          for(int i=1;i<num;++i) {
            float angle = parameters.resolution*i;
            rotate(-angle,gray_image,size,rotated);
            cv::boxFilter(rotated,filtered_h,-1,cv::Size(scale_size,scale_size2));
            cv::boxFilter(rotated,filtered_v,-1,cv::Size(scale_size2,scale_size));

            // rotate filtered images back
            rotate(angle,filtered_h,gray_image.size(),images[i]);
            rotate(angle,filtered_v,gray_image.size(),images[i+num]);
          }
          cv::merge(images,rotated_images[scale_id]);

          // calc feature map
          // i channels di rotated images contengono i vari angoli, e la prima metà sono le convoluzioni orizzontali e seconda verticali
          calcFeatureMap(rotated_images[scale_id],feature_maps[scale_id]);

          // filter feature map to improve impulse responses
          if(parameters.filter) {
            cv::Mat high,low;
            cv::boxFilter(feature_maps[scale_id],low,-1,cv::Size(scale_size,scale_size));
            int scale2 = int((scale_size/6))*2+1;
            cv::boxFilter(feature_maps[scale_id],high,-1,cv::Size(scale2,scale2));

            feature_maps[scale_id] = high-0.8*low;
          }
        }
      });

    }

    //**************************************************************************/
    // calcSubpixel() -
    //**************************************************************************/
    // Calcola la posizione subpixel a partire da tre campioni.
    inline float calcSubpixel(const float &x_l, const float &x, const float &x_r) {

      // prevent zero values
      if(x_l <= 0)
        return 0;
      if(x <= 0)
        return 0;
      if(x_r <= 0)
        return 0;
      const float l0 = float(std::log(x_l+1e-6));
      const float l1 = float(std::log(x+1e-6));
      const float l2 = float(std::log(x_r+1e-6));
      float delta = l2-l1-l1+l0;
      if(!delta) // this happens if all values are identical
        return 0;
      delta = (l0-l2)/(delta+delta);
      return delta;

    }

    //**************************************************************************/
    // findKeyPoints() -
    //**************************************************************************/
    // Estrae i keypoint dalla piramide di feature map.
    inline void findKeyPoints(const std::vector<cv::Mat> &feature_maps, std::vector<cv::KeyPoint>& keypoints, const cv::Mat& _mask) {

      //TODO check that all feature_maps have the same size
      int num_scales = parameters.max_scale-parameters.min_scale;
      CV_CheckGE(int(feature_maps.size()), num_scales, "missing feature maps");
      if (!_mask.empty()) {
        CV_CheckTypeEQ(_mask.type(), CV_8UC1, "wrong mask type");
        CV_CheckEQ(_mask.size(), feature_maps.front().size(),"wrong mask type or size");
      }
      keypoints.clear();

      cv::Mat mask;
      if(!_mask.empty())
        mask = _mask;
      else
        mask = cv::Mat::ones(feature_maps.front().size(),CV_8UC1);

      int super_res = int(parameters.super_resolution);
      int super_scale = super_res+1;
      float super_comp = 0.25F*super_res;

      // for each scale
      float strength = parameters.strength;
      std::vector<int> windows;
      cv::Point pt,pt2;
      double min,max;
      cv::Mat src;
      for(int scale=parameters.max_scale;scale>=parameters.min_scale;--scale) {
        int window_size = (1 << (scale + super_res)) + 1;
        float window_size2 = 0.5F*window_size;
        float window_size4 = 0.25F*window_size;
        int window_size2i = cvRound(window_size2);

        const cv::Mat &feature_map = feature_maps[scale-parameters.min_scale];
        int y = ((feature_map.rows)/window_size)-2;
        int x = ((feature_map.cols)/window_size)-2;
        for(int row=1;row<y;++row) {
          for(int col=1;col<x;++col) {
            cv::Rect rect(col*window_size,row*window_size,window_size,window_size);
            src = feature_map(rect);
            cv::minMaxLoc(src,&min,&max,NULL,&pt);
            if(min == max || max < strength)
              continue;

            cv::Point pos(pt.x+rect.x,pt.y+rect.y);
            if(mask.at<unsigned char>(pos.y,pos.x) == 0)
              continue;

            cv::Rect rect2(int(pos.x-window_size2),int(pos.y-window_size2),window_size,window_size);
            src = feature_map(rect2);
            cv::minMaxLoc(src,NULL,NULL,NULL,&pt2);
            if(pos.x == pt2.x+rect2.x && pos.y == pt2.y+rect2.y) {
              // the point is the best one on the current scale
              // check all larger scales if there is a stronger one
              double max2;
              int scale2= scale-1;
              //parameters.min_scale;
              for(;scale2>=parameters.min_scale;--scale2) {
                cv::minMaxLoc(feature_maps[scale2-parameters.min_scale](rect),NULL,&max2,NULL,NULL);
                if(max2 > max)
                  break;
              }
              if(scale2<parameters.min_scale && pos.x+1 < feature_map.cols && pos.y+1 < feature_map.rows) {
                float sub_x = float(calcSubpixel(feature_map.at<float>(pos.y,pos.x-1),
                        feature_map.at<float>(pos.y,pos.x),
                        feature_map.at<float>(pos.y,pos.x+1)));
                float sub_y = float(calcSubpixel(feature_map.at<float>(pos.y-1,pos.x),
                        feature_map.at<float>(pos.y,pos.x),
                        feature_map.at<float>(pos.y+1,pos.x)));
                cv::KeyPoint kpt(sub_x+pos.x,sub_y+pos.y,float(window_size),0.F,float(max),scale);
                int x2 = std::max(0,int(kpt.pt.x-window_size4));
                int y2 = std::max(0,int(kpt.pt.y-window_size4));
                int w = std::min(int(mask.cols-x2),window_size2i);
                int h = std::min(int(mask.rows-y2),window_size2i);
                mask(cv::Rect(x2,y2,w,h)) = 0.0;
                if(super_scale != 1) {
                  kpt.pt.x /= super_scale;
                  kpt.pt.y /= super_scale;
                  kpt.pt.x -= super_comp;
                  kpt.pt.y -= super_comp;
                  kpt.size /= super_scale;
                }
                keypoints.push_back(kpt);
              }
            }
          }
        }
      }

    }

    //**************************************************************************/
    // calcSubPos() -
    //**************************************************************************/
    // Calcola l'offset subpixel tramite interpolazione quadratica.
    inline float calcSubPos(const float &x_l, const float &x, const float &x_r) {

      float val = 2.0F *(x_l-2.0F*x+x_r);
      if(val == 0.0F)
        return 0.0F;
      val = (x_l-x_r)/val;
      if(val > 1.0F)
        return 1.0F;
      if(val < -1.0F)
        return -1.0F;
      return val;

    }

    static const float RAD2DEG = float(180.0/CV_PI);

    //**************************************************************************/
    // calcAngles() -
    //**************************************************************************/
    // Calcola gli angoli dominanti di ogni keypoint usando le immagini ruotate.
    inline std::vector<std::vector<float> > calcAngles(const std::vector<cv::Mat> &rotated_images, std::vector<cv::KeyPoint> &keypoints) {

      // validate rotated_images
      if(rotated_images.empty())
        CV_Error(cv::Error::StsBadArg,"no rotated images");
      std::vector<cv::Mat>::const_iterator iter = rotated_images.begin();
      for(;iter != rotated_images.end();++iter) {
        if(iter->empty())
          CV_Error(cv::Error::StsBadArg,"empty rotated images");
        if(iter->channels() < 4)
          CV_Error(cv::Error::StsBadArg,"rotated images must have at least four channels");
      }

      // assuming all elements of the same channel
      const int channels = rotated_images.front().channels();
      const int channels_1 = channels-1;
      const float resolution = float(CV_PI/channels);
      const float scale = float(parameters.super_resolution)+1.0F;

      // for each keypoint
      std::vector<std::vector<float> > angles;
      angles.resize(keypoints.size());
      parallel_for_(cv::Range(0,(int)keypoints.size()),[&](const cv::Range& range) {
        float angle;
        float val1,val2,val3,wrap_around;
        const unsigned char *pimages1,*pimages2,*pimages3,*pimages4;
        std::vector<cv::KeyPoint>::iterator pt_iter = keypoints.begin()+range.start;
        std::vector<cv::KeyPoint>::iterator pt_end = keypoints.begin()+range.end;
        for(int id=range.start ;pt_iter != pt_end;++pt_iter,++id) {
          int scale_id = pt_iter->octave - parameters.min_scale;
          if(scale_id>= int(rotated_images.size()) ||scale_id < 0)
            CV_Error(cv::Error::StsBadArg,"no rotated image for requested keypoint octave");
          const cv::Mat &s_rotated_images = rotated_images[scale_id];

          float x2 = pt_iter->pt.x*scale;
          float y2 = pt_iter->pt.y*scale;
          int row = int(y2);
          int col = int(x2);
          x2 -= col;
          y2 -= row;
          float x1 = 1.0F-x2; float y1 = 1.0F-y2;
          float a = x1*y1; float b = x2*y1; float c = x1*y2; float d = x2*y2;
          pimages1 = s_rotated_images.ptr<unsigned char>(row,col);
          pimages2 = s_rotated_images.ptr<unsigned char>(row,col+1);
          pimages3 = s_rotated_images.ptr<unsigned char>(row+1,col);
          pimages4 = s_rotated_images.ptr<unsigned char>(row+1,col+1);
          std::vector<float> &angles_i = angles[id];

          //calc rating
          val1 = a**(pimages1+channels_1)+b**(pimages2+channels_1)+
              c**(pimages3+channels_1)+d**(pimages4+channels_1);        // wrap around (last value)
          wrap_around = a**(pimages1++)+b**(pimages2++)+c**(pimages3++)+d**(pimages4++); // first value
          val2 = wrap_around;                                                             // first value
          for(int i=0;i<channels-1;++pimages1,++pimages2,++pimages3,++pimages4,++i) {
            val3 = a**(pimages1)+b**(pimages2)+c**(pimages3)+d**(pimages4);
            if(val1 <= val2) {
              if(val3 < val2) {
                angle = float((calcSubPos(val1,val2,val3)+i)*resolution);
                if(angle < 0)
                  angle += float(CV_PI);
                else if(angle > CV_PI)
                  angle -= float(CV_PI);
                angles_i.push_back(angle);
                pt_iter->angle = 360.0F-angle*RAD2DEG;
              }
            } else if(val1 > val2 && val3 >= val2) {
              angle = float((calcSubPos(val1,val2,val3)+i)*resolution);
              if(angle < 0)
                angle += float(CV_PI);
              else if(angle > CV_PI)
                angle -= float(CV_PI);
              angles_i.push_back(-angle);
              pt_iter->angle = 360.0F-angle*RAD2DEG;
            }
            val1 = val2;
            val2 = val3;
          }
          // wrap around
          if(val1 <= val2) {
            if(wrap_around< val2) {
              angle = float((calcSubPos(val1,val2,wrap_around)+channels-1)*resolution);
              if(angle < 0)
                angle += float(CV_PI);
              else if(angle > CV_PI)
                angle -= float(CV_PI);
              angles_i.push_back(angle);
              pt_iter->angle = 360.0F-angle*RAD2DEG;
            }
          } else if(val1 > val2 && wrap_around >= val2) {
            angle = float((calcSubPos(val1,val2,wrap_around)+channels-1)*resolution);
            if(angle < 0)
              angle += float(CV_PI);
            else if(angle > CV_PI)
              angle -= float(CV_PI);
            angles_i.push_back(-angle);
            pt_iter->angle = 360.0F-angle*RAD2DEG;
          }
        }
      });
      return angles;

    }

    //**************************************************************************/
    // sortKeyPoint() -
    //**************************************************************************/
    // Ordina i keypoint per response (decrescente). I keypoint con lo score
    // migliore vengono per primi.
    static bool sortKeyPoint(const cv::KeyPoint &pt1, const cv::KeyPoint &pt2) {
      return pt1.response > pt2.response;
    }

    //**************************************************************************/
    // testPointSymmetry() -
    //**************************************************************************/
    // Conta i fallimenti del test di simmetria attorno a un punto (su 10).
    inline int testPointSymmetry(const cv::Mat &mat, cv::Point2f pt, float dist, float max_error) {

      cv::Rect image_rect(int(0.5*dist),int(0.5*dist),int(mat.cols-0.5*dist),int(mat.rows-0.5*dist));
      cv::Size size(int(0.5*dist),int(0.5*dist));
      int count = 0;
      cv::Mat patch1,patch2;
      cv::Point2f center1,center2;
      for (int angle_i = 0; angle_i < 10; angle_i++) {
        double angle = angle_i * (CV_PI * 0.1);
        cv::Point2f n(float(cos(angle)),float(-sin(angle)));
        // patch fuori dall'immagine: il punto va scartato, quindi ritorno
        // il massimo numero di errori (la funzione conta i fallimenti, non è un bool)
        center1 = pt+dist*n;
        if(!image_rect.contains(center1))
          return MAX_SYMMETRY_ERRORS+1;
        center2 = pt-dist*n;
        if(!image_rect.contains(center2))
          return MAX_SYMMETRY_ERRORS+1;
        cv::getRectSubPix(mat,size,center1,patch1);
        cv::getRectSubPix(mat,size,center2,patch2);
        if(fabs(cv::mean(patch1)[0]-cv::mean(patch2)[0]) > max_error)
          ++count;
      }
      return count;

    }

    //**************************************************************************/
    // filterKeyPoints() -
    //**************************************************************************/
    // Filtra i keypoint usando feature map, simmetria e controllo degli angoli.
    inline void filterKeyPoints(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, std::vector<cv::Mat> &feature_maps,
            std::vector<std::vector<float> > &angles, const cv::Mat& mask) {

      keypoints.clear();
      angles.clear();
      std::vector<cv::KeyPoint> keypoints_temp;

      // Imposto i parametri specifici per le scacchiere direttamente sulla
      // "parameters" globale, letta da detectImpl/findKeyPoints/calcAngles.
      parameters.branches = 2;                     // this is always the case for checssboard corners
      // NOTA: valore incerto. In origine veniva impostato strength = 150 (ma su una
      //       struct locale mai usata, quindi di fatto valeva il 40 di default).
      //       Per preservare il comportamento storico uso 40; è possibile che
      //       l'intento fosse 150 -> da verificare sui dati reali.
      parameters.strength = 40;                    // minimal threshold (forse 150? vedi nota sopra)
      parameters.resolution = float(CV_PI*0.25);   // this gives the best results taking interpolation into account
      parameters.filter = 1;

      std::vector<cv::Mat> rotated_images;
      detectImpl(image,rotated_images,feature_maps,mask);

      //calculate seed chessboard corners
      findKeyPoints(feature_maps,keypoints_temp,mask);

      //sort points and limit number
      int max_seeds = std::min((int)keypoints_temp.size(),CBparameters.max_points);
      if(max_seeds < 9)
        return;

      std::partial_sort(keypoints_temp.begin(),keypoints_temp.begin()+max_seeds,
              keypoints_temp.end(),sortKeyPoint);
      keypoints_temp.resize(max_seeds);
      std::vector<std::vector<float> > angles_temp  = calcAngles(rotated_images,keypoints_temp);

      // filter out keypoints which are not symmetric
      std::vector<cv::KeyPoint>::iterator iter1 = keypoints_temp.begin();
      std::vector<std::vector<float> >::const_iterator iter2 = angles_temp.begin();
      for(;iter1 != keypoints_temp.end();++iter1,++iter2) {
        cv::KeyPoint &pt = *iter1;
        const std::vector<float> &angles_i3 = *iter2;
        if(angles_i3.size() != 2)// || pt.response < noise)
          continue;
        int result = testPointSymmetry(image,pt.pt,pt.size*0.7F,std::max(10.0F,sqrt(pt.response)+0.5F*pt.size));
        if(result > MAX_SYMMETRY_ERRORS)
          continue;
        else if(result > 3)
          pt.response = - pt.response;
        angles.push_back(angles_i3);
        keypoints.push_back(pt);
      }

    }

    //**************************************************************************/
    // cornerSubPixNewImpl() -
    //**************************************************************************/
    // Calcola un singolo angolo raffinato a partire da una ROI.
    inline cv::Point2d cornerSubPixNewImpl(const cv::Mat image) {

      std::vector<cv::Mat> rotated_images;
      std::vector<cv::Mat> feature_maps;
      std::vector<std::vector<float>> angles;
      cv::Mat mask;
      std::vector<cv::KeyPoint> keypoints;

      filterKeyPoints(image, keypoints, feature_maps, angles, mask);

      std::vector<cv::Point2f> points;

      std::sort(keypoints.begin(), keypoints.end(), sortKeyPoint);

      if(keypoints.empty())
        return cv::Point2d(0,0);

      return cv::Point2d(keypoints[0].pt.x, keypoints[0].pt.y);

    }

    //**************************************************************************/
    // fitRectangleInImage() -
    //**************************************************************************/
    // Limita la ROI alle coordinate valide dell'immagine.
    inline void fitRectangleInImage(const cv::Mat& image, cv::Rect &roi) {

      //limiti dell'immagine
      int leftBound = 0, topBound = 0, bottomBound = image.rows, rightBound = image.cols;

      if(roi.x < leftBound)
        roi.x = leftBound;
      if(roi.y < topBound)
        roi.y = topBound;
      if(roi.x + roi.width > rightBound)
        roi.width = rightBound - roi.x;
      if(roi.y + roi.height > bottomBound)
        roi.height = bottomBound - roi.y;

    }

    //**************************************************************************/
    // cornerSubPixNew() -
    //**************************************************************************/
    // Raffina le posizioni degli angoli usando il nuovo detector della scacchiera.
    inline void cornerSubPixNew(const cv::Mat image, std::vector<cv::Point2f> & corners, int roiSize = 60) {

      for(auto & corner : corners) {

        int x = corner.x - roiSize/2;  // Centro della maschera
        int y = corner.y - roiSize/2;

        cv::Rect roi(x, y, roiSize, roiSize);
        fitRectangleInImage(image, roi);
        cv::Mat subImage = image(roi);

        cv::Point2d refined = cornerSubPixNewImpl(subImage);

        // il detector ritorna (0,0) quando fallisce: in quel caso
        // conservo la stima originale invece di azzerare il corner
        if(refined.x != 0 || refined.y != 0) {
          corner.x = refined.x + roi.x; // mi riporto nel sistema di riferimento di image
          corner.y = refined.y + roi.y;
        }

      }

    }

  } /* namespace detail */

  //*****************************************************************************/
  // get() -
  //*****************************************************************************/
  // Refine corner positions to subpixel accuracy.
  inline void get(const cv::Mat & image, std::vector<cv::Point2f> & corners, int roiSize = 15, const std::string & algorithm = "old") {

    if(algorithm.compare("old") != 0 && algorithm.compare("new") != 0) {
      cvl::log::error("cvl::subpixel::get() error: unknown algorithm '%s'\n"
                      "You can choose between:\n"
                      "      old, new\n", algorithm.c_str());
    }

    if(corners.size() > 0) {

      // Talpa vecchio
      //cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1);

      // Criteri di stop
      cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001);

      // Cerco i subpixel con l'algoritmo vecchio
      // (cornerSubPix vuole la mezza finestra, quindi dimezzo roiSize)
      if(!algorithm.compare("old"))
        cv::cornerSubPix(image, corners, cv::Size(roiSize/2,roiSize/2), cv::Size(-1,-1), criteria);

      // Cerco i subpixel con l'algoritmo nuovo
      // (il detector multiscala lavora sull'intera ROI, quindi non dimezzo)
      if(!algorithm.compare("new"))
        detail::cornerSubPixNew(image, corners, roiSize);

    }

  }

} /* namespace cvl::subpixel */


#endif /* _H_CVL_SUBPIXEL_H_ */
