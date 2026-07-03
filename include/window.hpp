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

#ifndef _H_CVL_WINDOW_H_
#define _H_CVL_WINDOW_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdio>
#include <cstdlib>

#include <string>

//****************************************************************************/
// namescape cvl
//****************************************************************************/
namespace cvl {
  
  //****************************************************************************/
  // class window_t
  //****************************************************************************/
  // Simple OpenCV window wrapper with event hooks.
  class window_t {
  
  protected:
    
    static std::vector<window_t *> windows;
     
    static int idCounter;
    
    int id;
    
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
    // Static mouse callback that dispatches to the instance handler.
    static void mouseHandler(int event, int x, int y, int flags, void * window) {
      
      // azzero il bit del flag (con -= il flag non settato corrompeva il valore)
      if(event == cv::EVENT_LBUTTONDOWN || event == cv::EVENT_LBUTTONUP || event == cv::EVENT_LBUTTONDBLCLK) flags &= ~cv::EVENT_FLAG_LBUTTON;
      if(event == cv::EVENT_RBUTTONDOWN || event == cv::EVENT_RBUTTONUP || event == cv::EVENT_RBUTTONDBLCLK) flags &= ~cv::EVENT_FLAG_RBUTTON;
      if(event == cv::EVENT_MBUTTONDOWN || event == cv::EVENT_MBUTTONUP || event == cv::EVENT_MBUTTONDBLCLK) flags &= ~cv::EVENT_FLAG_MBUTTON;
      if(event == cv::EVENT_MOUSEMOVE) flags &= ~cv::EVENT_FLAG_RBUTTON;
      
      ((window_t*)window)->processMouseEvent(event, x, y, flags);

    }
    
    //****************************************************************************/
    // processMouseEvent
    //****************************************************************************/
    // Override to handle mouse events.
    virtual void processMouseEvent(int /* event */, int /* x */, int /* y */, int /* flags */) { }
    
    //****************************************************************************/
    // processKeyboardEvent
    //****************************************************************************/
    // Override to handle keyboard events.
    virtual void processKeyboardEvent(int /* key */) { }
    
    //****************************************************************************/
    // processProgramEvent
    //****************************************************************************/
    //virtual void processProgramEvent() { }

    //****************************************************************************/
    // processBroadcastEvent
    //****************************************************************************/
    // Override to respond to broadcast events from other windows.
    virtual void processBroadcastEvent() { }
    
    //****************************************************************************/
    // processKeyboardEvent
    //****************************************************************************/
    // Internal keyboard event dispatcher to all windows.
    void _processKeyboardEvent(int key) {
      
      // ciclo sulle finestre
      for(size_t i=0; i<windows.size(); ++i) {
        windows[i]->processKeyboardEvent(key);
      }
      
    }

  public:
    
    enum cursor {
           ARROW    = 0,  //!< Arrow Cursor
           CROSS    = 2,  //!< Cross Cursor
           WAIT     = 3,  //!< Wait Cursor
           OPENHAND = 17, //!< Open Hand Cursor
           LOSEHAND = 18  //!< Close Hand Cursor
         };
    
    //****************************************************************************/
    // window_t()
    //****************************************************************************/
    // Construct an uninitialized window.
    window_t() : id (++idCounter) {  }
    
    //****************************************************************************/
    // window_t()
    //****************************************************************************/
    // Construct and initialize a window with a title.
    window_t(const std::string & _name, int flags = cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL) : id (++idCounter) { init(_name, flags); }
    
    //****************************************************************************/
    // ~window_t()
    //****************************************************************************/
    // Close and unregister the window.
    ~window_t() { close(); }
    
    //****************************************************************************/
    // close()
    //****************************************************************************/
    // Close the window and unregister it.
    void close() {
      if(isInit) {
        isInit = false;
        auto itr = std::find(windows.begin(), windows.end(), this);
        windows.erase(itr);
        //cv::destroyWindow(name);
      }
    }

    //****************************************************************************/
    // isInited()
    //****************************************************************************
    // Return whether the window is initialized.
    bool isInited() { return isInit; }
    
