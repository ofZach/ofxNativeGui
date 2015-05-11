#pragma once

#include "ofMain.h"
#include "ofxNativeGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		// stuff we pass in when we are making the gui. 
		vector < string > comboVals;

		// the gui window. 
		ofxNativeGui GUI;

		// these variables are passed into the gui, and get altered as the gui is altered: 
		int comboVal;
		float sliderVal;
		string enteredText;

		void guiEvent(nativeWidget & widget);
};
