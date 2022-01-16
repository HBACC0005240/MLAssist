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
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
#pragma comment(lib, "../lib/grpc/grpc++_alts.lib")
#pragma comment(lib, "../lib/grpc/grpc_plugin_support.lib")
#pragma comment(lib, "../lib/grpc/grpc_unsecure.lib")
#pragma comment(lib, "../lib/grpc/grpcpp_channelz.lib")
#pragma comment(lib, "../lib/grpc/grpc++_error_details.lib")
#pragma comment(lib, "../lib/grpc/grpc++_reflection.lib")
#pragma comment(lib, "../lib/grpc/grpc.lib")
#pragma comment(lib, "../lib/grpc/grpc++.lib")
#pragma comment(lib, "../lib/grpc/gpr.lib")
#pragma comment(lib, "../lib/grpc/ssl.lib")
#pragma comment(lib, "../lib/grpc/crypto.lib")
#pragma comment(lib, "../lib/grpc/cares.lib")
#pragma comment(lib, "../lib/grpc/libprotobuf.lib")
#pragma comment(lib, "../lib/grpc/zlibstatic.lib")
#pragma comment(lib, "../lib/grpc/cares.lib")
#pragma comment(lib, "../lib/grpc/address_sorting.lib")
#pragma comment(lib, "../lib/grpc/re2.lib")
#pragma comment(lib, "../lib/grpc/absl_hash.lib")
#pragma comment(lib, "../lib/grpc/absl_city.lib")
#pragma comment(lib, "../lib/grpc/absl_wyhash.lib")
#pragma comment(lib, "../lib/grpc/absl_raw_hash_set.lib")
#pragma comment(lib, "../lib/grpc/absl_hashtablez_sampler.lib")
#pragma comment(lib, "../lib/grpc/absl_exponential_biased.lib")
#pragma comment(lib, "../lib/grpc/absl_statusor.lib")
#pragma comment(lib, "../lib/grpc/absl_bad_variant_access.lib")
#pragma comment(lib, "../lib/grpc/upb.lib")
#pragma comment(lib, "../lib/grpc/absl_status.lib")
#pragma comment(lib, "../lib/grpc/absl_cord.lib")
#pragma comment(lib, "../lib/grpc/absl_str_format_internal.lib")
#pragma comment(lib, "../lib/grpc/absl_synchronization.lib")
#pragma comment(lib, "../lib/grpc/absl_stacktrace.lib")
#pragma comment(lib, "../lib/grpc/absl_symbolize.lib")
#pragma comment(lib, "../lib/grpc/absl_debugging_internal.lib")
#pragma comment(lib, "../lib/grpc/absl_demangle_internal.lib")
#pragma comment(lib, "../lib/grpc/absl_graphcycles_internal.lib")
#pragma comment(lib, "../lib/grpc/absl_malloc_internal.lib")
#pragma comment(lib, "../lib/grpc/absl_time.lib")
#pragma comment(lib, "../lib/grpc/absl_strings.lib")
#pragma comment(lib, "../lib/grpc/absl_throw_delegate.lib")
#pragma comment(lib, "../lib/grpc/absl_strings_internal.lib")
#pragma comment(lib, "../lib/grpc/absl_base.lib")
#pragma comment(lib, "../lib/grpc/absl_spinlock_wait.lib")
#pragma comment(lib, "../lib/grpc/absl_int128.lib")
#pragma comment(lib, "../lib/grpc/absl_civil_time.lib")
#pragma comment(lib, "../lib/grpc/absl_time_zone.lib")
#pragma comment(lib, "../lib/grpc/absl_bad_optional_access.lib")
#pragma comment(lib, "../lib/grpc/absl_raw_logging_internal.lib")
#pragma comment(lib, "../lib/grpc/absl_log_severity.lib")

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
