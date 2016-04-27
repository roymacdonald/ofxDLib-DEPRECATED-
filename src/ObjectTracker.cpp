//
//  ObjectTracker.cpp
//  example-FaceTracker
//
//  Created by Stephan Schulz on 2016-03-26.
//
//

#include "ObjectTracker.h"
using namespace ofxDLib;
//--------------------------------------------------------------
void ObjectTracker::setup(string predictorDatFilePath){

}
//--------------------------------------------------------------
void ObjectTracker::draw(){
    ofPushStyle();
    
    ofSetColor(ofColor::red);
    ofNoFill();
    for (auto& r:dets) {
        ofDrawRectangle(toOf(r));
    }

    ofPopStyle();
}


//--------------------------------------------------------------
void ObjectTracker::setNewSelection(ofRectangle _rect){
    selectionTrackerInit = false;
    sel_x = _rect.getX();
    sel_y = _rect.getY();
    sel_w = _rect.getWidth();
    sel_h = _rect.getHeight();
}

//--------------------------------------------------------------
void ObjectTracker::findObjects(const ofPixels& pixels, bool bUpscale){
    //http://dlib.net/video_tracking_ex.cpp.html
    //http://blog.dlib.net/2015/02/dlib-1813-released.html
    
    dlib::array2d< dlib::rgb_pixel> img;
    toDLib(pixels , img);
    
    if (bUpscale) {
        pyramid_up(img);
    }
    
    if(selectionTrackerInit == false){
        selectionTrackerInit = true;
        
        // Now create a tracker and start a track on the juice box.  If you look at the first
        // frame you will see that the juice box is centered at pixel point(92,110) and 38
        // pixels wide and 86 pixels tall.
        
        cout<<"sel_x "<<sel_x<<" "<<sel_y<<" "<<sel_w;
        tracker.start_track(img, dlib::rectangle(sel_x,sel_y, sel_x+sel_w, sel_y+sel_h));
        //            centered_rect(point(sel_x,sel_y), sel_w, sel_h));
        
        dets.push_back(dlib::rectangle(0,0,1,1));
    }else{
        tracker.update(img);
        dets.back() = tracker.get_position();
        
    }
    
    
}

