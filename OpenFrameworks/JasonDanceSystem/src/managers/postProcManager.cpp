//
//  postProcManager.cpp
//  ETD_system
//
//  Created by Jason Levine on 2014-02-04.
//
//

#include "postProcManager.h"

void postProcManager::setup() {
    width = 1024;
    height = 768;
    ppWidth = ofNextPow2(width);
    ppHeight = ofNextPow2(height);
    
    
    ofFbo::Settings s;
    s.width = ppWidth;
    s.height = ppHeight;
    s.textureTarget = GL_TEXTURE_2D;
    s.useDepth = true;
    s.depthStencilInternalFormat = GL_DEPTH_COMPONENT24;
    s.depthStencilAsTexture = true;
    s.internalformat = GL_RGBA32F;
    
    fbo.allocate(s);
    
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    fadeAmt = 0.1;
    
    post.init(ppWidth, ppHeight);
    
    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<RimHighlightingPass>()->setEnabled(false);
    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<DofAltPass>()->setEnabled(false);
    post.createPass<ContrastPass>()->setEnabled(true);
    
    renderPasses = post.getPasses();
    post.setFlip(false);
}