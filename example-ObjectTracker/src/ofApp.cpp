#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ft.setup("");
    video.setDeviceID(0);
    video.setup(720, 480);
//     video.setup(960, 720);
    
    bSelecting = false;

    videoRect = ofRectangle(0,0,video.getWidth(),video.getHeight());
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    video.update();
    if(video.isFrameNew()){
        ft.findObjects(video.getPixels());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    video.draw(0, 0);
    ft.draw();
    
    if(bSelecting){
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofNoFill();
        ofDrawRectangle( ofRectangle(startPoint,endPoint));
        ofPopStyle();
    }

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
      endPoint = ofPoint(x,y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    startPoint = ofPoint(x,y);
    bSelecting = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    endPoint = ofPoint(x,y);
    if(videoRect.inside(startPoint) == true && videoRect.inside(endPoint) == true){
        ofLog()<<"ofRectangle(startPoint,endPoint) "<<ofRectangle(startPoint,endPoint);
        ft.setNewSelection(ofRectangle(startPoint,endPoint));

    }
    bSelecting = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
