/** /////////////////////////////////////////////////////////////
//
//  CVision: the flexible cascading-style GUI library for C++
//
// //////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2019 Damian Tran
//
// DESCRIPTION:
//
// CVision is a graphical user interface (GUI) library that
// attempts to simplify and speed up the process of desktop
// app design.  CVision incorporates a cascading structure
// scheme that resembles the following:
//
// App -> View -> Panel -> Element -> Primitives/Sprites
//
// The subsequent connection of each "leaf" of the hierarchy
// automatically ensures that the element will be updated,
// drawn to the renderer, and otherwise disposed of at
// the program's termination.
//
// LEGAL:
//
// Modification and redistribution of CVision is freely
// permissible under any circumstances.  Attribution to the
// Author ("Damian Tran") is appreciated but not necessary.
//
// CVision is an open source library that is provided to you
// (the "User") AS IS, with no implied or explicit
// warranties.  By using CVision, you acknowledge and agree
// to this disclaimer.  Use of CVision in the Users's programs
// or as a part of a derivative library is performed at
// the User's OWN RISK.
//
// ACKNOWLEDGEMENTS:
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
/////////////////////////////////////////////////////////////  **/

#pragma once

#ifndef CVIS_HPP
#define CVIS_HPP

#include "cvision/lib.hpp"

#include "hyper/algorithm.hpp"

#include <SFML/Graphics.hpp>

#include "cvision/algorithm.hpp"
#include "cvision/anim.hpp"
#include "cvision/widgets.hpp"
#include "cvision/panel.hpp"
#include "cvision/type.hpp"
#include "cvision/console.hpp"
#include "cvision/word.hpp"
#include "cvision/plot.hpp"
#include "cvision/button.hpp"
#include "cvision/dataview.hpp"
#include "cvision/templates.hpp"
#include "cvision/templates/calendar.hpp"
#include "cvision/templates/sketch.hpp"
#include "cvision/table.hpp"

#endif // CVIS_HPP
