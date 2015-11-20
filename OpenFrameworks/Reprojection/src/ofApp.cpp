#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFullscreen(true);
    
    surfaces.push_back(new Surface());
    surfaces.push_back(new Surface());
    surfaces.push_back(new Surface());

    //surfaces[0].setImage(0, "/Users/gene/Downloads/left_sq.JPG");
    surfaces[0]->setImage(0, "left_desat.JPG");
    surfaces[0]->setShader("standard.vert", "brcosa.frag");
    
//    surfaces[1].setImage(1, "/Users/gene/Downloads/mid_sq.JPG");
    surfaces[1]->setImage(1, "mid_desat.JPG");
    surfaces[1]->setShader("standard.vert", "brcosa.frag");
    
//   surfaces[2].setImage(2, "/Users/gene/Downloads/right_sq.JPG");
    surfaces[2]->setImage(2, "right_desat.JPG");
    surfaces[2]->setShader("standard.vert", "brcosa.frag");
    
    gui.setup("reproject");
    for (int s=0; s<surfaces.size(); s++) {
        surfaces[s]->addToGui(gui);
        projection.addQuad(surfaces[s]->img.getWidth(), surfaces[s]->img.getHeight());
    }
    gui.loadFromFile("settings.xml");
    
    
    projection.loadPreset("mapping.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    for (auto s : surfaces) {
        s->update();
    }

    /*
    surfaces[2].contrast = ofMap(sin(ofGetFrameNum()*0.11), -1, 1, 0.88, 1.12);
    surfaces[1].contrast = ofMap(sin(ofGetFrameNum()*0.1), -1, 1, 0.77, 1.13);
    surfaces[0].contrast = ofMap(sin(ofGetFrameNum()*0.12), -1, 1, 0.91, 1.09);
     */
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    for (int s=0; s<surfaces.size(); s++) {
        projection.begin(s);
        surfaces[s]->fbo.draw(0, 0);
        projection.end(s);
    }
    
    if (guiDraw) {
        gui.draw();
    }
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
    else if (key=='g') {
        guiDraw = !guiDraw;
        if (guiDraw) {
            ofShowCursor();
        }
        else {
            ofHideCursor();
        }
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
