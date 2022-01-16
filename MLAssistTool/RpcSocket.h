#pragma once
#include <QSharedPointer>
#include "grpcpp/grpcpp.h"

#include "../protos/cgdata.pb.h"
#include "../protos/cgdata.grpc.pb.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using CGData::CGRpcService;
using CGData::CGItemRequest;
using CGData::CGItemResponse;
#pragma comment(lib,"../lib/grpc/grpc++_alts.lib")
#pragma comment(lib,"../lib/grpc/grpc_plugin_support.lib")
#pragma comment(lib,"../lib/grpc/grpc_unsecure.lib")
#pragma comment(lib,"../lib/grpc/grpcpp_channelz.lib")
#pragma comment(lib,"../lib/grpc/grpc++_error_details.lib")
#pragma comment(lib,"../lib/grpc/grpc++_reflection.lib")
#pragma comment(lib,"../lib/grpc/grpc.lib")
#pragma comment(lib,"../lib/grpc/grpc++.lib")
#pragma comment(lib,"../lib/grpc/gpr.lib")
#pragma comment(lib,"../lib/grpc/ssl.lib")
#pragma comment(lib,"../lib/grpc/crypto.lib")
#pragma comment(lib,"../lib/grpc/cares.lib")
#pragma comment(lib,"../lib/grpc/libprotobuf.lib")
#pragma comment(lib,"../lib/grpc/zlibstatic.lib")
#pragma comment(lib,"../lib/grpc/cares.lib")
#pragma comment(lib,"../lib/grpc/address_sorting.lib")
#pragma comment(lib,"../lib/grpc/re2.lib")
#pragma comment(lib,"../lib/grpc/absl_hash.lib")
#pragma comment(lib,"../lib/grpc/absl_city.lib")
#pragma comment(lib,"../lib/grpc/absl_wyhash.lib")
#pragma comment(lib,"../lib/grpc/absl_raw_hash_set.lib")
#pragma comment(lib,"../lib/grpc/absl_hashtablez_sampler.lib")
#pragma comment(lib,"../lib/grpc/absl_exponential_biased.lib")
#pragma comment(lib,"../lib/grpc/absl_statusor.lib")
#pragma comment(lib,"../lib/grpc/absl_bad_variant_access.lib")
#pragma comment(lib,"../lib/grpc/upb.lib")
#pragma comment(lib,"../lib/grpc/absl_status.lib")
#pragma comment(lib,"../lib/grpc/absl_cord.lib")
#pragma comment(lib,"../lib/grpc/absl_str_format_internal.lib")
#pragma comment(lib,"../lib/grpc/absl_synchronization.lib")
#pragma comment(lib,"../lib/grpc/absl_stacktrace.lib")
#pragma comment(lib,"../lib/grpc/absl_symbolize.lib")
#pragma comment(lib,"../lib/grpc/absl_debugging_internal.lib")
#pragma comment(lib,"../lib/grpc/absl_demangle_internal.lib")
#pragma comment(lib,"../lib/grpc/absl_graphcycles_internal.lib")
#pragma comment(lib,"../lib/grpc/absl_malloc_internal.lib")
#pragma comment(lib,"../lib/grpc/absl_time.lib")
#pragma comment(lib,"../lib/grpc/absl_strings.lib")
#pragma comment(lib,"../lib/grpc/absl_throw_delegate.lib")
#pragma comment(lib,"../lib/grpc/absl_strings_internal.lib")
#pragma comment(lib,"../lib/grpc/absl_base.lib")
#pragma comment(lib,"../lib/grpc/absl_spinlock_wait.lib")
#pragma comment(lib,"../lib/grpc/absl_int128.lib")
#pragma comment(lib,"../lib/grpc/absl_civil_time.lib")
#pragma comment(lib,"../lib/grpc/absl_time_zone.lib")
#pragma comment(lib,"../lib/grpc/absl_bad_optional_access.lib")
#pragma comment(lib,"../lib/grpc/absl_raw_logging_internal.lib")
#pragma comment(lib,"../lib/grpc/absl_log_severity.lib")

class GGRpcServiceImpl final : public CGRpcService::Service
{
public:
	GGRpcServiceImpl();
	~GGRpcServiceImpl();
	Status GetCGItemData(::grpc::ServerContext* context, const ::CGData::CGItemRequest* request, ::CGData::CGItemResponse* response)override;
	Status GetPetGradeCalcData(::grpc::ServerContext* context, const ::CGData::CGPetGradeCalcRequest* request, ::CGData::CGPetGradeCalcResponse* response)override;
	Status GetServerStoreMapData(::grpc::ServerContext* context, const ::CGData::CGMapDataRequest* request, ::CGData::CGMapDataResponse* response)override;
	Status GetConnectState(::grpc::ServerContext* context, const ::CGData::CGVoidData* request, ::CGData::CGVoidData* response)override;

	Status StoreCGItemData(::grpc::ServerContext* context, const ::CGData::CGStoreItemRequest* request, ::CGData::CGStoreItemResponse* response)override;
	Status StoreCGMapData(::grpc::ServerContext* context, const ::CGData::CGStoreMapRequest* request, ::CGData::CGStoreMapResponse* response)override;

	Status UploadGidData(::grpc::ServerContext* context, const ::CGData::UploadGidDataRequest* request, ::CGData::UploadGidDataResponse* response)override;
	Status UploadGidBankData(::grpc::ServerContext* context, const ::CGData::UploadGidBankDataRequest* request, ::CGData::UploadGidBankDataResponse* response)override;


	::grpc::Status Publish(::grpc::ServerContext* context, const ::CGData::StringPub* request, ::CGData::StringPub* response);
	// 订阅则是一个单向的流服务，服务端返回的数据可能很大
	::grpc::Status Subscribe(::grpc::ServerContext* context, const ::CGData::StringPub* request, ::grpc::ServerWriter< ::CGData::StringPub>* writer);


};

class RpcSocket
{
public:
	RpcSocket();
	~RpcSocket();

	void init();
	void fini();
	static RpcSocket& getInstance();
private:
	GGRpcServiceImpl _service;

	std::unique_ptr<grpc::Server> _server;
};

