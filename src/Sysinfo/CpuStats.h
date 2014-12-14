//------------------------------------------------------------------------------
// CpuStats.h
//    
//   This file contains the CpuStats class. This class is basically a 
//   simple class to hold CPU information with member value accessors 
//   and mutators. It really is used internally by CpuInfo and most
//   clients probably won't be seeing it.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef CPUSTATS_H_
#define CPUSTATS_H_

#include "CpuFeatures.h"
#include "SysInfoClasses.h"
#include "RegisterValues.h"

#include <string>
#include <windows.h>

namespace DSystemInfo {

	//-------------------------------------------------------------------
	// This is the CpuStats class. It holds the different types of 
	// information from Intel CPUs that are relevant to the CpuInfo class
	//
	class CpuStats
	{
	public:        // object creation/destruction
	CpuStats();
	CpuStats(const CpuStats& source);
	CpuStats& operator=(const CpuStats& right);
	virtual ~CpuStats();

	public:
	// this enum returns a simple value to test against for CPU vendor
	// it isn't explicitly set in this class; when setVendorId is called,
	// the class sets this enumeration member variable
	enum e_CPUVENDORS { VENDOR_UNKNOWN = 0, VENDOR_INTEL, VENDOR_AMD, VENDOR_CYRIX, VENDOR_CENTAUR };

	friend class CpuInfo;

	public:        
	// cpu identification
	virtual inline void setKnowsCpuId(const bool& fKnowsCpuId);
	virtual inline bool getKnowsCpuId(void) const;

	virtual inline DWORD getSignature(void) const;
	virtual inline int getFamily(void) const;   // implicitly set by setSignature()
	virtual inline int getModel(void) const;    // implicitly set by setSignature()
	virtual inline int getStepping(void) const; // implicitly set by setSignature()
	virtual inline std::string getName(void) const;

	// vendor identification
	virtual inline std::string getVendorId(void) const;
	virtual inline e_CPUVENDORS getVendor(void) const; // implicitly set by setVendorId()

	// individual features supported by the processor
	// test for the feature itself here
	virtual inline bool getFeature(const DWORD& dwFeatureMask) const;
	virtual inline bool getExtendedFeature(const DWORD& dwMask) const;

	// these provide access to the entire [Ext]Features values
	virtual inline DWORD getFeatures(void) const;
	virtual inline DWORD getExtendedFeatures(void) const;

	// so far, these are only used with Intel CPUs
	virtual inline DWORD getFeatureEbx(void) const;
	virtual inline DWORD getFeatureEcx(void) const;

	// cache values. cache2 is currently used only for AMD CPUs
	virtual inline DWORD getCacheEax(void) const;
	virtual inline DWORD getCacheEbx(void) const;
	virtual inline DWORD getCacheEcx(void) const;
	virtual inline DWORD getCacheEdx(void) const;
	virtual inline DWORD getCache2Eax(void) const;
	virtual inline DWORD getCache2Ebx(void) const;
	virtual inline DWORD getCache2Ecx(void) const;
	virtual inline DWORD getCache2Edx(void) const;

	protected:
	// signature dictates family, model, stepping, and processor identification number [for p4]
	// it also can indirectly determine name, for older CPUs
	virtual inline void setSignature(const DWORD& dwSignature);
	virtual inline void setName(const std::string& strName);
	virtual void setVendorId(const std::string& strVendorId);
	virtual inline void setFeatures(const DWORD& dwFeatures);
	virtual inline void setExtendedFeatures(const DWORD& dwExtFeatures);
	virtual inline void setFeatureEbx(const DWORD& dwFeatureEbx);
	virtual inline void setFeatureEcx(const DWORD& dwFeatureEcx);
	virtual inline void setCacheEax(const DWORD& dwCacheEax);
	virtual inline void setCacheEbx(const DWORD& dwCacheEbx);
	virtual inline void setCacheEcx(const DWORD& dwCacheEcx);
	virtual inline void setCacheEdx(const DWORD& dwCacheEdx);
	virtual inline void setCache2Eax(const DWORD& dwCacheEax);
	virtual inline void setCache2Ebx(const DWORD& dwCacheEbx);
	virtual inline void setCache2Ecx(const DWORD& dwCacheEcx);
	virtual inline void setCache2Edx(const DWORD& dwCacheEdx);

	protected:     // protected members
	virtual void assign(const CpuStats& source);

	private:       // attributes
	bool  m_fCpuIdFlag;              // whether opcode CPU_ID is supported -- late 486 and after
	DWORD m_dwExtendedFeatures;      // AMD CPUs have extended features to test for things like 3dNow!
	std::string   m_strVendorId;     // vendor id in string form
	std::string   m_strName;         // name string -- either returned by modern CPU's or determined for them
	e_CPUVENDORS  m_eVendor;         // vendor id in enum form
	RegisterValues m_cpuidStats;     // registers from CPUID-1
	RegisterValues m_cacheStats;
	RegisterValues m_cacheStats2;

