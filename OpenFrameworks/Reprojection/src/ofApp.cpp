#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFullscreen(true);
    
    img.loadImage("/Users/gene/Code/of_v0.8-4.4_osx_release/addons/ofxHomography/example/bin/data/harvey_resized.jpg");
    //img.resize(500, 500);
    
    fbo.allocate(img.getWidth(), img.getHeight());
    projection.addQuad(img.getWidth(), img.getHeight());
    projection.loadPreset("mapping.xml");
    shader.load("standard.vert", "brcosa.frag");
    
    gui.setup("reproject");
    gui.add(brightness.set("brightness", 1.0, 0.0, 5.0));
    gui.add(saturation.set("saturation", 1.0, -5.0, 5.0));
    gui.add(contrast.set("contrast", 1.0, -5.0, 5.0));
    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    fbo.begin();
    ofSetColor(255, 0, 0);
    ofRect(0, 0, fbo.getWidth(), fbo.getHeight());
    ofSetColor(255);
    shader.begin();
    shader.setUniform1f("brightness", brightness);
    shader.setUniform1f("contrast", contrast);
    shader.setUniform1f("saturation", saturation);
    img.draw(0, 0);
    shader.end();
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    projection.begin(0);
    fbo.draw(0, 0);
    projection.end(0);
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    gui.saveToFile("settings.xml");
    projection.savePreset("mapping.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='c') {
        projection.toggleDebug();
    }
    else if (key=='t') {
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
