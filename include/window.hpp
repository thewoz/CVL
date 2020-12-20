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


#ifndef _H_CVL_WINDOW_H_
#define _H_CVL_WINDOW_H_

#include <cstdio>
#include <cstdlib>

#include <string>

#include <opencv2/opencv.hpp>


//****************************************************************************/
// namescape cvl
//****************************************************************************/
namespace cvl {
  
  //****************************************************************************/
  // class window_t
  //****************************************************************************/
  class window_t {
  
  private:
    
    std::string name;
    
    int width;
    int height;
    
    int screenPosX;
    int screenPosY;

    int mouseClickX;
    int mouseClickY;

    //****************************************************************************/
    // mouseHandler
    //****************************************************************************/
    static void mouseHandler(int event, int x, int y, int flags, void * window) {
      
      if(event == cv::EVENT_LBUTTONUP) {
        
        ((window_t*)window)->mouseClickX = x;
        ((window_t*)window)->mouseClickX = y;

        ((window_t*)window)->processMouseEvent();
        
      }
      
    }
    
    //****************************************************************************/
    // processMouseEvent
    //****************************************************************************/
    virtual void processMouseEvent() {}
    
  public:
    
    //****************************************************************************/
    // window_t()
    //****************************************************************************/
    window_t(const std::string & _name) {
      
      name = _name;
      
      cv::namedWindow(name, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL);
      
      cv::setMouseCallback(name, mouseHandler, this);
      
    }
    
    //****************************************************************************/
    // ~window_t()
    //****************************************************************************/
    ~window_t() {
      
      cv::destroyWindow(name);
    }
    
    //****************************************************************************/
    // show() -
    //****************************************************************************/
    void show(const cv::Mat & image, int time = 0) {
      
      cv::imshow(name, image);
      
      if(time == 0) {
        
        time = 250;
        
        while(cv::getWindowProperty(name, cv::WND_PROP_VISIBLE) >= 1)
          cv::waitKey(time);
        
      } else { cv::waitKey(time); }

    }
    
    //****************************************************************************/
    // resize() -
    //****************************************************************************/
    void resize(int _width, int _height) {
      
      width  = _width;
      height = _height;
      
      cv::resizeWindow(name, width, height);
      
    }
    
    //****************************************************************************/
    // move() -
    //****************************************************************************/
    void move(int _screenPosX, int _screenPosY) {
      
      screenPosX = _screenPosX;
      screenPosY = _screenPosY;
      
      cv::moveWindow(name, screenPosX, screenPosY);
      
    }

  }; /* class window_t */

} /* namespace cvl */


#endif /* _H_CVL_WINDOW_H_ */
