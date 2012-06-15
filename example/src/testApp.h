#pragma once

#include "ofMain.h"
#include "ofxNativeGui.h"

class testApp : public ofBaseApp, public CMainWindowEventInterface {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        // stuff we pass in
        vector < string > comboVals;
    
        ofxNativeGui GUI;
    
    
//        // these are callbacks from the CCocoaMainWindow
//        void ButtonPushed(WIDGET_HANDLE widget);
//        void ValueChanged(WIDGET_HANDLE widget, int value);
//		
        // results for the callback
        int comboVal;
        float sliderVal;
        string enteredText;
    
    
    
};
