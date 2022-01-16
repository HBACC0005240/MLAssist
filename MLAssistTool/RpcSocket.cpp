#include "RpcSocket.h"
#include <QDebug>
#include <QtConcurrent>
#include "ITObjectDataMgr.h"
RpcSocket::RpcSocket()
{
	init();
}

RpcSocket::~RpcSocket()
{

}
RpcSocket& RpcSocket::getInstance()
{
	static RpcSocket instance;
	return instance;
}

void RpcSocket::init()
{
	std::string server_address("0.0.0.0:50051");
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&_service);
	_server = (builder.BuildAndStart());
	if (_server == nullptr)
	{
		qDebug() << "创建服务错误！";
	}
	else
	{
		std::cout << "Server listening on " << server_address << std::endl;
		QtConcurrent::run([=] {
			qDebug() << "RunServer() -> Thread: " << QThread::currentThreadId();
			this->_server->Wait();
		});
	}

}

void RpcSocket::fini()
{
	if (_server)
		_server->Shutdown();
}

GGRpcServiceImpl::GGRpcServiceImpl()
{

}

GGRpcServiceImpl::~GGRpcServiceImpl()
{

}

Status GGRpcServiceImpl::GetCGItemData(::grpc::ServerContext* context, const ::CGData::CGItemRequest* request, ::CGData::CGItemResponse* response)
{
	auto pItem = ITObjectDataMgr::getInstance().FindItemFromCode(request->item_id());
	if (pItem)
	{
		response->set_id(pItem->getObjectID());
		response->set_item_id(pItem->getObjectCode());
		response->set_name(pItem->getObjectName().toStdString());
		response->set_attribute(pItem->_itemAttr.toStdString());
		response->set_desc(pItem->getObjectDesc().toStdString());
		response->set_type(pItem->_itemType);
		response->set_price(pItem->_itemPrice);
		response->set_maxpile(pItem->_itemPile);
		response->set_level(pItem->_itemLevel);
		response->set_selllimitcount(pItem->_sellMinCount);
		response->set_exist(true);
	}
	else
		response->set_exist(false);
	return Status::OK;
}



Status GGRpcServiceImpl::StoreCGItemData(::grpc::ServerContext* context, const ::CGData::CGStoreItemRequest* request, ::CGData::CGStoreItemResponse* response)
{
	QString sMaybeName = ITObjectDataMgr::getInstance().StoreServerItemData(request);
	response->set_id(request->item_id());
	response->set_maybename(sMaybeName.toStdString());
	//qDebug() << QString::fromStdString(request->name());
	//qDebug() << request->item_id();
	//qDebug() << QString::fromStdString(request->attribute());
	return Status::OK;
}

Status GGRpcServiceImpl::StoreCGMapData(::grpc::ServerContext* context, const ::CGData::CGStoreMapRequest* request, ::CGData::CGStoreMapResponse* response)
{
	ITObjectDataMgr::getInstance().StoreServerMapData(QString::fromStdString(request->map_name()), request->map_number());
	return Status::OK;
}

Status GGRpcServiceImpl::UploadGidData(::grpc::ServerContext* context, const ::CGData::UploadGidDataRequest* request, ::CGData::UploadGidDataResponse* response)
{
	ITObjectDataMgr::getInstance().StoreUploadGidData(request);
	return Status::OK;
}

Status GGRpcServiceImpl::UploadGidBankData(::grpc::ServerContext* context, const ::CGData::UploadGidBankDataRequest* request, ::CGData::UploadGidBankDataResponse* response)
{
	ITObjectDataMgr::getInstance().StoreUploadGidBankData(request);
	return Status::OK;
}

Status GGRpcServiceImpl::GetPetGradeCalcData(::grpc::ServerContext* context, const ::CGData::CGPetGradeCalcRequest* request, ::CGData::CGPetGradeCalcResponse* response)
{
	auto pObjectList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_PetBook);
	for (auto pObj : pObjectList)
	{
		auto pPet = qSharedPointerCast<ITCGPetPictorialBook>(pObj);
		if (pPet)
		{
			CGData::CGPetGradeCalcData* petData = response->add_pet();
			petData->set_name(pPet->getObjectName().toStdString());
			petData->set_racetypename(pPet->_petRace.toStdString());
			petData->set_racetype(pPet->_petRaceType);
			petData->set_number(pPet->_petNumber);
			petData->add_bestbpgrade(pPet->_growVigor);
			petData->add_bestbpgrade(pPet->_growStrength);
			petData->add_bestbpgrade(pPet->_growDefense);
			petData->add_bestbpgrade(pPet->_growAgility);
			petData->add_bestbpgrade(pPet->_growMagic);
			petData->set_basebp(pPet->_petBaseBp);
			//petData->set_rate_endurance(pPet->_growVigor);
			//petData->set_rate_strength(pPet->_growStrength);
			//petData->set_rate_defense(pPet->_growDefense);
			//petData->set_rate_agility(pPet->_growAgility);
			//petData->set_rate_magical(pPet->_growMagic);
			petData->set_can_catch(pPet->_canCatch);
			petData->set_card_type(pPet->_cardType);
			petData->set_image_id(pPet->_imageId);
			petData->set_skill_slots(pPet->_skillCount);
			petData->set_element_earth(pPet->_elementEarth);
			petData->set_element_water(pPet->_elementWater);
			petData->set_element_fire(pPet->_elementFire);
			petData->set_element_wind(pPet->_elementWind);


		}
	}
	return Status::OK;
}

Status GGRpcServiceImpl::GetServerStoreMapData(::grpc::ServerContext* context, const ::CGData::CGMapDataRequest* request, ::CGData::CGMapDataResponse* response)
{
	auto pObjectList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_Map);
	for (auto tmpObj : pObjectList)
	{
		auto pObj = qSharedPointerCast<ITGameMap>(tmpObj);
		if (pObj)
		{
			CGData::CGMapData* mapData = response->add_maps();
			mapData->set_map_name(pObj->getObjectName().toStdString());
			mapData->set_id(pObj->getObjectID());
			mapData->set_map_number(pObj->_mapNumber);
			mapData->set_desc(pObj->getObjectDesc().toStdString());
			mapData->set_often(pObj->_oftenMap);
		}
	}
	return Status::OK;

}


Status GGRpcServiceImpl::GetConnectState(::grpc::ServerContext* context, const ::CGData::CGVoidData* request, ::CGData::CGVoidData* response)
{
	return Status::OK;
}

::grpc::Status GGRpcServiceImpl::Publish(::grpc::ServerContext* context, const ::CGData::StringPub* request, ::CGData::StringPub* response)
{
	return Status::OK;
}


::grpc::Status GGRpcServiceImpl::Subscribe(::grpc::ServerContext* context, const ::CGData::StringPub* request, ::grpc::ServerWriter< ::CGData::StringPub>* writer)
{
	return Status::OK;

}