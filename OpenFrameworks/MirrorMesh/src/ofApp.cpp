#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    process_occlusion = false;
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, depthFragmentShader);
    depthShader.linkProgram();
    
    irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
    irShader.linkProgram();

    
    kinect0.open(true, true, 0);
    // Note :
    // Default OpenCL device might not be optimal.
    // e.g. Intel HD Graphics will be chosen instead of GeForce.
    // To avoid it, specify OpenCL device index manually like following.
    // kinect1.open(true, true, 0, 2); // GeForce on MacBookPro Retina
    
    kinect0.start();
    
    mesh.setUsage(GL_DYNAMIC_DRAW);
    mesh.setMode(OF_PRIMITIVE_POINTS);
    
    ecam.setAutoDistance(false);
    ecam.setDistance(200);
    
    
    gr.setup(kinect0.getProtonect(), 2);
    
    shader.load("k2");

}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (ofGetFrameNum() % 60 == 0) {
        shader.load("k2");
    }
    
    kinect0.update();
    if (kinect0.isFrameNew()) {
        colorTex0.loadData(kinect0.getColorPixelsRef());
        depthTex0.loadData(kinect0.getDepthPixelsRef());
        irTex0.loadData(kinect0.getIrPixelsRef());
        
        depthTex0.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        gr.update(depthTex0, colorTex0, process_occlusion);
        
        gr.getRegisteredTexture(process_occlusion).readToPixels(pixels);
        
        
        mesh.clear();
        {
            int step = 2;
            int h = kinect0.getDepthPixelsRef().getHeight();
            int w = kinect0.getDepthPixelsRef().getWidth();
            for(int y = 0; y < h; y += step) {
                for(int x = 0; x < w; x += step) {
                    float dist = kinect0.getDistanceAt(x, y);
                    if(dist > 50 && dist < 500) {
                        ofVec3f pt = kinect0.getWorldCoordinateAt(x, y, dist);
                        ofColor c;
                        
                        float h = ofMap(dist, 50, 200, 0, 255, true);
                        c.setHsb(h, 255, 255);
                        
                        
                        mesh.addColor(c);
                        //mesh.addColor(pixels.getColor(x, y));
//                        mesh.addColor(pixels.getColor(floor(ofMap(x, 0, w, 0, pixels.getWidth()-1)),
//                                                      floor(ofMap(y, 0, h, 0, pixels.getHeight()-1))));
                        
                        mesh.addVertex(pt);
                    }
                }
            }
        }
        

    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    
    //gr.getRegisteredTexture(process_occlusion).draw(0, 0, 1024, 848);
    
    
    //return;
    
    if (depthTex0.isAllocated()) {
        ofPushStyle();
        glPointSize(2);
        ecam.begin();
        ofDrawAxis(100);
        ofPushMatrix();
        ofTranslate(0, 0, -100);
        
        shader.begin();
        //shader.setUniform2f("resolution", gr.getRegisteredTexture(process_occlusion).getWidth(), gr.getRegisteredTexture(process_occlusion).getHeight());
        //shader.setUniformTexture("colorTex", gr.getRegisteredTexture(process_occlusion), 0);
        
        mesh.draw();
        shader.end();
        
        ofPopMatrix();
        ecam.end();
        ofPopStyle();
    }
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("Device Count : " + ofToString(ofxMultiKinectV2::getDeviceCount()), 10, 40);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'd') {
        process_occlusion =  !process_occlusion;
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
