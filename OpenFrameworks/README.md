# OpenFrameworks apps

## Installation

This parent folder must be copied to the "apps" folder of your OpenFrameworks root folder, i.e. from 

    mv Your_Opera_Toolkit_Folder/OpenFrameworks Your_OpenFrameworks_Folder/apps/

This is a complete list of all addons required to run the applications here.

 - [ofxMultiKinectV2](https://github.com/hanasaan/ofxMultiKinectV2)
 - [ofxTurboJpeg](https://github.com/armadillu/ofxTurboJpeg)
 - [ofxSecondWindow](https://github.com/genekogan/ofxSecondWindow)


## Application descriptions

#### ThousandArms

This app draws records and draws multiple (many!) copies of a performer onto the wall behind them.  Requires [Kinect V2](http://www.amazon.com/Microsoft-Kinect-for-Windows-V2/dp/B00KZIVEXO)

#### ThousandArmsV1

This app draws records and draws multiple (many!) copies of a performer onto the wall behind them.  Requires [original Kinect](http://www.amazon.com/Kinect-Sensor-Adventures-Xbox-360/dp/B002BSA298/ref=sr_1_2?s=videogames&ie=UTF8&qid=1442012505&sr=1-2&keywords=kinect)


#### KinectProjector_Calibration

Application for calibrating Kinect and projector together for use in the following apps.

#### KinectProjector_Ribbons

Noisy ribbons tracing the contours around performers bodies. 

#### KinectProjector_BgLines

Tracing performers bodies on a wall with black and white lines around their contours. This is currently not working yet but will be soon.

#### MirrorMesh

Deforming your mesh -- to-do: make audioreactive.

#### JasonDanceSystem

Jason Levine's kinect-tracked visuals -- to-do: hook this up to ofxKinectProjectorToolkit.

#### Doppelganger

Interact with your own shadow -- a copy of yourself which follows you around. This is currently buggy -- needs to be fixed.


### To-do:

Apps to add:
 - Interacting with own shadow + mirror image
 - Fix BgLines
 - Add more KinectProjector apps : fluids, falling objects, and more.
 - Instructions
 - Release binaries for people to run without source code
 - Instructions for how to get the openni data files necessary to run in any ofxOpenNi applications.

Tasks:
 - release runnable applications
 - pictures/thumbnails
 - instructions for each app
