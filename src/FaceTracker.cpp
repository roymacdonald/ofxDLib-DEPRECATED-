
//
//  FaceTracker.cpp
//  example_FaceTracker
//
//  Created by Roy Macdonald on 15-01-16.
//
//

#include "FaceTracker.h"
using namespace ofxDLib;

FaceTracker::FaceTracker() {
    smoothingRate = 0.5;
    drawStyle = lines;
    tracker.setSmoothingRate(smoothingRate);
}

//--------------------------------------------------------------
void FaceTracker::setup(string predictorDatFilePath) {
    detector = dlib::get_frontal_face_detector();
    if(predictorDatFilePath.empty()){
        predictorDatFilePath = ofToDataPath("shape_predictor_68_face_landmarks.dat");
    }
    ofFile f(predictorDatFilePath);
    if (f.exists()) {
        dlib::deserialize(f.getAbsolutePath()) >> predictor;
    } else {
        ofLogError("ofxDLib::FaceTracker","SHAPE PREDICTOR DAT FILE MISSING!!!");
    }
}
//--------------------------------------------------------------
tuple<int,int> FaceTracker::findFaces(const ofPixels& pixels, bool bUpscale) {

    
    dlib::array2d<dlib::rgb_pixel> img;
    toDLib(pixels, img);
    if (bUpscale) pyramid_up(img);
    
    std::vector<dlib::rectangle> dets = detector(img);
    tracker.track(toOf(dets));
    
    faces.clear();
    
    for (int i=0; i<dets.size(); i++) {
        vector<ofVec3f> currentLandmarks;
        dlib::full_object_detection shapes = predictor(img, dets[i]);
        unsigned int label = tracker.getLabelFromIndex(i);
        bool existsShapeHistory = shapeHistory.count(label) > 0;
        bool existsSmoothingPerFace = smoothingRatePerFace.count(label) > 0;
        if (!existsSmoothingPerFace) smoothingRatePerFace[label] = smoothingRate;
        float currentSmoothingRate = smoothingRatePerFace[label];
        
        //TODO: face.rect comes from tracker and not dlib this means currentSmoothingRate is not what influences this rects smoothing. but rather setSmoothing(float xx). shouldn't be split
        //because if one uses veloctiy to adjust smoothing of face features this will not effect the rect somoothing, and rect smoothing is applied to all found track faces
        
        Face face;
        face.label = label;
        face.rect = tracker.getSmoothed(label);
        face.age = tracker.getAge(label);
//        ofLog()<<label<<" rect "<<face.rect;
//        ofLog()<<label<<" age "<<face.age;
//         ofLog()<<label<<" currentSmoothingRate "<<currentSmoothingRate;
//        ofLog()<<label<<" getLastSeen "<<tracker.getLastSeen(label);
        
        face.velocity = tracker.getVelocity(i);
        for (int j=0; j<shapes.num_parts(); j++) {
            ofVec3f point;
            ofVec3f current = toOf(shapes.part(j));
            ofVec3f previous = existsShapeHistory ? shapeHistory[label][j] : current;
            point.x = ofLerp(previous.x, current.x, currentSmoothingRate);
            point.y = ofLerp(previous.y, current.y, currentSmoothingRate);
            
            currentLandmarks.push_back(point);
            face.landmarks.push_back(point);
        }
        if (face.landmarks.size() == 68) {
            for (int j=0; j<=16; j++) { // jaw
                face.jaw.addVertex(face.landmarks[j]);
            }
            
            for (int j=17; j<=21; j++) { // leftEyebrow
                face.leftEyebrow.addVertex(face.landmarks[j]);
            }
            
            for (int j=22; j<=26; j++) { // rightEyebrow
                face.rightEyebrow.addVertex(face.landmarks[j]);
            }
            
            for (int j=27; j<=30; j++) { // noseBridge
                face.noseBridge.addVertex(face.landmarks[j]);
            }
            
            for (int j=30; j<=35; j++) { // noseTip
                face.noseTip.addVertex(face.landmarks[j]);
            }
            face.noseTip.addVertex(face.landmarks[30]);
            face.noseTip.close();
            
            for (int j=36; j<=41; j++) { // leftEye
                face.leftEye.addVertex(face.landmarks[j]);
            }
            face.leftEye.addVertex(face.landmarks[36]);
            face.leftEye.close();
            face.leftEyeCenter = face.leftEye.getCentroid2D();
            
            for (int j=42; j<=47; j++) { // rightEye
                face.rightEye.addVertex(face.landmarks[j]);
            }
            face.rightEye.addVertex(face.landmarks[42]);
            face.rightEye.close();
            face.rightEyeCenter = face.rightEye.getCentroid2D();
            
            for (int j=48; j<=59; j++) { // outerMouth
                face.outerMouth.addVertex(face.landmarks[j]);
            }
            face.outerMouth.addVertex(face.landmarks[48]);
            face.outerMouth.close();
            
            for (int j=60; j<=67; j++) { // innerMouth
                face.innerMouth.addVertex(face.landmarks[j]);
            }
            face.innerMouth.addVertex(face.landmarks[60]);
            face.innerMouth.close();
        }
        shapeHistory[label] = currentLandmarks;
        faces.push_back(face);
        
        std::map<unsigned int, vector<ofVec3f>>::iterator shapeHistoryItr = shapeHistory.begin();
        while(shapeHistoryItr != shapeHistory.end()) {
            unsigned int label = shapeHistoryItr->first;
            if(!tracker.existsCurrent(label)) {
                shapeHistory.erase(shapeHistoryItr++);
                smoothingRatePerFace.erase(label); // dirty but should do it for now
            } else {
                ++shapeHistoryItr;
            }
        }
    }
    
     return make_tuple(img.nc(),img.nr());
}
//--------------------------------------------------------------
unsigned int FaceTracker::size() {
    return faces.size();
}

