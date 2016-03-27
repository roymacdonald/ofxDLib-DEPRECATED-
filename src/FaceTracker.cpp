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
void FaceTracker::setup(string predictorDatFilePath){
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
//--------------------------------------------------------------
tuple<int,int> FaceTracker::findFaces(const ofPixels& pixels, bool bUpscale){
    
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
    
    return make_tuple(img.nc(),img.nr());
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
