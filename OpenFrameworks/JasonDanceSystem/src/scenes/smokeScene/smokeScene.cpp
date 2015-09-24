//
//  smokeScene.cpp
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-03.
//
//

#include "smokeScene.h"


//--------------------------------------------------------------
void smokeScene::setup(openNIManager * _oni, flowManager * _flow ) {
    oni = _oni;
    flow = _flow;
	
	// setup fluid stuff
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5).setVisc(0.00015).setColorDiffusion(0);
	fluidDrawer.setup(&fluidSolver);
	
	fluidCellsX			= 150;
	
	drawFluid			= true;
	drawParticles		= true;
    
    colorMult           = 50;
    velocityMult        = 34;
	
    threshold           = 0;
    
    
//    bgColor.set(13, 13, 26);
    fluidColor.set(255,0,0);
    particleColor.set(0,255,0);
    
	ofSetFrameRate(60);
	ofBackground(0, 0, 0);

	
    
    //init gui dims
    float dim = 16;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    //gui!
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 60);
    
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("threshold", 0, 100, &threshold);
	gui->addIntSlider("fluidCellsX", 20, 400, &fluidCellsX);
	gui->addButton("resizeFluid", &resizeFluid);
    gui->addSlider("colorMult", 0, 100, &colorMult);
    gui->addSlider("velocityMult", 0, 100, &velocityMult);
	gui->addSlider("fs.viscocity", 0.0, 0.01, &fluidSolver.viscocity);
	gui->addSlider("fs.colorDiffusion", 0.0, 0.0003, &fluidSolver.colorDiffusion);
	gui->addSlider("fs.fadeSpeed", 0.0, 0.1, &fluidSolver.fadeSpeed);
	gui->addIntSlider("fs.solverIterations", 1, 50, &fluidSolver.solverIterations);
	gui->addSlider("fs.deltaT", 0.1, 5, &fluidSolver.deltaT);
	gui->addToggle("fs.doRGB", &fluidSolver.doRGB);
	gui->addToggle("fs.doVorticityConfinement", &fluidSolver.doVorticityConfinement);
	gui->addToggle("drawFluid", &drawFluid);
	gui->addToggle("drawParticles", &drawParticles);
	gui->addToggle("fs.wrapX", &fluidSolver.wrap_x);
	gui->addToggle("fs.wrapY", &fluidSolver.wrap_y);
    gui->addSpacer(length-xInit, 1);
//    gui->addLabel("BACKGROUND");
//    gui->addSlider("red", 0.0, 1.0, &bgColor.r, length-xInit, dim);
//    gui->addSlider("green", 0.0, 1.0, &bgColor.g, length-xInit, dim);
//    gui->addSlider("blue", 0.0, 1.0, &bgColor.b, length-xInit, dim);
    gui->addLabel("FLUID");
    gui->addSlider("red", 0.0, 1.0, &fluidColor.r, length-xInit, dim);
    gui->addSlider("green", 0.0, 1.0, &fluidColor.g, length-xInit, dim);
    gui->addSlider("blue", 0.0, 1.0, &fluidColor.b, length-xInit, dim);
    gui->addLabel("PARTICLES");
    gui->addSlider("red", 0.0, 1.0, &particleColor.r, length-xInit, dim);
    gui->addSlider("green", 0.0, 1.0, &particleColor.g, length-xInit, dim);
    gui->addSlider("blue", 0.0, 1.0, &particleColor.b, length-xInit, dim);
    
    gui->setDrawBack(false);
    gui->setVisible(false);

	
//	windowResized(ofGetWidth(), ofGetHeight());		// force this at start (cos I don't think it is called)
	pMouse = msa::getWindowCenter();
	resizeFluid			= true;
	

}


void smokeScene::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}


// add force and dye to fluid, and create particles
void smokeScene::addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce) {
    float speed = vel.x * vel.x  + vel.y * vel.y * msa::getWindowAspectRatio() * msa::getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if(speed > 0) {
		pos.x = ofClamp(pos.x, 0.0f, 1.0f);
		pos.y = ofClamp(pos.y, 0.0f, 1.0f);
		
        int index = fluidSolver.getIndexForPos(pos);
		
		if(addColor) {
            //			Color drawColor(CM_HSV, (getElapsedFrames() % 360) / 360.0f, 1, 1);
//			ofColor drawColor;
//			drawColor.setHsb((ofGetFrameNum() % 255), 255, 255);
			
			fluidSolver.addColorAtIndex(index, fluidColor * colorMult);
			
			if(drawParticles)
				particleSystem.addParticles(pos * ofVec2f(ofGetWindowSize()), 10);
		}
		
		if(addForce)
			fluidSolver.addForceAtIndex(index, vel * velocityMult);
		
    }
}


void smokeScene::update(int width, int height){
	if(resizeFluid) 	{
		fluidSolver.setSize(fluidCellsX, fluidCellsX / msa::getWindowAspectRatio());
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}
    

}

void smokeScene::draw(float x, float y, float scale){
    
    //moved this from update as the easiest wy to get mapping vars
    for (int i = 0; i < flow->current.size(); i++) {
        if (flow->motion[i + flow->motion.size()/2].lengthSquared() > threshold) {
            
            ofVec2f featNorm;
            featNorm.x = ((flow->features[i].x - 160) * scale + 160 + x)  / ofGetWidth();
            featNorm.y = ((flow->features[i].y - 120) * scale + 120 + y)  / ofGetHeight();
            
            ofVec2f featVel;
            featVel.x = flow->motion[i + flow->motion.size()/2].x * scale / ofGetWidth();
            featVel.y = flow->motion[i + flow->motion.size()/2].y * scale / ofGetHeight();
            
            addToFluid(featNorm, featVel, true, true);
        }
    }
    
	fluidSolver.update();
    //end update
    
    
    ofPushStyle();
    ofSetBackgroundAuto(false);
//    ofEnableAlphaBlending();
    
    if(drawFluid) {
        ofClear(0);
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
	} else {
        //		if(ofGetFrameNum()%5==0)
        fadeToColor(0,0,0, 0.01);
	}
	if(drawParticles) {
        ofSetColor(particleColor);
		particleSystem.updateAndDraw(fluidSolver, ofGetWindowSize(), drawFluid, particleColor);
    }
	
    ofSetBackgroundAuto(true);
    ofPopStyle();
}


