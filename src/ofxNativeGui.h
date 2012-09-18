
#include "ofMain.h"


// this could be done with OF events.


#ifdef TARGET_OSX
#include "CocoaMainWindow.h"
#endif

#ifdef TARGET_WIN32
#include "Win32MainWindow.h"
#endif

#ifdef TARGET_LINUX
#include "GTKMainWindow.h"
#include "GTKMainWindowWidget.h"
#endif



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

    WIDGET_HANDLE   widgetLabel;
};

class ofxNativeGuiEventInterface {
public:
    virtual void guiEvent(nativeWidget & widget) = 0;
};



class ofxNativeGui : public CMainWindowEventInterface {

public:

    void setup(ofRectangle bounds, string name){
        interface = NULL;


#ifdef TARGET_OSX
        CMW = new CCocoaMainWindow(bounds.x, bounds.y, bounds.width, bounds.height, name.c_str());
#endif

#ifdef TARGET_WIN32
        CMW = new Win32MainWindow(bounds.x, bounds.y, bounds.width, bounds.height, name.c_str());
#endif
#ifdef TARGET_LINUX
		gtk_init_check(NULL, 0);
		gtk_main_iteration();
        CMW = new CGTKMainWindow(bounds.x, bounds.y, bounds.width, bounds.height, name.c_str());
#endif

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

    CMainWindowInterface * CMW;

};
