#include "ofApp.h"

//using namespace ofxDLib;

void FaceAugmented::setup(const ofxDLib::Face & track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = track.rect.getCenter();
    smooth = cur;
    roi = track.rect;
    face = track;
    leftEye = track.leftEyeCenter;
    rightEye = track.rightEyeCenter;
    velocity = track.velocity;
}

void FaceAugmented::update(const ofxDLib::Face & track) {
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
    ofDrawBitmapString(ofToString(label)+" : "+ofToString(lastSeen), cur);
    
//    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::setup(){
    ft.setup("shape_predictor_68_face_landmarks.dat");
    ft.setSmoothingRate(0.5);
    ft.setDrawStyle(ofxDLib::lines);
    

    video.setDeviceID(0);
    video.setup(720, 480);
    
    persistance = 20;
    tracker.setPersistence(persistance);
    tracker.setMaximumDistance(170);
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    if(video.isFrameNew()){
        
        ft.findFaces(video.getPixels());
        tracker.track(ft.getFaces());
   
        //
//        vector<FaceAugmented>& facesAugmented = tracker.getFollowers();
//        
//         for(int i=0;i<facesAugmented.size(); i++){
//             unsigned int temp_label = tracker.getLabelFromIndex(i);
//             ofLog()<<temp_label<<" last seen_ "<<tracker.getLastSeen(temp_label);
////             tracker.getIndexFromLabel(temp_label);
//             facesAugmented[i].lastSeen = tracker.getLastSeen(temp_label);
//             facesAugmented[i].setImage(video.getPixels());
//         }
        
        
        //set smoothing seperatly for each face
        for(int i=0;i<ft.size(); i++){
            float temp_v = ft.getVelocity(i).length();
            temp_v = ofMap(temp_v,0,40,0.3,1,true);
            unsigned int temp_label = ft.getLabel(i);
            
            ft.setSmoothingRate(temp_label,temp_v);//only for landmarks, does not apply to .rect since that comes from the rectTracker direcly
            
        }
        
        vector<FaceAugmented>& facesAugmented = tracker.getFollowers();
    
        ofLog()<<"tracker.getFollowers().size() "<<tracker.getFollowers().size()<<" "<<ft.size()<<" "<<facesAugmented.size();
        for (auto & face : facesAugmented) {
            face.setImage(video.getPixels());
            
            face.lastSeen = tracker.getLastSeen(face.label);
            ofLog()<<"face "<<face.label<<" lastSeen "<<tracker.getLastSeen(face.label);
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

        float temp_alpha = ofMap(face.lastSeen, 0, persistance, 255, 0, true);
        ofSetColor(255,255,255,temp_alpha); //face.alpha);
        face.draw();
        
        ofSetColor(255,255,255,200);
        ofDrawCircle(face.leftEye, 7);
        ofDrawCircle(face.rightEye, 5);
        
//        int temp_ = ofToString(tracker.getLastSeen(face.label));
        int temp_lastSeen = tracker.getLastSeen(face.label);
        int temp_age = tracker.getLastSeen(face.label);
        
        ofDrawBitmapString(ofToString(face.label)+" , "+ofToString(temp_lastSeen)+ " , "+ofToString(temp_age) ,face.roi.getLeft(), face.roi.getBottom()+10);
        
    }
    ofDisableAlphaBlending();
    
    ofDrawBitmapStringHighlight("faces found "+ofToString(facesAugmented.size()), 10, 10);
    ft.draw();
}