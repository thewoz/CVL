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


#ifndef _H_CVL_TOOLSBAR_H_
#define _H_CVL_TOOLSBAR_H_

#include <cstdio>
#include <cstdlib>

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>

#include <mpl/stdio.hpp>

//****************************************************************************/
// namespace cvl
//****************************************************************************/
namespace cvl {

  //****************************************************************************/
  // namespace toolsbar
  //****************************************************************************/
  namespace toolsbar {

    cv::Vec3b background(83, 83, 83);
    cv::Vec3b overlay(69, 69, 69);
    cv::Vec3b border(102, 102, 102);
    
    int toolSize = 50;
    int topSize  = 20;

  }

  //****************************************************************************/
  // tool_t
  //****************************************************************************/
  class tool_t {
    
  private:
    
    cv::Rect roi;
    
    int order;

    bool isOver     = false;
    bool isSelected = false;

    std::string icon = "\uf004";
    
    static cv::Ptr<cv::freetype::FreeType2> font;
    
  public:
    
    //****************************************************************************/
    // tool_t
    //****************************************************************************/
    tool_t(const std::string & _icon, int _order) : icon(_icon), order(_order) {
      
      if(font.empty()) {
        font = cv::freetype::createFreeType2();
        font->loadFontData("/usr/local/include/cvl/data/fa-solid-900.ttf", 0);
      }
      
    }
    
    //****************************************************************************/
    // processMouseEvent
    //****************************************************************************/
    bool processMouseEvent(int event, int x, int y, int flags) {
      
      // controllo se il mouse sta sopra il bottone
      if(roi.contains(cv::Point(x,y))) {
        
        // se stava muovendo
        if(event == cv::EVENT_MOUSEMOVE) { isOver = true; }
        
        // se ci ho cliccato
        if(event == cv::EVENT_LBUTTONDOWN) { isSelected = true; }
        
      } else { isOver = false; }
        
        
      return isSelected;
        
    }
    
    //****************************************************************************/
    // draw
    //****************************************************************************/
    void draw(cv::Mat & dst, int mode, int order) {

      if(isOver)  cv::rectangle(dst, roi, toolsbar::overlay,    cv::FILLED);
      if(!isOver) cv::rectangle(dst, roi, toolsbar::background, cv::FILLED);

      font->putText(dst, icon, cv::Point(roi.x, roi.y+toolsbar::toolSize), toolsbar::toolSize*0.5, cvl::White, cv::FILLED, cv::LINE_AA, true);

      cv::rectangle(dst, roi, toolsbar::border, 1);

    }

    //****************************************************************************/
    // update
    //****************************************************************************/
    void update(const cv::Point2i & offset) {
      
      cv::Point2i position;

      position.x = offset.x + (toolsbar::toolSize * order);
      position.y = offset.y + (toolsbar::toolSize * order);

      roi = cv::Rect(position.x, position.y, toolsbar::toolSize, toolsbar::toolSize);

    }
  
    //****************************************************************************/
    // resetSelected
    //****************************************************************************/
    void resetSelected() { isSelected = false; }
    
  }; /* class tool_t */

  cv::Ptr<cv::freetype::FreeType2> tool_t::font = cv::Ptr<cv::freetype::FreeType2>();

  //****************************************************************************/
  // toolsbar_t
  //****************************************************************************/
  class toolsbar_t {
    
  private:
    
    // lista dei tool
    std::vector<tool_t> tools;

    // posizione della barra
    cv::Point2i position;
    
    //
    enum { HORIZONTAL, VERTICAL};
    
    // modalità di visualizzazione
    int mode = HORIZONTAL;
    
    //
    cv::Rect roiToolsbar;
    
    //
    cv::Rect roiTop;

    //
    bool isClicked = false;

  public:
    
    //****************************************************************************/
    // toolsbar_t
    //****************************************************************************/
    toolsbar_t(const cv::Point2i & position = cv::Point2i(0,0)) { setPosition(position); }
    
