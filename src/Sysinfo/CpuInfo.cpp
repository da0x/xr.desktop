//------------------------------------------------------------------------------
// CpuInfo.cpp
//    
//   This file contains the class CpuInfo. It basically determines all 
//   pertinent information for the CPU. It currently gets in-depth 
//   information for both Intel and AMD CPUs. 
//   Known Problems:
//   1) algorithm for determining CPU speed is not great; it will be updated
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#include "CpuInfo.h"
#include <sstream>
using namespace std;

namespace DSystemInfo {

	CpuInfo::CpuInfo(const bool fCpuSpeed/* = true*/)
	{
	m_strCpuSpeed = "0";
	determineCpuInfo();
	if (fCpuSpeed)
	{
		determineCpuSpeed();
	}
	}

	CpuInfo::CpuInfo(const CpuInfo& source)
	{
	assign(source);
	}

	CpuInfo& CpuInfo::operator=(const CpuInfo& right)
	{
	if (this != &right)
	{
		assign(right);
	}

	return (*this);
	}

	CpuInfo::~CpuInfo()
	{
	// nothing to do yet
	}

	void CpuInfo::determineCpuInfo(void)
	{
	SYSTEM_INFO stCpuInfo;

	GetSystemInfo(&stCpuInfo);

	ostringstream os;
	os << stCpuInfo.dwNumberOfProcessors << ends;
	m_strNumProcessors = os.str();

	determineCpuStats(stCpuInfo.wProcessorArchitecture);
	}

	// this is pretty straight forward. as long as it's a fairly-recent processor, we'll
	// try to get the speed. first, though, we set the priority to high ... so that
	// we'll get a greater chance of getting the correct speed.
	// the current algorithm just goes until it gets two results in a row that match.
	// a better approach may be to take two or three readings and average them.
	void CpuInfo::determineCpuSpeed(void)
	{
	const int MAX_TRIES = 10;
	DWORD dwSpeed = 0;
	DWORD dwTestValue = UINT_MAX;
	int   nNumTimes = 0;

	if ( ((m_cpuStats.getVendor() == CpuStats::VENDOR_INTEL) && (m_cpuStats.getFamily() > 4)) ||
			((m_cpuStats.getVendor() == CpuStats::VENDOR_AMD)   && (m_cpuStats.getFamily() > 5)) )
	{
		DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

		//--------------------------------------------------------
		// Find the first two similarities up to ten times.
		// This should be a maximum of five seconds.
		// 
		while ((dwSpeed != dwTestValue) && (nNumTimes < MAX_TRIES))
		{
			dwTestValue = dwSpeed;
			dwSpeed = calculateCpuSpeed();
			nNumTimes++;
		}	

		SetPriorityClass(GetCurrentProcess(), dwStartingPriority);

		//--------------------------------------------------------
		// If we've done ten queries and STILL don't have a 
		// valid reading, don't change anything so the speed will
		// be Unknown. This if below should almost always evauluate
		// to TRUE.
		//
		if (nNumTimes != MAX_TRIES)
		{
			ostringstream os;
			os << dwSpeed << " MHz" << ends;
			m_strCpuSpeed = os.str();
		}
	}
	}

	void CpuInfo::assign(const CpuInfo& source)
	{
	m_cpuStats = source.m_cpuStats;
	m_strCpuSpeed = source.m_strCpuSpeed;
	m_strNumProcessors = source.m_strNumProcessors;
	}

	void CpuInfo::determineCpuStats(WORD wProcessorArchitecture)
	{
	// make sure the cpu knows CPUID
	if (!determineCpuId())
	{
		return;
	}

	m_cpuStats.setKnowsCpuId(true);

	DWORD dwHighestCpuId = determineHighestCpuId();
	switch (dwHighestCpuId)
	{
	case 2:     // intel cpu's find cache information here
		determineIntelCacheInfo();
		// no break;
	case 1:     // x86 cpu's do processor identification here
		determineCpuIdentification();
		break;

	case 0:     // don't do anything funky; return
	default:
		return;
	}

	if (m_cpuStats.getVendor() == CpuStats::VENDOR_INTEL)
	{
		determineOldIntelName();
	}
	 
	DWORD dwLargestExtendedFeature = determineLargestExtendedFeature();

	switch (dwLargestExtendedFeature)
	{
	case AMD_L2CACHE_FEATURE:
		determineAmdL2CacheInfo();
		// no break;
	case AMD_L1CACHE_FEATURE:
		determineAmdL1CacheInfo();
		// no break;
	case NAMESTRING_FEATURE:
		determineNameString();
		// no break;
	case AMD_EXTENDED_FEATURE:
		determineExtendedFeature();
		break;

	case 0:     // if they have no feature, do nothing
	default:
		break;
	}
	}

