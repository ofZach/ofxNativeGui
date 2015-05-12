#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);

	comboVals.push_back("one");
	comboVals.push_back("two");
	comboVals.push_back("three");

	sliderVal = 0;
	comboVal = 0; //"no combo selected yet";
	enteredText = "no entered text yet";
	// --------------- setup the gui window

	GUI.setup(ofRectangle(0, 0, 300, 300), "my sample gui");

	//GUI.setEventInterface(this);

	GUI.addComboBox("combo", ofRectangle(10, 50, 100, 30), comboVals, &comboVal);
	GUI.addSlider("slider", ofRectangle(10, 90, 180, 20), -2, 2, 0, &sliderVal);
	GUI.addTextEntry("textBox", "please enter text here", ofRectangle(10, 120, 180, 100), &enteredText);

	//  doing this in the setup() means the window pops behind.  I do it in update after 1 full frame, so it pops in front.
	//  GUI.Show();
}

//--------------------------------------------------------------
void ofApp::update(){
	GUI.update();
	if (ofGetFrameNum() == 2) GUI.show();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofDrawBitmapStringHighlight(comboVals[comboVal], ofPoint(400, 30));
	ofDrawBitmapStringHighlight(ofToString(sliderVal), ofPoint(400, 60));
	ofDrawBitmapStringHighlight(enteredText, ofPoint(400, 90));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::guiEvent(nativeWidget & widget){
	cout << "gui event " << widget.name << endl;
}