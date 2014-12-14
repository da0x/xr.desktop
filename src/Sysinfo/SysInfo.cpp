//------------------------------------------------------------------------------
// SysInfo.cpp
//    
//   This is the SysInfo class. It gets different types of system 
//   information, including hard-drives, CPU information, total system 
//   RAM, etc.... 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#include "SysInfo.h"
#include <sstream>
using namespace std;

namespace DSystemInfo {

	SysInfo::SysInfo(const bool& fCpuSpeed/* = true*/,
					const unsigned int& unDriveTypes/* = HARDDRIVE | NETWORK*/,
					const int& nSockMajor/* = 2*/, const int& nSockMinor/* = 0*/) 
					: m_cpuInfo(fCpuSpeed), 
					m_driveInfo(unDriveTypes), 
					m_socketInfo(nSockMajor, nSockMinor)
	{
	}

	SysInfo::SysInfo(const SysInfo& source)
	{
	assign(source);
	}

	SysInfo& SysInfo::operator=(const SysInfo& right)
	{
	if (this != &right)
	{
		assign(right);
	}

	return (*this);
	}

	SysInfo::~SysInfo()
	{
	// nothing to do yet
	}

	void SysInfo::determineSysInfo(const bool& fCpuSpeed/* = true */,
								const unsigned int& unDriveTypes/* = HARDDRIVE | NETWORK*/,
								const int& nSockMajor/* = 2*/, const int& nSockMinor/* = 0*/)
	{
	m_cpuInfo.determineCpuInfo();
	if (fCpuSpeed)
	{
		m_cpuInfo.determineCpuSpeed();
	}
	m_driveInfo.determineDriveInfo(unDriveTypes);
	m_socketInfo.determineSocketInfo(nSockMajor, nSockMinor);
	m_memoryInfo.determineMemoryInfo();
	}  

	void SysInfo::assign(const SysInfo& source)
	{
	m_cpuInfo = source.m_cpuInfo;
	m_osInfo = source.m_osInfo;
	m_memoryInfo = source.m_memoryInfo;
	m_socketInfo = source.m_socketInfo;
	m_driveInfo = source.m_driveInfo;
	}
}