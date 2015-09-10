#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    //ofSetFullscreen(true);
    ofSetFrameRate(60);

    kinect.setRegistration(true);
    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    
    kinect.open();		// opens first available kinect

    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    masker.setup(kinect.getWidth(), kinect.getHeight());
    masker.newLayer();

    gui.setup();
    gui.setPosition(660, 20);
    gui.setName("ThousandArms");
    gui.add(appFrameSkip.set("app frame skip", 1, 1, 30));
    gui.add(debug.set("debug", false));
    gui.add(recording.set("recording", true));
    gui.add(totalNumImages.set("total number images", 300, 100, 500));
    gui.add(nearThreshold.set("near threshold", 255, 0, 255));
    gui.add(farThreshold.set("near threshold", 150, 0, 255));
    gui.add(frameRate.set("frame rate", 5, 1, 15));
    gui.add(numImages.set("number images draw", 10, 1, 30));
    gui.add(imageSkip.set("image skip", 5, 1, 15));
    gui.add(frameSkip.set("frame skip", 5, 1, 30));
    gui.add(translate.set("translate", ofVec2f(0, 0), ofVec2f(-1000, -1000), ofVec2f(1000, 1000)));
    gui.add(scale.set("scale", ofVec2f(1, 1), ofVec2f(1, 1), ofVec2f(5, 5)));

    indexImage = 0;

    projector.setup("projector", 0, 0, 1280, 800, false);
}

//--------------------------------------------------------------
void ofApp::update() {
    kinect.update();
    
    if(kinect.isFrameNew())
    {
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
    
        if (recording) {
        
            // add a new texture to the history
            ofFbo newFbo;
            newFbo.allocate(kinect.getWidth(), kinect.getHeight());
            newFbo.begin();
            ofClear(0, 0);

            // draw graphics from the mask
            masker.beginLayer();
            ofClear(0, 0, 0, 255);
            kinect.draw(0, 0, 640, 480);
            masker.endLayer();
            
            // draw the mask
            masker.beginMask();
            ofClear(0, 0, 0, 255);
            grayImage.draw(0, 0, newFbo.getWidth(), newFbo.getHeight());
            masker.endMask();
            
            //Draw the combined result into the fbo
            masker.draw();

            newFbo.end();
            
            fbo.push_back(newFbo);
        }
    }
    
    
    while (fbo.size() > totalNumImages) {
        fbo.erase(fbo.begin());
    }
    
}

//--------------------------------------------------------------
void ofApp::draw() {

    if (debug && fbo.size() > 0) {
        ofBackground(0);
        fbo[fbo.size()-1].draw(0, 0);
    }
    gui.draw();
    
    projector.begin();

    ofPushMatrix();
    ofTranslate(translate->x, translate->y);
    ofScale(scale->x, scale->y);

    ofBackground(0);
    if (ofGetFrameNum() % appFrameSkip == 0 && fbo.size() > 0)
    {
        ofBackground(0);
        for (int i=0; i < numImages; i++)
        {
            int idx = (indexImage + i * imageSkip) % fbo.size();
            fbo[idx].draw(0, 0);
        }
        indexImage = (indexImage + frameSkip) % fbo.size();
    }
    
    ofPopMatrix();
    
    projector.end();

}


//--------------------------------------------------------------
void ofApp::exit() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{}
