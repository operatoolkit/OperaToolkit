//
//  postProcManager.h
//  ETD_system
//
//  Created by Jason Levine on 2014-02-04.
//
//

#pragma once

#include "ofxPostProcessing.h"
#include "ofMain.h"

class postProcManager {
public:
    void setup();
    
    int width, height, ppWidth, ppHeight;
    
    ofFbo fbo;
    ofxPostProcessing post;
    vector<RenderPass::Ptr> renderPasses;
    
    float fadeAmt;
    
    void begin(ofFloatColor bgColor, bool bgColorChanged) {
        fbo.begin();
        if (!bgColorChanged) {
            bgColor.a = fadeAmt;
            bgColorChanged = false;
        }
        ofSetColor(bgColor);
        ofRect(0,0,ofGetWidth(),ofGetHeight());
    }
    
    void end() { fbo.end(); post.process(fbo); }
    
    void draw() { ofSetColor(255); post.draw(0,0); }
};