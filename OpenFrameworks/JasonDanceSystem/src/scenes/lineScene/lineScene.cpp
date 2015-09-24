//
//  lineScene.cpp
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-03.
//
//

#include "lineScene.h"

void lineScene::setup(openNIManager * _oni, flowManager * _flow ){
    oni = _oni;
    flow = _flow;
    
    width = 1920;
    height = 1080;
    
    bDrawMoire = true;
    bDrawBody = true;
    
    distanceScale = 10;
    lineWidth = 2;
    
    lineLength = linesLength = 1000;
    
    llArm = true;
    rlArm = true;
    luArm = true;
    ruArm = true;
    llLeg = true;
    rlLeg = true;
    luLeg = true;
    ruLeg = true;
    
//    bgColor.set(0.0);
    lineColor.set(1.0);
    
    //init gui dims
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    //gui!
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 60);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("lineWidth", 0.1, 10, &lineWidth, length-xInit, dim);
    gui->addSlider("lineLength", 1, 2000, &lineLength, length-xInit, dim);
    gui->addSlider("linesWidth", 0.1, 10, &linesWidth, length-xInit, dim);
    gui->addSlider("linesLength", 1, 2000, &linesLength, length-xInit, dim);
    gui->addSlider("distanceScale", 0.1, 1.0, &distanceScale, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addLabelToggle("drawBody", &bDrawBody);
    gui->addLabelToggle("drawMoire", &bDrawMoire);
    gui->addSpacer(length-xInit, 1);
    gui->addLabelToggle("llArm", &llArm);
    gui->addLabelToggle("rlArm", &rlArm);
    gui->addLabelToggle("luArm", &luArm);
    gui->addLabelToggle("ruArm", &ruArm);
    gui->addLabelToggle("llLeg", &llLeg);
    gui->addLabelToggle("rlLeg", &rlLeg);
    gui->addLabelToggle("luLeg", &luLeg);
    gui->addLabelToggle("ruLeg", &ruLeg);
    gui->addSpacer(length-xInit, 1);
    gui->addLabel("LINES");
    gui->addSlider("red", 0.0, 1.0, &lineColor.r, length-xInit, dim);
    gui->addSlider("green", 0.0, 1.0, &lineColor.g, length-xInit, dim);
    gui->addSlider("blue", 0.0, 1.0, &lineColor.b, length-xInit, dim);
    gui->addSlider("alpha", 0.0, 1.0, &lineColor.a, length-xInit, dim);
    
    gui->setDrawBack(false);
    gui->setVisible(false);


    
}

void lineScene::update(int width, int height){

}

void lineScene::draw(float x, float y, float scale){
    
//    if ( oni->isSkeleton() ) {
    ofEnableAntiAliasing();
//    ofEnableAlphaBlending();
    
//    ofBackground(bgColor);
    ofSetColor(lineColor);
    
    ofPushMatrix();
    ofTranslate(x, y);
    ofTranslate(160, 120);
    ofScale(scale, scale);
    ofTranslate(-160, -120);
    ofScale(0.5, 0.5);
    if ( bDrawMoire) drawMoire();
    if ( bDrawBody ) drawBodyLines();
    ofPopMatrix();

//    ofDisableAlphaBlending();
    ofDisableAntiAliasing();
//    }

}

void lineScene::drawLine(ofVec3f jointA, ofVec3f jointB){

    ofVec2f diff = ofVec2f((jointB.x - jointA.x), (jointB.y - jointA.y));
    
    diff.normalize();
    
    ofVec2f midpoint = ofVec2f((jointA.x + jointB.x) / 2, (jointA.y + jointB.y) / 2);
    midpoint = midpoint;
    
    ofPushStyle();
    ofSetLineWidth(lineWidth);
    
    ofLine(midpoint, midpoint + diff * linesLength);
    ofLine(midpoint, midpoint - diff * linesLength);
    
    ofPopStyle();

}

void lineScene::drawLines(ofVec3f jointA, ofVec3f jointB, float spacing){
    
    ofVec2f diff = ofVec2f((jointB.x - jointA.x), (jointB.y - jointA.y));
    diff.normalize();

    ofVec2f midpoint = ofVec2f((jointA.x + jointB.x) / 2, (jointA.y + jointB.y) / 2);
    
    ofVec2f normal = diff.getRotated(90);
    
    ofVec2f midpointB = midpoint + normal * 10;
    
    
    ofPushStyle();
    ofSetLineWidth(linesWidth);
    ofEnableAlphaBlending();

    ofPushMatrix();
    
    ofLine(midpoint, midpoint + diff * linesLength);
    ofLine(midpoint, midpoint - diff * linesLength);
    
    for( int i = 1; i < width / spacing; i++) {
        
        ofVec2f midpointB = midpoint + normal * spacing * i;
        ofLine(midpointB.x, midpointB.y, midpointB.x + diff.x * linesLength, midpointB.y + diff.y * linesLength);
        ofLine(midpointB.x, midpointB.y, midpointB.x - diff.x * linesLength, midpointB.y - diff.y * linesLength);
        
        midpointB = midpoint - normal * spacing * i;
        ofLine(midpointB.x, midpointB.y, midpointB.x + diff.x * linesLength, midpointB.y + diff.y * linesLength);
        ofLine(midpointB.x, midpointB.y, midpointB.x - diff.x * linesLength, midpointB.y - diff.y * linesLength);
    }
    
    ofPopMatrix();
    ofDisableAlphaBlending();
    ofPopStyle();

}

