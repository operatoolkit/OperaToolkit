//
//  particleSystem.cpp
//  2_particle_particle_manyAreSlow
//
//  Created by Jason Levine on 2013-10-25.
//
//

#include "particleSystem.h"

//--------------------------------------------------------------
bool particleSystem::sortOnX(const particle *a, const particle *b) {
    return (a->pos.x < b->pos.x);
}

//--------------------------------------------------------------
void particleSystem::setup(int _maxParticles, float _width, float _height, bool _wrap, bool _bounce) {
    for (int i = 0; i < 100; i++){
		particle * myParticle = new particle();
		myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
        
	}
    
    width = _width;
    height = _height;
    
    wrap = _wrap;
    bounce = _bounce;
    
    maxParticles = _maxParticles;
    
    alpha = 255;
    
    baseSize = 3;
    sizeMod = 0;
    
    tailLength = 5;
    
    drawCircles = true;
    drawLines = false;
//    attractionRadius = 300;
//    repulsionRadius = 5;
//    
//    attractionForce = 0.5;
//    repulsionForce = 0.5;
}

//--------------------------------------------------------------
void particleSystem::add(ofVec2f pos, ofVec2f vel) {
    particle * myParticle = new particle();
    myParticle->setInitialCondition(pos.x, pos.y, vel.x, vel.y);
    particles.push_back(myParticle);

}


//--------------------------------------------------------------
void particleSystem::update(int mouseX, int mouseY) {
    // on every frame
	// we reset the forces
	// add in any forces on the particle
	// perfom damping and
	// then update
    
    // get rid of any particles that have flown off the screen
    if (!bounce && !wrap) {
        for (int i = 0; i < particles.size(); i++){
            if ( particles[i]->pos.x > width || particles[i]->pos.x < 0 || particles[i]->pos.y > height || particles[i]->pos.y < 0 ) {
                particles.erase(particles.begin() + i);
            }
        }
    }
    // get rid of particles if there are too many
    while (particles.size() > maxParticles) {
        int index = ofRandom(particles.size()-1);
        particles.erase(particles.begin() + index);
    }
    
    // sort remaining particles
	ofSort(particles, particleSystem::sortOnX);
	for (int i = 0; i < particles.size(); i++){
        particles[i]->resetForce();
	}
	
	int count = 0;
	for (int i = 0; i < particles.size(); i++){
//        particles[i]->addRepulsionForce(mouseX, mouseY, 40, 1.01f);
        particles[i]->addAttractionForce(mouseX, mouseX, 1000, attractionForce);
    }
    
    for (int i = 0; i < particles.size(); i++){
		// go forward through the array until a neighbour is out of reach
        int j = i - 1 ;
        bool withinRadius = true;
        while( j > 0 && withinRadius ) {
            
            float distX = abs(particles[i]->pos.x - particles[j]->pos.x);
            if ( distX < repulsionRadius ) {
                float distY = abs(particles[i]->pos.y - particles[j]->pos.y);
                if ( distY < repulsionRadius ) {
                    particles[i]->addRepulsionForce(*(particles[j]), repulsionRadius, repulsionForce);
                    count++;
                }
                j--;
            }
            else {
                withinRadius = false;
            }
        }
    }
	
	for (int i = 0; i < particles.size(); i++){
		particles[i]->addDampingForce();
		particles[i]->update(tailLength);
        if ( wrap ) particles[i]->wrapAround(0, 0, width, height);
        if ( bounce ) particles[i]->bounceOffWalls(0, 0, width, height);
	}
}

//--------------------------------------------------------------
void particleSystem::draw(ofFloatColor col) {

//    ofBackground(0);
//    ofEnableAlphaBlending();
//    ofNoFill();
	for (int i = 0; i < particles.size(); i++){

//        ofPushStyle();
        ofSetColor(col);
        
        ofNoFill();
        if ( drawLines ) {
            ofSetLineWidth(baseSize + particles[i]->vel.lengthSquared() * sizeMod);
            ofBeginShape();
            for (int j = 0; j < particles[i]->posHistory.size(); j++) {
                ofVertex(particles[i]->posHistory[j].x, particles[i]->posHistory[j].y); //
            }
            ofEndShape();
        }
        

        col.a = particles[i]->alpha;
        ofSetColor(col);
        
        ofFill();
        if ( drawCircles ) ofCircle(particles[i]->pos.x, particles[i]->pos.y, baseSize + particles[i]->vel.lengthSquared() * sizeMod);
//        ofPopStyle();
	}
//    ofFill();
//    ofDisableAlphaBlending();
}


