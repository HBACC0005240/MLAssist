#include "RpcSocketClient.h"
#include "GPCalc.h"
#include "GameCtrl.h"
#include "ITObjectDataMgr.h"
#include "constDef.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
RpcSocketClient::RpcSocketClient()
{
}

void RpcSocketClient::init()
{
	QString tgtServer = _serverIP + ":" + _serverPort;
	qDebug() << "管理服务器:" << tgtServer;
	//target_str = "www.wzqlive.com:50051"; //"localhost:50051";
	_stub = CGRpcService::NewStub(grpc::CreateChannel(tgtServer.toStdString(), grpc::InsecureChannelCredentials()));
}

void RpcSocketClient::fini()
{
}

RpcSocketClient &RpcSocketClient::getInstance()
{
	static RpcSocketClient instance;
	return instance;
}

bool RpcSocketClient::isConnected()
{
	return _stub ? true : false;
}

ITGameItemPtr RpcSocketClient::GetCGItemData(int itemid)
{
	if (!isConnected())
		return nullptr;
	CGItemRequest request;
	request.set_item_id(itemid);

	CGItemResponse reply;
	ClientContext context;
	Status status = _stub->GetCGItemData(&context, request, &reply);

	// Act upon its status.
	if (status.ok() && reply.exist())
	{
		ITGameItemPtr pItem(new ITGameItem);
		pItem->_itemPile = reply.maxpile();
		pItem->setObjectCode(reply.item_id());
		pItem->setObjectName(QString::fromStdString(reply.name()));
		pItem->_itemType = reply.type();
		pItem->_itemPrice = reply.price();
		pItem->_itemLevel = reply.level();
		pItem->_sellMinCount = reply.selllimitcount();
		pItem->_itemAttr = QString::fromStdString(reply.attribute());
		pItem->setObjectDsec(QString::fromStdString(reply.desc()));
		return pItem;
	}
	else
	{
		std::cout << status.error_code() << ": " << status.error_message()
				  << std::endl;
		return nullptr;
	}
}

void RpcSocketClient::StoreCGItemData(GameItemPtr pItem)
{
	if (!pItem || !isConnected())
		return;

	CGStoreItemRequest request;
	request.set_item_id(pItem->id);
	request.set_name(pItem->name.toStdString());
	request.set_attribute(pItem->attr.toStdString());
	request.set_type(pItem->type);
	request.set_level(pItem->level);
	request.set_desc(pItem->info.toStdString());
	request.set_assessed(pItem->assessed);
	CGStoreItemResponse reply;
	ClientContext context;
	Status status = _stub->StoreCGItemData(&context, request, &reply);
	if (status.ok())
	{
		if (pItem->assessed == false && pItem->maybeName.toStdString() != reply.maybename())
		{
			pItem->maybeName = QString::fromStdString(reply.maybename());
		}
	}
	return;
}

void RpcSocketClient::StoreCGMapData(ITGameMapPtr map)
{
	if (!map || !isConnected())
		return;

	CGData::CGStoreMapRequest request;
	request.set_map_number(map->_mapNumber);
	request.set_map_name(map->getObjectName().toStdString());
	CGData::CGStoreMapResponse reply;
	ClientContext context;
	Status status = _stub->StoreCGMapData(&context, request, &reply);
	if (status.ok())
	{
	}
	return;
}

