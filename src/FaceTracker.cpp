//
//  FaceTracker.cpp
//  example_FaceTracker
//
//  Created by Roy Macdonald on 15-01-16.
//
//

#include "FaceTracker.h"
using namespace ofxDLib;
//--------------------------------------------------------------
void FaceTracker::setup(string predictorDatFilePath, int trackType){
    
    if(trackType == 0){
        detector = dlib::get_frontal_face_detector();
        if(predictorDatFilePath.empty()){
            predictorDatFilePath = ofToDataPath("shape_predictor_68_face_landmarks.dat");
        }
        ofFile f(predictorDatFilePath);
        if (f.exists()) {
            dlib::deserialize(f.getAbsolutePath()) >> sp;
        }else{
            ofLogError("ofxDLib::FaceTracker","SHAPE PREDICTOR DAT FILE MISSING!!!");
        }
    }
    
    if(trackType == 1){
        
    }
}
//--------------------------------------------------------------
void FaceTracker::findFaces(const ofPixels& pixels, bool bUpscale){
    
    dlib::array2d< dlib::rgb_pixel> img;
    
    toDLib(pixels , img);
    if (bUpscale) {
        pyramid_up(img);
    }
    
    dets.clear();
    dets = detector(img);
    shapes.clear();
    for (unsigned long j = 0; j < dets.size(); ++j){
        shapes.push_back(sp(img, dets[j]));
    }
}
//--------------------------------------------------------------
void FaceTracker::draw(){
    ofPushStyle();
    
    ofSetColor(ofColor::red);
    ofNoFill();
    for (auto& r:dets) {
        ofDrawRectangle(toOf(r));
    }
    
    ofFill();
    ofSetColor(ofColor::red);
    for (auto & s:shapes) {
        for (int i = 0; i < s.num_parts(); i++) {
            ofDrawCircle(toOf(s.part(i)),3);
        }
    }
    ofPopStyle();
}
//--------------------------------------------------------------
void FaceTracker::setNewSelection(ofRectangle _rect){
    selectionTrackerInit = false;
    sel_x = _rect.getX();
    sel_y = _rect.getY();
    sel_w = _rect.getWidth();
    sel_h = _rect.getHeight();
}

//--------------------------------------------------------------
void FaceTracker::findObjects(const ofPixels& pixels, bool bUpscale){
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

