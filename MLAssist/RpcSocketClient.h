#pragma once
#include "../protos/CGData.grpc.pb.h"
#include "../protos/CGData.pb.h"
#include "grpcpp/grpcpp.h"
#include <QSharedPointer>
using CGData::CGItemRequest;
using CGData::CGItemResponse;
using CGData::CGRpcService;
using CGData::CGStoreItemRequest;
using CGData::CGStoreItemResponse;
using CGData::DownloadMapDataRequest;
using CGData::DownloadMapDataResponse;
using CGData::UploadMapDataRequest;
using CGData::UploadMapDataResponse;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;
#ifdef _DEBUG
#pragma comment(lib, "../lib/grpc/debug/grpc++_alts.lib")
#pragma comment(lib, "../lib/grpc/debug/grpc_plugin_support.lib")
#pragma comment(lib, "../lib/grpc/debug/grpc_unsecure.lib")
#pragma comment(lib, "../lib/grpc/debug/grpcpp_channelz.lib")
#pragma comment(lib, "../lib/grpc/debug/grpc++_error_details.lib")
#pragma comment(lib, "../lib/grpc/debug/grpc++_reflection.lib")
#pragma comment(lib, "../lib/grpc/debug/grpc.lib")
#pragma comment(lib, "../lib/grpc/debug/grpc++.lib")
#pragma comment(lib, "../lib/grpc/debug/gpr.lib")
#pragma comment(lib, "../lib/grpc/debug/ssl.lib")
#pragma comment(lib, "../lib/grpc/debug/crypto.lib")
#pragma comment(lib, "../lib/grpc/debug/cares.lib")
#pragma comment(lib, "../lib/grpc/debug/libprotobufd.lib")
#pragma comment(lib, "../lib/grpc/debug/zlibstaticd.lib")
#pragma comment(lib, "../lib/grpc/debug/address_sorting.lib")
#pragma comment(lib, "../lib/grpc/debug/re2.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_hash.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_city.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_wyhash.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_raw_hash_set.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_hashtablez_sampler.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_exponential_biased.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_statusor.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_bad_variant_access.lib")
#pragma comment(lib, "../lib/grpc/debug/upb.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_status.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_cord.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_str_format_internal.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_synchronization.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_stacktrace.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_symbolize.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_debugging_internal.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_demangle_internal.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_graphcycles_internal.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_malloc_internal.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_time.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_strings.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_throw_delegate.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_strings_internal.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_base.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_spinlock_wait.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_int128.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_civil_time.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_time_zone.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_bad_optional_access.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_raw_logging_internal.lib")
#pragma comment(lib, "../lib/grpc/debug/absl_log_severity.lib")
#else
#pragma comment(lib, "../lib/grpc/release/grpc++_alts.lib")
#pragma comment(lib, "../lib/grpc/release/grpc_plugin_support.lib")
#pragma comment(lib, "../lib/grpc/release/grpc_unsecure.lib")
#pragma comment(lib, "../lib/grpc/release/grpcpp_channelz.lib")
#pragma comment(lib, "../lib/grpc/release/grpc++_error_details.lib")
#pragma comment(lib, "../lib/grpc/release/grpc++_reflection.lib")
#pragma comment(lib, "../lib/grpc/release/grpc.lib")
#pragma comment(lib, "../lib/grpc/release/grpc++.lib")
#pragma comment(lib, "../lib/grpc/release/gpr.lib")
#pragma comment(lib, "../lib/grpc/release/ssl.lib")
#pragma comment(lib, "../lib/grpc/release/crypto.lib")
#pragma comment(lib, "../lib/grpc/release/cares.lib")
#pragma comment(lib, "../lib/grpc/release/libprotobuf.lib")
#pragma comment(lib, "../lib/grpc/release/zlibstatic.lib")
#pragma comment(lib, "../lib/grpc/release/cares.lib")
#pragma comment(lib, "../lib/grpc/release/address_sorting.lib")
#pragma comment(lib, "../lib/grpc/release/re2.lib")
#pragma comment(lib, "../lib/grpc/release/absl_hash.lib")
#pragma comment(lib, "../lib/grpc/release/absl_city.lib")
#pragma comment(lib, "../lib/grpc/release/absl_wyhash.lib")
#pragma comment(lib, "../lib/grpc/release/absl_raw_hash_set.lib")
#pragma comment(lib, "../lib/grpc/release/absl_hashtablez_sampler.lib")
#pragma comment(lib, "../lib/grpc/release/absl_exponential_biased.lib")
#pragma comment(lib, "../lib/grpc/release/absl_statusor.lib")
#pragma comment(lib, "../lib/grpc/release/absl_bad_variant_access.lib")
#pragma comment(lib, "../lib/grpc/release/upb.lib")
#pragma comment(lib, "../lib/grpc/release/absl_status.lib")
#pragma comment(lib, "../lib/grpc/release/absl_cord.lib")
#pragma comment(lib, "../lib/grpc/release/absl_str_format_internal.lib")
#pragma comment(lib, "../lib/grpc/release/absl_synchronization.lib")
#pragma comment(lib, "../lib/grpc/release/absl_stacktrace.lib")
#pragma comment(lib, "../lib/grpc/release/absl_symbolize.lib")
#pragma comment(lib, "../lib/grpc/release/absl_debugging_internal.lib")
#pragma comment(lib, "../lib/grpc/release/absl_demangle_internal.lib")
#pragma comment(lib, "../lib/grpc/release/absl_graphcycles_internal.lib")
#pragma comment(lib, "../lib/grpc/release/absl_malloc_internal.lib")
#pragma comment(lib, "../lib/grpc/release/absl_time.lib")
#pragma comment(lib, "../lib/grpc/release/absl_strings.lib")
#pragma comment(lib, "../lib/grpc/release/absl_throw_delegate.lib")
#pragma comment(lib, "../lib/grpc/release/absl_strings_internal.lib")
#pragma comment(lib, "../lib/grpc/release/absl_base.lib")
#pragma comment(lib, "../lib/grpc/release/absl_spinlock_wait.lib")
#pragma comment(lib, "../lib/grpc/release/absl_int128.lib")
#pragma comment(lib, "../lib/grpc/release/absl_civil_time.lib")
#pragma comment(lib, "../lib/grpc/release/absl_time_zone.lib")
#pragma comment(lib, "../lib/grpc/release/absl_bad_optional_access.lib")
#pragma comment(lib, "../lib/grpc/release/absl_raw_logging_internal.lib")
#pragma comment(lib, "../lib/grpc/release/absl_log_severity.lib")
#endif

#include "GlobalDefine.h"
#include "ITObject.h"
class RpcSocketClient
{
public:
	RpcSocketClient();
	ITGameItemPtr GetCGItemData(int itemid);
	QMap<QString, QSharedPointer<CGPetPictorialBook> > GetPetGradeCalcData();
	void GetServerStoreMapData();
	void UploadGidData();
	void UploadGidBankData();
	void UploadMapData();
	bool DownloadMapData(QImage &image);
	bool SelectGidData(const QString &gid, int roleIndex, CGData::SelectGidDataResponse &reply);
	bool SelectCharacterData(const QString &sName, int nBigLine, CGData::SelectCharacterDataResponse &reply);

	bool GetConnectState();
	void StoreCGItemData(GameItemPtr pItem);
	void StoreCGMapData(ITGameMapPtr map);

	void init();
	void fini();
	static RpcSocketClient &getInstance();

	void setServerIp(const QString &sIP) { _serverIP = sIP; }
	void setServerPort(const QString &sPort) { _serverPort = sPort; }
	bool isConnected();

private:
	std::unique_ptr<CGRpcService::Stub> _stub;
	QString _serverIP;
	QString _serverPort;
};
