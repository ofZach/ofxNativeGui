
#include "ofMain.h"


// this could be done with OF events. 


#include "CocoaMainWindow.h"
#include "MainWindowEventInterface.h"




class nativeWidget {
    
public: 

    int             type;                // which type of widget are we!
    WIDGET_HANDLE   widget;
    ofRectangle     bounds;
    string          name;
    void *          variablePtr;        // bool, int, string, etc based on the type.
    
};

class nativeSliderWidget : public nativeWidget {
    public: 
    // for sliders, we need some of this around, as these sliders are int only
    float startVal;
    float minVal;
    float maxVal;
};

class ofxNativeGuiEventInterface {
public: 
    virtual void guiEvent(nativeWidget & widget);
};


class ofxNativeGui : public CMainWindowEventInterface {
    
public: 
    
    void setup(ofRectangle bounds, string name){
        interface = NULL;
        CMW = new CCocoaMainWindow(bounds.x, bounds.y, bounds.width, bounds.height, name.c_str());
        CMW->SetEventCallback(this);
    }
    
    void update();      // for labels that are associated with sliders that need updating. 
    
    ofxNativeGuiEventInterface * interface;
    
    void setEventInterface(ofxNativeGuiEventInterface * _interface) {
        interface = _interface;
    }
    
    void show();
    void hide();
    
    void addSlider(string name, ofRectangle bounds, float minValue, float maxValue, float startValue, float * variable = NULL);              
    void addButton(string name, string buttonText, ofRectangle bounds, bool * variable = NULL);
    void addTextEntry(string name, string defaultName, ofRectangle bounds, string * variable = NULL);
    void addComboBox(string name, ofRectangle bounds, vector < string > options, int * selectedItem = NULL );
    
    void ButtonPushed(WIDGET_HANDLE widget);
    void ValueChanged(WIDGET_HANDLE widget, int value);
    
    vector < nativeWidget * > widgets;    
    
    CCocoaMainWindow * CMW;
    
};