	bool CpuInfo::determineCpuId(void)
	{
	int fKnowsCpuId = 0;
	   
	__asm
	{
		pushfd                      // save EFLAGS to stack.
		pop     eax                 // store EFLAGS in EAX.
		mov     edx, eax            // save in EBX for testing later.
		xor     eax, 0200000h       // switch bit 21.
		push    eax                 // copy "changed" value to stack.
		popfd                       // save "changed" EAX to EFLAGS.
		pushfd
		pop     eax
		xor     eax, edx            // See if bit changeable.
		jnz     short has_cpuid     // if so, mark 
		mov     fKnowsCpuId, 0      // if not, put 0
		jmp     done
	has_cpuid:
		mov     fKnowsCpuId, 1      // put 1
	done:
	}

	return ((bool)fKnowsCpuId);
	}

	DWORD CpuInfo::determineHighestCpuId(void)
	{
	DWORD dwHighest = 0;
	char szTemp[16] = {0};

	__asm
	{
		mov      eax, 0
		CPUID

		test    eax,eax                   // 0 is highest function, then don't query more info
		jz      no_features
	    
		mov     dwHighest, eax            // highest supported instruction
		mov     DWORD PTR [szTemp+0],ebx  // Stash the manufacturer string for later
		mov     DWORD PTR [szTemp+4],edx
		mov     DWORD PTR [szTemp+8],ecx

	no_features:
	}

	if (dwHighest != 0)
	{
		string strVendorId = szTemp;
		m_cpuStats.setVendorId(strVendorId);
	}

	return (dwHighest);
	}

	void CpuInfo::determineCpuIdentification(void)
	{
	DWORD dwSignature = 0;
	DWORD dwFeatureEbx = 0;
	DWORD dwFeatureEcx = 0 ;
	DWORD dwFeatures = 0;

	__asm
	{
		mov      eax, 1
		CPUID

		mov      [dwSignature], eax     // store CPU signature
		mov      [dwFeatureEbx], ebx    
		mov      [dwFeatureEcx], ecx
		mov      [dwFeatures], edx      // features
	}  

	m_cpuStats.setSignature(dwSignature);
	m_cpuStats.setFeatureEbx(dwFeatureEbx);
	m_cpuStats.setFeatureEcx(dwFeatureEcx);
	m_cpuStats.setFeatures(dwFeatures);
	}

	DWORD CpuInfo::determineLargestExtendedFeature(void)
	{
	DWORD dwHighest = 0;

	__asm
	{
		mov      eax, 0x80000000
		CPUID

		mov dwHighest, eax
	}

	return (dwHighest);
	}

	void CpuInfo::determineExtendedFeature(void)
	{
	DWORD dwExtendedFeatures = 0;

	__asm
	{
		mov      eax, 0x80000001
		CPUID

		mov dwExtendedFeatures, edx
	}

	m_cpuStats.setExtendedFeatures(dwExtendedFeatures);
	}

	void CpuInfo::determineNameString(void)
	{
	char szName[64] = {0};

	__asm
	{
		mov      eax, 0x80000002
		CPUID
		mov      DWORD PTR [szName+0],eax
		mov      DWORD PTR [szName+4],ebx
		mov      DWORD PTR [szName+8],ecx
		mov      DWORD PTR [szName+12],edx
		mov      eax, 0x80000003
		CPUID
		mov      DWORD PTR [szName+16],eax
		mov      DWORD PTR [szName+20],ebx
		mov      DWORD PTR [szName+24],ecx
		mov      DWORD PTR [szName+28],edx
		mov      eax, 0x80000004
		CPUID
		mov      DWORD PTR [szName+32],eax
		mov      DWORD PTR [szName+36],ebx
		mov      DWORD PTR [szName+40],ecx
		mov      DWORD PTR [szName+44],edx
	}

	m_cpuStats.setName(szName);
	}

	void CpuInfo::determineAmdL1CacheInfo(void)
	{
	DWORD dwEax = 0;
	DWORD dwEbx = 0;
	DWORD dwEcx = 0;
	DWORD dwEdx = 0;

	__asm
	{
		mov      eax, 0x80000005
		CPUID
		mov      dwEax, eax
		mov      dwEbx, ebx
		mov      dwEcx, ecx
		mov      dwEdx, edx
	}

	m_cpuStats.setCacheEax(dwEax);
	m_cpuStats.setCacheEbx(dwEbx);
	m_cpuStats.setCacheEcx(dwEcx);
	m_cpuStats.setCacheEdx(dwEdx);
	}

