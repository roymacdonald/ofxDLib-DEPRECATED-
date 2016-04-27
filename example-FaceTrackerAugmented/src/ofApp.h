#pragma once

#include "ofMain.h"
#include "FaceTracker.h"

namespace ofxDLib {
    float trackingDistance(const Face& a, const Face& b) {
        ofVec3f aCenter = a.rect.getCenter();
        ofVec3f bCenter = b.rect.getCenter();
        return aCenter.distance(bCenter);
    };
}

class FaceAugmented : public ofxDLib::Follower<ofxDLib::Face> {
protected:
    ofImage image;
    ofColor color;
    ofRectangle roi;
    ofVec2f cur, smooth;
    unsigned int label;
    ofPolyline all;
    ofxDLib::Face face;
    float startedDying = 0;
public:
    float dyingTime = 1;
    ofVec2f leftEye, rightEye;
    ofVec2f velocity;
    float alpha;
    
    FaceAugmented(){
    };
    void setup(const ofxDLib::Face & track);
    void update(const ofxDLib::Face & track);
    void setImage(const ofPixels & pixels);
    void kill();
    void draw();
};


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    ofxDLib::FaceTracker ft;
    ofVideoGrabber video;
    
    ofxDLib::TrackerFollower<ofxDLib::Face, FaceAugmented> tracker;
    
    
};
