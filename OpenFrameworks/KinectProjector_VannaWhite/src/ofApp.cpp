#include "ofApp.h"


Contour::Contour(vector<ofVec2f> & points, ofPoint center, int label)
{
    this->points = points;
    this->center = center;
    this->label = label;
    age = 0;
    color = ofColor(ofRandom(60,255), ofRandom(60,255), ofRandom(60,255));
}

void Contour::setPoints(vector<ofVec2f> & points, ofPoint center)
{
    this->points = points;
    this->center = center;
}

int Contour::getNumPoints()
{
    return points.size();
}

void Contour::draw()
{
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(2);
    ofSetColor(color);
    ofBeginShape();
    for (int j=0; j<points.size(); j++) {
        ofVertex(points[j].x, points[j].y);
    }
    ofEndShape();
    ofPopStyle();
}

TV::TV(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    active = false;
    pActive = false;
    color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
}

void TV::update() {
    if (active && !pActive) {
        color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
        alph = 255;
    }
    pActive = active;
}

bool TV::setActive(bool active) {
    this->active = active;
}

void TV::draw() {
    ofPushMatrix();
    ofPushStyle();
    //ofSetColor(color);
    ofSetColor(255, alph);
    alph *= 0.95;
    ofFill();
    ofRect(x, y, w, h);
    ofPopStyle();
    ofPopMatrix();
}

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetWindowPosition(50, 50);
    ofSetWindowShape(1024, 768);
    
    
    //////////////////////////
    // setup gui

    ofParameterGroup guiProjector;
    guiProjector.setName("Projector");
    guiProjector.add(projectorWidth.set("projectorWidth", PROJECTOR_RESOLUTION_X, 720, 1600));
    guiProjector.add(projectorHeight.set("projectorHeight", PROJECTOR_RESOLUTION_Y, 560, 1200));
    guiProjector.add(toProject.set("project!", true));
    
    toProject.addListener(this, &ofApp::eventToProject);
    projectorWidth.addListener(this, &ofApp::eventResizeProjector);
    projectorHeight.addListener(this, &ofApp::eventResizeProjector);
    
    ofParameterGroup guiKinect;
    guiKinect.setName("Kinect");
    guiKinect.add(useUserImage.set("useUserImage", true));
    guiKinect.add(nearThreshold.set("nearThreshold", 9, 0, 255));
    guiKinect.add(farThreshold.set("farThreshold", 0, 0, 255));
    guiKinect.add(minArea.set("minArea", 1000, 0, 5000));
    guiKinect.add(maxArea.set("maxArea", 70000, 15000, 150000));
    guiKinect.add(smoothness.set("smoothness", 3, 1, 30));
    
    ofParameterGroup guiApp;
    guiApp.setName("App");
    guiApp.add(tvWidth.set("windowsWidth", 60, 5, 300));
    guiApp.add(tvHeight.set("windowsHeight", 40, 5, 300));
    
    tvWidth.addListener(this, &ofApp::eventResizeTVs);
    tvHeight.addListener(this, &ofApp::eventResizeTVs);
   
    gui.setup("Vanna White");
    gui.add(guiProjector);
    gui.add(guiKinect);
    gui.add(guiApp);
    gui.setPosition(0, 0);
    

    //////////////////////////
    // setup kinect

    if (DEBUG_MODE) {
        kinect.setupFromONI("/Users/gene/Code/of_v0.8-4.4_osx_release/of-tools/KinectProjectorToolkit/alecsroom.oni");
    }
    else {
        kinect.setup();
    }
    kinect.addImageGenerator();
    kinect.addDepthGenerator();
    kinect.addUserGenerator();
    kinect.setMaxNumUsers(5);
    kinect.setUseMaskPixelsAllUsers(true);
    kinect.setMaskPixelFormatAllUsers(OF_PIXELS_MONO);
    kinect.setUseDepthRawPixels(true);
    kinect.setRegister(true);
    kinect.setMirror(false);
    kinect.start();
    
    grayImage.allocate(kinect.getWidth(), kinect.getHeight());
    
    // setup calibration
    kpt.loadCalibration("/Users/Gene/Desktop/calibration.xml");
    
    // setup projection window
    projector.setup("main", 0, 0, PROJECTOR_RESOLUTION_X/2, PROJECTOR_RESOLUTION_Y/2, true);
    //projector.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);

    
    
    //////////////////////////
    // setup app

    setupTVs();

    loadPreset();
}

