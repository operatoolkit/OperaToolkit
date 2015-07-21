#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    looper.setup(&sc3);
}

//--------------------------------------------------------------
void ofApp::update(){
    looper.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='1') {
        //e->setRecording(true);
    }
    if (key=='2') {
        //e->setPlaying(true);
    }
}