QMap<QString, QSharedPointer<CGPetPictorialBook> > RpcSocketClient::GetPetGradeCalcData()
{
	QMap<QString, QSharedPointer<CGPetPictorialBook> > petBookData;
	if (!isConnected())
		return petBookData;

	CGData::CGPetGradeCalcRequest request;
	CGData::CGPetGradeCalcResponse reply;
	ClientContext context;
	Status status = _stub->GetPetGradeCalcData(&context, request, &reply);
	if (status.ok())
	{
		int nsize = reply.pet_size();
		for (int i = 0; i < nsize; ++i)
		{
			auto petData = reply.pet(i);
			QSharedPointer<CGPetPictorialBook> pPet(new CGPetPictorialBook);
			pPet->number = petData.number();

			pPet->name = QString::fromStdString(petData.name());
			pPet->raceType = petData.racetype();
			for (int i = 0; i < petData.bestbpgrade_size(); ++i)
			{
				pPet->bestBPGrade[i] = petData.bestbpgrade(i);
			}
			pPet->raceTypeName = QString::fromStdString(petData.racetypename());
			pPet->can_catch = petData.can_catch();
			pPet->card_type = petData.card_type();
			pPet->image_id = petData.image_id();
			pPet->rate_endurance = petData.rate_endurance();
			pPet->rate_strength = petData.rate_strength();
			pPet->rate_defense = petData.rate_defense();
			pPet->rate_agility = petData.rate_agility();
			pPet->rate_magical = petData.rate_magical();
			pPet->element_earth = petData.element_earth();
			pPet->element_water = petData.element_water();
			pPet->element_fire = petData.element_fire();
			pPet->element_wind = petData.element_wind();
			pPet->skill_slots = petData.skill_slots();
			petBookData.insert(pPet->name, pPet);
		}
	}
	else
	{
		qDebug() << status.error_code() << ": " << status.error_message().c_str();
	}
	return petBookData;
}

void RpcSocketClient::GetServerStoreMapData()
{
	if (!isConnected())
		return;

	CGData::CGMapDataRequest request;
	CGData::CGMapDataResponse reply;
	ClientContext context;
	Status status = _stub->GetServerStoreMapData(&context, request, &reply);
	if (status.ok())
	{
		int nsize = reply.maps_size();
		for (int i = 0; i < nsize; ++i)
		{
			auto mapData = reply.maps(i);
			ITGameMapPtr pObj = ITObjectDataMgr::getInstance().newOneObject(TObject_Map, mapData.id()).dynamicCast<ITGameMap>();
			if (pObj)
			{
				pObj->_mapNumber = mapData.map_number();
				pObj->_oftenMap = mapData.often();
				pObj->setObjectName(QString::fromStdString(mapData.map_name()));
				pObj->setObjectDsec(QString::fromStdString(mapData.desc()));
				ITObjectDataMgr::getInstance().addObjectToObjectList(pObj);
				ITObjectDataMgr::getInstance().insertMapNumForObj(pObj->_mapNumber, pObj);
			}
		}
	}
	else
	{
		qDebug() << status.error_code() << ": " << status.error_message().c_str();
	}
}

