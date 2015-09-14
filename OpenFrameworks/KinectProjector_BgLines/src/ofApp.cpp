#include "ofApp.h"


void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetWindowShape(1280, 960);
    
    // set up kinect
    //kinect.setup();
    kinect.setupFromONI("/Users/gene/Code/of_v0.8-4.4_osx_release/templates_old/Kinect/openni_oniRecording/bin/data/alecsroom.oni");
    //kinect.setupFromONI("/Users/gene/Desktop/oni/2012-06-25-14-08-00-860.oni");
    
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
    projector.setup("main", 0, 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, false);
    //projector.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);
    
    // setup gui
    gui.setup("backgrounding");
    gui.add(minArea.set("minArea", 1000, 0, 0.1*640*480));
    gui.add(maxArea.set("maxArea", 300000, 0, 640*480));
    gui.add(color.set("color", ofColor(255), ofColor(0,0,0), ofColor(255,255,255,255)));
    gui.add(numPoints.set("numPoints", 60, 1, 300));
    gui.add(offset.set("offset", 0, -TWO_PI, TWO_PI));
    gui.add(strokeWeight.set("strokeWeight", 1.0, 0.5, 8.0));
    gui.add(alpha.set("alpha", 100, 0, 255));
    gui.add(lerpRate.set("lerpRate", 0.1, 0.0, 1.0));
    gui.add(smoothness.set("contour smooth", 2.0, 1.0, 10.0));
    gui.setPosition(0, 0);
    gui.loadFromFile("settings.xml");
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

    // clean up old users
    map<int, Contour*>::iterator itu = users.begin();
    while (itu != users.end()) {
        bool foundUser = false;
        for (auto e : eligibleUsers) {
            if (e == itu->first) {
                foundUser = true;
            }
        }
        if (foundUser) {
            ++itu;
        }
        else {
            delete itu->second;
            users.erase(itu++);
        }
    }
    
    
    if (points.size() != numPoints) {
        points.resize(numPoints);
    }
    
    if (users.size() == 0) return;
    
    // draw the first contour only?
    itu = users.begin();
    Contour *c = itu->second;
    for (int i=0; i<points.size(); i++) {
        ofVec2f cPoint = c->points[ofMap(i, 0, points.size(), 0, c->points.size()-1)];
        points[i].set(ofLerp(points[i].x, cPoint.x, lerpRate),
                      ofLerp(points[i].y, cPoint.y, lerpRate)); //new PVector(lerp(pts[j].x, p2.x, LERP_RATE), lerp(pts[j].y, p2.y, LERP_RATE));
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
    ofPopStyle();
    ofPopMatrix();
    gui.draw();
    
    // Ribbons
    projector.begin();
    
    ofPushMatrix();
    ofPushStyle();
    ofClear(0, 0);
    
    ofNoFill();
    ofSetLineWidth(strokeWeight);
    ofSetColor(color, alpha);
    for (int i=0; i<points.size(); i++)
    {
        //float ang = (0.01*frameCount + map(j, 0, NUM_PTS, 0, TWO_PI)) % TWO_PI;
        float ang = fmodf(ofMap(i, 0, points.size(), 0, TWO_PI) + offset, TWO_PI);
        ofPoint ctr(projector.getWidth() / 2 + projector.getWidth() * cos(ang),
                    projector.getHeight() / 2 + projector.getWidth() * sin(ang));
        ofLine(ctr.x, ctr.y, points[i].x, points[i].y);
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