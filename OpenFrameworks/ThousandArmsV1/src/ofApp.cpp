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
    gui.add(debug.set("debug", true));
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
    indexDraw = -1;

    projector.setup("projector", 0, 0, 1280, 800, false);
    
    fbo.resize(totalNumImages);
    for (int i=0; i<fbo.size(); i++) {
        fbo[i].allocate(kinect.getWidth(), kinect.getHeight());
        fbo[i].begin();
        ofClear(0, 0);
        fbo[i].end();
    }
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
        
            indexDraw = (indexDraw + 1) % fbo.size();
            // add a new texture to the history
            fbo[indexDraw].begin();
            ofClear(0, 0);

            // draw graphics from the mask
            masker.beginLayer();
            ofClear(0, 0, 0, 255);
            kinect.draw(0, 0, 640, 480);
            masker.endLayer();
            
            // draw the mask
            masker.beginMask();
            ofClear(0, 0, 0, 255);
            grayImage.draw(0, 0, fbo[indexDraw].getWidth(), fbo[indexDraw].getHeight());
            masker.endMask();
            
            //Draw the combined result into the fbo
            masker.draw();

            fbo[indexDraw].end();
            
            
        }
    }
    
    /*
    while (fbo.size() > totalNumImages) {
        fbo[0].allocate(0, 0);
        fbo.erase(fbo.begin());
    }*/
    
}

//--------------------------------------------------------------
void ofApp::draw() {

    if (debug && indexDraw >= 0) {
        ofBackground(0);
        kinect.drawDepth(640, 0);
        fbo[indexDraw].draw(0, 0);
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
            
            ofSetColor(ofMap(sin(0.011*ofGetFrameNum()+5 + 4*i), -1, 1, 0, 255),
                       ofMap(sin(0.013*ofGetFrameNum()+10 + 5*i), -1, 1, 0, 255),
                       ofMap(sin(0.017*ofGetFrameNum()+20 + 6*i), -1, 1, 0, 255));
            
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
