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
        void setup(string predictorDatFilePath, int trackType);
        void findFaces(const ofPixels& pixels, bool bUpscale = false);
        
        void findObjects(const ofPixels& pixels, bool bUpscale = false);
        void setNewSelection(ofRectangle _rect = ofRectangle(0,0,38,86));
        
        void draw();
    protected:
        dlib::frontal_face_detector detector;
        dlib::shape_predictor sp;
        std::vector<dlib::rectangle> dets;
        std::vector<dlib::full_object_detection> shapes;
        
        dlib::correlation_tracker tracker;
        bool selectionTrackerInit;
        int sel_x,sel_y, sel_w, sel_h;
        
    };
}