void ofApp::setupTVs() {
    nx = ceil((float)ofGetWidth() / tvWidth);
    ny = ceil((float)ofGetHeight() / tvHeight);
    int idx = 0;
    for (int y=0; y<ofGetHeight(); y+=tvHeight) {
        for (int x=0; x<ofGetWidth(); x+=tvWidth) {
            if (tvs.count(idx) == 0) {
                tvs[idx] = new TV(x, y, tvWidth, tvHeight);
            }
            else {
                tvs[idx]->x = x;
                tvs[idx]->y = y;
                tvs[idx]->w = tvWidth;
                tvs[idx]->h = tvHeight;
            }
            idx++;
        }
    }
}

void ofApp::update() {
    contourFinder.setMinArea(minArea);
    contourFinder.setMaxArea(maxArea);
    
    kinect.update();
    
    if(!kinect.isNewFrame()) {
        clean();
        return;
    }
    
    depthPixels = kinect.getDepthRawPixels();
    vector<int> eligibleUsers;
    
    if (useUserImage)
    {
        int numUsers = kinect.getNumTrackedUsers();
        for (int i=0; i<numUsers; i++)
        {
            ofxOpenNIUser & user = kinect.getTrackedUser(i);
            if (!user.getMaskPixels().isAllocated()) {continue;}
            
            user.setMaskPixelFormat(OF_PIXELS_MONO);
            grayImage.setFromPixels(user.getMaskPixels());
            contourFinder.findContours(grayImage);
            
            if (contourFinder.size() == 0) {continue;}
            eligibleUsers.push_back(user.getXnID());
            
            vector<ofVec2f> calibratedPoints;
            getCalibratedContour(depthPixels, contourFinder.getContour(0), calibratedPoints);
            
            if (!users.count(user.getXnID())) {
                users[user.getXnID()] = new Contour(calibratedPoints, toOf(contourFinder.getCenter(0)), user.getXnID());
            }
            else {
                users[user.getXnID()]->setPoints(calibratedPoints, toOf(contourFinder.getCenter(0)));
            }
            
            for (int j=1; j<contourFinder.size(); j++) {
                if (contourFinder.getContour(j).size() > users[user.getXnID()]->getNumPoints()) {
                    getCalibratedContour(depthPixels, contourFinder.getContour(j), calibratedPoints);
                    users[user.getXnID()]->setPoints(calibratedPoints, toOf(contourFinder.getCenter(j)));
                }
            }
        }
    }
    else
    {
        grayImage.setFromPixels(kinect.getDepthRawPixels());
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        
        // determine found contours
        contourFinder.findContours(grayImage);
        
        for(int i = 0; i < contourFinder.size(); i++) {
            vector<cv::Point> points = contourFinder.getContour(i);
            int label = contourFinder.getLabel(i);
            eligibleUsers.push_back(label);
            
            vector<ofVec2f> calibratedPoints;
            getCalibratedContour(depthPixels, points, calibratedPoints);
            if (!users.count(label)) {
                users[label] = new Contour(calibratedPoints, toOf(contourFinder.getCenter(i)), label);
            }
            else {
                users[label]->setPoints(calibratedPoints, toOf(contourFinder.getCenter(i)));
            }
        }
    }
    
    // get rid of old contours
    map<int, Contour*>::iterator itu = users.begin();
    while (itu != users.end()) {
        bool found = false;
        for (auto e : eligibleUsers) {
            if (e == itu->first) {
                found = true;
            }
        }
        if (!found) {
            delete itu->second;
            users.erase(itu++);
        }
        else {
            ++itu;
        }
    }
    
    clean();
}

void ofApp::clean() {
    // trick to reset user generator in case all users are lose
    if (kinect.getNumTrackedUsers()) {
        hadUsers = true;
    }
    else if (!kinect.getNumTrackedUsers() && hadUsers){
        hadUsers = false;
        kinect.setPaused(true);
        kinect.removeUserGenerator();
        kinect.addUserGenerator();
        kinect.setPaused(false);
    }
}

