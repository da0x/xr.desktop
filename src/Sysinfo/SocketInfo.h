//------------------------------------------------------------------------------
// SocketInfo.h
//    
//   This file contains SocketInfo. All it does is enable the client 
//   to obtain information about the network ... like the IP address, 
//   the computer name, as well as socket information, if requested. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#ifndef SOCKETINFO_H_
#define SOCKETINFO_H_

#include "SysInfoClasses.h"
#include <string>

#pragma comment(lib, "wsock32.lib")

namespace DSystemInfo {

	class SocketInfo
	{
		public:        // object creation/destruction
			SocketInfo(const int& nMajorVersion = 2, const int& nMinorVersion = 0);
			SocketInfo(const SocketInfo& source);
			SocketInfo& operator=(const SocketInfo& right);
			virtual ~SocketInfo();

		public:
			void determineSocketInfo(const int& nMajorVersion, const int& nMinorVersion);

			virtual inline std::string getVersion(void) const;
			virtual inline std::string getHighestVersion(void) const;
			virtual inline std::string getDescription(void) const;
			virtual inline std::string getSystemStatus(void) const;
			virtual inline std::string getMax(void) const;
			virtual inline std::string getIPAddress(void) const;
			virtual inline std::string getDomainName(void) const;
			virtual inline std::string getUdpMax(void) const;

		protected:     // protected members
			virtual void assign(const SocketInfo& source);
			virtual void determineHostAndIPAddress(void);

		private:       // attributes
			std::string m_strVersion;
			std::string m_strHighestVersion;
			std::string m_strDescription;
			std::string m_strSystemStatus;
			std::string m_strMax;
			std::string m_strIPAddress;
			std::string m_strDomainName;
			std::string m_strUdpMax;
	};

	inline std::string SocketInfo::getVersion(void) const { return (m_strVersion); }
	inline std::string SocketInfo::getHighestVersion(void) const { return (m_strHighestVersion); }
	inline std::string SocketInfo::getDescription(void) const { return (m_strDescription); }
	inline std::string SocketInfo::getSystemStatus(void) const { return (m_strSystemStatus); }
	inline std::string SocketInfo::getMax(void) const { return (m_strMax); }
	inline std::string SocketInfo::getIPAddress(void) const { return (m_strIPAddress); }
	inline std::string SocketInfo::getDomainName(void) const { return (m_strDomainName); }
	inline std::string SocketInfo::getUdpMax(void) const { return (m_strUdpMax); }

}

#endif