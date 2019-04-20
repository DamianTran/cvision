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

//Auto-generated header for KYC font binaries

#pragma once

#ifndef KYC_FONT_REGISTRY
#define KYC_FONT_REGISTRY

#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>

namespace KYC_FONT{

	extern const size_t ArialNova_Bold_SIZE;
	extern const unsigned char ArialNova_Bold[69096];
	extern const size_t ArialNova_BoldItalic_SIZE;
	extern const unsigned char ArialNova_BoldItalic[70512];
	extern const size_t ArialNova_Italic_SIZE;
	extern const unsigned char ArialNova_Italic[71672];
	extern const size_t ArialNova_Light_SIZE;
	extern const unsigned char ArialNova_Light[70392];
	extern const size_t ArialNova_LightItalic_SIZE;
	extern const unsigned char ArialNova_LightItalic[71720];
	extern const size_t ArialNova_SIZE;
	extern const unsigned char ArialNova[69528];
	extern const size_t ArialNovaCond_Bold_SIZE;
	extern const unsigned char ArialNovaCond_Bold[68424];
	extern const size_t ArialNovaCond_BoldItalic_SIZE;
	extern const unsigned char ArialNovaCond_BoldItalic[71588];
	extern const size_t ArialNovaCond_Italic_SIZE;
	extern const unsigned char ArialNovaCond_Italic[71720];
	extern const size_t ArialNovaCond_Light_SIZE;
	extern const unsigned char ArialNovaCond_Light[68652];
	extern const size_t ArialNovaCond_LightItalic_SIZE;
	extern const unsigned char ArialNovaCond_LightItalic[71872];
	extern const size_t ArialNovaCond_SIZE;
	extern const unsigned char ArialNovaCond[67700];


};

const std::vector<std::string> FONT_IDS = {
		"ArialNova_Bold",
		"ArialNova_BoldItalic",
		"ArialNova_Italic",
		"ArialNova_Light",
		"ArialNova_LightItalic",
		"ArialNova",
		"ArialNovaCond_Bold",
		"ArialNovaCond_BoldItalic",
		"ArialNovaCond_Italic",
		"ArialNovaCond_Light",
		"ArialNovaCond_LightItalic",
		"ArialNovaCond",
};

const std::map<std::string, const unsigned char*> FONT_ID_REG = {
		{ "ArialNova_Bold", KYC_FONT::ArialNova_Bold },
		{ "ArialNova_BoldItalic", KYC_FONT::ArialNova_BoldItalic },
		{ "ArialNova_Italic", KYC_FONT::ArialNova_Italic },
		{ "ArialNova_Light", KYC_FONT::ArialNova_Light },
		{ "ArialNova_LightItalic", KYC_FONT::ArialNova_LightItalic },
		{ "ArialNova", KYC_FONT::ArialNova },
		{ "ArialNovaCond_Bold", KYC_FONT::ArialNovaCond_Bold },
		{ "ArialNovaCond_BoldItalic", KYC_FONT::ArialNovaCond_BoldItalic },
		{ "ArialNovaCond_Italic", KYC_FONT::ArialNovaCond_Italic },
		{ "ArialNovaCond_Light", KYC_FONT::ArialNovaCond_Light },
		{ "ArialNovaCond_LightItalic", KYC_FONT::ArialNovaCond_LightItalic },
		{ "ArialNovaCond", KYC_FONT::ArialNovaCond },
};

const std::map<std::string, size_t> FONT_SIZE_REG = {
		{ "ArialNova_Bold", KYC_FONT::ArialNova_Bold_SIZE },
		{ "ArialNova_BoldItalic", KYC_FONT::ArialNova_BoldItalic_SIZE },
		{ "ArialNova_Italic", KYC_FONT::ArialNova_Italic_SIZE },
		{ "ArialNova_Light", KYC_FONT::ArialNova_Light_SIZE },
		{ "ArialNova_LightItalic", KYC_FONT::ArialNova_LightItalic_SIZE },
		{ "ArialNova", KYC_FONT::ArialNova_SIZE },
		{ "ArialNovaCond_Bold", KYC_FONT::ArialNovaCond_Bold_SIZE },
		{ "ArialNovaCond_BoldItalic", KYC_FONT::ArialNovaCond_BoldItalic_SIZE },
		{ "ArialNovaCond_Italic", KYC_FONT::ArialNovaCond_Italic_SIZE },
		{ "ArialNovaCond_Light", KYC_FONT::ArialNovaCond_Light_SIZE },
		{ "ArialNovaCond_LightItalic", KYC_FONT::ArialNovaCond_LightItalic_SIZE },
		{ "ArialNovaCond", KYC_FONT::ArialNovaCond_SIZE },
};


#endif // KYC_FONT_REGISTRY