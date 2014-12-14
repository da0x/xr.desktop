//------------------------------------------------------------------------------
// DriveInfo.cpp
//    
//   This file contains the class DriveInfo. It's just a class that iterates 
//   through all drives on a system, getting information for each one. 
//   The information can be obtained via the getDriveStats function [returns 
//   a pointer to a vector of DriveStats objects].
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#include "DriveInfo.h"
#include "SysUtils.h"

#include <sstream>
using namespace std;

namespace DSystemInfo {

	DriveInfo::DriveInfo(const unsigned int& unDriveTypes/* = HARDDRIVE | NETWORK*/)
	{
	determineDriveInfo(unDriveTypes);
	}

	DriveInfo::DriveInfo(const DriveInfo& source)
	{
	assign(source);
	}

	DriveInfo& DriveInfo::operator=(const DriveInfo& right)
	{
	if (this != &right)
	{
		assign(right);
	}

	return (*this);
	}

	DriveInfo::~DriveInfo()
	{
	// nothing to do yet
	}

	void DriveInfo::determineDriveInfo(const unsigned int& unDriveTypes/* = HARDDRIVE | NETWORK*/)
	{
	HINSTANCE hModule = NULL;                     

	if ((hModule = LoadLibrary("KERNEL32.DLL")) != NULL)
	{
		DWORD dwDriveList = ::GetLogicalDrives();
		DWORD dwMinMask = 0x00000001;
		m_vdriveStats.clear();

		// check to see if they want to test for floppy drives
		for (int i = (unDriveTypes & FLOPPY) == FLOPPY ? 0 : 2; i < 26; i++)
		{
			DriveStats driveStats;

			if (dwDriveList & (dwMinMask << i))
			{
				// set the drive name
				ostringstream os;
				os << (char)('A' + i) << ":\\" << ends;
				driveStats.setName(os.str());

				// make sure the drive is a valid type
				// we don't want floppy or cdrom, because they will
				// require the user to insert a disk if there is no disk....
				unsigned int unType = GetDriveType(driveStats.getName().c_str());
				if (unType != DRIVE_NO_ROOT_DIR)
				{
				if ( (((unDriveTypes & CDROM) == CDROM) && (unType == DRIVE_CDROM)) ||
						(((unDriveTypes & HARDDRIVE) == HARDDRIVE) && (unType == DRIVE_FIXED)) ||
						(((unDriveTypes & NETWORK) == NETWORK) && (unType == DRIVE_REMOTE)) ||
						(((unDriveTypes & FLOPPY) == FLOPPY) && (unType == DRIVE_REMOVABLE)) )

				{
					PFNGETDISKFREESPACEEX pDiskFreeSpaceEx = NULL;

					driveStats.setType(unType);               
	              
					// set the string version of the type based on unType
					switch (unType)
					{
					case DRIVE_UNKNOWN:
						driveStats.setType("Unknown drive");
						break;
					case DRIVE_REMOVABLE:
						driveStats.setType("Removable drive");
						break;
					case DRIVE_FIXED:
						driveStats.setType("Fixed drive");
						break;
					case DRIVE_REMOTE:
						driveStats.setType("Network drive");
						break;
					case DRIVE_CDROM:
						driveStats.setType("CD-ROM drive");
						break;
					case DRIVE_RAMDISK:
						driveStats.setType("RAM disk");
						break;
					}

					// use GetDiskFreeSpaceEx, if available. otherwise, use the crappy version [GetDiskFreeSpace]
					if ((pDiskFreeSpaceEx = (PFNGETDISKFREESPACEEX)GetProcAddress(hModule, "GetDiskFreeSpaceExA")) == NULL)
					{	
						DWORD dwSectorsPerCluster = 0; 
						DWORD dwBytesPerSector = 0;    
						DWORD dwFreeClusters = 0;      
						DWORD dwClusters = 0;          		
						BOOL fResult = GetDiskFreeSpace(driveStats.getName().c_str(), &dwSectorsPerCluster, &dwBytesPerSector, 
														&dwFreeClusters, &dwClusters);

						DWORD dwTotal = fResult ? (dwClusters * dwBytesPerSector * dwSectorsPerCluster) : 0;
						DWORD dwFree = fResult ? (dwFreeClusters * dwBytesPerSector * dwSectorsPerCluster) : 0;

						driveStats.setTotalSpace(SysUtils::ByteToStr(dwTotal));
						driveStats.setFreeSpace(SysUtils::ByteToStr(dwFree));
					}
					else
					{
						ULARGE_INTEGER uliTotalNumBytes;         
						ULARGE_INTEGER uliTotalFreeBytes;        
						ULARGE_INTEGER uliTotalAvailableToCaller;

						BOOL fResult = pDiskFreeSpaceEx(driveStats.getName().c_str(), &uliTotalAvailableToCaller, &uliTotalNumBytes, &uliTotalFreeBytes);
						driveStats.setTotalSpace(fResult ? SysUtils::ByteToStr(uliTotalNumBytes.QuadPart) : "0");
						driveStats.setFreeSpace(fResult ? SysUtils::ByteToStr(uliTotalFreeBytes.QuadPart) : "0");
					}

					m_vdriveStats.push_back(driveStats);
				}
				}
			}
		}

			FreeLibrary(hModule);
		}
	}

	void DriveInfo::assign(const DriveInfo& source)
	{
	m_vdriveStats = source.m_vdriveStats;
	}
}