void ofApp::draw() {
    // gui
    ofPushMatrix();
    ofPushStyle();
    kinect.drawImage(0, 0);
    ofTranslate(640, 0);
    kinect.drawDepth();
    kinect.drawSkeletons();
    ofTranslate(-640, 480);
    grayImage.draw(0, 0);
    ofPopStyle();
    ofPopMatrix();

    gui.draw();
    
    
    // then draw the output into the projector window
    projector.begin();
    
    ofPushMatrix();
    ofPushStyle();

    ofClear(0, 0);
    ofBackground(0);
    
    map<int, TV*>::iterator it = tvs.begin();
    for (; it != tvs.end(); ++it) {
        it->second->setActive(false);
        it->second->draw();
    }
    
    
    for (int i=0; i<contourFinder.size(); i++)
    {
        vector<ofVec2f> calibratedPoints;
        getCalibratedContour(depthPixels, contourFinder.getContour(i), calibratedPoints);
        
        ofBeginShape();
        ofFill();
        ofSetColor(255, 255, 0);
        for (int j=0; j<calibratedPoints.size(); j+=3) {
            ofVertex(calibratedPoints[j].x, calibratedPoints[j].y);
        }
        ofEndShape();
        
        for (int j=0; j<calibratedPoints.size(); j+=4) {
            int x = floor(calibratedPoints[j].x / tvWidth);
            int y = floor(calibratedPoints[j].y / tvHeight);
            int idx = x + y * nx;
            if (tvs.count(idx) > 0 ) {
                tvs[idx]->setActive(true);
            }
        }
    }
    
    it = tvs.begin();
    for (; it != tvs.end(); ++it) {
        it->second->update();
    }

    ofPopStyle();
    ofPopMatrix();
    
    projector.end();
}

void ofApp::getCalibratedContour(ofShortPixels & depthPixels, vector<cv::Point> & points, vector<ofVec2f> & calibratedPoints) {
    for (int j=0; j<points.size(); j+=smoothness) {
        ofPoint depthPoint = ofPoint(points[j].x, points[j].y,
                                     depthPixels[(int)(points[j].x + (int) points[j].y * kinect.getWidth())]);
        ofVec3f worldPoint = kinect.projectiveToWorld(depthPoint);
        ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);
        ofPoint mappedPoint(projectedPoint.x * projector.getWidth(),
                            projectedPoint.y * projector.getHeight());
        calibratedPoints.push_back(mappedPoint);
    }
}

void ofApp::keyPressed(int key) {
}

void ofApp::exit() {
    savePreset();
}

void ofApp::eventToProject(bool & b) {
    projector.setWindowPosition(toProject ? ofGetScreenWidth() : 0, 0);
}

void ofApp::eventResizeProjector(int & v) {
    projector.setWindowSize(projectorWidth, projectorHeight);
}

void ofApp::eventResizeTVs(int & v) {
    setupTVs();
}

void ofApp::loadPreset() {
    ofXml xml;
    bool loaded = xml.load(ofToDataPath("settings.xml"));
    if (!loaded) return;
    xml.setTo("Preset");
    projectorWidth = xml.getValue<int>("projectorWidth");
    projectorHeight = xml.getValue<int>("projectorHeight");
    toProject = xml.getValue<bool>("toProject");
    useUserImage = xml.getValue<bool>("useUserImage");
    nearThreshold = xml.getValue<float>("nearThreshold");
    farThreshold = xml.getValue<float>("farThreshold");
    minArea = xml.getValue<float>("minArea");
    maxArea = xml.getValue<float>("maxArea");
    smoothness = xml.getValue<int>("smoothness");
    tvWidth = xml.getValue<int>("tvWidth");
    tvHeight = xml.getValue<int>("tvHeight");
}

void ofApp::savePreset() {
    ofXml xml;
    xml.addChild("Preset");
    xml.setTo("Preset");
    xml.addValue("projectorWidth", projectorWidth);
    xml.addValue("projectorHeight", projectorHeight);
    xml.addValue("toProject", toProject);
    xml.addValue("useUserImage", useUserImage);
    xml.addValue("nearThreshold", nearThreshold);
    xml.addValue("farThreshold", farThreshold);
    xml.addValue("minArea", minArea);
    xml.addValue("maxArea", maxArea);
    xml.addValue("smoothness", smoothness);
    xml.addValue("tvWidth", tvWidth);
    xml.addValue("tvHeight", tvHeight);
    xml.save(ofToDataPath("settings.xml"));
}