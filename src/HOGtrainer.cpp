//
//  ofxDLib.cpp
//  DLibTest
//
//  Created by Stephan Schulz on 2016-02-12.
//
//
#include "ofMain.h"

#include "HOGtrainer.h"

using namespace ofxDLib;

HOGtrainer::HOGtrainer()
{
}

HOGtrainer::~HOGtrainer(){
    //close();
}


void HOGtrainer::setup(string _trainDir){
    
    curTestImage = 0;
    //    uint64_t st =ofGetElapsedTimeMillis();
    //
    //    detector = get_frontal_face_detector();
    //    cout << "get_frontal_face_detector: " << ofGetElapsedTimeMillis() - st << " ms." << endl;
    //    //http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
    //    ofFile f(ofToDataPath("shape_predictor_68_face_landmarks.dat"));
    //    if (f.exists()) {
    //        deserialize(f.getAbsolutePath()) >> shape_predictor_object;
    //    }else{
    //        cout << "SHAPE PREDICTOR DAT FILE MISSING!!!" << endl;
    //    }
    //
    const std::string faces_directory = ofToDataPath(_trainDir);
    
    load_image_dataset(images_train, face_boxes_train, faces_directory+"/training.xml");//face_boxes_train_ignore
    load_image_dataset(images_test, face_boxes_test, faces_directory+"/testing.xml");
    
    upsample_image_dataset<pyramid_down<2> >(images_train, face_boxes_train);
    upsample_image_dataset<pyramid_down<2> >(images_test,  face_boxes_test);
    
    add_image_left_right_flips(images_train, face_boxes_train);
    cout << "num training images: " << images_train.size() << endl;
    cout << "num testing images:  " << images_test.size() << endl;
    
    ofxDLib::toOf(images_test[0],testPix);
    
    
    image_scanner_type scanner;
    // The sliding window detector will be 80 pixels wide and 80 pixels tall.
    scanner.set_detection_window_size(80, 80);
    structural_object_detection_trainer<image_scanner_type> trainer(scanner);
    // Set this to the number of processing cores on your machine.
    trainer.set_num_threads(4);
    // The trainer is a kind of support vector machine and therefore has the usual SVM
    // C parameter.  In general, a bigger C encourages it to fit the training data
    // better but might lead to overfitting.  You must find the best C value
    // empirically by checking how well the trained detector works on a test set of
    // images you haven't trained on.  Don't just leave the value set at 1.  Try a few
    // different C values and see what works best for your data.
    trainer.set_c(1);
    // We can tell the trainer to print it's progress to the console if we want.
    trainer.be_verbose();
    // The trainer will run until the "risk gap" is less than 0.01.  Smaller values
    // make the trainer solve the SVM optimization problem more accurately but will
    // take longer to train.  For most problems a value in the range of 0.1 to 0.01 is
    // plenty accurate.  Also, when in verbose mode the risk gap is printed on each
    // iteration so you can see how close it is to finishing the training.
    trainer.set_epsilon(0.01);
    
    // Now we run the trainer.  For this example, it should take on the order of 10
    // seconds to train.
    detector = trainer.train(images_train, face_boxes_train); //,face_boxes_train_ignore);
    
    // Now that we have a face detector we can test it.  The first statement tests it
    // on the training data.  It will print the precision, recall, and then average precision.
    cout << "training results: " << test_object_detection_function(detector, images_train, face_boxes_train) << endl;
    // However, to get an idea if it really worked without overfitting we need to run
    // it on images it wasn't trained on.  The next line does this.  Happily, we see
    // that the object detector works perfectly on the testing images.
    cout << "testing results:  " << test_object_detection_function(detector, images_test, face_boxes_test) << endl;
    
    
    // If you have read any papers that use HOG you have probably seen the nice looking
    // "sticks" visualization of a learned HOG detector.  This next line creates a
    // window with such a visualization of our detector.  It should look somewhat like
    // a face.
//    image_window hogwin(draw_fhog(detector), "Learned fHOG detector");
    
    ofxDLib::toOf(draw_fhog(detector),hogPix);
    
    // Now for the really fun part.  Let's display the testing images on the screen and
    // show the output of the face detector overlaid on each image.  You will see that
    // it finds all the faces without false alarming on any non-faces.
    //    image_window win;
    
    
    
    
    // Like everything in dlib, you can save your detector to disk using the
    // serialize() function.
    serialize("face_detector.svm") << detector;
    
    // Then you can recall it using the deserialize() function.
    object_detector<image_scanner_type> detector2;
    deserialize("face_detector.svm") >> detector2;
    
    
    std::vector<object_detector<image_scanner_type> > my_detectors;
    my_detectors.push_back(detector);
    std::vector<rectangle> dets = evaluate_detectors(my_detectors, images_train[0]);
    //
    //
    // Finally, you can add a nuclear norm regularizer to the SVM trainer.  Doing has
    // two benefits.  First, it can cause the learned HOG detector to be composed of
    // separable filters and therefore makes it execute faster when detecting objects.
    // It can also help with generalization since it tends to make the learned HOG
    // filters smoother.  To enable this option you call the following function before
    // you create the trainer object:
    //    scanner.set_nuclear_norm_regularization_strength(1.0);
    // The argument determines how important it is to have a small nuclear norm.  A
    // bigger regularization strength means it is more important.  The smaller the
    // nuclear norm the smoother and faster the learned HOG filters will be, but if the
    // regularization strength value is too large then the SVM will not fit the data
    // well.  This is analogous to giving a C value that is too small.
    //
    // You can see how many separable filters are inside your detector like so:
    cout << "num filters: "<< num_separable_filters(detector) << endl;
    // You can also control how many filters there are by explicitly thresholding the
    // singular values of the filters like this:
    detector = threshold_filter_singular_values(detector,0.1);
    // That removes filter components with singular values less than 0.1.  The bigger
    // this number the fewer separable filters you will have and the faster the
    // detector will run.  However, a large enough threshold will hurt detection
    // accuracy.
    
    
       nextTestImage(0);
        nextTrainImage(0);
}


