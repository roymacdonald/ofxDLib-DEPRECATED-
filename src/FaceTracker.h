//
//  FaceTracker.h
//  example_FaceTracker
//
//  Created by Roy Macdonald on 15-01-16.
//
//

#pragma once
#include "ofxDLib.h"
#include "Tracker.h"

namespace ofxDLib {
    
    enum ShapeType {
        leftEye, rightEye, innerMouth, outerMouth, leftEyebrow, rightEyebrow, jaw, noseBridge, noseTip
    };
    
    enum DrawStyle {
        none, lines, circles
    };
    
    typedef struct {
        int label;
        int age;
        ofRectangle rect;
        ofVec2f velocity, leftEyeCenter, rightEyeCenter;
        ofPolyline leftEye, rightEye, innerMouth, outerMouth, leftEyebrow, rightEyebrow, jaw, noseBridge, noseTip;
        vector<ofVec3f> landmarks;
    } Face;
    
    class FaceTracker {
    protected:
        // face tracker
        dlib::frontal_face_detector detector;
        dlib::shape_predictor predictor;
        vector<Face> faces;
        map<unsigned int, vector<ofVec3f>> shapeHistory;
        map<unsigned int, float> smoothingRatePerFace;
        float smoothingRate;
        DrawStyle drawStyle;
        
        // assign labels
        RectTracker tracker;
    public:
        FaceTracker();
        void setup(string predictorDatFilePath);
        tuple<int,int> findFaces(const ofPixels& pixels, bool bUpscale = false);
        unsigned int size();
        RectTracker& getTracker();
        Face getFace(unsigned int i);
        vector<Face>& getFaces();
        ofRectangle getRectangle(unsigned int i);
        vector<ofVec3f> getLandmarks(unsigned int i);
        ofPolyline getShape(unsigned int i, ShapeType t);
        unsigned int getLabel(unsigned int i);
        int getIndexFromLabel(unsigned int label);
        ofVec2f getVelocity(unsigned int i);
        void setSmoothingRate(float smoothingRate);
        void setSmoothingRate(unsigned int label, float smoothingRate);
        float getSmoothingRate();
        float getSmoothingRate(unsigned int label);
        void setDrawStyle(DrawStyle style);
        void draw();

    };
    
}