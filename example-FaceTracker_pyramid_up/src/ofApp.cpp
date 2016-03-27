#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ft.setup("");
    video.setDeviceID(0);
//    video.setup(720, 480);
    video.setup(320, 240);
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    if(video.isFrameNew()){
        //http://www.cppsamples.com/common-tasks/return-multiple-values.html
        tie(newImgWidth, newImgHeight) = ft.findFaces(video.getPixels(),true);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    video.draw(0, 0,newImgWidth,newImgHeight);
    ft.draw();
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
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
