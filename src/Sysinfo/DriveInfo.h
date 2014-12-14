//------------------------------------------------------------------------------
// DriveInfo.h
//    
//   This file contains the class DriveInfo. It's just a class that iterates 
//   through all drives on a system, getting information for each one. 
//   The information can be obtained via the getDriveStats function [returns 
//   a pointer to a vector of DriveStats objects].
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef DRIVEINFO_H_
#define DRIVEINFO_H_

#include "SysInfoClasses.h"
#include "DriveStats.h"
#include <vector>
#include <iterator>
#include <utility>
#include <string>


namespace DSystemInfo {


#define HARDDRIVE 1
#define NETWORK   2
#define FLOPPY    4
#define CDROM     8

	// this is the function pointer for GetDiskFreeSpaceEx() [doesn't exist on Win95A]
	// so we have to see if kernel32.dll exports this function. if it doesn't, we'll
	// use GetDiskFreeSpace()
	typedef BOOL (WINAPI* PFNGETDISKFREESPACEEX)(LPCTSTR, PULARGE_INTEGER,
															PULARGE_INTEGER, PULARGE_INTEGER);

	class DriveInfo
	{
	public:        // object creation/destruction
	DriveInfo(const unsigned int& unDriveTypes = HARDDRIVE | NETWORK);
	DriveInfo(const DriveInfo& source);
	DriveInfo& operator=(const DriveInfo& right);
	virtual ~DriveInfo();

	public: 
	virtual void determineDriveInfo(const unsigned int& unDriveTypes = HARDDRIVE | NETWORK);

	virtual inline std::vector<DriveStats> const * getDriveStats(void) const;

	protected:     // protected members
	virtual void assign(const DriveInfo& source);

	private:       // attributes
	std::vector<DriveStats> m_vdriveStats;
	};

	// because a dll uses its own heap manager, any memory allocated by the dll should
	// also be freed by the dll. when a vector is returned, the dll calls the copy 
	// constructor ... which allocates memory. however, it's the calling program's job 
	// to free the memory. this doens't work well. therefore, we just return a const *
	// to our member object. this way, no new memory is allocated.
	// the only alternative to this would be to duplicate all of DriveStats functions
	// with an index. then we'd manually do the lookups in the vector -- tiring.
	inline std::vector<DriveStats> const * DriveInfo::getDriveStats(void) const
	{
	return (&m_vdriveStats);
	}

}

#endif