void RpcSocketClient::UploadGidData()
{
	if (!isConnected())
		return;

	auto pChara = g_pGameFun->GetGameCharacter();
	if (!pChara)
		return;

	if (pChara->level == 0)
	{
		return;
	}
	CGData::UploadGidDataRequest request;
	request.set_gid(g_pGameCtrl->GetGameGid().toStdString());
	request.set_character_name(pChara->name.toStdString());
	CGData::CGCharacterData *charData = new CGData::CGCharacterData;
	request.set_allocated_character_data(charData);

	//基础信息
	{

		CGData::CGBaseData *base_data = new CGData::CGBaseData;
		charData->set_allocated_base_data(base_data);
		base_data->set_name(pChara->name.toStdString());
		base_data->set_hp(pChara->hp);
		base_data->set_maxhp(pChara->maxhp);
		base_data->set_mp(pChara->mp);
		base_data->set_maxmp(pChara->maxmp);
		base_data->set_xp(pChara->xp);
		base_data->set_maxxp(pChara->maxxp);
		base_data->set_level(pChara->level);
		base_data->set_health(pChara->health);

		charData->set_souls(pChara->souls);
		charData->set_level(pChara->level);
		charData->set_gold(pChara->gold);
		charData->set_image_id(pChara->image_id);
		charData->set_score(pChara->score);
		charData->set_skillslots(pChara->skillslots);
		charData->set_use_title(pChara->use_title);
		charData->set_avatar_id(pChara->avatar_id);
		charData->set_unitid(pChara->unitid);
		charData->set_petid(pChara->petid);
		charData->set_petriding(pChara->petriding);
		charData->set_direction(pChara->direction);
		charData->set_punchclock(pChara->punchclock);
		charData->set_usingpunchclock(pChara->usingpunchclock);
		charData->set_job(pChara->job.toStdString());
		CGA::cga_player_info_t info;
		if (g_CGAInterface->GetPlayerInfo(info))
		{
			for (int i = 0; i < info.titles.size(); ++i)
			{
				charData->add_titles(info.titles[i]);
			}
		}

		charData->set_manu_endurance(pChara->manu_endurance);
		charData->set_manu_skillful(pChara->manu_skillful);
		charData->set_manu_intelligence(pChara->manu_intelligence);
		charData->set_value_charisma(pChara->value_charisma);
		QPoint mapPos = g_pGameFun->GetMapCoordinate();
		charData->set_x(mapPos.x());
		charData->set_y(mapPos.y());
		std::string sMapName;
		g_CGAInterface->GetMapName(sMapName);
		charData->set_map_name(sMapName);
		charData->set_map_number(g_pGameFun->GetMapIndex());
		
		charData->set_server_line(g_pGameFun->GetGameServerLine());
		CGData::CGAttrBaseData *charDetail = new CGData::CGAttrBaseData;
		charData->set_allocated_detail(charDetail);
		charDetail->set_element_earth(pChara->detail.element_earth);
		charDetail->set_element_water(pChara->detail.element_water);
		charDetail->set_element_fire(pChara->detail.element_fire);
		charDetail->set_element_wind(pChara->detail.element_wind);
		charDetail->set_points_remain(pChara->detail.points_remain);
		charDetail->set_points_endurance(pChara->detail.points_endurance);
		charDetail->set_points_strength(pChara->detail.points_strength);
		charDetail->set_points_defense(pChara->detail.points_defense);
		charDetail->set_points_agility(pChara->detail.points_agility);
		charDetail->set_points_magical(pChara->detail.points_magical);
		charDetail->set_value_attack(pChara->detail.value_attack);
		charDetail->set_value_defensive(pChara->detail.value_defensive);
		charDetail->set_value_agility(pChara->detail.value_agility);
		charDetail->set_value_spirit(pChara->detail.value_spirit);
		charDetail->set_value_recovery(pChara->detail.value_recovery);
		charDetail->set_resist_poison(pChara->detail.resist_poison);
		charDetail->set_resist_sleep(pChara->detail.resist_sleep);
		charDetail->set_resist_medusa(pChara->detail.resist_medusa);
		charDetail->set_resist_drunk(pChara->detail.resist_drunk);
		charDetail->set_resist_chaos(pChara->detail.resist_chaos);
		charDetail->set_resist_forget(pChara->detail.resist_forget);
		charDetail->set_fix_critical(pChara->detail.fix_critical);
		charDetail->set_fix_strikeback(pChara->detail.fix_strikeback);
		charDetail->set_fix_accurancy(pChara->detail.fix_accurancy);
		charDetail->set_fix_dodge(pChara->detail.fix_dodge);
	}
	auto pSkillList = g_pGameFun->GetGameSkills();
	for (int i = 0; i < pSkillList.size(); ++i)
	{
		auto pSkill = pSkillList[i];
		auto charSkill = charData->add_skill();
		charSkill->set_name(pSkill->name.toStdString());
		charSkill->set_info(pSkill->info.toStdString());
		charSkill->set_id(pSkill->id);
		charSkill->set_flags(pSkill->flags);
		charSkill->set_index(pSkill->index);
		charSkill->set_level(pSkill->level);
		charSkill->set_maxlevel(pSkill->maxLevel);
		charSkill->set_available(pSkill->available);
		charSkill->set_xp(pSkill->xp);
		charSkill->set_maxxp(pSkill->maxxp);

		auto pSubSkills = pSkill->subskills;
		for (int i = 0; i < pSubSkills.size(); ++i)
		{
			auto pSubSkill = pSubSkills[i];
			auto charSubSkill = charSkill->add_subskills();
			charSubSkill->set_name(pSubSkill->name.toStdString());
			charSubSkill->set_info(pSubSkill->info.toStdString());
			charSubSkill->set_id(pSubSkill->id);
			charSubSkill->set_flags(pSubSkill->flags);
			charSubSkill->set_index(pSubSkill->index);
			charSubSkill->set_level(pSubSkill->level);
			charSubSkill->set_maxlevel(pSubSkill->maxLevel);
			charSubSkill->set_available(pSubSkill->available);
			charSubSkill->set_xp(pSubSkill->xp);
			charSubSkill->set_maxxp(pSubSkill->maxxp);
		}
	}

	auto pPets = g_pGameFun->GetGamePets();
	for (auto pPet : pPets)
	{
		if (pPet->exist)
		{
			auto charPetData = request.add_pet_data();
			CGData::CGBaseData *base_data = new CGData::CGBaseData;
			charPetData->set_allocated_base_data(base_data);
			base_data->set_name(pPet->name.toStdString());
			base_data->set_hp(pPet->hp);
			base_data->set_maxhp(pPet->maxhp);
			base_data->set_mp(pPet->mp);
			base_data->set_maxmp(pPet->maxmp);
			base_data->set_xp(pPet->xp);
			base_data->set_maxxp(pPet->maxxp);
			base_data->set_level(pPet->level);
			base_data->set_health(pPet->health);
			charPetData->set_index(pPet->index);

			CGData::CGAttrBaseData *charDetail = new CGData::CGAttrBaseData;
			charPetData->set_allocated_detail(charDetail);
			charDetail->set_element_earth(pPet->detail.element_earth);
			charDetail->set_element_water(pPet->detail.element_water);
			charDetail->set_element_fire(pPet->detail.element_fire);
			charDetail->set_element_wind(pPet->detail.element_wind);
			charDetail->set_points_remain(pPet->detail.points_remain);
			charDetail->set_points_endurance(pPet->detail.points_endurance);
			charDetail->set_points_strength(pPet->detail.points_strength);
			charDetail->set_points_defense(pPet->detail.points_defense);
			charDetail->set_points_agility(pPet->detail.points_agility);
			charDetail->set_points_magical(pPet->detail.points_magical);
			charDetail->set_value_attack(pPet->detail.value_attack);
			charDetail->set_value_defensive(pPet->detail.value_defensive);
			charDetail->set_value_agility(pPet->detail.value_agility);
			charDetail->set_value_spirit(pPet->detail.value_spirit);
			charDetail->set_value_recovery(pPet->detail.value_recovery);
			charDetail->set_resist_poison(pPet->detail.resist_poison);
			charDetail->set_resist_sleep(pPet->detail.resist_sleep);
			charDetail->set_resist_medusa(pPet->detail.resist_medusa);
			charDetail->set_resist_drunk(pPet->detail.resist_drunk);
			charDetail->set_resist_chaos(pPet->detail.resist_chaos);
			charDetail->set_resist_forget(pPet->detail.resist_forget);
			charDetail->set_fix_critical(pPet->detail.fix_critical);
			charDetail->set_fix_strikeback(pPet->detail.fix_strikeback);
			charDetail->set_fix_accurancy(pPet->detail.fix_accurancy);
			charDetail->set_fix_dodge(pPet->detail.fix_dodge);

			charPetData->set_flags(pPet->flags);
			charPetData->set_battle_flags(pPet->battle_flags);
			charPetData->set_loyality(pPet->loyality);
			charPetData->set_default_battle(pPet->default_battle);
			charPetData->set_state(pPet->state);
			charPetData->set_grade(pPet->grade);
			charPetData->set_lossmingrade(pPet->lossMinGrade);
			charPetData->set_lossmaxgrade(pPet->lossMaxGrade);
			charPetData->set_real_name(pPet->realname.toStdString());
			charPetData->set_race(pPet->race);
			charPetData->set_skillslots(pPet->skillslots);


			auto pSkillList = pPet->skills;
			for (int i = 0; i < pSkillList.size(); ++i)
			{
				auto pSkill = pSkillList[i];
				auto charSkill = charData->add_skill();
				charSkill->set_name(pSkill->name.toStdString());
				charSkill->set_info(pSkill->info.toStdString());
				charSkill->set_id(pSkill->id);
				charSkill->set_flags(pSkill->flags);
				charSkill->set_index(pSkill->index);
				charSkill->set_level(pSkill->level);
				charSkill->set_maxlevel(pSkill->maxLevel);
				charSkill->set_available(pSkill->available);
				charSkill->set_xp(pSkill->xp);
				charSkill->set_maxxp(pSkill->maxxp);
			}
		}
	}
	auto pItems = g_pGameFun->GetGameItems();
	for (auto pItem : pItems)
	{
		if (pItem->exist)
		{
			auto charItemData = request.add_items();
			charItemData->set_item_id(pItem->id);
			charItemData->set_name(pItem->name.toStdString());
			charItemData->set_attr(pItem->attr.toStdString());
			charItemData->set_info(pItem->info.toStdString());
			charItemData->set_type(pItem->type);
			charItemData->set_level(pItem->level);
			charItemData->set_pos(pItem->pos);
			charItemData->set_assessed(pItem->assessed);
			charItemData->set_count(pItem->count);
		}
	}

	CGData::UploadGidDataResponse reply;
	ClientContext context;
	Status status = _stub->UploadGidData(&context, request, &reply);
	if (status.ok())
	{
	}
	else
	{
		qDebug() << status.error_code() << ": " << status.error_message().c_str();
	}
}

