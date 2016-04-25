#include "ofApp.h"

using namespace ofxDLib;

void FaceAugmented::setup(const Face & track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = track.rect.getCenter();
    smooth = cur;
    roi = track.rect;
    face = track;
    leftEye = track.leftEyeCenter;
    rightEye = track.rightEyeCenter;
    velocity = track.velocity;
}

void FaceAugmented::update(const Face & track) {
    cur = track.rect.getCenter();
    roi = track.rect;
    smooth.interpolate(cur, .5);
    all.addVertex(smooth);
    face = track;
    label = track.label;
    leftEye = track.leftEyeCenter;
    rightEye = track.rightEyeCenter;
    
    
    alpha = 255;
}

void FaceAugmented::setImage(const ofPixels & pixels) {
    pixels.cropTo(image.getPixels(), roi.getX(), roi.getY(), roi.getWidth(), roi.getHeight());
    image.update();
    // how about calculating fisherfaces at this point?!
}

void FaceAugmented::kill() {
    float curTime = ofGetElapsedTimef();
    if(startedDying == 0) {
        startedDying = curTime;
    } else if(curTime - startedDying > dyingTime) {
        dead = true;
    }
    
    alpha = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, 255, 0, true);
}

void FaceAugmented::draw() {
//    ofPushStyle();
//    ofSetColor(255);
    image.draw(roi);
    
    ofSetColor(255);
    float size = 16;
    if(startedDying) {
        ofSetColor(ofColor::red);
        size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
    }
    ofNoFill();
    ofDrawCircle(cur, size);

    ofSetColor(color);
    all.draw();
    
    ofSetColor(255);
    ofDrawBitmapString(ofToString(label), cur);
    
//    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::setup(){
    ft.setup("shape_predictor_68_face_landmarks.dat");
    ft.setSmoothingRate(0.5);
    ft.setDrawStyle(ofxDLib::lines);
    

    video.setDeviceID(0);
    video.setup(720, 480);
    
    tracker.setPersistence(20);
    tracker.setMaximumDistance(170);
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    if(video.isFrameNew()){
        
        ft.findFaces(video.getPixels());
        tracker.track(ft.getFaces());
   
        //look at using lastSeen instead of dyingTime
         for(int i=0;i<tracker.getFollowers().size(); i++){
             unsigned int temp_label = tracker.getLabelFromIndex(i);
             ofLog()<<temp_label<<" last seen_ "<<tracker.getLastSeen(temp_label);
         }
        
        //set smoothing seperatly for each face based on velocity
        for(int i=0;i<ft.size(); i++){
            
            float temp_v = ft.getVelocity(i).length();
            temp_v = ofMap(temp_v,0,40,0.3,1,true);
            unsigned int temp_label = ft.getLabel(i);
            
            ft.setSmoothingRate(temp_label,temp_v);
        }
        
        vector<FaceAugmented>& facesAugmented = tracker.getFollowers();
        for (auto & face : facesAugmented) {
            face.setImage(video.getPixels());
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(128);
    video.draw(0, 0);
    ofEnableAlphaBlending();
    vector<FaceAugmented>& facesAugmented = tracker.getFollowers();
    for (auto & face : facesAugmented) {

        ofSetColor(255,255,255,face.alpha);
        face.draw();
        
        ofSetColor(255,255,255,200);
        ofDrawCircle(face.leftEye, 7);
        ofDrawCircle(face.rightEye, 5);
    }
    ofDisableAlphaBlending();
    
    ft.draw();
}