//
//  smokeScene.h
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-03.
//
//

#pragma once

#include "scene.h"

#include "MSAFluid.h"
#include "FluidParticleSystem.h"

class smokeScene : public scene {
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
    
    //----------------------------
    
    void fadeToColor(float r, float g, float b, float speed);
	void addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce);
    float                   colorMult;
    float                   velocityMult;
	int                     fluidCellsX;
	bool                    resizeFluid;
	bool                    drawFluid;
	bool                    drawParticles;

    msa::fluid::Solver      fluidSolver;
	msa::fluid::DrawerGl	fluidDrawer;
	
	fluidParticleSystem     particleSystem;
	
	ofVec2f                 pMouse;
    
    float threshold;
    
//    ofFloatColor bgColor;
    ofFloatColor fluidColor;
    ofFloatColor particleColor;

    
};
