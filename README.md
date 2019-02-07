# CVision
CVision: The open source C++ library for cascading-style graphical user interfaces
(c) Damian Tran 2017-2018

## ABOUT

CVision aims to form the basis of a free, simplified API in C++ for front-end development.  A program based on CVision initiates the main APP class from the main() program entry point, initializing a runtime loop for each viewport that updates every widget under the app's control in a hierarchical manner.

APP -> Views -> Panels -> Element Groups -> Elements -> SFML Primitives

Base elements all inherit abilities including:
  * Mouse capture
  * Highlight
  * Masking/Clipping
  * Drag and drop
  * Throw and bounce physics
  * Programmed animation
  
Panels are specialized elements that can transduce update and draw events to child elements of any type (including other panels).

Other specialized elements include:
 * Text boxes
 * Buttons
 * Scroll bar widgets
 * Plots (full interactivity)
 * Spreadsheets (work in-progress)
 
Every CVision element class derives from the base visElement, which is recognized globally by the base CVViewPanel class.  Thus, a user-defined visElement child class requires the override of only two pure virtual functions:

`virtual bool draw();
virtual bool update(CVEvent, 2D float vector);`

These functions will be automatically activated as long as the widgets are connected in a circuit to the main app by attaching elements to panels, and panels to views.

Fonts and images are managed upon instantiation of an APP class (or custom derived class), and can be locally accessed by all child elements.  Each can be accessed from a created element by calling `getAppTexture("textureName")` or `getAppFont("fontName")`, which simplifies the process of managing media files.

## INSTALL

This library requires your GCC compiler version to be at or above 7.3.0.  The precompiled library files in the /lib directory will work immediately for Windows.  Precompiled versions for macOS and Linux will be soon to follow!  Otherwise, you're always welcome to build the library from source using the included headers and source files.

The supporting documentation for this library is a work in progress, as is the library itself (as of June 2018).  Please check back on this repo on a monthly basis for increasingly up-to-date coverage, more extensive library capabilities, and more efficient optimizations.
