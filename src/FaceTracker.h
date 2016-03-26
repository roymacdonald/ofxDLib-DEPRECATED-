//
//  FaceTracker.h
//  example_FaceTracker
//
//  Created by Roy Macdonald on 15-01-16.
//
//

#pragma once
#include "ofxDLib.h"

namespace ofxDLib{
    class FaceTracker{
    public:
        void setup(string predictorDatFilePath);
        void findFaces(const ofPixels& pixels, bool bUpscale = false);
        void draw();
    protected:
        dlib::frontal_face_detector detector;
        dlib::shape_predictor sp;
        std::vector<dlib::rectangle> dets;
        std::vector<dlib::full_object_detection> shapes;
    };
}