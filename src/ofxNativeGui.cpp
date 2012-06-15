

#include "ofxNativeGui.h"





void ofxNativeGui::update(){  
    for (int i = 0; i < widgets.size(); i++){
        //widgets[i]->update();
    }
} 

void ofxNativeGui::show(){  CMW->Show(); }
void ofxNativeGui::hide(){  CMW->Hide(); }
    
void ofxNativeGui::addSlider(string name, ofRectangle bounds, float minValue, float maxValue, float startValue, float * variable){  
    
    float pct = ofMap(startValue, minValue, maxValue, 0,1);
    nativeWidget * guiElement = new nativeSliderWidget();;
    guiElement->widget = CMW->AddSlider(bounds.x, bounds.y, bounds.width, bounds.height,  0, 1000, 50, 1000*pct);
    guiElement->type = eSlider;
    guiElement->name = name;
    
    ((nativeSliderWidget*)guiElement)->minVal = minValue;
    ((nativeSliderWidget*)guiElement)->maxVal = maxValue;

    if (variable != NULL){
        guiElement->variablePtr = (void * )variable;
    } else {
        guiElement->variablePtr = (void * )new float();
    }
    widgets.push_back(guiElement);
}       

void ofxNativeGui::addButton(string name, string buttonText, ofRectangle bounds, bool * variable){ 
    nativeWidget * guiElement = new nativeWidget();
    guiElement->widget = CMW->AddButton(bounds.x, bounds.y, bounds.width, bounds.height, buttonText.c_str());
    guiElement->type = eButton;
    guiElement->name = name;
    if (variable != NULL){
        *variable = 0;                                  // for buttons, we'll just increase a count, up to user to 
                                                        // check for differences or just recv events. 
        guiElement->variablePtr = (void * )variable;
    }
    widgets.push_back(guiElement);
}
void ofxNativeGui::addTextEntry(string name, string defaultText, ofRectangle bounds, string * variable){ 
    nativeWidget * guiElement = new nativeWidget();
    guiElement->widget = CMW->AddTextEdit(bounds.x, bounds.y, bounds.width, bounds.height, defaultText.c_str());
    guiElement->type = eTextEdit;
    guiElement->name = name;
    if (variable != NULL){
        guiElement->variablePtr = (void * )variable;
    } else {
        
        
    }
    widgets.push_back(guiElement);
}

void ofxNativeGui::addComboBox(string name, ofRectangle bounds, vector < string > options, int * variable){   
   
    const char ** constArray;
    int size = options.size();
    constArray = new const char * [size];
    for (int i = 0; i < size; i++){
        constArray[i] = (const char * )options[i].c_str();
    }    
    nativeWidget * guiElement = new nativeWidget();
    guiElement->widget = CMW->AddComboBox(bounds.x, bounds.y, bounds.width, bounds.height, size, constArray, 0);
    guiElement->type = eComboBox;
    guiElement->name = name;
    if (variable != NULL){
        guiElement->variablePtr = (void * )variable;
    } else {
        guiElement->variablePtr = (void *) new int();
        
    }
    widgets.push_back(guiElement);
    
}
void ofxNativeGui::ButtonPushed(WIDGET_HANDLE widget){ 
   
    for (int i = 0; i < widgets.size(); i++){
        if (widget == widgets[i]->widget){
            if (widgets[i]->variablePtr != NULL){
                (*((int *)widgets[i]->variablePtr))++;
            }
        }
    }
    
    
    
}
void ofxNativeGui::ValueChanged(WIDGET_HANDLE widget, int value){  

    // we do two things here -- update the variables and trigger events.  only updating variables is hooked up
    
    for (int i = 0; i < widgets.size(); i++){
        if (widget == widgets[i]->widget){
        switch (widgets[i]->type){
            case eCheckBox:
            {
                if (widgets[i]->variablePtr != NULL){
                    (*((int *)widgets[i]->variablePtr)) = value;
                }

            }
                break;
            case eTextEdit: 
            {
                if (widgets[i]->variablePtr != NULL){
                    char text[1024]; // does this need to be longer?  hmm.....
                    memset(text, 0, 1024); // null it. 
                    CMW->GetText(widgets[i]->widget, text, 1024);
                    (*((string *)widgets[i]->variablePtr)) = string(text);
                }
                
            }/*m_event_callback->ValueChanged(w, -1);*/ break;
            case eSlider:
            {
                if (widgets[i]->variablePtr != NULL){
                    
                    float minVal = ((nativeSliderWidget*)widgets[i])->minVal;
                    float maxVal = ((nativeSliderWidget*)widgets[i])->maxVal;
                    float val = ofMap(value, 0, 1000, minVal, maxVal);
                    // do some calc here
                    (*((float *)widgets[i]->variablePtr)) = val;
                }
            }
            break;
            case eComboBox:
            {
                if (widgets[i]->variablePtr != NULL){
                    (*((int *)widgets[i]->variablePtr)) = value;
                }
            }
                break;
                
            default: break;
        }
        }
    }



}
    