void lineScene::drawBodyLines(){
    
    if (llArm) {
        drawLine(oni->leftElbowPos, oni->leftHandPos);
    }
    
    if (rlArm) {
        drawLine(oni->rightElbowPos, oni->rightHandPos);
    }
    
    if (luArm) {
        drawLine(oni->leftShoulderPos, oni->leftElbowPos);
    }
    
    if (ruArm) {
        drawLine(oni->rightShoulderPos, oni->rightElbowPos);
    }
    
    if (llLeg) {
        drawLine(oni->leftFootPos, oni->leftKneePos);
    }
    
    if (rlLeg) {
        drawLine(oni->rightFootPos, oni->rightKneePos);
    }
    
    if (luLeg) {
        drawLine(oni->leftKneePos, oni->leftHipPos);
    }
    
    if (ruLeg) {
        drawLine(oni->rightKneePos, oni->rightHipPos);
    }

}

void lineScene::drawMoire(){
    ofPushStyle();
    ofSetLineWidth(lineWidth);
    
    
    if (llArm) {
        ofVec2f leftElbow2d = ofVec2f(oni->leftElbowPos);
        ofVec2f leftHand2d = ofVec2f(oni->leftHandPos);
        
        if ( leftElbow2d != leftHand2d ) {
            float distLeft = leftElbow2d.distance(leftHand2d);
            drawLines(oni->leftElbowPos, oni->leftHandPos, distLeft * distanceScale);
        }
    }
    
    if (rlArm) {
        ofVec2f rightElbow2d = ofVec2f(oni->rightElbowPos);
        ofVec2f rightHand2d = ofVec2f(oni->rightHandPos);
        
        if ( rightElbow2d != rightHand2d ) {
            float distRight = rightElbow2d.distance(rightHand2d);
            drawLines(oni->rightElbowPos, oni->rightHandPos, distRight * distanceScale);
        }
    }
    
    if (luArm) {
        ofVec2f leftShoulder2d = ofVec2f(oni->leftShoulderPos);
        ofVec2f leftElbow2d = ofVec2f(oni->leftElbowPos);
        
        if ( leftShoulder2d != leftElbow2d ) {
            float distLeft = leftShoulder2d.distance(leftElbow2d);
            drawLines(oni->leftShoulderPos, oni->leftElbowPos, distLeft * distanceScale);
        }
    }
    
    if (ruArm) {
        ofVec2f rightShoulder2d = ofVec2f(oni->rightShoulderPos);
        ofVec2f rightElbow2d = ofVec2f(oni->rightElbowPos);
        
        if ( rightShoulder2d != rightElbow2d ) {
            float distRight =  rightShoulder2d.distance(rightElbow2d);
            drawLines(oni->rightShoulderPos, oni->rightElbowPos, distRight * distanceScale);
        }
    }
    
    if (llLeg) {
        ofVec2f leftKnee2d = ofVec2f(oni->leftKneePos);
        ofVec2f leftFoot2d = ofVec2f(oni->leftFootPos);
        
        if ( leftKnee2d != leftFoot2d ) {
            float distLeft = leftKnee2d.distance(leftFoot2d);
            drawLines(oni->leftKneePos, oni->leftFootPos, distLeft * distanceScale);
        }
    }
    
    if (rlLeg) {
        ofVec2f rightKnee2d = ofVec2f(oni->rightKneePos);
        ofVec2f rightFoot2d = ofVec2f(oni->rightFootPos);
        
        if ( rightKnee2d != rightFoot2d ) {
            float distRight = rightKnee2d.distance(rightFoot2d);
            drawLines(oni->rightKneePos, oni->rightFootPos, distRight * distanceScale);
        }
    }
    
    if (luLeg) {
        ofVec2f leftHip2d = ofVec2f(oni->leftHipPos);
        ofVec2f leftKnee2d = ofVec2f(oni->leftKneePos);
        
        if ( leftHip2d != leftKnee2d ) {
            float distLeft = leftHip2d.distance(leftKnee2d);
            drawLines(oni->leftHipPos, oni->leftKneePos, distLeft * distanceScale);
        }
    }
    
    if (ruLeg) {
        ofVec2f rightHip2d = ofVec2f(oni->rightHipPos);
        ofVec2f rightKnee2d = ofVec2f(oni->rightKneePos);
        
        if ( rightHip2d != rightKnee2d ) {
            float distRight = rightHip2d.distance(rightKnee2d);
            drawLines(oni->rightHipPos, oni->rightKneePos, distRight * distanceScale);
        }
    }
    
    ofPopStyle();

}