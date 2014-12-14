//------------------------------------------------------------------------------
// OSInfo.h
//    
//   This file contains OSInfo. Basically, this class just determines 
//   which windows operating system is running. It also figures out service 
//   pack or build information. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef OSINFO_H_
#define OSINFO_H_

#include "SysInfoClasses.h"
#include <string>


namespace DSystemInfo {

    
	class OSInfo
	{
	public:        // object creation/destruction
	OSInfo();
	OSInfo(const OSInfo& source);
	OSInfo& operator=(const OSInfo& right);
	virtual ~OSInfo();

	public:
	enum e_OS { WIN95 = 0, WIN98, WINME, WIN32S, WINNT351, WINNT40, WIN2000, WINXP };

	public:        // attribute modification
	virtual inline e_OS getVersion(void) const;
	virtual inline std::string getDescription(void) const;

	protected:     // protected members
	virtual void assign(const OSInfo& source);
	virtual void determineOSInfo(void);

	private:       // attributes
	e_OS m_eVersion;
	std::string m_strDescription;
	};

	inline OSInfo::e_OS OSInfo::getVersion(void) const { return (m_eVersion); }
	inline std::string OSInfo::getDescription(void) const { return (m_strDescription); }
}

#endif