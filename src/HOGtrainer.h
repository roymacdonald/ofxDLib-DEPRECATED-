//
//  ofxDLib.h
//  DLibTest
//
//  Created by Stephan Schulz on 2016-02-12.
//
//http://dlib.net/fhog_object_detector_ex.cpp.html
//http://blog.dlib.net/2014/02/dlib-186-released-make-your-own-object.html




#pragma once

#include "ofxDLib.h"
/////---------------
// This is a really ugly hack to avoid problems with dlib, where, by reasons I don't know uses nil to name objects, yet in objective-C nil is a typedef for nullptr. With this it compiles and runs, although I have no idea if this affects anything else.
#if defined(TARGET_OSX) || defined(TARGET_OF_IPHONE) || defined(TARGET_OF_IOS)
#ifdef nil
#undef nil
#endif
#endif
/////---------------
#include <dlib/svm_threaded.h>
//#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <vector>



using namespace dlib;

namespace ofxDLib{
    
    class HOGtrainer {
    public:
        
        HOGtrainer();
        virtual ~HOGtrainer();
        
        void setup(string _trainDir);
        void update();
        void draw();
        
        void drawTest();
        void drawTrain();
        
        void nextTestImage(int i);
        void nextTrainImage(int i);
        
        int curTestImage;
        int curTrainImage;
        
        dlib::array<array2d<unsigned char> > images_train, images_test;
        std::vector<std::vector<rectangle> > face_boxes_train, face_boxes_test,face_boxes_train_ignore;
        
        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;
        object_detector<image_scanner_type> detector;
        //     image_window win;
        
        ofPixels testPix;
        ofPixels trainPix;
        ofPixels hogPix;
        
        std::vector<ofRectangle> testRects;
        std::vector<ofRectangle> trainRects;
        
    };
}
