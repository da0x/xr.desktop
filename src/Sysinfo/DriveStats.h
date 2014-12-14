//------------------------------------------------------------------------------
// DriveStats.h
//    
//   This class contains the DriveStats class, which is basically just 
//   a glorified structure with member accessors/mutators. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef DRIVESTATS_H_
#define DRIVESTATS_H_

#include "SysInfoClasses.h"
#include <windows.h>
#include <string>


namespace DSystemInfo {
	
	class DriveStats
	{
	public:        // object creation/destruction
	DriveStats();
	DriveStats(const DriveStats& source);
	DriveStats& operator=(const DriveStats& right);
	virtual ~DriveStats();

	public:      
	virtual inline void setType(const unsigned int& unType);
	virtual inline void getType(unsigned int* punType) const;
	virtual inline void setType(const std::string& strType);
	virtual inline std::string getType(void) const;
	virtual inline void setName(const std::string& strName);
	virtual inline std::string getName(void) const;
	virtual inline void setTotalSpace(const std::string& strTotalSpace);
	virtual inline std::string getTotalSpace(void) const;
	virtual inline void setFreeSpace(const std::string& strFreeSpace);
	virtual inline std::string getFreeSpace(void) const;

	protected:     // protected members
	virtual void assign(const DriveStats& source);

	private:       // attributes
	unsigned int m_unType;           // type of drive
	std::string  m_strType;          // type of drive
	std::string  m_strName;          // drive name
	std::string  m_strTotalSpace;    // total drive space
	std::string  m_strFreeSpace;     // total free space
	};

	inline void DriveStats::setType(const unsigned int& unType) { m_unType = unType; }
	inline void DriveStats::getType(unsigned int* punType) const { *punType = m_unType; }
	inline void DriveStats::setType(const std::string& strType) { m_strType = strType; }
	inline std::string DriveStats::getType(void) const { return (m_strType); }
	inline void DriveStats::setName(const std::string& strName) { m_strName = strName; }
	inline std::string DriveStats::getName(void) const { return (m_strName); }
	inline void DriveStats::setTotalSpace(const std::string& strTotalSpace) { m_strTotalSpace = strTotalSpace; }
	inline std::string DriveStats::getTotalSpace(void) const { return (m_strTotalSpace); }
	inline void DriveStats::setFreeSpace(const std::string& strFreeSpace) { m_strFreeSpace = strFreeSpace; }
	inline std::string DriveStats::getFreeSpace(void) const {  return (m_strFreeSpace); }

}

#endif