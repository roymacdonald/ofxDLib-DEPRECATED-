//
//  ObjectTracker.h
//  example-FaceTracker
//
//  Created by Stephan Schulz on 2016-03-26.
//
//
#pragma once
#include "ofxDLib.h"


namespace ofxDLib{
    class ObjectTracker{
    public:
        void setup(string predictorDatFilePath);
        
        void findObjects(const ofPixels& pixels, bool bUpscale = false);
        void setNewSelection(ofRectangle _rect = ofRectangle(0,0,38,86));
        
        void draw();
    protected:
        
        std::vector<dlib::rectangle> dets;

        
        std::vector<dlib::full_object_detection> shapes;
        
        //object tracker
        dlib::correlation_tracker tracker;
        bool selectionTrackerInit;
        int sel_x,sel_y, sel_w, sel_h;
        
    };
}