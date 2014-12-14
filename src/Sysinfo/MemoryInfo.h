//------------------------------------------------------------------------------
// MemoryInfo.h
//    
//   This file contains MemoryInfo, which is basically just determines 
//   the system memory information and then stortes the results. 
//   This class COULD provide member functions that would return the
//   number of bytes instead of strings. These would probably be overloaded
//   functions that take pointer arguments.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef MEMORYINFO_H_
#define MEMORYINFO_H_

#include "SysInfoClasses.h"
#include <windows.h>
#include <string>


namespace DSystemInfo {
    
	class MemoryInfo
	{
	public:        // object creation/destruction
	MemoryInfo();
	MemoryInfo(const MemoryInfo& source);
	MemoryInfo& operator=(const MemoryInfo& right);
	virtual ~MemoryInfo();

	public:        
	// operations
	void determineMemoryInfo(void);

	// attribute modification
	virtual inline std::string getTotalRam(void) const;
	virtual inline std::string getAvailRam(void) const;
	virtual inline std::string getTotalPageFile(void) const;
	virtual inline std::string getAvailPageFile(void) const;
	virtual inline std::string getTotalVirtual(void) const;
	virtual inline std::string getAvailVirtual(void) const;

	protected:     // protected members
	void assign(const MemoryInfo& source);

	private:       // attributes
	std::string m_strTotalRam;
	std::string m_strAvailRam;
	std::string m_strTotalPageFile;
	std::string m_strAvailPageFile;
	std::string m_strTotalVirtual;
	std::string m_strAvailVirtual;
	MEMORYSTATUS m_stMemStatus;
	};

	inline std::string MemoryInfo::getTotalRam(void) const { return (m_strTotalRam); }
	inline std::string MemoryInfo::getAvailRam(void) const { return (m_strAvailRam); }
	inline std::string MemoryInfo::getTotalPageFile(void) const { return (m_strTotalPageFile); }
	inline std::string MemoryInfo::getAvailPageFile(void) const { return (m_strAvailPageFile); }
	inline std::string MemoryInfo::getTotalVirtual(void) const { return (m_strTotalVirtual); }
	inline std::string MemoryInfo::getAvailVirtual(void) const { return (m_strAvailVirtual); }

}

#endif