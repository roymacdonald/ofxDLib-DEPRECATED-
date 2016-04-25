

#pragma once
#include "ofMain.h"
#ifdef SHIFT
#undef SHIFT
#endif

//#include "dlib/geometry/rectangle.h"
//#include "dlib/pixel.h"
//#include "dlib/array2d.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing.h"
#include "dlib/image_io.h"

namespace ofxDLib{
//------------------------------------------------------------------------
static ofRectangle toOf(const dlib::rectangle& r){
    return ofRectangle(r.left(), r.top(), r.width(), r.height());
}
//------------------------------------------------------------------------
static ofPoint toOf(const dlib::point& p){
    return ofPoint(p.x(), p.y(), p.z() );
}
//------------------------------------------------------------------------
static vector<ofVec3f> toOf(const vector<dlib::point>& p){
    vector<ofVec3f> out(p.size());
    for(int i = 0; i < p.size(); i++) {
        out[i] = toOf(p[i]);
    }
    return out;
}
//------------------------------------------------------------------------
static vector<ofRectangle> toOf(const vector<dlib::rectangle>& r){
    vector<ofRectangle> out(r.size());
    for(int i = 0; i < r.size(); i++) {
        out[i] = toOf(r[i]);
    }
    return out;
}
//------------------------------------------------------------------------
static void toDLib(const ofPixels& inPix, dlib::array2d<dlib::rgb_pixel>& outPix){
    
    int width = inPix.getWidth();
    int height = inPix.getHeight();
    outPix.set_size( height, width );
    int chans = inPix.getNumChannels();
    const unsigned char* data = inPix.getData();
    for ( unsigned n = 0; n < height;n++ )
    {
        const unsigned char* v =  &data[n * width *  chans];
        for ( unsigned m = 0; m < width;m++ )
        {
            if ( chans==1 )
            {
                unsigned char p = v[m];
                dlib::assign_pixel( outPix[n][m], p );
            }
            else{
                dlib::rgb_pixel p;
                p.red = v[m*3];
                p.green = v[m*3+1];
                p.blue = v[m*3+2];
                dlib::assign_pixel( outPix[n][m], p );
            }
        }
    }
}
    
    
static bool toOf(const dlib::matrix<unsigned char>& inMat, ofPixels& outPix){
        
    int w = inMat.nc();
    int h = inMat.nr();
    
    outPix.allocate(w, h, 1);
    
    for(int y = 0; y<h; y++){
        for(int x=0; x<w;x++){
            outPix.setColor(x, y, ofColor(inMat(y,x)));
        }
    }
    
    return true;
}
    
static bool toOf(const dlib::array2d<unsigned char>& inPix, ofPixels& outPix){
    
    int h = inPix.nr(); //number of rows
    int w = inPix.nc(); //nuber of cols
    ofLog()<<"inPix.nr() "<<h<<",  inPix.nc() "<<w;
    outPix.allocate(w, h, 1);
    
    for(int y = 0; y<h; y++){
        for(int x=0; x<w;x++){
            outPix.setColor(x, y, ofColor(inPix[y][x]));
        }
    }
    
    return true;
}
    
    //*/
////------------------------------------------------------------------------
//void toOf(const array2d<rgb_pixel>& inPix, ofPixels& outPix){
//    
//    int width = inPix.nc();
//    int height = inPix.nr();
//    outPix.allocate(height, width,  );
//    int chans = inPix.getNumChannels();
//    const unsigned char* data = inPix.getData();
//    for ( unsigned n = 0; n < height;n++ )
//    {
//        const unsigned char* v =  &data[n * width *  chans];
//        for ( unsigned m = 0; m < width;m++ )
//        {
//            if ( chans==1 )
//            {
//                unsigned char p = v[m];
//                assign_pixel( outPix[n][m], p );
//            }
//            else{
//                rgb_pixel p;
//                p.red = v[m*3];
//                p.green = v[m*3+1];
//                p.blue = v[m*3+2];
//                assign_pixel( outPix[n][m], p );
//            }
//        }
//    }
//}
}