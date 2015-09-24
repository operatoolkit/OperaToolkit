//
//  particleScene.h
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-03.
//
//

#pragma once

#include "scene.h"
#include "particleSystem.h"

class particleScene : public scene {
public:
    void setup(openNIManager * _oni, flowManager * _flow );
    void update(int width, int height);
    void draw(float x, float y, float scale);
    
    openNIManager * oni;
    flowManager * flow;
    
    ofxUICanvas * gui;
    void toggleGUI(){ gui->toggleVisible(); }
    void hideGUI(){ gui->setVisible(false); }
    void saveGUI( string filename ) { gui->saveSettings(filename); }
    void loadGUI( string filename ) { gui->loadSettings(filename); }
    
    //----------------------------------
    particleSystem particles;
    
    bool bDebug, bGenerate, bReverse;
    
    float threshold, velMult;
    
    int alpha;

    ofFloatColor particleColor;
};
