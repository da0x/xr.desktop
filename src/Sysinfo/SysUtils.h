//------------------------------------------------------------------------------
// SysUtils.h
//    
//   This file contains SysUtils. Use this class just like a namespace 
//   for now. This class cannot be instantiated; it exists only to provide 
//   useful functions for other classes. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef SYSUTILS_H_
#define SYSUTILS_H_

#include "SysInfoClasses.h"
#include "windows.h"
#include <string>


namespace DSystemInfo {

	class SysUtils
	{
	private:
	SysUtils();
	virtual ~SysUtils();
	SysUtils(const SysUtils& source);
	SysUtils& operator=(const SysUtils& right);

	public:
	static std::string ByteToStr(const __int64& n64ByteCount = 0, const int& nPrecision = 2 );
	};

}

#endif