    //****************************************************************************/
    // processMouseEvent
    //****************************************************************************/
    void processMouseEvent(int event, int x, int y, int flags) {
      
      // controllo se il mouse sta sopra il bottone
      if(roiTop.contains(cv::Point(x,y))) {
        
        // se ci ho cliccato sopra
        if(event == cv::EVENT_LBUTTONDOWN) { isClicked = true; }
        
        // se lo sto muovendo
        if(event == cv::EVENT_MOUSEMOVE && isClicked) { setPosition(cv::Point2i(x,y)); }
        
        // se l'ho lasciato
        if(event == cv::EVENT_LBUTTONUP) { isClicked = false; }
        
      }
      
      //NOTE: questo andrebbe fatto con un sistema di broadcast
      if(roiToolsbar.contains(cv::Point(x,y))) {
        int isSelected = -1;
        for(size_t i=0; i<tools.size(); ++i)
          if(tools[i].processMouseEvent(event, x, y, flags)) isSelected = (int)i;
        if(isSelected != -1) {
          for(size_t i=0; i<tools.size(); ++i)
            if(isSelected != i) tools[i].resetSelected();
        }
      }
      
    }
    
    //****************************************************************************/
    // draw
    //****************************************************************************/
    void draw(cv::Mat & dst) {
      
      // disegno il top
      cv::rectangle(dst, roiTop, toolsbar::overlay, cv::FILLED);
      cv::rectangle(dst, roiTop, toolsbar::border, 1);

      // disegno lo sfondo della toolsbar
      cv::rectangle(dst, roiToolsbar, toolsbar::background, cv::FILLED);
            
      // disegno i tools
      for(int i=0; i<tools.size(); ++i) tools[i].draw(dst, mode, i);
       
      // disegno il bordo della toolsbar
      cv::rectangle(dst, roiToolsbar, toolsbar::border, 1);

    }
    
    //****************************************************************************/
    // add
    //****************************************************************************/
    void add(const std::string & icon) {
      
      tools.push_back(cvl::tool_t(icon, (int)tools.size()));
      
      update();
      
    }
    
    //****************************************************************************/
    // setPosition
    //****************************************************************************/
    void setPosition(const cv::Point2i & coord) { position = coord; update(); }
    
    //****************************************************************************/
    // size_t
    //****************************************************************************/
    size_t size() const { return tools.size(); }
    
    //****************************************************************************/
    // operator []
    //****************************************************************************/
    const tool_t & operator [] (size_t index) const { return tools[index]; }
          tool_t & operator [] (size_t index)       { return tools[index]; }
    
  private:
    
    //****************************************************************************/
    // update
    //****************************************************************************/
    void update() {
      
      cv::Size sizeTop;
      cv::Size sizeToolsbar;
      
      
      if(mode == HORIZONTAL) {
        sizeTop.width  = toolsbar::topSize;
        sizeTop.height = toolsbar::toolSize;
        sizeToolsbar.width  = sizeTop.width  + (toolsbar::toolSize * (int)tools.size());
        sizeToolsbar.height = sizeTop.height +  toolsbar::toolSize;
      }
      
      if(mode == VERTICAL) {
        sizeTop.width  = toolsbar::toolSize;
        sizeTop.height = toolsbar::topSize;
        sizeToolsbar.width  = sizeTop.width  +  toolsbar::toolSize;
        sizeToolsbar.height = sizeTop.height + (toolsbar::toolSize * (int)tools.size());
      }
      
      roiTop = cv::Rect(position.x, position.y, sizeTop.width, sizeTop.height);
      roiToolsbar = cv::Rect(position.x, position.y, sizeToolsbar.width, sizeToolsbar.height);

      cv::Point2i offset(position.x+sizeTop.width, position.y+sizeTop.height);
      
      for(size_t i=0; i<tools.size(); ++i) tools[i].update(offset);
      
    }
    
    
  }; /* class toolsbar */

} /* namespace */

#endif /* _H_CVL_TOOLSBAR_H_ */
