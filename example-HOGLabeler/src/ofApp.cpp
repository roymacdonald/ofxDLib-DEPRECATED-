#include "ofApp.h"

/*
 This app lets you select all section of an image that you want the system to train on.
 Everytime you hit the key n the image name and the list of rectangles gets safed.
 Once done, go to boxFile.xml. Here you find all the stored data.
 Inside the image folder you will find testing.xml and training.xml.
 Copy some data from boxFile.xml in to training.xml and the rest in to training.xml.
 Copy the complete image to the HOGTrainer example data folder.
 
 Read through the code commands of the original example to understand what all this data does:
 http://dlib.net/fhog_object_detector_ex.cpp.html
 http://blog.dlib.net/2014/02/dlib-186-released-make-your-own-object.html
 
 */
//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    
    currentIndex = 0;
    
    ofDirectory imagesDirectory(ofToDataPath("images"));
    imagesDirectory.allowExt("bmp");
    imagesDirectory.listDir();

    ofLog()<<"found "<<imagesDirectory.size()<<" in images folder";
    
    for(int i = 0; i < imagesDirectory.size(); i++){
        imageFilePaths.push_back(imagesDirectory.getPath(i));
        imageFileNames.push_back(imagesDirectory.getName(i));
        ofLog()<<"imageFilePaths "<<imageFilePaths.back()<<" with fileName = "<<imageFileNames.back();
    }
    
    currentImg.load(imageFilePaths[0]);
    ofLog()<<"currentImg width "<<currentImg.getWidth()<<" "<<currentImg.getHeight();

}

//--------------------------------------------------------------
void ofApp::update(){
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(255);
    currentImg.draw(0, 0);
    
    ofNoFill();
    ofSetColor(0, 0, 255);
    for(int i=0; i<allRects.size();i++){
        ofDrawRectangle(allRects[i]);
    }
    
    
    if(isPressed){
        ofSetColor(255);
        int w = mouseX-startPoint.x;
        int h = w; //mouseY-startPoint.y;
        currentRect = ofRectangle(startPoint.x,startPoint.y,w,h);
        
        ofDrawRectangle(currentRect);
    }
    
    ofSetColor(255);
    int str_y = 10;
    ofDrawBitmapStringHighlight("drag rect over object of interest", 10, str_y+=20);
    ofDrawBitmapStringHighlight("press x to remove last selection", 10, str_y+=20);
    ofDrawBitmapStringHighlight("use arrow keys to adjust position", 10, str_y+=20);
    ofDrawBitmapStringHighlight("press n for next image and save current data", 10, str_y+=20);
    
    ofDrawBitmapString("source: "+imageFilePaths[currentIndex], 10, ofGetHeight() - 30);
    
}

void ofApp::saveRects(){
    
    string _filename = "boxFile";
    ofFile outFile;
    string text;
    
    //add standard header data
    text += "<dataset>\n";
    text += "<name>Training People from overhead</name>\n";
    text += "<comment>These are images from me.</comment>\n";
    
    text += "<images>\n";
    
   
    //check if we already have such a file and read its data
    bool bSuccess = outFile.open(ofToDataPath(_filename+".xml"), ofFile::ReadWrite, false);
    if(bSuccess == true){
         ofBuffer old_textBuffer = outFile.readToBuffer();
        
        if(old_textBuffer.size()) {
            bool startAdding = false;
            
            for (ofBuffer::Line it = old_textBuffer.getLines().begin(), end = old_textBuffer.getLines().end(); it != end; ++it) {
                
                string line = *it;
                
                // copy the line to draw later
                // make sure its not a empty line
    
                
            
                if(line == "</images>"){
                    startAdding = false;
                }
                
                if(line.empty() == false && startAdding == true) {
                    text += line;
                    text += "\n";
                    // print out the line
                    cout << line << endl;
                }else{
                     cout <<"---bad line--"<< line << endl;
                }
                
                if(line == "<images>"){
                    startAdding = true;
                }
                
              
                
            }
            
        }
    }
    
   
   //append new data
    string bmpFileName;
    bmpFileName = imageFileNames[currentIndex];
    text += "<image file='"+bmpFileName+"'>\n";
    
    for(int i=0; i<allRects.size();i++){
        text += "<box top='" + ofToString(allRects[i].y) + "' left='" + ofToString(allRects[i].x) + "' width='" + ofToString(allRects[i].getWidth())+ "' height='" + ofToString(allRects[i].getHeight())+"'/>\n";
    }
    text += "</image>\n";
    
    
    text += "</images>\n";
    text += "</dataset>\n";
    
 ofBuffer new_textBuffer;
    new_textBuffer.set(text);
    // fill the buffer with something important
    bool fileWritten = ofBufferToFile(_filename+".xml", new_textBuffer);
    
    allRects.clear();

}
//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    
    if(key == 'n'){
     
        
        saveRects();
        
        currentIndex++;
        currentIndex = ofClamp(currentIndex, 0, imageFilePaths.size()-1);
        currentImg.load(imageFilePaths[currentIndex]);
        
    }
    if(key == 'x'){
        allRects.pop_back();
    }
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
    
    //no data gets saved unless you hit the s key
//    if(key == 's'){
//        saveRects();
//    }
    
    if( key == OF_KEY_LEFT){
        allRects.back().x -= 1;
    }
    if( key == OF_KEY_RIGHT){
        allRects.back().x += 1;
    }
    if( key == OF_KEY_UP){
        allRects.back().y -= 1;
    }
    if( key == OF_KEY_DOWN){
        allRects.back().y += 1;
    }
    /*
  
     
     
     <?xml version='1.0' encoding='ISO-8859-1'?>
     <?xml-stylesheet type='text/xsl' href='image_metadata_stylesheet.xsl'?>
     <dataset>
     <name>Training faces</name>
     <comment>These are images from the PASCAL VOC 2011 dataset.</comment>
     <images>
     <image file='2007_007763.bmp'>
     <box top='90' left='194' width='37' height='37'/>
     <box top='114' left='158' width='37' height='37'/>
     <box top='89' left='381' width='45' height='44'/>
     <box top='198' left='94' width='37' height='37'/>
     <box top='214' left='178' width='37' height='37'/>
     <box top='86' left='294' width='37' height='37'/>
     <box top='233' left='309' width='45' height='44'/>
     </image>
     <image file='2008_002079.bmp'>
     <box top='166' left='406' width='37' height='37'/>
     <box top='134' left='122' width='37' height='37'/>
     <box top='138' left='346' width='37' height='37'/>
     <box top='175' left='439' width='44' height='44'/>
     <box top='134' left='62' width='37' height='37'/>
     <box top='194' left='41' width='44' height='44'/>
     </image>
     <image file='2008_001009.bmp'>
     <box top='79' left='145' width='76' height='76'/>
     <box top='214' left='125' width='90' height='91'/>
     </image>
     <image file='2008_001322.bmp'>
     <box top='162' left='104' width='76' height='76'/>
     <box top='218' left='232' width='63' height='63'/>
     <box top='155' left='344' width='90' height='90'/>
     </image>
     </images>
     </dataset>
     */
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    isPressed = true;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    isPressed = true;
    startPoint = ofVec2f(x,y);
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    isPressed = false;
    endPoint = ofVec2f(x,y);
    int w = endPoint.x-startPoint.x;
    int h = w; //endPoint.y-startPoint.y;
    allRects.push_back(ofRectangle(startPoint.x,startPoint.y,w,h));
}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

