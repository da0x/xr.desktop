//------------------------------------------------------------------------------
// SysInfoClasses.h
//    
//   This file uses #pragma's and #define's that are shared between all 
//   SysInfo classes. In order to link statically, #define 
//   SYSINFO_LINKSTATIC in your project's settings. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef SYSINFOCLASSES_H_
#define SYSINFOCLASSES_H_

//--------------------------------------------------
// this gets rid of annoying 4251 warning about 
// std::string needing dll interface
// it also gets rid of DSys::sv_iScreenHeight0 -- BOOL to bool cast warnings
//
#pragma warning(disable : 4251 6400)

#endif