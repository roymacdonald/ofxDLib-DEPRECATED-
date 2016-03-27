//
//  FaceTracker.h
//  example_FaceTracker
//
//  Created by Roy Macdonald on 15-01-16.
//
//

#pragma once
#include "ofxDLib.h"

typedef struct{
    int label;
    int age;
    ofRectangle rect;
    bool used;
    
    ofVec2f leftEyeCenter;
    ofVec2f rightEyeCenter;
    
    ofPolyline leftEye, rightEye,innerMouth, outerMouth, leftEyebrow, rightEyebrow, jaw, noseBridge, noseTip;
} face;

namespace ofxDLib{
    class FaceTracker{
    public:
        void setup(string predictorDatFilePath);
        void findFaces(const ofPixels& pixels, bool bUpscale = false);

        void draw();
    protected:

        std::vector<dlib::rectangle> dets;
    
        
        //face tracker
        dlib::frontal_face_detector detector;
        dlib::shape_predictor sp;
        std::vector<face> allFaces;
        std::vector<dlib::full_object_detection> shapes;

    };
}