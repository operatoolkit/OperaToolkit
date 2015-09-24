//
//  particleScene.cpp
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-03.
//
//

#include "particleScene.h"

void particleScene::setup(openNIManager * _oni, flowManager * _flow ){
    oni = _oni;
    flow = _flow;
    
    particles.setup(200, ofGetWidth(), ofGetHeight(), true, false);
    
    velMult = 0.375;
    threshold = 1;
    bReverse = false;
    bGenerate = true;
    
    
    //init gui dims
    float dim = 16;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    //gui!
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    //gui!
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 60);
    
    gui->addSpacer(length-xInit, 1);
    gui->addLabelToggle("generate/control", &bGenerate);
    gui->addIntSlider("totalParticles", 1, 2000, 200, length-xInit, dim);
    gui->addIntSlider("maxParticles", 2, 5000, &particles.maxParticles, length-xInit, dim);
    gui->addSlider("threshold", 0.0, 100.0, &threshold, length-xInit, dim);
    gui->addSlider("velMult", 0.01, 2.0, &velMult, length-xInit, dim);
    gui->addLabelToggle("reverse", &bReverse);
    gui->addLabelToggle("bounce", &particles.bounce);
    gui->addLabelToggle("wrap", &particles.wrap);
    gui->addIntSlider("attractionRadius", 1, 1000, &particles.attractionRadius, length-xInit, dim);
    gui->addSlider("attractionForce", 0.0, 5.0, &particles.attractionForce, length-xInit, dim);
    gui->addIntSlider("repulsionRadius", 1, 100, &particles.repulsionRadius, length-xInit, dim);
    gui->addSlider("repulsionForce", 0.0, 5.0, &particles.repulsionForce, length-xInit, dim);
    
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("baseSize", 1, 10, &particles.baseSize, length-xInit, dim);
    gui->addSlider("sizeMod", 0.0001, 0.1, &particles.sizeMod, length-xInit, dim);
    gui->addIntSlider("tailLength", 1, 25, &particles.tailLength, length-xInit, dim);
    gui->addIntSlider("alpha", 0, 255, &particles.alpha, length-xInit, dim);
    gui->addLabelToggle("circles", &particles.drawCircles);
    gui->addLabelToggle("lines", &particles.drawLines);
    
    gui->addSpacer(length-xInit, 1);
//    gui->addLabel("BACKGROUND");
//    gui->addSlider("red", 0.0, 1.0, &bgColor.r, length-xInit, dim);
//    gui->addSlider("green", 0.0, 1.0, &bgColor.g, length-xInit, dim);
//    gui->addSlider("blue", 0.0, 1.0, &bgColor.b, length-xInit, dim);
    gui->addLabel("PARTICLES");
    gui->addSlider("red", 0.0, 1.0, &particleColor.r, length-xInit, dim);
    gui->addSlider("green", 0.0, 1.0, &particleColor.g, length-xInit, dim);
    gui->addSlider("blue", 0.0, 1.0, &particleColor.b, length-xInit, dim);
    gui->addSlider("alpha", 0.0, 1.0, &particleColor.a, length-xInit, dim);

    gui->setDrawBack(false);
    
    gui->setVisible(false);
    
//    ofAddListener(gui->newGUIEvent,this,&particleScene::guiEvent);
}




void particleScene::update(int width, int height){
//    
//    if (bGenerate) {
//        for (int i = 0; i < flow->features.size(); i++) {
//            if (flow->motion[i + flow->motion.size()/2].lengthSquared() > threshold) {
//                ofVec2f featMapped;
//                featMapped.x = ((flow->features[i].x - 160) * scale + 160 + x);
//                featMapped.y = ((flow->features[i].y - 120) * scale + 120 + y);
//                
//                ofVec2f featVel;
//                featVel.x = flow->motion[i + flow->motion.size()/2].x * scale;
//                featVel.y = flow->motion[i + flow->motion.size()/2].y * scale;
//                
//                particles.add(featMapped, featVel * velMult);
//                
//            }
//        }
//
//    }
//    else {
//        if (flow->motion.size() > 0) {
//            for (int i = 0; i < particles.particles.size(); i++) {
//                ofVec2f featVel;
//                featVel.x = flow->motion[i + flow->motion.size()/2].x * scale;
//                featVel.y = flow->motion[i + flow->motion.size()/2].y * scale;
//                
//                
//                if (bReverse) featVel *= -1;
//                particles.particles[i]->vel+= featVel * velMult;
//            }
//        }
//    }
//    
//    ofVec2f centroid(0,0);
//    for (int i = 0; i < flow->features.size(); i++) {
//        centroid+=flow->features[i];
//    }
//    centroid/=flow->features.size();
//    
//    particles.update(centroid.x, centroid.y);
}

void particleScene::draw(float x, float y, float scale){
// moved here for convenience
    if (bGenerate) {
        for (int i = 0; i < flow->features.size(); i++) {
            if (flow->motion[i + flow->motion.size()/2].lengthSquared() > threshold) {
                ofVec2f featMapped;
                featMapped.x = ((flow->features[i].x - 160) * scale + 160 + x);
                featMapped.y = ((flow->features[i].y - 120) * scale + 120 + y);
                
                ofVec2f featVel;
                featVel.x = flow->motion[i + flow->motion.size()/2].x * scale;
                featVel.y = flow->motion[i + flow->motion.size()/2].y * scale;
                
                particles.add(featMapped, featVel * velMult);
                
            }
        }
        
    }
    else {
        if (flow->motion.size() > 0) {
            for (int i = 0; i < particles.particles.size(); i++) {
                ofVec2f featVel;
                featVel.x = flow->motion[i + flow->motion.size()/2].x * scale;
                featVel.y = flow->motion[i + flow->motion.size()/2].y * scale;
                
                
                if (bReverse) featVel *= -1;
                particles.particles[i]->vel+= featVel * velMult;
            }
        }
    }
    
    ofVec2f centroid(0,0);
    for (int i = 0; i < flow->features.size(); i++) {
        centroid+=flow->features[i];
    }
    centroid/=flow->features.size();
    
    particles.update(centroid.x, centroid.y);
    particles.draw(particleColor);
///draw
    
//    
//    ofPushMatrix();
//    ofTranslate(x, y);
//    ofTranslate(160, 120);
//    ofScale(scale, scale);
//    ofTranslate(-160, -120);
  
//    ofPopMatrix();

//    if (bDebug) {
//        for (int i = 0; i < particles.particles.size(); i++) {
//            cout << "particle[" << i << "].pos = " << particles.particles[i]->pos << endl;
//        }
//        bDebug = false;
//    }
//    bDebug = false;
}