//--------------------------------------------------------------
RectTracker & FaceTracker::getTracker() {
    return tracker;
}

//--------------------------------------------------------------
Face FaceTracker::getFace(unsigned int i) {
    return faces[i];
}

//--------------------------------------------------------------
vector<Face> & FaceTracker::getFaces() {
    return faces;
}

//--------------------------------------------------------------
ofRectangle FaceTracker::getRectangle(unsigned int i) {
    return faces[i].rect;
}

//--------------------------------------------------------------
vector<ofVec3f> FaceTracker::getLandmarks(unsigned int i) {
    return faces[i].landmarks;
}

ofPolyline FaceTracker::getShape(unsigned int i, ShapeType t) {
    Face face = faces[i];
    ofPolyline out;
    
    switch (t) {
        case leftEye:
            out = face.leftEye;
            break;
        case rightEye:
            out = face.rightEye;
            break;
        case innerMouth:
            out = face.innerMouth;
            break;
        case outerMouth:
            out = face.outerMouth;
            break;
        case leftEyebrow:
            out = face.leftEyebrow;
            break;
        case rightEyebrow:
            out = face.rightEyebrow;
            break;
        case jaw:
            out = face.jaw;
            break;
        case noseBridge:
            out = face.noseBridge;
            break;
        case noseTip:
            out = face.noseTip;
            break;
        default:
            break;
    }
    
    return out;
}

//--------------------------------------------------------------
unsigned int FaceTracker::getLabel(unsigned int i) {
    return faces[i].label;
}

//--------------------------------------------------------------
int FaceTracker::getIndexFromLabel(unsigned int label) {
    int index = -1;
    for (int i=0; i<faces.size(); i++) {
        if (faces[i].label == label) {
            index = i;
            break;
        }
    }
    return index;
}

//--------------------------------------------------------------
ofVec2f FaceTracker::getVelocity(unsigned int i) {
    return faces[i].velocity;
}

//--------------------------------------------------------------
void FaceTracker::setSmoothingRate(float smoothingRate) {
    this->smoothingRate = smoothingRate;
    tracker.setSmoothingRate(smoothingRate);
}

//--------------------------------------------------------------
void FaceTracker::setSmoothingRate(unsigned int label, float smoothingRate) {
    if (smoothingRatePerFace.count(label) > 0) {
        smoothingRatePerFace[label] = smoothingRate;
    }
}

//--------------------------------------------------------------
float FaceTracker::getSmoothingRate() {
    return smoothingRate;
}

//--------------------------------------------------------------
float FaceTracker::getSmoothingRate(unsigned int label) {
    if (smoothingRatePerFace.count(label) > 0) {
        return smoothingRatePerFace[label];
    } else {
        return 1.0;
    }
}

//--------------------------------------------------------------
void FaceTracker::setDrawStyle(DrawStyle style) {
    this->drawStyle = style;
}

//--------------------------------------------------------------
void FaceTracker::draw() {
    ofPushStyle();
    
    ofSetColor(ofColor::red);
    ofNoFill();
    
    for (auto & face : faces) {
        ofDrawBitmapString(ofToString(face.label), face.rect.getTopLeft());
        ofDrawRectangle(face.rect);
        
        switch (drawStyle) {
            case lines:
                face.leftEye.draw();
                face.rightEye.draw();
                face.innerMouth.draw();
                face.outerMouth.draw();
                face.leftEyebrow.draw();
                face.rightEyebrow.draw();
                face.jaw.draw();
                face.noseBridge.draw();
                face.noseTip.draw();
                break;
            case circles:
                for (auto & landmark : face.landmarks) {
                    ofDrawCircle(landmark, 3);
                }
                break;
            case none:
                break;
        }
        
    }
    
    ofPopStyle();
}