void HOGtrainer::update(){
    
}


void HOGtrainer::drawTest(){
    ofSetColor(255);
    ofImage img;
    img.setFromPixels(testPix);
    
    ofPushMatrix();
    ofScale(0.3, 0.3);
    img.draw(0, 0);
    
    ofNoFill();
    ofSetColor(255);
    for(int i=0; i<testRects.size(); i++){
        ofDrawRectangle(testRects[i]);
    }
    ofPopMatrix();
   
}

void HOGtrainer::drawTrain(){
    ofSetColor(255);
    ofImage img;
    img.setFromPixels(trainPix);
    
    ofPushMatrix();
    ofScale(0.5, 0.5);
    img.draw(0, 0);
    
    ofNoFill();
    ofSetColor(255);
    for(int i=0; i<trainRects.size(); i++){
        ofDrawRectangle(trainRects[i]);
    }
    ofPopMatrix();

}

void HOGtrainer::draw(){
    
    
    drawTest();
    ofDrawBitmapStringHighlight("test data", 10,20);
    
    
    ofPushMatrix();
    ofTranslate(600, 0);
    drawTrain();
    ofDrawBitmapStringHighlight("training data", 10,20);
    ofPopMatrix();
    
    ofImage HOGimg;
    HOGimg.setFromPixels(hogPix);
    HOGimg.draw(0,600);
    
    
}

void HOGtrainer::nextTestImage(int i){
    
    i = curTestImage;
    
    ofLog()<<"nextTestImage "<<curTestImage;
    
    // Run the detector and get the face detections.
    std::vector<rectangle> dets = detector(images_test[i]);
    
    testRects.clear();
    for(int i=0; i<dets.size(); i++){
        testRects.push_back(ofxDLib::toOf(dets[i]));
    }
    
    ofxDLib::toOf(images_test[curTestImage],testPix);
    
    
    curTestImage++;
    if(curTestImage >= images_test.size()) curTestImage = 0;
    
}

void HOGtrainer::nextTrainImage(int i){
    
    i = curTrainImage;
    
    ofLog()<<"nextTestImage "<<curTestImage;
    
    // Run the detector and get the face detections.
    std::vector<rectangle> dets = detector(images_train[i]);
    
    trainRects.clear();
    for(int i=0; i<dets.size(); i++){
        trainRects.push_back(ofxDLib::toOf(dets[i]));
    }
    
    ofxDLib::toOf(images_train[curTrainImage],trainPix);
    
    
    curTrainImage++;
    if(curTrainImage >= images_train.size()) curTrainImage = 0;
    
}


/*
 do selection rects need to be square?
 insure selection rects are inside image rect
 use exclusing rects
 
 get nice HOG drawing thingy
 
 */


