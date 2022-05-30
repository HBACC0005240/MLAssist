#include "CGAPython.h"

namespace py = pybind11;



CGAPython::CGAPython()
{
	m_interface = CGA::CreateInterface();
}

bool CGAPython::IsConnected()
{
	return m_interface->IsConnected();
}

void CGAPython::Disconnect()
{
	m_interface->Disconnect();
}

bool CGAPython::RegisterChatMsgNotify(const std::function<void(CGA::cga_chat_msg_t)>& f)
{
	return m_interface->RegisterChatMsgNotify(std::bind(f, std::placeholders::_1));
}

bool CGAPython::RegisterServerShutdownNotify(const std::function<void(int)>& callback)
{
	return m_interface->RegisterServerShutdownNotify(std::bind(callback, std::placeholders::_1));
}

bool CGAPython::RegisterGameWndKeyDownNotify(const std::function<void(unsigned int)>& callback)
{
	return m_interface->RegisterGameWndKeyDownNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterBattleActionNotify(const std::function<void(int)>& callback)
{
	return m_interface->RegisterBattleActionNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterPlayerMenuNotify(const std::function<void(cga_player_menu_items_t)>& callback)
{
	return m_interface->RegisterPlayerMenuNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterUnitMenuNotify(const std::function<void(cga_unit_menu_items_t)>& callback)
{
	return m_interface->RegisterUnitMenuNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterNPCDialogNotify(const std::function<void(cga_npc_dialog_t)>& callback)
{
	return m_interface->RegisterNPCDialogNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterWorkingResultNotify(const std::function<void(cga_working_result_t)>& callback)
{
	return m_interface->RegisterWorkingResultNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterTradeStuffsNotify(const std::function<void(cga_trade_stuff_info_t)>& callback)
{
	return m_interface->RegisterTradeStuffsNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterTradeDialogNotify(const std::function<void(cga_trade_dialog_t)>& callback)
{
	return m_interface->RegisterTradeDialogNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterTradeStateNotify(const std::function<void(int)>& callback)
{
	return m_interface->RegisterTradeStateNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterDownloadMapNotify(const std::function<void(cga_download_map_t)>& callback)
{
	return m_interface->RegisterDownloadMapNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::RegisterConnectionStateNotify(const std::function<void(cga_conn_state_t)>& callback)
{
	return m_interface->RegisterConnectionStateNotify(std::bind(callback, std::placeholders::_1));

}

bool CGAPython::Connect(int port)
{
	bool bConnect = m_interface->Connect(port);
	if (bConnect)
	{
		//	m_interface->RegisterChatMsgNotify(std::bind(&ChatMsgNotify, std::placeholders::_1));
	}
	return bConnect;
}

bool CGAPython::Initialize(cga_game_data_t& data)
{
	return m_interface->Initialize(data);
}

int CGAPython::IsInGame()
{
	int ingame = 0;
	m_interface->IsInGame(ingame);
	return ingame;
}

int CGAPython::GetWorldStatus()
{
	int val = 0;
	m_interface->GetWorldStatus(val);
	return val;
}

int CGAPython::GetGameStatus()
{
	int val = 0;
	m_interface->GetGameStatus(val);
	return val;
}

int CGAPython::GetBGMIndex()
{
	int val = 0;
	m_interface->GetBGMIndex(val);
	return val;
}

CGA::cga_sys_time_t CGAPython::GetSysTime()
{
	CGA::cga_sys_time_t gameTime;
	m_interface->GetSysTime(gameTime);
	return gameTime;
}

CGA::cga_player_info_t CGAPython::GetPlayerInfo()
{
	CGA::cga_player_info_t info;
	m_interface->GetPlayerInfo(info);
	return info;
}

bool CGAPython::SetPlayerFlagEnabled(int index, bool enable)
{
	return m_interface->SetPlayerFlagEnabled(index, enable);
}

bool CGAPython::IsPlayerFlagEnabled(int index)
{
	bool bEnable = false;
	m_interface->IsPlayerFlagEnabled(index, bEnable);
	return bEnable;
}

bool CGAPython::IsPetValid(int petid)
{
	bool bVal = false;
	m_interface->IsPetValid(petid, bVal);
	return bVal;
}

CGA::cga_pet_info_t CGAPython::GetPetInfo(int petid)
{
	CGA::cga_pet_info_t info;
	m_interface->GetPetInfo(petid, info);
	return info;
}

CGA::cga_pets_info_t CGAPython::GetPetsInfo()
{
	CGA::cga_pets_info_t infos;
	m_interface->GetPetsInfo(infos);
	return infos;
}

CGA::cga_pets_info_t CGAPython::GetBankPetsInfo()
{
	CGA::cga_pets_info_t infos;
	m_interface->GetBankPetsInfo(infos);
	return infos;
}

bool CGAPython::IsPetSkillValid(int petid, int skillid)
{
	bool bVal = false;
	m_interface->IsPetSkillValid(petid, skillid, bVal);
	return bVal;
}

CGA::cga_pet_skill_info_t CGAPython::GetPetSkillInfo(int petid, int skillid)
{
	CGA::cga_pet_skill_info_t infos;
	m_interface->GetPetSkillInfo(petid, skillid, infos);
	return infos;
}

//bool CGAPython::GetPetSkillsInfo(int petid, cga_pet_skills_info_t& skills)
//{
//	return m_interface->GetPetSkillsInfo(petid, skills);
//}

CGA::cga_pet_skills_info_t CGAPython::GetPetSkillsInfo(int petid)
{
	CGA::cga_pet_skills_info_t infos;
	m_interface->GetPetSkillsInfo(petid, infos);
	return infos;
}

bool CGAPython::IsSkillValid(int skillid)
{
	bool bVal = false;
	m_interface->IsSkillValid(skillid, bVal);
	return bVal;
}

cga_skill_info_t CGAPython::GetSkillInfo(int skillid)
{
	CGA::cga_skill_info_t info;
	m_interface->GetSkillInfo(skillid, info);
	return info;
}

CGA::cga_skills_info_t CGAPython::GetSkillsInfo()
{
	CGA::cga_skills_info_t info;
	m_interface->GetSkillsInfo(info);
	return info;
}

CGA::cga_subskill_info_t CGAPython::GetSubSkillInfo(int id, int stage)
{
	CGA::cga_subskill_info_t info;
	m_interface->GetSubSkillInfo(id, stage, info);
	return info;
}

CGA::cga_subskills_info_t CGAPython::GetSubSkillsInfo(int id)
{
	CGA::cga_subskills_info_t info;
	m_interface->GetSubSkillsInfo(id, info);
	return info;
}

CGA::cga_picbooks_info_t CGAPython::GetPicBooksInfo()
{
	CGA::cga_picbooks_info_t info;
	m_interface->GetPicBooksInfo(info);
	return info;
}

CGA::cga_cards_info_t CGAPython::GetCardsInfo()
{
	CGA::cga_cards_info_t info;
	m_interface->GetCardsInfo(info);
	return info;
}

pcga_cards_recv_msg_t CGAPython::GetCardsRecvMsg()
{
	CGA::cga_cards_recv_msg_t info;
	m_interface->GetCardsRecvMsg(info);
	pcga_cards_recv_msg_t rinfo;
	for (int i = 0; i < info.size(); ++i)
	{
		pcga_card_recv_msg_t tmpInfo;
		tmpInfo.index = info[i].index;
		tmpInfo.name = info[i].name;
		for (int n = 0; n < 10; ++n)
		{
			tmpInfo.msgs.push_back(info[i].msgs[n]);
		}
		rinfo.push_back(tmpInfo);
	}
	return rinfo;
}

bool CGAPython::SetCardRecvMsgState(int index, int item, int state)
{
	return m_interface->SetCardRecvMsgState(index, item, state);
}

bool CGAPython::IsItemValid(int itempos)
{
	bool bVal = false;
	m_interface->IsItemValid(itempos, bVal);
	return bVal;
}

CGA::cga_item_info_t CGAPython::GetItemInfo(int itempos)
{
	CGA::cga_item_info_t info;
	m_interface->GetItemInfo(itempos, info);
	return info;
}

CGA::cga_items_info_t CGAPython::GetItemsInfo()
{
	CGA::cga_items_info_t info;
	m_interface->GetItemsInfo(info);
	return info;
}

CGA::cga_items_info_t CGAPython::GetBankItemsInfo()
{
	CGA::cga_items_info_t info;
	m_interface->GetBankItemsInfo(info);
	return info;
}

int CGAPython::GetBankGold()
{
	int gold = 0;
	m_interface->GetBankGold(gold);
	return gold;
}

bool CGAPython::MoveItem(int itempos, int dstpos, int count)
{
	bool bVal = false;
	m_interface->MoveItem(itempos, dstpos, count, bVal);
	return bVal;
}

bool CGAPython::MovePet(int itempos, int dstpos)
{
	bool bVal = false;
	m_interface->MovePet(itempos, dstpos, bVal);
	return bVal;
}

bool CGAPython::MoveGold(int gold, int opt)
{
	bool bVal = false;
	m_interface->MoveGold(gold, opt, bVal);
	return bVal;
}

bool CGAPython::DropItem(int itempos)
{
	bool bVal = false;
	m_interface->DropItem(itempos, bVal);
	return bVal;
}

bool CGAPython::DropPet(int petpos)
{
	bool bVal = false;
	m_interface->DropPet(petpos, bVal);
	return bVal;
}

bool CGAPython::ChangePetState(int petpos, int state)
{
	bool bVal = false;
	m_interface->ChangePetState(petpos, state, bVal);
	return bVal;

}

std::tuple<int, int, int, std::string> CGAPython::GetMapIndex()
{
	int index1, index2, index3;
	std::string filemap;
	m_interface->GetMapIndex(index1, index2, index3, filemap);
	return std::make_tuple(index1, index2, index3, filemap);
}

std::map<std::string, int> CGAPython::GetMapXY()
{
	int x, y;
	m_interface->GetMapXY(x, y);
	return std::map<std::string, int>({ { "x",x }, { "y",y } });
}

std::map<std::string, float> CGAPython::GetMapXYFloat()
{
	float x, y;
	m_interface->GetMapXYFloat(x, y);
	return std::map<std::string, float>({ { "x",x }, { "y",y } });
}

std::map<std::string, float> CGAPython::GetMoveSpeed()
{
	float x, y;
	m_interface->GetMoveSpeed(x, y);
	return std::map<std::string, float>({ { "x",x }, { "y",y } });
}

std::map<std::string, int> CGAPython::GetMouseXY()
{
	int x, y;
	m_interface->GetMouseXY(x, y);
	return std::map<std::string, int>({ { "x",x }, { "y",y } });
}

int CGAPython::GetMouseOrientation()
{
	int dir;
	m_interface->GetMouseOrientation(dir);
	return dir;
}

std::string CGAPython::GetMapName()
{
	std::string name;
	m_interface->GetMapName(name);
	return name;
}

CGA::cga_map_units_t CGAPython::GetMapUnits()
{
	CGA::cga_map_units_t mapinfo;
	m_interface->GetMapUnits(mapinfo);
	return mapinfo;
}

CGA::cga_map_cells_t CGAPython::GetMapCollisionTable(bool loadall)
{
	CGA::cga_map_cells_t mapinfo;
	m_interface->GetMapCollisionTable(loadall, mapinfo);
	return mapinfo;
}

CGA::cga_map_cells_t CGAPython::GetMapCollisionTableRaw(bool loadall)
{
	CGA::cga_map_cells_t mapinfo;
	m_interface->GetMapCollisionTableRaw(loadall, mapinfo);
	return mapinfo;
}

CGA::cga_map_cells_t CGAPython::GetMapObjectTable(bool loadall)
{
	CGA::cga_map_cells_t mapinfo;
	m_interface->GetMapObjectTable(loadall, mapinfo);
	return mapinfo;
}

CGA::cga_map_cells_t CGAPython::GetMapTileTable(bool loadall)
{
	CGA::cga_map_cells_t mapinfo;
	m_interface->GetMapTileTable(loadall, mapinfo);
	return mapinfo;
}

bool CGAPython::WalkTo(int x, int y)
{
	return m_interface->WalkTo(x, y);
}

bool CGAPython::TurnTo(int x, int y)
{
	return m_interface->TurnTo(x, y);
}

bool CGAPython::SetMoveSpeed(int speed)
{
	return m_interface->SetMoveSpeed(speed);
}

bool CGAPython::ForceMove(int dir, bool show)
{
	bool bVal = false;
	m_interface->ForceMove(dir, show, bVal);
	return bVal;
}

bool CGAPython::ForceMoveTo(int x, int y, bool show)
{
	bool bVal = false;
	m_interface->ForceMoveTo(x, y, show, bVal);
	return bVal;
}

bool CGAPython::IsMapCellPassable(int x, int y)
{
	bool bVal = false;
	m_interface->IsMapCellPassable(x, y, bVal);
	return bVal;
}

bool CGAPython::LogBack()
{
	return m_interface->LogBack();
}

bool CGAPython::LogOut()
{
	return m_interface->LogOut();
}

bool CGAPython::BackSelectServer()
{
	return m_interface->BackSelectServer();
}

bool CGAPython::SayWords(const std::string& str, int v1, int v2, int v3)
{
	return m_interface->SayWords(str, v1, v2, v3);
}

bool CGAPython::ChangeNickName(std::string& str)
{
	bool bVal = false;
	m_interface->ChangeNickName(str, bVal);
	return bVal;
}

bool CGAPython::ChangeTitleName(int titleId)
{
	bool bVal = false;
	m_interface->ChangeTitleName(titleId, bVal);
	return bVal;
}

CGA::cga_pers_desc_t CGAPython::ChangePersDesc()
{
	CGA::cga_pers_desc_t desc;
	m_interface->ChangePersDesc(desc);
	return desc;
}

bool CGAPython::ChangePetName(int petId, const std::string& name)
{
	bool bVal = false;
	m_interface->ChangePetName(petId, name, bVal);
	return bVal;
}

bool CGAPython::UseItem(int itempos)
{
	bool bVal = false;
	m_interface->UseItem(itempos, bVal);
	return bVal;
}

bool CGAPython::ClickNPCDialog(int option, int index)
{
	bool bVal = false;
	m_interface->ClickNPCDialog(option, index, bVal);
	return bVal;
}

bool CGAPython::SellNPCStore(cga_sell_items_t& items)
{
	bool bVal = false;
	m_interface->SellNPCStore(items, bVal);
	return bVal;
}

bool CGAPython::BuyNPCStore(cga_buy_items_t& items)
{
	bool bVal = false;
	m_interface->BuyNPCStore(items, bVal);
	return bVal;
}

bool CGAPython::PlayerMenuSelect(int menuindex, const std::string& menustring)
{
	bool bVal = false;
	m_interface->PlayerMenuSelect(menuindex, menustring, bVal);
	return bVal;
}

bool CGAPython::UnitMenuSelect(int menuindex)
{
	bool bVal = false;
	m_interface->UnitMenuSelect(menuindex, bVal);
	return bVal;
}

bool CGAPython::UpgradePlayer(int attr)
{
	return m_interface->UpgradePlayer(attr);
}

bool CGAPython::UpgradePet(int petid, int attr)
{
	return m_interface->UpgradePet(petid, attr);
}

bool CGAPython::IsBattleUnitValid(int pos)
{
	bool bVal = false;
	m_interface->IsBattleUnitValid(pos, bVal);
	return bVal;
}

CGA::cga_battle_unit_t CGAPython::GetBattleUnit(int pos)
{
	CGA::cga_battle_unit_t info;
	m_interface->GetBattleUnit(pos, info);
	return info;
}

CGA::cga_battle_units_t CGAPython::GetBattleUnits()
{
	CGA::cga_battle_units_t info;
	m_interface->GetBattleUnits(info);
	return info;
}

CGA::cga_battle_context_t CGAPython::GetBattleContext()
{
	CGA::cga_battle_context_t info;
	m_interface->GetBattleContext(info);
	return info;
}

bool CGAPython::BattleNormalAttack(int target)
{
	bool bVal = false;
	m_interface->BattleNormalAttack(target, bVal);
	return bVal;
}

bool CGAPython::BattleSkillAttack(int skillpos, int skilllv, int target, bool packetOnly)
{
	bool bVal = false;
	m_interface->BattleSkillAttack(skillpos, skilllv, target, packetOnly, bVal);
	return bVal;
}

bool CGAPython::BattleRebirth()
{
	bool bVal = false;
	m_interface->BattleRebirth(bVal);
	return bVal;
}

bool CGAPython::BattleGuard()
{
	bool bVal = false;
	m_interface->BattleGuard(bVal);
	return bVal;
}

bool CGAPython::BattleEscape()
{
	bool bVal = false;
	m_interface->BattleEscape(bVal);
	return bVal;
}

bool CGAPython::BattleExchangePosition()
{
	bool bVal = false;
	m_interface->BattleExchangePosition(bVal);
	return bVal;
}

bool CGAPython::BattleChangePet(int petid)
{
	bool bVal = false;
	m_interface->BattleChangePet(petid, bVal);
	return bVal;
}

bool CGAPython::BattleUseItem(int itempos, int target)
{
	bool bVal = false;
	m_interface->BattleUseItem(itempos, target, bVal);
	return bVal;
}

bool CGAPython::BattlePetSkillAttack(int skillpos, int target, bool packetOnly)
{
	bool bVal = false;
	m_interface->BattlePetSkillAttack(skillpos, target, packetOnly, bVal);
	return bVal;
}

bool CGAPython::BattleDoNothing()
{
	bool bVal = false;
	m_interface->BattleDoNothing(bVal);
	return bVal;
}

bool CGAPython::BattleSetHighSpeedEnabled(bool enable)
{
	return m_interface->BattleSetHighSpeedEnabled(enable);
}

bool CGAPython::SetGameTextUIEnabled(bool enable)
{
	return m_interface->SetGameTextUIEnabled(enable);
}

bool CGAPython::SetGameTextUICurrentScript(const std::string& script)
{
	return m_interface->SetGameTextUICurrentScript(script);
}

int CGAPython::GetBattleEndTick()
{
	int msec;
	m_interface->GetBattleEndTick(msec);
	return msec;
}

bool CGAPython::SetBattleEndTick(int msec)
{
	return m_interface->SetBattleEndTick(msec);
}

bool CGAPython::SetWorkDelay(int delay)
{
	return m_interface->SetWorkDelay(delay);
}

bool CGAPython::SetWorkAcceleration(int percent)
{
	return m_interface->SetWorkAcceleration(percent);
}

bool CGAPython::SetImmediateDoneWork(bool enable)
{
	return m_interface->SetImmediateDoneWork(enable);
}

int CGAPython::GetImmediateDoneWorkState()
{
	int state;
	m_interface->GetImmediateDoneWorkState(state);
	return state;
}

bool CGAPython::StartWork(int skill_index, int sub_index)
{
	bool bVal = false;
	m_interface->StartWork(skill_index, sub_index, bVal);
	return bVal;
}

bool CGAPython::CraftItem(pcga_craft_item_t& craft)
{
	CGA::cga_craft_item_t item;
	item.skill_index = craft.skill_index;
	item.subskill_index = craft.subskill_index;
	item.sub_type = craft.sub_type;
	for (int i = 0; i < 6; ++i)
	{
		item.itempos[i] = craft.itempos[i];
	}
	bool bVal = false;
	m_interface->CraftItem(item, bVal);
	return bVal;
}

bool CGAPython::AssessItem(int skill_index, int itempos)
{
	bool bVal = false;
	m_interface->AssessItem(skill_index, itempos, bVal);
	return bVal;
}

pcga_craft_info_t CGAPython::GetCraftInfo(int skill_index, int sub_index)
{
	CGA::cga_craft_info_t info;
	m_interface->GetCraftInfo(skill_index, sub_index, info);
	pcga_craft_info_t rinfo;
	rinfo.id = info.id;
	rinfo.cost = info.cost;
	rinfo.level = info.level;
	rinfo.itemid = info.itemid;
	rinfo.index = info.index;
	rinfo.name = info.name;
	rinfo.info = info.info;
	rinfo.available = info.available;
	for (int i = 0; i < 5; ++i)
	{
		rinfo.materials.push_back(info.materials[i]);
	}
	return rinfo;
}

pcga_crafts_info_t CGAPython::GetCraftsInfo(int skill_index)
{
	CGA::cga_crafts_info_t info;
	m_interface->GetCraftsInfo(skill_index, info);
	pcga_crafts_info_t rinfos;
	for (int i = 0; i < info.size(); ++i)
	{
		pcga_craft_info_t rinfo;
		rinfo.id = info[i].id;
		rinfo.cost = info[i].cost;
		rinfo.level = info[i].level;
		rinfo.itemid = info[i].itemid;
		rinfo.index = info[i].index;
		rinfo.name = info[i].name;
		rinfo.info = info[i].info;
		rinfo.available = info[i].available;
		for (int n = 0; n < 5; ++n)
		{
			rinfo.materials.push_back(info[i].materials[n]);
		}
		rinfos.push_back(rinfo);
	}
	return rinfos;
}

int CGAPython::GetCraftStatus()
{
	int nVal;
	m_interface->GetCraftStatus(nVal);
	return nVal;
}

bool CGAPython::DoRequest(int request_type)
{
	bool bVal;
	m_interface->DoRequest(request_type, bVal);
	return bVal;
}

bool CGAPython::EnableFlags(int type, bool enable)
{
	bool bVal;
	m_interface->EnableFlags(type, enable, bVal);
	return bVal;
}

bool CGAPython::TradeAddStuffs(cga_sell_items_t items, cga_sell_pets_t pets, int gold)
{
	return m_interface->TradeAddStuffs(items, pets, gold);
}

bool CGAPython::AddAllTradeItems(int step)
{
	return m_interface->AddAllTradeItems(step);
}

cga_team_players_t CGAPython::GetTeamPlayerInfo(cga_team_players_t& result)
{
	CGA::cga_team_players_t info;
	m_interface->GetTeamPlayerInfo(info);
	return info;
}

bool CGAPython::FixMapWarpStuck(int type)
{
	return m_interface->FixMapWarpStuck(type);
}

bool CGAPython::SetNoSwitchAnim(bool enable)
{
	return m_interface->SetNoSwitchAnim(enable);
}

bool CGAPython::SetSwitchAnimForceWait(int state, int ticks)
{
	return m_interface->SetSwitchAnimForceWait(state, ticks);
}

bool CGAPython::SetWindowResolution(int w, int h)
{
	return m_interface->SetWindowResolution(w, h);
}

bool CGAPython::RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize)
{
	return m_interface->RequestDownloadMap(xbottom, ybottom, xsize, ysize);
}

double CGAPython::GetNextAnimTickCount()
{
	double dVal;
	m_interface->GetNextAnimTickCount(dVal);
	return dVal;
}

bool CGAPython::LoginGameServer(const std::string& gid, const std::string& glt, int serverid, int bigServerIndex, int serverIndex, int character)
{
	return m_interface->LoginGameServer(gid, glt, serverid, bigServerIndex, serverIndex, character);
}

bool CGAPython::CreateCharacter(const CGA::cga_create_chara_t& req)
{
	return m_interface->CreateCharacter(req);
}

bool CGAPython::IsUIDialogPresent(int dialog)
{
	bool bVal = false;
	m_interface->IsUIDialogPresent(dialog, bVal);
	return bVal;
}

bool CGAPython::PlayGesture(int index)
{
	return m_interface->PlayGesture(index);
}

bool CGAPython::DeleteCard(int index, bool packetonly)
{
	bool bVal = false;
	m_interface->DeleteCard(index, packetonly, bVal);
	return bVal;
}

bool CGAPython::SendMail(int index, const std::string& msg)
{
	bool bVal = false;
	m_interface->SendMail(index, msg, bVal);
	return bVal;
}

bool CGAPython::SendPetMail(int index, int petid, int itempos, const std::string& msg)
{
	bool bVal = false;
	m_interface->SendPetMail(index, petid, itempos, msg, bVal);
	return bVal;
}

//bool CGAPython::GetPetSkillsInfo1(std::vector<int>& tests)
//{
//	for (int i = 0; i < 100; ++i)
//	{
//		tests.push_back(i);
//	}
//	return true;
//}