	static const std::string VENDOR_INTEL_STR;
	static const std::string VENDOR_AMD_STR;
	static const std::string VENDOR_CYRIX_STR;
	static const std::string VENDOR_CENTAUR_STR;
	};

	inline void CpuStats::setKnowsCpuId(const bool& fKnowsCpuId) { m_fCpuIdFlag = fKnowsCpuId;}
	inline bool CpuStats::getKnowsCpuId(void) const { return (m_fCpuIdFlag);}
	inline void CpuStats::setSignature(const DWORD& dwSignature) { m_cpuidStats.setEaxValue(dwSignature);}
	inline DWORD CpuStats::getSignature(void) const { return (m_cpuidStats.getEaxValue());}
	inline int CpuStats::getFamily(void) const { return ((getSignature() >> 8) & 0x0f);}
	inline int CpuStats::getModel(void) const { return ((getSignature() >> 4) & 0x0f); }
	inline int CpuStats::getStepping(void) const { return (getSignature() & 0x0f); }
	inline void CpuStats::setName(const std::string& strName) { m_strName = strName; }
	inline std::string CpuStats::getName(void) const { return (m_strName); }
	inline std::string CpuStats::getVendorId(void) const { return (m_strVendorId); }
	inline CpuStats::e_CPUVENDORS CpuStats::getVendor(void) const { return (m_eVendor); }
	inline bool CpuStats::getFeature(const DWORD& dwFeatureMask) const { return ((m_cpuidStats.getEdxValue() & dwFeatureMask) == dwFeatureMask); }
	inline bool CpuStats::getExtendedFeature(const DWORD& dwMask) const { return ((m_dwExtendedFeatures & dwMask) == dwMask); }
	inline void CpuStats::setFeatures(const DWORD& dwFeatures) { m_cpuidStats.setEdxValue(dwFeatures);}
	inline DWORD CpuStats::getFeatures(void) const { return (m_cpuidStats.getEdxValue());}
	inline void CpuStats::setExtendedFeatures(const DWORD& dwExtFeatures) { m_dwExtendedFeatures = dwExtFeatures;}
	inline DWORD CpuStats::getExtendedFeatures(void) const { return (m_dwExtendedFeatures);}
	inline void CpuStats::setFeatureEbx(const DWORD& dwFeatureEbx) { m_cpuidStats.setEbxValue(dwFeatureEbx); }
	inline DWORD CpuStats::getFeatureEbx(void) const { return (m_cpuidStats.getEbxValue()); }
	inline void CpuStats::setFeatureEcx(const DWORD& dwFeatureEcx) { m_cpuidStats.setEcxValue(dwFeatureEcx); }
	inline DWORD CpuStats::getFeatureEcx(void) const { return (m_cpuidStats.getEcxValue()); }
	inline void CpuStats::setCacheEax(const DWORD& dwCacheEax) { m_cacheStats.setEaxValue(dwCacheEax); }
	inline DWORD CpuStats::getCacheEax(void) const { return (m_cacheStats.getEaxValue());}
	inline void CpuStats::setCacheEbx(const DWORD& dwCacheEbx) { m_cacheStats.setEbxValue(dwCacheEbx);}
	inline DWORD CpuStats::getCacheEbx(void) const { return (m_cacheStats.getEbxValue());}
	inline void CpuStats::setCacheEcx(const DWORD& dwCacheEcx) { m_cacheStats.setEcxValue(dwCacheEcx);}
	inline DWORD CpuStats::getCacheEcx(void) const { return (m_cacheStats.getEcxValue());}
	inline void CpuStats::setCacheEdx(const DWORD& dwCacheEdx) { m_cacheStats.setEdxValue(dwCacheEdx);}
	inline DWORD CpuStats::getCacheEdx(void) const { return (m_cacheStats.getEdxValue());}
	inline void CpuStats::setCache2Eax(const DWORD& dwCacheEax) { m_cacheStats2.setEaxValue(dwCacheEax);}
	inline DWORD CpuStats::getCache2Eax(void) const { return (m_cacheStats2.getEaxValue());}
	inline void CpuStats::setCache2Ebx(const DWORD& dwCacheEbx) { m_cacheStats2.setEbxValue(dwCacheEbx);}
	inline DWORD CpuStats::getCache2Ebx(void) const { return (m_cacheStats2.getEbxValue());}
	inline void CpuStats::setCache2Ecx(const DWORD& dwCacheEcx) { m_cacheStats2.setEcxValue(dwCacheEcx);}
	inline DWORD CpuStats::getCache2Ecx(void) const { return (m_cacheStats2.getEcxValue());}
	inline void CpuStats::setCache2Edx(const DWORD& dwCacheEdx) { m_cacheStats2.setEdxValue(dwCacheEdx);}
	inline DWORD CpuStats::getCache2Edx(void) const { return (m_cacheStats2.getEdxValue());}

}

#endif