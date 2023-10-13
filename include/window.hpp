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

//#include <QtWidgets/QWidget>
//#include <QtGui/QCursor>
//#include <QtGui/QGuiApplication>

//****************************************************************************/
// namescape cvl
//****************************************************************************/
namespace cvl {
  
  //****************************************************************************/
  // class window_t
  //****************************************************************************/
  class window_t {
  
  protected:
    
    std::string name;
    
    int width;
    int height;
    
    int screenPosX;
    int screenPosY;

    //int mouseClickX;
    //int mouseClickY;
    
    bool isToClose = false;
    
    //bool isToUpdate = false;
    
    bool isInit = false;
    
    bool isFullScreen = false;

    //cv::Mat image;
    
    //****************************************************************************/
    // mouseHandler
    //****************************************************************************/
    static void mouseHandler(int event, int x, int y, int flags, void * window) {
      
      ((window_t*)window)->processMouseEvent(event, x, y, flags);

    }
    
    //****************************************************************************/
    // processMouseEvent
    //****************************************************************************/
    virtual void processMouseEvent(int event, int x, int y, int flags) { }
    
    //****************************************************************************/
    // processKeyboardEvent
    //****************************************************************************/
    virtual int processKeyboardEvent(int key) { return -1; }
    
    //****************************************************************************/
    // processProgramEvent
    //****************************************************************************/
    virtual void processProgramEvent(void) { ; }


  public:
    
    //****************************************************************************/
    // window_t()
    //****************************************************************************/
    window_t() {  }
    
    //****************************************************************************/
    // window_t()
    //****************************************************************************/
    window_t(const std::string & _name, int flags = cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL) { init(_name, flags); }
    
    //****************************************************************************/
    // ~window_t()
    //****************************************************************************/
    ~window_t() { close(); }
    
    //****************************************************************************/
    // close()
    //****************************************************************************/
    void close() {
      if(isInit) {
        cv::destroyWindow(name);
        isInit = false;
      }
    }

    //****************************************************************************/
    // isInited()
    //****************************************************************************
    bool isInited() { return isInit; }
    
    //****************************************************************************/
    // init() -
    //****************************************************************************/
    void init(const std::string & _name, int flags = cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL) {
      
      if(isInit) {
        fprintf(stderr, "error the window is already inited\n");
        abort();
      }
      
      name = _name;
            
      cv::namedWindow(name, flags);

      cv::setMouseCallback(name, mouseHandler, this);
      
      isToClose = false;
      
      isInit = true;
      
    }
    
    //****************************************************************************/
    // show() -
    //****************************************************************************/
    int show(const cv::Mat & _image, const std::string & str, int time = 0, bool mode = true) {
      
      setTitle(str);
      
      return show(_image, time, mode);
      
    }
    
    //****************************************************************************/
    // show() -
    //****************************************************************************/
    int show(const cv::Mat & _image, int time = 0, bool mode = true) {
      
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }

      cv::imshow(name, _image);
            
      int key = -1;
      
      if(time == 0) { 
        
        time = 100;
        
        while(cv::getWindowProperty(name, cv::WND_PROP_VISIBLE) >= 1) {
          //if(isToUpdate) break;
          processProgramEvent();
          if((key=cv::waitKey(time)) != -1) {
            key = processKeyboardEvent(key);
            if(mode || isToClose) break;
          }
        }
        
          
      } else { key = cv::waitKey(time); }

      isToClose = false;
      
      return key; // processKeyboardEvent(key);
      
    }
    
    //****************************************************************************/
    // setTitle() -
    //****************************************************************************/
    void setTitle(const std::string & str) {
      
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }
      
      cv::setWindowTitle(name, str);
            
    }
    
    //****************************************************************************/
    // toggleFullScreen() -
    //****************************************************************************/
    void toggleFullScreen() {
            
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }
      
      isFullScreen = !isFullScreen;
      
      if( isFullScreen) cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
      if(!isFullScreen) cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);
      
    }
    
    //****************************************************************************/
    // setFullScreen() -
    //****************************************************************************/
    void setFullScreen() {
            
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }
      
      isFullScreen = true;
      
      cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
      
    }
    
    //****************************************************************************/
    // unsetFullScreen() -
    //****************************************************************************/
    void unsetFullScreen() {
            
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }
      
      isFullScreen = false;
      
      cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);
      
    }
    
    //****************************************************************************/
    // update() -
    //****************************************************************************/
    void update(const cv::Mat & _image, std::string title = "") {
      
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }
      
      if(!title.empty()) setTitle(title);
      
      cv::imshow(name, _image);
      
    }
    
    //****************************************************************************/
    // resize() -
    //****************************************************************************/
    void resize(int _width, int _height) {
      
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }
      
      width  = _width;
      height = _height;
      
      cv::resizeWindow(name, width, height);
      
    }
    
    //****************************************************************************/
    // move() -
    //****************************************************************************/
    void move(int _screenPosX, int _screenPosY) {
      
      if(!isInit) {
        fprintf(stderr, "error the window must be init before\n");
        abort();
      }
      
      screenPosX = _screenPosX;
      screenPosY = _screenPosY;
      
      cv::moveWindow(name, screenPosX, screenPosY);
      
    }

    //****************************************************************************/
    // setCursor() -
    //****************************************************************************/
    void setCursor() {
      
      //QWidget * handle = (QWidget*) cv::getWindowHandle(name);

      //handle->setCursor(Qt::WaitCursor);
      
      //QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
    }
    
  }; /* class window_t */

} /* namespace cvl */


#endif /* _H_CVL_WINDOW_H_ */
