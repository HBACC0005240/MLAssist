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
using CGData::UploadMapDataRequest;
using CGData::UploadMapDataResponse;
using CGData::SelectCharacterServerRequest;
using CGData::SelectCharacterServerResponse;
using CGData::SelectCharacterDataRequest;
using CGData::SelectCharacterDataResponse;
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

	Status UploadLocalPCData(::grpc::ServerContext *context, const ::CGData::UploadLocalPCInfoRequest *request, ::CGData::UploadLocalPCInfoResponse *response);
	Status UploadGidData(::grpc::ServerContext* context, const ::CGData::UploadGidDataRequest* request, ::CGData::UploadGidDataResponse* response)override;
	Status UploadGidBankData(::grpc::ServerContext* context, const ::CGData::UploadGidBankDataRequest* request, ::CGData::UploadGidBankDataResponse* response)override;
	Status UploadMapData(::grpc::ServerContext* context, ::grpc::ServerReader< ::CGData::UploadMapDataRequest>* stream, ::CGData::UploadMapDataResponse* response)override;
	Status DownloadMapData(::grpc::ServerContext* context, const ::CGData::DownloadMapDataRequest* request, ::grpc::ServerWriter< ::CGData::DownloadMapDataResponse>* writer)override;
	Status UploadCharcterServer(::grpc::ServerContext *context, const ::CGData::UploadCharcterServerRequest *request, ::CGData::UploadCharcterServerResponse *response) override;

	Status SelectGidData(::grpc::ServerContext* context, const ::CGData::SelectGidDataRequest* request, ::CGData::SelectGidDataResponse* response)override;
	Status SelectAccountGidData(::grpc::ServerContext* context, const ::CGData::SelectAccountGidDataRequest* request, ::CGData::SelectAccountGidDataResponse* response)override;
	Status SelectCharacterServer(::grpc::ServerContext *context, const ::CGData::SelectCharacterServerRequest *request, ::CGData::SelectCharacterServerResponse *response) override;
	Status SelectCharacterData(::grpc::ServerContext *context, const ::CGData::SelectCharacterDataRequest *request, ::CGData::SelectCharacterDataResponse *response) override;

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

