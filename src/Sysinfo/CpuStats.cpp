//------------------------------------------------------------------------------
// CpuStats.cpp
//    
//   This file contains the CpuStats class. This class is basically a 
//   simple class to hold CPU information with member value accessors 
//   and mutators. It really is used internally by CpuInfo and most
//   clients probably won't be seeing it.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#include "CpuStats.h"
using namespace std;


namespace DSystemInfo {

	const string CpuStats::VENDOR_INTEL_STR = "GenuineIntel";
	const string CpuStats::VENDOR_AMD_STR = "AuthenticAMD";
	const string CpuStats::VENDOR_CYRIX_STR = "CyrixInstead";
	const string CpuStats::VENDOR_CENTAUR_STR = "CentaurHauls";

	CpuStats::CpuStats()
	{
	setKnowsCpuId(false);    
	setExtendedFeatures(0);
	setVendorId("Unknown");
	}

	CpuStats::CpuStats(const CpuStats& source)
	{
	assign(source);
	}

	CpuStats& CpuStats::operator=(const CpuStats& right)
	{
	if (this != &right)
	{
		assign(right);
	}

	return (*this);
	}

	CpuStats::~CpuStats()
	{
	// nothing to do yet
	}

	void CpuStats::setVendorId(const std::string& strVendorId) 
	{ 
	m_strVendorId = strVendorId; 

	if (strVendorId == VENDOR_INTEL_STR)
	{
		m_eVendor = VENDOR_INTEL;
	}
	else if (strVendorId == VENDOR_AMD_STR)
	{
		m_eVendor = VENDOR_AMD;
	}
	else if (strVendorId == VENDOR_CYRIX_STR)
	{
		m_eVendor = VENDOR_CYRIX;
	}
	else if (strVendorId == VENDOR_CENTAUR_STR)
	{
		m_eVendor = VENDOR_CENTAUR;
	}
	else
	{
		m_eVendor = VENDOR_UNKNOWN;
	}
	}

	void CpuStats::assign(const CpuStats& source)
	{
	setKnowsCpuId(source.getKnowsCpuId());    
	setVendorId(source.getVendorId()); // also sets m_eVendor
	setName(source.getName());
	setExtendedFeatures(source.getExtendedFeatures());
	m_cpuidStats = source.m_cpuidStats;
	m_cacheStats = source.m_cacheStats;
	m_cacheStats2 = source.m_cacheStats2;
	}
}