	void CpuInfo::determineAmdL2CacheInfo(void)
	{
	DWORD dwEax = 0;
	DWORD dwEbx = 0;
	DWORD dwEcx = 0;
	DWORD dwEdx = 0;

	__asm
	{
		mov      eax, 0x80000006
		CPUID
		mov      dwEax, eax
		mov      dwEbx, ebx
		mov      dwEcx, ecx
		mov      dwEdx, edx
	}

	m_cpuStats.setCache2Eax(dwEax);
	m_cpuStats.setCache2Ebx(dwEbx);
	m_cpuStats.setCache2Ecx(dwEcx);
	m_cpuStats.setCache2Edx(dwEdx);
	}

	void CpuInfo::determineIntelCacheInfo(void)
	{
	DWORD dwEax = 0;
	DWORD dwEbx = 0;
	DWORD dwEcx = 0;
	DWORD dwEdx = 0;

	__asm
	{
		mov      eax, 2
		CPUID
		cmp      al, 1
		jne      end_find_intel_cacheinfo

		// if one iteration isn't enough, this code won't produce anything meaningful!
		// this is from intel; look into changing it
		mov      dwEax, eax
		mov      dwEbx, ebx
		mov      dwEcx, ecx
		mov      dwEdx, edx

	end_find_intel_cacheinfo:

	}

	m_cpuStats.setCacheEax(dwEax);
	m_cpuStats.setCacheEbx(dwEbx);
	m_cpuStats.setCacheEcx(dwEcx);
	m_cpuStats.setCacheEdx(dwEdx);
	}


	// this function is mostly ripped from intel's sv_bDeveloper.intel.com webpage
	// on the CPUID instruction
	void CpuInfo::determineOldIntelName()
	{
	bool fIsCeleron;
	bool fIsXeon;

	struct brand_entry
	{
		long  lBrandValue;
		char* pszBrand;
	};

	struct brand_entry brand_table[BRANDTABLESIZE] = 
	{
		1, "Genuine Intel Celeron(TM) processor",
		2, "Genuine Intel Pentium(R) III processor",
		3, "Genuine Intel Pentium(R) III Xeon(TM) processor",
		8, "Genuine Intel Pentium(R) 4 processor"
	};

	/* we don't support old intel cpu's that don't know CPUID
	if (m_cpuStats.getKnowsCpuId() == 0)
	{
	// 8086 - 80486
	switch(m_cpuStats.getFamily())
	{
	case 3:
		m_strCpuIdentification = "Intel386(TM) processor";
		break;
	case 4:
		m_strCpuIdentification = "Intel486(TM) processor";
		break;
	default:
		m_strCpuIdentification = "Unknown processor";
		break;
	} */

	if (determineHighestCpuId() < NAMESTRING_FEATURE)
	{
		switch (m_cpuStats.getFamily())
		{
		case 4:  // 486
			switch(m_cpuStats.getModel())
			{
			case 0:
			case 1:
				m_cpuStats.setName("Intel486(TM) DX processor");
				break;
			case 2:
				m_cpuStats.setName("Intel486(TM) SX processor");
				break;
			case 3:
				m_cpuStats.setName("IntelDX2(TM) processor");
				break;
			case 4:
				m_cpuStats.setName("Intel486(TM) processor");
				break;
			case 5:
				m_cpuStats.setName("IntelSX2(TM) processor");
				break;
			case 7:
				m_cpuStats.setName("Writeback Enhanced IntelDX2(TM) processor");
				break;
			case 8:
				m_cpuStats.setName("IntelDX4(TM) processor");
				break;
			default:
				m_cpuStats.setName("Intel 486 processor");
				break;
				}

				break;

		case 5:  // pentium
			m_cpuStats.setName("Intel Pentium(R) processor");
			break;

		case 6:  // pentium II and family
			switch (m_cpuStats.getModel())
			{
			case 1:
				m_cpuStats.setName("Intel Pentium(R) Pro processor");
				break;
			case 3:
				m_cpuStats.setName("Intel Pentium(R) II processor, model 3");
				break;
			case 5:
			case 7:
				fIsCeleron = false;
				fIsXeon = false;

				determineCeleronAndXeon(m_cpuStats.getCacheEax(), &fIsCeleron, &fIsXeon, true);
				determineCeleronAndXeon(m_cpuStats.getCacheEbx(), &fIsCeleron, &fIsXeon);
				determineCeleronAndXeon(m_cpuStats.getCacheEcx(), &fIsCeleron, &fIsXeon);
				determineCeleronAndXeon(m_cpuStats.getCacheEdx(), &fIsCeleron, &fIsXeon);
	        
				if (fIsCeleron)
				{
				m_cpuStats.setName("Intel Celeron(TM) processor, model 5");
				}
				else
				{
				if (fIsXeon)
				{
					if (m_cpuStats.getModel() == 5)
					{
						m_cpuStats.setName("Intel Pentium(R) II Xeon(TM) processor");
					}
					else
					{
						m_cpuStats.setName("Intel Pentium(R) III Xeon(TM) processor");
					}
				}
				else
				{
					if (m_cpuStats.getModel() == 5)
					{
						m_cpuStats.setName("Intel Pentium(R) II processor, model 5");
					}
					else
					{
						m_cpuStats.setName("Intel Pentium(R) III processor");
					}
				}
				}

				break;

			case 6:
				m_cpuStats.setName("Intel Celeron(TM) processor, model 6");
				break;
			case 8:
				m_cpuStats.setName("Intel Pentium(R) III Coppermine processor");
				break;
			default:
				{
				int brand_index = 0;
				while ((brand_index < BRANDTABLESIZE) &&
						((m_cpuStats.getFeatureEbx() & 0xff) != brand_table[brand_index].lBrandValue))
				{
					brand_index++;
				}

				if (brand_index < BRANDTABLESIZE)
				{
					m_cpuStats.setName(brand_table[brand_index].pszBrand);
				}
				else
				{
					m_cpuStats.setName("Unknown Genuine Intel processor");
				}

				break;
				}
			} 
		}

		if (m_cpuStats.getFeature(MMX_FLAG))
		{
			string strName = m_cpuStats.getName();
			strName += " with MMX";
			m_cpuStats.setName(strName);
		}		   
	}
	}

