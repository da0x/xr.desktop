//------------------------------------------------------------------------------
// OSInfo.cpp
//    
//   This file contains OSInfo. Basically, this class just determines 
//   which windows operating system is running. It also figures out service 
//   pack or build information. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#include "OSInfo.h"
#include <windows.h>

#include <sstream>
using namespace std;


namespace DSystemInfo {

	OSInfo::OSInfo()
	{
	determineOSInfo();
	}

	OSInfo::OSInfo(const OSInfo& source)
	{
	assign(source);
	}

	OSInfo& OSInfo::operator=(const OSInfo& right)
	{
	if (this != &right)
	{
		assign(right);
	}

	return (*this);
	}

	OSInfo::~OSInfo()
	{
	// nothing to do yet
	}

	void OSInfo::assign(const OSInfo& source)
	{
	m_eVersion = source.m_eVersion;
	m_strDescription = source.m_strDescription;
	}

	void OSInfo::determineOSInfo(void)
	{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	//--------------------------------------------------------------------
	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.
	//
	memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		if (!GetVersionEx((OSVERSIONINFO*) &osvi)) 
		{
			return;
		}
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:

		//------------------------------------------
		// Test for the product.
		//
		if (osvi.dwMajorVersion <= 4)
		{
			m_strDescription += "Microsoft Windows NT ";
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
		{
			m_strDescription += "Microsoft Windows 2000 ";
			m_eVersion = WIN2000;
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
		{
			m_strDescription += "Microsoft Windows XP ";
			m_eVersion = WINXP;
		}

		//-------------------------------------------------------------
		// get version, service pack (if any), and build number.
		//
		if (osvi.dwMajorVersion <= 4)
		{
			ostringstream os;
			os << "version " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion
				<< " " << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) 
				<< ") " << ends;
			m_strDescription += os.str();

			if (osvi.dwMajorVersion == 4)
			{
				m_eVersion = WINNT40;
			}
			else if ((osvi.dwMajorVersion == 3) && (osvi.dwMinorVersion == 51))
			{
				m_eVersion = WINNT351;
			}
		}
		else
		{ 
			ostringstream os;
			os << osvi.szCSDVersion << " (Build " 
				<< (osvi.dwBuildNumber & 0xFFFF) << ")" << ends;
			m_strDescription += os.str();
		}
		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			m_strDescription += "Microsoft Windows 95 ";
			if (osvi.szCSDVersion[1] == 'C')
			{
			m_strDescription += "OSR2 ";
			}
			else
			{
			m_strDescription += osvi.szCSDVersion[1];
			m_strDescription += " ";
			}

			m_eVersion = WIN95;
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			m_strDescription += "Microsoft Windows 98 ";
			if (osvi.szCSDVersion[1] == 'A')
			{
			m_strDescription += "SE ";
			}

			m_eVersion = WIN98;
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			m_strDescription += "Microsoft Windows Me ";
			m_eVersion = WINME;
		} 
		break;

	case VER_PLATFORM_WIN32s:
		m_strDescription += "Microsoft Win32s ";
		m_eVersion = WIN32S;
		break;
	}

	return; 
	}
}