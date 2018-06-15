# cvision
CVision: The open source C++ library for graphical user interfaces
(c) Damian Tran 2017-2018
See included LICENCE file for GPL 3.0 terms and conditions of use and redistribution

CVision aims to form the basis of a free, simplified API in C++ for front-end development.  A program based on CVision initiates the main APP class from the main() program entry point, initializing a runtime loop that updates every widget under the app's control in a hierarchical manner.

APP -> Panels -> Element Groups -> Elements -> SFML Primitives

Every CVision element class derives from the base visElement, which is recognized globally by the base CVViewPanel class.  Thus, a user-defined visElement child class requires the override of only two pure virtual functions:

virtual bool draw();
virtual bool update(CVEvent, 2D float vector);

This library requires your GCC compiler version to be at or above 6.3.0, and an installation of the SFML (Simple and Fast Multimedia Library) of at least version 2.4.2 (Found at www.sfml-dev.org)

The supporting documentation for this library is a work in progress, as is the library itself (as of June 2018).  Please check back on this repo on a monthly basis for increasingly up-to-date coverage, more extensive library capabilities, and more efficient optimizations.
