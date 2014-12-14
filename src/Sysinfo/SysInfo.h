//------------------------------------------------------------------------------
// SysInfo.h
//    
//   This is the SysInfo class. It gets different types of system 
//   information, including hard-drives, CPU information, total system 
//   RAM, etc.... 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef SYSINFO_H_
#define SYSINFO_H_

#include "SysInfoClasses.h"
#include "CpuInfo.h"
#include "OSInfo.h"
#include "MemoryInfo.h"
#include "SocketInfo.h"
#include "DriveInfo.h"
#include <windows.h>

#include <string>
#include <vector>

namespace DSystemInfo {

	class SysInfo
	{
	public:        // object creation/destruction
	SysInfo(const bool& fCpuSpeed = true,
			const unsigned int& unDriveTypes = HARDDRIVE | NETWORK,
			const int& nSockMajor = 2, const int& nSockMinor = 0);
	SysInfo(const SysInfo& source);
	SysInfo& operator=(const SysInfo& right);
	virtual ~SysInfo();

	public:        
	// find operations
	virtual void determineSysInfo(const bool& fCpuSpeed = true,
									const unsigned int& unDriveTypes = HARDDRIVE | NETWORK,
									const int& nSockMajor = 2, const int& nSockMinor = 0);

	// CPU information
	virtual inline std::string getCpuIdentification(void) const;
	virtual inline std::string getCpuVendor(void) const;
	virtual inline std::string getCpuSpeed(void) const;
	virtual inline void getCpuSpeed(DWORD* pdwCpuSpeed) const;
	virtual inline std::string getNumProcessors(void) const;
	virtual inline void getNumProcessors(DWORD* pdwNumProcessors) const;
	virtual inline int getCpuFamily(void) const;
	virtual inline int getCpuModel(void) const;
	virtual inline int getCpuStepping(void) const;
	virtual inline bool getFeature(const DWORD& dwFeatureMask) const;
	virtual inline bool getExtFeature(const DWORD& dwExtFeatureMask) const;

	// OS information
	virtual inline std::string getOSDescription(void) const;
	virtual inline OSInfo::e_OS getOSVersion(void) const;

	// memory information
	virtual inline std::string getTotalRam(void) const;
	virtual inline std::string getAvailRam(void) const;
	virtual inline std::string getTotalPageFile(void) const;
	virtual inline std::string getAvailPageFile(void) const;
	virtual inline std::string getTotalVirtual(void) const;
	virtual inline std::string getAvailVirtual(void) const;

	// socket information
	virtual inline std::string getSocketVersion(void) const;
	virtual inline std::string getHighestSocketVersion(void) const;
	virtual inline std::string getSocketDescription(void) const;
	virtual inline std::string getSocketSystemStatus(void) const;
	virtual inline std::string getSocketMax(void) const;
	virtual inline std::string getIPAddress(void) const;
	virtual inline std::string getDomainName(void) const;
	virtual inline std::string getSocketUdpMax(void) const;

	// drive information
	virtual inline std::vector<DriveStats> const * getDriveStats(void) const;

	protected:     // protected members
	virtual void assign(const SysInfo& source);

	private:       // attributes
	CpuInfo    m_cpuInfo;
	OSInfo     m_osInfo;
	MemoryInfo m_memoryInfo;
	SocketInfo m_socketInfo;
	DriveInfo  m_driveInfo;
	};

	inline std::string SysInfo::getCpuIdentification(void) const
	{
	return (m_cpuInfo.getCpuIdentification());
	}

	inline std::string SysInfo::getCpuVendor(void) const
	{
	return (m_cpuInfo.getStats().getVendorId());
	}

	inline std::string SysInfo::getCpuSpeed(void) const
	{
	return (m_cpuInfo.getCpuSpeed());
	}

	inline void SysInfo::getCpuSpeed(DWORD* pdwCpuSpeed) const
	{
	m_cpuInfo.getCpuSpeed(pdwCpuSpeed);
	}

	inline std::string SysInfo::getNumProcessors(void) const
	{
	return (m_cpuInfo.getNumProcessors());
	}

	inline void SysInfo::getNumProcessors(DWORD* pdwNumProcessors) const
	{
	m_cpuInfo.getNumProcessors(pdwNumProcessors);
	}

	inline int SysInfo::getCpuFamily(void) const
	{
	return (m_cpuInfo.getFamily());
	}

	inline int SysInfo::getCpuModel(void) const
	{
	return (m_cpuInfo.getModel());
	}

	inline int SysInfo::getCpuStepping(void) const
	{
	return (m_cpuInfo.getStepping());
	}

	inline bool SysInfo::getFeature(const DWORD& dwFeatureMask) const
	{
	return (m_cpuInfo.getStats().getFeature(dwFeatureMask));
	}

	inline bool SysInfo::getExtFeature(const DWORD& dwExtFeatureMask) const
	{
	return (m_cpuInfo.getStats().getExtendedFeature(dwExtFeatureMask));
	}

	inline std::string SysInfo::getOSDescription(void) const
	{
	return (m_osInfo.getDescription());
	}

	inline OSInfo::e_OS SysInfo::getOSVersion(void) const
	{
	return (m_osInfo.getVersion());
	}

	inline std::string SysInfo::getTotalRam(void) const
	{
	return (m_memoryInfo.getTotalRam());
	}

	inline std::string SysInfo::getAvailRam(void) const
	{
	return (m_memoryInfo.getAvailRam());
	}

	inline std::string SysInfo::getTotalPageFile(void) const
	{
	return (m_memoryInfo.getTotalPageFile());
	}

	inline std::string SysInfo::getAvailPageFile(void) const
	{
	return (m_memoryInfo.getAvailPageFile());
	}

	inline std::string SysInfo::getTotalVirtual(void) const
	{
	return (m_memoryInfo.getTotalVirtual());
	}

	inline std::string SysInfo::getAvailVirtual(void) const
	{
	return (m_memoryInfo.getAvailVirtual());
	}

	inline std::string SysInfo::getSocketVersion(void) const
	{
	return (m_socketInfo.getVersion());
	}

	inline std::string SysInfo::getHighestSocketVersion(void) const
	{
	return (m_socketInfo.getHighestVersion());
	}

	inline std::string SysInfo::getSocketDescription(void) const
	{
	return (m_socketInfo.getDescription());
	}

	inline std::string SysInfo::getSocketSystemStatus(void) const
	{
	return (m_socketInfo.getSystemStatus());
	}

	inline std::string SysInfo::getSocketMax(void) const
	{
	return (m_socketInfo.getMax());
	}

	inline std::string SysInfo::getIPAddress(void) const
	{
	return (m_socketInfo.getIPAddress());
	}

	inline std::string SysInfo::getDomainName(void) const
	{
	return (m_socketInfo.getDomainName());
	}

	inline std::string SysInfo::getSocketUdpMax(void) const
	{
	return (m_socketInfo.getUdpMax());
	}

	inline std::vector<DriveStats> const * SysInfo::getDriveStats(void) const
	{
	return (m_driveInfo.getDriveStats());
	}

}

#endif