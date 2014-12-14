//------------------------------------------------------------------------------
// CpuInfo.h
//    
//   This file contains the class CpuInfo. It basically determines all 
//   pertinent information for the CPU. It currently gets in-depth 
//   information for both Intel and AMD CPUs. 
//   Known Problems:
//   1) algorithm for determining CPU speed is not great; it will be updated
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef CPUINFO_H_
#define CPUINFO_H_

#include "SysInfoClasses.h"
#include "CpuStats.h"
#include <windows.h>
#include <string>

// this gives us multimedia timer functions -- used for CPU benchmarking
#pragma comment(lib, "winmm.lib")

namespace DSystemInfo {
	//---------------------------------------------------------------------------
	// CPU information defines -- VERY x86-dependent stuff going on here
	//
	#define CPU_ID __asm _emit 0xf __asm _emit 0xa2
	#define BRANDTABLESIZE 4

	class CpuInfo
	{
	public:        // object creation/destruction
	CpuInfo(const bool fCpuSpeed = true);
	CpuInfo(const CpuInfo& source);
	CpuInfo& operator=(const CpuInfo& right);
	virtual ~CpuInfo();

	public:        
	// operations
	virtual void determineCpuInfo(void);
	virtual void determineCpuSpeed(void);

	// CPU attributes
	virtual inline std::string getCpuIdentification(void) const;
	virtual inline std::string getCpuSpeed(void) const;
	virtual inline void getCpuSpeed(DWORD* pdwCpuSpeed) const;
	virtual inline std::string getNumProcessors(void) const;
	virtual inline void getNumProcessors(DWORD* pdwNumProcessors) const;
	virtual inline DWORD getFamily(void) const;
	virtual inline DWORD getModel(void) const;
	virtual inline DWORD getStepping(void) const;
	virtual inline CpuStats getStats(void) const;

	protected:     // protected members
	virtual void assign(const CpuInfo& source);

	virtual bool determineCpuId(void);
	virtual void determineCpuStats(WORD wProcessorArchitecture);
	   
	// this function determines which of the below can be called
	virtual DWORD determineHighestCpuId(void);
	virtual void determineCpuIdentification(void); // eax = 1
	virtual void determineIntelCacheInfo(void);    // eax = 2
	virtual void determineOldIntelName(void); 
	virtual void determineCeleronAndXeon(DWORD dwRegisterCache, bool* pfIsCeleron, 
											bool* pfIsXeon, bool fIsEax = false);

	// this function determines which of the below can be called
	virtual DWORD determineLargestExtendedFeature(void);
	virtual void determineExtendedFeature(void);   // eax = 0x80000001
	virtual void determineNameString(void);        // eax = 0x80000002 - 0x80000004
	virtual void determineAmdL1CacheInfo(void);    // eax = 0x80000005
	virtual void determineAmdL2CacheInfo(void);    // eax = 0x80000006

	// cpu speed
	virtual DWORD calculateCpuSpeed(void) const;
	virtual DWORD determineTimeStamp(void) const;

	private:       // attributes
	CpuStats    m_cpuStats;
	std::string m_strCpuSpeed;
	std::string m_strNumProcessors;
	};

	inline std::string CpuInfo::getCpuIdentification(void) const
	{
	return (m_cpuStats.getName());
	}

	inline void CpuInfo::getCpuSpeed(DWORD* pdwCpuSpeed) const
	{
	*pdwCpuSpeed = atoi(m_strCpuSpeed.c_str());
	}

	inline void CpuInfo::getNumProcessors(DWORD* pdwNumProcessors) const
	{
	*pdwNumProcessors = atoi(m_strNumProcessors.c_str());
	}

	inline std::string CpuInfo::getNumProcessors(void) const
	{
	return (m_strNumProcessors);
	}

	inline std::string CpuInfo::getCpuSpeed(void) const
	{
	return (m_strCpuSpeed);
	}

	inline DWORD CpuInfo::getFamily(void) const
	{
	return (m_cpuStats.getFamily());
	}

	inline DWORD CpuInfo::getModel(void) const
	{
	return (m_cpuStats.getModel());
	}

	inline DWORD CpuInfo::getStepping(void) const
	{
	return (m_cpuStats.getStepping());
	}

	inline CpuStats CpuInfo::getStats(void) const
	{
	return (m_cpuStats);
	}

}

#endif