    //****************************************************************************/
    // init() -
    //****************************************************************************/
    // Initialize the window and register callbacks.
    void init(const std::string & _name, int flags = cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL) {
      
      if(isInit) {
        cvl::log::error("cvl::window_t error: the window is already initialized\n");
      }
      
      name = _name;
      
      cv::namedWindow(name, flags);
            
      cv::setMouseCallback(name, mouseHandler, this);
      
      windows.push_back(this);
      
      isToClose = false;
      
      isInit = true;
      
    }
    
    //****************************************************************************/
    // show()
    //****************************************************************************/
    // Show an image with an updated title.
    int show(const cv::Mat & _image, const std::string & str, int time = 0) {
      
      setTitle(str);
      
      return show(_image, time);
      
    }
    
    //****************************************************************************/
    // show()
    //****************************************************************************/
    // Show an image and optionally wait for keyboard input.
    int show(const cv::Mat & _image, int time = 0) {
      
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }

      cv::imshow(name, _image);
            
      int key = -1;
      
      // se devo aspettare un tempo indefinito
      if(time == 0) {
        
        time = 100;
        
        while(cv::getWindowProperty(name, cv::WND_PROP_VISIBLE) >= 1) {
          //processProgramEvent();
          key = cv::waitKey(time);
          if(key != -1) {
            _processKeyboardEvent(key);
            break;
          }
          if(isToClose) break;
        }
      
      // se devo aspettare un tempo fissato
      } else {
        key = cv::waitKey(time);
        if(key != -1) _processKeyboardEvent(key);
      }
      
      return key;
      
    }
    
    //****************************************************************************/
    // setTitle()
    //****************************************************************************/
    // Set the window title.
    void setTitle(const std::string & str) {
      
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }
      
      cv::setWindowTitle(name, str);
            
    }
    
    //****************************************************************************/
    // toggleFullScreen() -
    //****************************************************************************/
    // Toggle full-screen mode.
    void toggleFullScreen() {
            
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }
      
      isFullScreen = !isFullScreen;
      
      if( isFullScreen) cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
      if(!isFullScreen) cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);
      
    }
    
    //****************************************************************************/
    // setFullScreen() -
    //****************************************************************************/
    // Enable full-screen mode.
    void setFullScreen() {
            
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }
      
      isFullScreen = true;
      
      cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
      
    }
    
    //****************************************************************************/
    // unsetFullScreen() -
    //****************************************************************************/
    // Disable full-screen mode.
    void unsetFullScreen() {
            
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }
      
      isFullScreen = false;
      
      cv::setWindowProperty(name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);
      
    }
    
    //****************************************************************************/
    // update() -
    //****************************************************************************/
    // Update the window contents without waiting.
    void update(const cv::Mat & _image, std::string title = "") {
      
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }
      
      if(!title.empty()) setTitle(title);
      
      cv::imshow(name, _image);
      
    }
    
    //****************************************************************************/
    // resize() -
    //****************************************************************************/
    // Resize the window.
    void resize(int _width, int _height) {
      
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }
      
      width  = _width;
      height = _height;
      
      cv::resizeWindow(name, width, height);
      
    }
    
    //****************************************************************************/
    // move() -
    //****************************************************************************/
    // Move the window on screen.
    void move(int _screenPosX, int _screenPosY) {
      
      if(!isInit) {
        cvl::log::error("cvl::window_t error: the window must be initialized first\n");
      }
      
      screenPosX = _screenPosX;
      screenPosY = _screenPosY;
      
      cv::moveWindow(name, screenPosX, screenPosY);
      
    }

    //****************************************************************************/
    // broadcast()
    //****************************************************************************
    // Broadcast an event to other windows.
    void broadcast() { for(size_t i=0; i<windows.size(); ++i) if(windows[i]->id != id) windows[i]->processBroadcastEvent(); }
    
    //****************************************************************************/
    // setCursor() -
    //****************************************************************************/
    // Set the cursor type if supported by OpenCV.
    void setCursor(int cursorTypes) {
      
#ifdef OPENCV_EXTENDED_QT_GUI
      
      cv::setDefaultCursor(name, cursorTypes);
      
#else

      (void) cursorTypes;

      cvl::log::warning("cvl::window_t::setCursor() warning: the function is not available with this version of OpenCV\n");

#endif
      
    }
    
  }; /* class window_t */

  inline std::vector<window_t *> window_t::windows = std::vector<window_t *>();

  inline int window_t::idCounter = -1;

} /* namespace cvl */


#endif /* _H_CVL_WINDOW_H_ */
