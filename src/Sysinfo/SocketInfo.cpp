//------------------------------------------------------------------------------
// SocketInfo.cpp
//    
//   This file contains SocketInfo. All it does is enable the client 
//   to obtain information about the network ... like the IP address, 
//   the computer name, as well as socket information, if requested. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@home.com]
// 
#include "SocketInfo.h"

#include <windows.h>
#include <sstream>
using namespace std;


namespace DSystemInfo {

	SocketInfo::SocketInfo(const int& nMajorVersion /* = 2 */,
						const int& nMinorVersion /* = 0 */)
	{
		determineSocketInfo(nMajorVersion, nMinorVersion);
	}

	SocketInfo::SocketInfo(const SocketInfo& source)
	{
		assign(source);
	}

	SocketInfo& SocketInfo::operator=(const SocketInfo& right)
	{
		if (this != &right)
		{
			assign(right);
		}

		return (*this);
	}

	SocketInfo::~SocketInfo()
	{
	// nothing to do yet
	}

	void SocketInfo::determineSocketInfo(const int& nMajorVersion, 
										const int& nMinorVersion)
	{
		const string UNKNOWN = "Unknown";
		WSADATA stWsaData;

		if (WSAStartup(MAKEWORD(nMajorVersion, nMinorVersion), &stWsaData) != 0)
		{
			m_strVersion = UNKNOWN;
			m_strHighestVersion = UNKNOWN;
			m_strDescription = UNKNOWN;
			m_strSystemStatus = UNKNOWN;
			m_strMax = UNKNOWN;
			m_strUdpMax = UNKNOWN;
			return;
		}

		//------------------------------------------------------------------------------
		// Get information from the socket structure 
		//
		ostringstream os;
		BYTE bLow = 0;
		BYTE bHigh = 0;

		bLow  = (stWsaData.wVersion >> 8);
		bHigh = static_cast<BYTE>(stWsaData.wVersion);
		os << (int)bHigh << "." << (int)bLow << ends;
		m_strVersion = os.str();
		os.str() = "";

		bLow  = (stWsaData.wHighVersion >> 8);
		bHigh = static_cast<BYTE>(stWsaData.wHighVersion);
		os << (int)bHigh << "." << (int)bLow << ends;
		m_strHighestVersion = os.str();
		os.str() = "";

		m_strDescription = stWsaData.szDescription;
		m_strSystemStatus = stWsaData.szSystemStatus;

		os << stWsaData.iMaxSockets << ends;
		m_strMax = os.str();
		os.str() = "";

		os << stWsaData.iMaxUdpDg << ends;
		m_strUdpMax = os.str();
		os.str() = "";

		determineHostAndIPAddress();

		WSACleanup();
		}

		void SocketInfo::assign(const SocketInfo& source)
		{
		m_strVersion = source.m_strVersion;
		m_strHighestVersion = source.m_strHighestVersion;
		m_strDescription = source.m_strDescription;
		m_strSystemStatus = source.m_strSystemStatus;
		m_strMax = source.m_strMax;
		m_strIPAddress = source.m_strIPAddress;
		m_strDomainName = source.m_strDomainName;
		m_strUdpMax = source.m_strUdpMax;
	}

	// WSAStartup() must be called before calling this function
	void SocketInfo::determineHostAndIPAddress(void)
	{
		const string UNKNOWN = "Unknown"; 
		string strIPAddress;
		char szHostName[128];

		if (gethostname(szHostName, 128) == 0)
		{
			//------------------------------------------------------------------------------
			// Get host addresses 
			//
			struct hostent* pHost;
			int i; 

			pHost = gethostbyname(szHostName);

			for (i = 0; ((pHost!= NULL) && (pHost->h_addr_list[i] != NULL)); i++)
			{
				string strTmp;
				int j;  
				ostringstream os;

				for (j = 0; j < pHost->h_length; j++)
				{
					if (j > 0)
					{
					os << ".";
					}

					os << (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j];
				}

				strTmp = os.str();

				//------------------------------------------------------------------------------
				// strTmp now contains one local IP address
				//
				m_strIPAddress = strTmp;
				m_strDomainName = szHostName;
			}
		}

		if (m_strIPAddress.empty())
		{
			m_strIPAddress = UNKNOWN;
		}

		if (m_strDomainName.empty())
		{
			m_strDomainName = UNKNOWN;
		}
	}
}