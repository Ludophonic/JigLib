//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file jiglibconfig.hpp 
//                     
//==============================================================
#ifndef JIGLIB_CONFIG_HPP
#define JIGLIB_CONFIG_HPP

//#define USING_DOUBLE

// disable MSVC specific warnings
#ifdef _MSC_VER
// long names in debug info
#pragma warning (disable : 4786)    
// massive boost if we disable exception handling but MS generates warnings
#pragma warning(disable: 4530)
#endif

#endif
