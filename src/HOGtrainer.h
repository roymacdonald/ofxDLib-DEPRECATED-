//
//  ofxDLib.h
//  DLibTest
//
//  Created by Stephan Schulz on 2016-02-12.
//
//http://dlib.net/fhog_object_detector_ex.cpp.html
//http://blog.dlib.net/2014/02/dlib-186-released-make-your-own-object.html


//!!! in min_cut.h had to change nil to a different variabl name like c_nil !!!

#ifndef __DLibTest__ofxDLib__
#define __DLibTest__ofxDLib__


#include "ofMain.h"

#ifdef SHIFT
#undef SHIFT
#endif


#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <vector>



using namespace dlib;



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

#endif /* defined(__DLibTest__ofxDLib__) */