	// this is ripped from intel, but put into function form. instead of testing for
	// each register, we have a function to test a register's value.
	// newer processors won't even use this.
	void CpuInfo::determineCeleronAndXeon(DWORD dwRegisterCache, bool* pfIsCeleron, bool* pfIsXeon, bool fIsEax)
	{
	DWORD dwCacheTemp;

	dwCacheTemp = dwRegisterCache & 0xFF000000;
	if (dwCacheTemp == 0x40000000)
	{
		*pfIsCeleron = true;
	}
	if ((dwCacheTemp >= 0x44000000) && (dwCacheTemp <= 0x45000000))
	{
		*pfIsXeon = true;
	}

	dwCacheTemp = dwRegisterCache & 0xFF0000;
	if (dwCacheTemp == 0x400000)
	{
		*pfIsCeleron = true;
	}
	if ((dwCacheTemp >= 0x440000) && (dwCacheTemp <= 0x450000))
	{
		*pfIsXeon = true;
	}

	dwCacheTemp = dwRegisterCache & 0xFF00;
	if (dwCacheTemp == 0x4000)
	{
		*pfIsCeleron = true;
	}
	if ((dwCacheTemp >= 0x4400) && (dwCacheTemp <= 0x4500))
	{
		*pfIsXeon = true;
	}

	if (!fIsEax)
	{
		dwCacheTemp = dwRegisterCache & 0xFF;     // possibly not needed for m_dwCacheEax
		if (dwCacheTemp == 0x40000000)            
		{
			*pfIsCeleron = true;
		}
		if ((dwCacheTemp >= 0x44000000) && (dwCacheTemp <= 0x45000000))
		{
			*pfIsXeon = true;
		}
	}
	}

	// this function is ripped from amd
	DWORD CpuInfo::calculateCpuSpeed(void) const
	{
	int   nTimeStart = 0;
	int   nTimeStop = 0;
	DWORD dwStartTicks = 0;
	DWORD dwEndTicks = 0;
	DWORD dwTotalTicks = 0;
	DWORD dwCpuSpeed = 0;

	nTimeStart = timeGetTime();

	for(;;)
	{
		nTimeStop = timeGetTime();

		if ((nTimeStop - nTimeStart) > 1)
		{
			dwStartTicks = determineTimeStamp();
			break;
		}
	}

	nTimeStart = nTimeStop;

	for(;;)
	{
		nTimeStop = timeGetTime();
		if ((nTimeStop - nTimeStart) > 500)    // one-half second
		{
			dwEndTicks = determineTimeStamp();
			break;
		}
	}

	dwTotalTicks = dwEndTicks - dwStartTicks;
	dwCpuSpeed = dwTotalTicks / 500000;

	return (dwCpuSpeed);
	}

	// stolen from amd
	DWORD CpuInfo::determineTimeStamp(void) const
	{
	DWORD dwTickVal;

	__asm
	{
		_emit 0Fh
		_emit 31h
		mov   dwTickVal, eax
	}

	return (dwTickVal);
	}
}