#include "ITSystemInfo.h"
#include <windows.h>

#include <Iphlpapi.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Iphlpapi")

#include <QNetworkInterface>
std::string ITSystemInfo::GetPcName()
{
	char pcName[1024] = {};
	DWORD pcNameLen = sizeof(pcName);

	if (!GetComputerNameA(pcName, &pcNameLen))
	{
		return "";
	}

	return pcName;
}

std::string ITSystemInfo::GetPcUserName()
{
	char pcUserName[1024] = {};
	DWORD pcNameLen = sizeof(pcUserName);

	if (!GetUserNameA(pcUserName, &pcNameLen))
	{
		return "";
	}

	return pcUserName;
}
std::vector<ITSystemInfo::LocalNetworkInfo> ITSystemInfo::GetNetworkInfo()
{
	std::vector<LocalNetworkInfo> ret;

	PIP_ADAPTER_INFO pIpAdaptTab = NULL;
	unsigned long ulLen = 0;

	GetAdaptersInfo(pIpAdaptTab, &ulLen);
	if (ulLen == 0)
	{
		return ret;
	}

	pIpAdaptTab = (PIP_ADAPTER_INFO)malloc(ulLen);
	if (pIpAdaptTab == NULL)
	{
		return ret;
	}

	GetAdaptersInfo(pIpAdaptTab, &ulLen);
	PIP_ADAPTER_INFO pTmp = pIpAdaptTab;
	while (pTmp != NULL)
	{
		char buf[256];
		sprintf_s(buf, sizeof(buf), "%02X-%02X-%02X-%02X-%02X-%02X",
				pTmp->Address[0],
				pTmp->Address[1],
				pTmp->Address[2],
				pTmp->Address[3],
				pTmp->Address[4],
				pTmp->Address[5]);

		LocalNetworkInfo val;
		val.isDHCP = pTmp->DhcpEnabled == 0 ? false : true;
		val.strGateway = pTmp->GatewayList.IpAddress.String;
		val.strGatewayMask = pTmp->GatewayList.IpMask.String;
		val.strIP = pTmp->IpAddressList.IpAddress.String;
		val.strMask = pTmp->IpAddressList.IpMask.String;
		val.strMac = buf;

		ret.push_back(std::move(val));
		pTmp = pTmp->Next;
	}

	free(pIpAdaptTab);

	return ret;
}

std::vector<ITSystemInfo::LocalNetworkInfo> ITSystemInfo::GetNetworkInfoFromQt()
{
	std::vector<LocalNetworkInfo> ret;

	auto tmpInterfaceList = QNetworkInterface::allInterfaces();
	std::sort(tmpInterfaceList.begin(), tmpInterfaceList.end(), [&]( QNetworkInterface & p1, QNetworkInterface & p2)
	{
		return p1.index() < p2.index();
	});
	for (QNetworkInterface &tmpInterface : tmpInterfaceList)
	{
		//qDebug() << tmpInterface.flags() << tmpInterface.humanReadableName() << tmpInterface.hardwareAddress() << tmpInterface.name() << tmpInterface.type() << tmpInterface.index();
		auto faceFlags = tmpInterface.flags();
		if (faceFlags.testFlag(QNetworkInterface::IsUp) && faceFlags.testFlag(QNetworkInterface::IsRunning) && !faceFlags.testFlag(QNetworkInterface::IsLoopBack))
		{
			//qDebug() << "Filter:" << tmpInterface.flags() << tmpInterface.humanReadableName() << tmpInterface.hardwareAddress() << tmpInterface.name() << tmpInterface.type() << tmpInterface.index();
			for (auto tmpAddress : tmpInterface.addressEntries())
			{
				if (tmpAddress.ip().protocol() == QAbstractSocket::IPv4Protocol)
				{
				//	qDebug() << tmpAddress.ip().toString() << tmpAddress.ip().protocol();
					LocalNetworkInfo val;
					//val.isDHCP = pTmp->DhcpEnabled == 0 ? false : true;
					//val.strGateway = pTmp->GatewayList.IpAddress.String;
					//val.strGatewayMask = pTmp->GatewayList.IpMask.String;
					val.strIP = tmpAddress.ip().toString().toStdString();
					//val.strMask = pTmp->IpAddressList.IpMask.String;
					val.strMac = tmpInterface.hardwareAddress().toStdString();
					ret.push_back(std::move(val));
					break;
				}
			}
		}
	}
	return ret;
}
