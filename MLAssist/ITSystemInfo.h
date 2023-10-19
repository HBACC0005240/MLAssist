#pragma once
#include <memory>
#include <string>
#include <vector>

class ITSystemInfo : public std::enable_shared_from_this<ITSystemInfo>
{
public:
	//本地网络信息
	struct LocalNetworkInfo
	{
		std::string strIP;
		std::string strMask;
		std::string strMac;
		std::string strGateway;
		std::string strGatewayMask;
		bool isDHCP;
	};
	//获取计算机名称
	static std::string GetPcName();

	//获取用户名称
	static std::string GetPcUserName();

	//获取网卡信息
	std::vector<LocalNetworkInfo> GetNetworkInfo();
	static std::vector<LocalNetworkInfo> GetNetworkInfoFromQt();
};
