//
//  particleSystem.h
//  2_particle_particle_manyAreSlow
//
//  Created by Jason Levine on 2013-10-25.
//
//
#pragma once

#include "particle.h"

class particleSystem {
public:
    void setup(int _maxParticles, float _width, float _height, bool _wrap, bool _bounce);
    void update(int mouseX, int mouseY);
    void draw(ofFloatColor col);
    
    void add(ofVec2f pos, ofVec2f vel);
    
    static bool sortOnX(const particle *a, const particle *b);
    vector <particle *> particles;
	
    int attractionRadius, repulsionRadius;
    float attractionForce, repulsionForce;
    
    float width, height;
    int maxParticles;
    
    bool wrap, bounce;
    
    float baseSize;
    float sizeMod;
    
    int alpha;
    
    bool drawCircles;
    bool drawLines;
    
    int tailLength;
};
