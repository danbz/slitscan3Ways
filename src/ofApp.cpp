#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //    camWidth =  640;  // try to grab at this size from the camera.
    //    camHeight = 480;
    camWidth =  1280;  // try to grab at this size from the camera.
    camHeight = 720;
    ySteps = 0;
    speed = 1;
    scanStyle = 1;
    scanName = "horizontal";
    b_radial = false;
    b_drawCam = true;
    // ask the video grabber for a list of attached camera devices.
    // put it into a vector of devices
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    
    // loop through and print out the devices to the console log
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    
    // set the ID of the camera we will use
    vidGrabber.setDeviceID(0);
    // set how fast we will grab frames from the camera
    vidGrabber.setDesiredFrameRate(30);
    // set the width and height of the camera
    vidGrabber.initGrabber(camWidth, camHeight);
    // set up our pixel object to be the same size as our camera object
    videoInverted.allocate(camWidth,camHeight, OF_PIXELS_RGB);
    videoTexture.allocate(videoInverted);
    ofSetVerticalSync(true);
    
    ofSetBackgroundColor(0, 0, 0); // set the background colour to dark black
}

//--------------------------------------------------------------
void ofApp::update(){
    
    vidGrabber.update();
    ofPixels & pixels = vidGrabber.getPixels();
    
    switch (scanStyle) {
        case 1: // scan horizontal
            
            for (int y=0; y<camHeight; y++ ) { // loop through all the pixels on a line
                ofColor color = pixels.getColor( xSteps, y); // get the pixels on line ySteps
                videoInverted.setColor(xSteps, y, color);
            }
            
            videoTexture.loadData(videoInverted);
            
            if ( xSteps >= camWidth ) {
                xSteps = 0; // if we are on the bottom line of the image then start at the top again
            }
            xSteps += speed; // step on to the next line. increase this number to make things faster
            break;
            
        case 2: // scan vertical
            
            for (int x=0; x<camWidth; x++ ) { // loop through all the pixels on a line
                ofColor color = pixels.getColor(x, ySteps); // get the pixels on line ySteps
                videoInverted.setColor(x, ySteps, color);
            }
            
            videoTexture.loadData(videoInverted);
            
            if ( ySteps >= camHeight ) {
                ySteps = 0; // if we are on the bottom line of the image then start at the top again
            }
            ySteps += speed; // step on to the next line. increase this number to make things faster
            break;
            
        case 3: // scan horizontal from centre
            for (int y=0; y<camHeight; y++ ) { // loop through all the pixels on a line to draw new line at 0 in target image
                ofColor color = pixels.getColor( camWidth/2, y); // get the pixels on line ySteps
                videoInverted.setColor(1, y, color);
            }
            
            for (int x = camWidth; x>=0; x-= 1){
                for (int y=0; y<camHeight; y++ ) { // loop through all the pixels on a line
                    
                    videoInverted.setColor(x, y, videoInverted.getColor( x-1, y )); // copy each pixel in the target to 1 pixel the right
                }
            }
            videoTexture.loadData(videoInverted);
            break;
            
        case 4: // scan vertical from centre
            for (int x=0; x<camWidth; x++ ) { // loop through all the pixels on a line to draw new column at 0 in target image
                ofColor color = pixels.getColor(x, camHeight/2); // get the pixels on line ySteps
                videoInverted.setColor(x, 1, color);
            }
            
            for (int y = camHeight; y>=0; y-= 1){
                for (int x=0; x<camWidth; x++ ) { // loop through all the pixels on a column
                    
                    videoInverted.setColor(x, y, videoInverted.getColor( x, y-1 )); // copy each pixel in the target to 1 pixel below
                }
            }
            videoTexture.loadData(videoInverted);
            break;
            
        default:
            break;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if (b_radial){ // radial ribbon
        for (int i =0; i<videoTexture.getWidth(); i+=speed){
            ofPushMatrix();
            ofTranslate(ofGetWidth()/2, ofGetHeight()/2); // centre in right portion of screen
            ofRotateZDeg( ofMap(i, 0, videoTexture.getWidth()/speed, 0, 360));
            videoTexture.drawSubsection(0, 0, speed +2, videoTexture.getHeight(), i, 0);
            ofPopMatrix();
        }
    } else {
        videoTexture.draw( 0, 0, camWidth, camHeight); // draw the video texture we have constructed
    }
    if (b_drawCam){
        vidGrabber.draw(ofGetWidth()-camWidth/4 -10, ofGetHeight()-camHeight/4 -10, camWidth/4, camHeight/4); // draw our plain image
    }
    
    ofDrawBitmapString(" scanning " + scanName + " , press 1,2 or 3: for scantype, r: radial, c: camview" , 10, ofGetHeight() -10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
            
        case '1':
            scanStyle = 1;
            scanName = "horizontal";
            break;
        case '2':
            scanStyle = 2;
            scanName = "vertical";
            break;
        case '3':
            scanStyle = 3;
            scanName = "horizontal ribbon";
            break;
            
        case '4':
            scanStyle = 4;
            scanName = "vertical ribbon";
            break;
            
        case 'r':
            b_radial =!b_radial;
            break;
            
        case 'c':
            b_drawCam =!b_drawCam;
            break;
            
        case OF_KEY_UP:
            speed ++;
            break;
            
        case OF_KEY_DOWN:
            if (speed>1){
                speed --;
            }
            break;
            
        default:
            break;
    }
    
    
}
