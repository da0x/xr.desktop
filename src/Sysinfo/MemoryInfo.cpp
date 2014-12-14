//------------------------------------------------------------------------------
// MemoryInfo.cpp
//    
//   This file contains MemoryInfo, which is basically just determines 
//   the system memory information and then stortes the results. 
//   This class COULD provide member functions that would return the
//   number of bytes instead of strings. These would probably be overloaded
//   functions that take pointer arguments.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#include "MemoryInfo.h"
#include "SysUtils.h"

#include <sstream>
using namespace std;


namespace DSystemInfo {

	MemoryInfo::MemoryInfo()
	{
	determineMemoryInfo();
	}

	MemoryInfo::MemoryInfo(const MemoryInfo& source)
	{
	assign(source);
	}

	MemoryInfo& MemoryInfo::operator=(const MemoryInfo& right)
	{
	if (this != &right)
	{
		assign(right);
	}

	return (*this);
	}

	MemoryInfo::~MemoryInfo()
	{
	// nothing to do yet
	}

	void MemoryInfo::determineMemoryInfo(void)
	{
	GlobalMemoryStatus(&m_stMemStatus);

	m_strTotalRam = SysUtils::ByteToStr(m_stMemStatus.dwTotalPhys + 655360, 0);
	m_strAvailRam = SysUtils::ByteToStr(m_stMemStatus.dwAvailPhys + 655360);
	m_strTotalPageFile = SysUtils::ByteToStr(m_stMemStatus.dwTotalPageFile + 655360);
	m_strAvailPageFile = SysUtils::ByteToStr(m_stMemStatus.dwAvailPageFile + 655360);
	m_strTotalVirtual = SysUtils::ByteToStr(m_stMemStatus.dwTotalVirtual + 655360);
	m_strAvailVirtual = SysUtils::ByteToStr(m_stMemStatus.dwAvailVirtual + 655360);
	}

	void MemoryInfo::assign(const MemoryInfo& source)
	{
	m_strTotalRam = source.m_strTotalRam;
	m_strAvailRam = source.m_strAvailRam;
	m_strTotalPageFile = source.m_strTotalPageFile;
	m_strAvailPageFile = source.m_strAvailPageFile;
	m_strTotalVirtual = source.m_strTotalVirtual;
	m_strAvailVirtual = source.m_strAvailVirtual;
	m_stMemStatus = source.m_stMemStatus;
	}
}