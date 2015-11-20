#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ProjectionMapping.h"




class Surface {
public:
    Surface() {
        tc = ofRandom(1000);
        tb = ofRandom(1000);
    }
    
    void setImage(int idx, string path) {
        this->idx = idx;
        img.load(path);
        img.resize(500, img.getHeight() * 500 / img.getWidth());
        fbo.allocate(img.getWidth(), img.getHeight());
    }

    void setShader(string vert, string frag) {
        shader.load(vert, frag);
    }
    
    void eventSetBreathe(bool & b) {
        if (b) {
            offset = -0.01 * ofGetFrameNum();
        }
    }
    
    void addToGui(ofxPanel & gui) {
        group.setName("surface "+ofToString(idx));
        group.add(brightness.set("brightness", 1.0, 0.0, 5.0));
        group.add(saturation.set("saturation", 1.0, -5.0, 5.0));
        group.add(contrastCtr.set("contrastCtr", 1.0, -4.0, 4.0));
        group.add(contrastRange.set("contrastRange", 0.0, -1.0, 1.0));
        group.add(breathe.set("breathe", false));
        group.add(breatheMargin.set("breatheMargin", 0.2, 0, 1.0));
        group.add(breatheSpeed.set("breatheSpeed", 0.1, 0, 0.2));

        group.add(breatheSubtleInterval.set("breatheSubtleInterval", 10, 5, 100));
        group.add(breatheSubtleLerp.set("breatheSubtleLerp", 0.01, 0.001, 0.1));
        group.add(breatheBigInterval.set("breatheBigInterval", 3, 5, 100));
        group.add(breatheBigLerp.set("breatheBigLerp", 0.1, 0.001, 0.1));
        
        breathe.addListener(this, &Surface::eventSetBreathe);
        
        gui.add(group);
    }
    
    void update() {
        tc++;
        tb++;
        
        
        if (breatheMode == 0) {
            // subtle breathe
            breatheMargin = ofLerp(breatheMargin, 0.1, breatheSubtleLerp);
            if (tb % (breatheSubtleInterval*60) == 0) {tb=0; breatheMode = 1;}
        }
        else if (breatheMode == 1) {
            // exaggerated breathe
            breatheMargin = ofLerp(breatheMargin, 0.3, breatheBigLerp);
            if (tb % (breatheBigInterval*60) == 0) {tb=0; breatheMode = 0;}
        }
        
        if (contrastMode == 0) {
            contrastRange = ofLerp(contrastRange, 0.1, 0.01);
            if (tc % (60*60) == 0) {tc=0;contrastMode = 1;}
        }
        else if (contrastMode == 1) {
            contrastRange = ofLerp(contrastRange, 0.3, 0.01);
            if (tc % (5*60) == 0) {tc=0;contrastMode = 0;}
        }

        float breatheOffset = ofMap(sin(ofGetFrameNum() * breatheSpeed + offset), -1, 1, 0, breatheMargin);
        float contrast = ofMap(sin(ofGetFrameNum() * 0.1), -1, 1, contrastCtr-contrastRange, contrastCtr+contrastRange);
        
        fbo.begin();
        ofSetColor(0);
        ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
        ofSetColor(255);
        shader.begin();

        shader.setUniform2f("resolution", fbo.getWidth(), fbo.getHeight());
        shader.setUniform1f("brightness", brightness);
        shader.setUniform1f("contrast", contrast);
        shader.setUniform1f("saturation", saturation);
        shader.setUniform1f("offset", breatheOffset);
        
        img.draw(0, 0);
        
        shader.end();
        fbo.end();
    }
    
    void draw() {
        
    }
    
    int idx;
    ofImage img;
    ofShader shader;
    ofFbo fbo;
    
    ofParameterGroup group;
    ofParameter<float> brightness;
    ofParameter<float> saturation;
    ofParameter<float> contrastCtr;
    ofParameter<float> contrastRange;
    ofParameter<bool> breathe;
    ofParameter<float> breatheMargin;
    ofParameter<float> breatheSpeed;
    ofParameter<int> breatheSubtleInterval;
    ofParameter<float> breatheSubtleLerp;
    ofParameter<int> breatheBigInterval;
    ofParameter<float> breatheBigLerp;

    float offset;
    
    int contrastMode = 0;
    int breatheMode = 0;
    int tb, tc;
};


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    vector<Surface*> surfaces;
    ofxPanel gui;
    bool guiDraw;
    ProjectionMapping projection;
};

