//------------------------------------------------------------------------------
// RegisterValues.h
//    
//   This file holds RegisterValues, a class meant to stores the values 
//   of the four main registers [usually after a CPUID instruction]. 
//   
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef REGISTERVALUES_H_
#define REGISTERVALUES_H_

#include "SysInfoClasses.h"
#include <windows.h>

namespace DSystemInfo {
	
	class RegisterValues
	{
	public:        // object creation/destruction
	RegisterValues();
	RegisterValues(const DWORD& dwEax, const DWORD& dwEbx,
					const DWORD& dwEcx, const DWORD& dwEdx);
	RegisterValues(const RegisterValues& source);
	RegisterValues& operator=(const RegisterValues& right);
	virtual ~RegisterValues();

	public:        // attribute modification
	virtual inline DWORD getEaxValue(void) const;
	virtual inline void setEaxValue(const DWORD& dwEax);
	virtual inline DWORD getEbxValue(void) const;
	virtual inline void setEbxValue(const DWORD& dwEbx);
	virtual inline DWORD getEcxValue(void) const;
	virtual inline void setEcxValue(const DWORD& dwEcx);
	virtual inline DWORD getEdxValue(void) const;
	virtual inline void setEdxValue(const DWORD& dwEdx);

	protected:     // protected members
	virtual void assign(const RegisterValues& source);

	private:       // attributes
	DWORD m_dwEaxValue;
	DWORD m_dwEbxValue;
	DWORD m_dwEcxValue;
	DWORD m_dwEdxValue;
	};

	inline DWORD RegisterValues::getEaxValue(void) const { return (m_dwEaxValue); }
	inline void RegisterValues::setEaxValue(const DWORD& dwEax) { m_dwEaxValue = dwEax; }
	inline DWORD RegisterValues::getEbxValue(void) const { return (m_dwEbxValue); }
	inline void RegisterValues::setEbxValue(const DWORD& dwEbx) { m_dwEbxValue = dwEbx;}
	inline DWORD RegisterValues::getEcxValue(void) const { return (m_dwEcxValue); }
	inline void RegisterValues::setEcxValue(const DWORD& dwEcx) { m_dwEcxValue = dwEcx;}
	inline DWORD RegisterValues::getEdxValue(void) const { return (m_dwEdxValue); }
	inline void RegisterValues::setEdxValue(const DWORD& dwEdx) { m_dwEdxValue = dwEdx;}

}

#endif