void RpcSocketClient::UploadGidBankData()
{
	if (!isConnected())
		return;

	auto pChara = g_pGameFun->GetGameCharacter();
	if (!pChara)
		return;
	if (pChara->level == 0)
	{
		return;
	}
	CGData::UploadGidBankDataRequest request;
	request.set_gid(g_pGameCtrl->GetGameGid().toStdString());
	request.set_character_name(pChara->name.toStdString());

	int bankGold = 0;
	g_CGAInterface->GetBankGold(bankGold);
	request.set_gold(bankGold);
	CGA::cga_pets_info_t bankPets;
	g_CGAInterface->GetBankPetsInfo(bankPets);
	for (auto pPet : bankPets)
	{
		auto charPetData = request.add_pet_data();
		CGData::CGBaseData *base_data = new CGData::CGBaseData;
		charPetData->set_allocated_base_data(base_data);
		base_data->set_name(pPet.name);
		base_data->set_hp(pPet.hp);
		base_data->set_maxhp(pPet.maxhp);
		base_data->set_mp(pPet.mp);
		base_data->set_maxmp(pPet.maxmp);
		base_data->set_xp(pPet.xp);
		base_data->set_maxxp(pPet.maxxp);
		base_data->set_level(pPet.level);
		base_data->set_health(pPet.health);
		charPetData->set_index(pPet.index);

		CGData::CGAttrBaseData *charDetail = new CGData::CGAttrBaseData;
		charPetData->set_allocated_detail(charDetail);
		charDetail->set_element_earth(pPet.detail.element_earth);
		charDetail->set_element_water(pPet.detail.element_water);
		charDetail->set_element_fire(pPet.detail.element_fire);
		charDetail->set_element_wind(pPet.detail.element_wind);
		charDetail->set_points_remain(pPet.detail.points_remain);
		charDetail->set_points_endurance(pPet.detail.points_endurance);
		charDetail->set_points_strength(pPet.detail.points_strength);
		charDetail->set_points_defense(pPet.detail.points_defense);
		charDetail->set_points_agility(pPet.detail.points_agility);
		charDetail->set_points_magical(pPet.detail.points_magical);
		charDetail->set_value_attack(pPet.detail.value_attack);
		charDetail->set_value_defensive(pPet.detail.value_defensive);
		charDetail->set_value_agility(pPet.detail.value_agility);
		charDetail->set_value_spirit(pPet.detail.value_spirit);
		charDetail->set_value_recovery(pPet.detail.value_recovery);
		charDetail->set_resist_poison(pPet.detail.resist_poison);
		charDetail->set_resist_sleep(pPet.detail.resist_sleep);
		charDetail->set_resist_medusa(pPet.detail.resist_medusa);
		charDetail->set_resist_drunk(pPet.detail.resist_drunk);
		charDetail->set_resist_chaos(pPet.detail.resist_chaos);
		charDetail->set_resist_forget(pPet.detail.resist_forget);
		charDetail->set_fix_critical(pPet.detail.fix_critical);
		charDetail->set_fix_strikeback(pPet.detail.fix_strikeback);
		charDetail->set_fix_accurancy(pPet.detail.fix_accurancy);
		charDetail->set_fix_dodge(pPet.detail.fix_dodge);

		charPetData->set_flags(pPet.flags);
		charPetData->set_battle_flags(pPet.battle_flags);
		charPetData->set_loyality(pPet.loyality);
		charPetData->set_state(pPet.state);
		if (pPet.level == 1)
		{
			QStringList inputData = g_pGameFun->GetPetCalcBpData(pPet);
			auto pCalcData = g_pGamePetCalc->ParseLine(inputData);
			if (pCalcData)
			{
				charPetData->set_grade(pCalcData->lossMin);
				charPetData->set_lossmingrade(pCalcData->lossMin);
				charPetData->set_lossmaxgrade(pCalcData->lossMax);
			}
		}
		
		charPetData->set_real_name(pPet.realname);
		charPetData->set_race(pPet.race);
		charPetData->set_skillslots(pPet.skillslots);



		/*auto pSkillList = pPet.skills;
		for (int i = 0; i < pSkillList.size(); ++i)
		{
			auto pSkill = pSkillList[i];
			auto charSkill = charData->add_skill();
			charSkill->set_name(pSkill->name.toStdString());
			charSkill->set_info(pSkill->info.toStdString());
			charSkill->set_id(pSkill->id);
			charSkill->set_flags(pSkill->flags);
			charSkill->set_index(pSkill->index);
			charSkill->set_level(pSkill->level);
			charSkill->set_maxlevel(pSkill->maxLevel);
			charSkill->set_available(pSkill->available);
			charSkill->set_xp(pSkill->xp);
			charSkill->set_maxxp(pSkill->maxxp);
		}*/
	}
	CGA::cga_items_info_t myinfos;
	if (g_CGAInterface->GetBankItemsInfo(myinfos))
	{
		for (int i = 0; i < myinfos.size(); ++i)
		{
			auto pItem = myinfos[i];
			auto charItemData = request.add_items();
			charItemData->set_item_id(pItem.itemid);
			charItemData->set_name(pItem.name);
			charItemData->set_attr(pItem.attr);
			charItemData->set_info(pItem.info);
			charItemData->set_type(pItem.type);
			charItemData->set_level(pItem.level);
			charItemData->set_pos(pItem.pos);
			charItemData->set_assessed(pItem.assessed);
		}
	}

	CGData::UploadGidBankDataResponse reply;
	ClientContext context;
	Status status = _stub->UploadGidBankData(&context, request, &reply);
	if (status.ok())
	{
	}
	else
	{
		qDebug() << status.error_code() << ": " << status.error_message().c_str();
	}
}

bool RpcSocketClient::GetConnectState()
{
	if (!isConnected())
		return false;

	CGData::CGVoidData request;

	CGData::CGVoidData reply;
	ClientContext context;
	Status status = _stub->GetConnectState(&context, request, &reply);
	if (status.ok())
		return true;
	return false;
}