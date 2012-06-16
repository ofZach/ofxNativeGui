ofxNativeGui
============

a native gui for openframeworks -- it uses native gui code that's very simple / hackable found in IVT.  it wraps it a bit, and will be tested across all platforms (cocoa is in and working now). 

produces a secondary window, like this: 

http://i.imgur.com/86VjD.png

the other platforms are not hooked up, but my alterations of the IVT code are minimal (exposing a few enums) so it should be a piece of cake to hook up.  Some other things I do are make the int sliders float compatible, etc. 

To do: 

* need to add the propograting of events (for now, there's a system of attaching a variable to a gui element that works and the example shows this off)
* need hiding / showing of gui elements
* sliders need some numerial display of output
* getters and setters for gui objects (set value, get value, etc)
* hiding and showing of the main window is a bit funky.  Some code that lets you focus the gui would be good (it pops up behind, can get closed, etc)

