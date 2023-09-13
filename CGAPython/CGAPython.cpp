#include "CGAPython.h"
#include <windows.h>
namespace py = pybind11;



CGAPython::CGAPython()
{
	//py::scoped_ostream_redirect output;
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

 bool CGAPython::ChangePersDesc(CGA::cga_pers_desc_t desc)
{
	return m_interface->ChangePersDesc(desc);
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
	std::cout << "SellNPCStore" << std::endl;
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

//bool CGAPython::TradeAddStuffs(cga_sell_items_t& items, std::vector<int>& pets, int gold)
//{
//	return m_interface->TradeAddStuffs(items, pets, gold);
//}
bool CGAPython::TradeAddStuffs(cga_sell_items_t items, cga_sell_pets_t pets, int gold)
{
	std::cout << "TradeAddStuffs" << std::endl;
	for (auto item:items)
	{
		std::cout << item.itemid <<"count:" << item.count << "pos:"<<item.itempos << std::endl;
	}
	return m_interface->TradeAddStuffs(items, pets, gold);
}
bool CGAPython::AddAllTradeItems(int step)
{
	return m_interface->AddAllTradeItems(step);
}

cga_team_players_t CGAPython::GetTeamPlayerInfo()
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

bool CGAPython::GetGameServerInfo(cga_game_server_info_t& info)
{
	return m_interface->GetGameServerInfo(info);
}

//bool CGAPython::GetPetSkillsInfo1(std::vector<int>& tests)
//{
//	for (int i = 0; i < 100; ++i)
//	{
//		tests.push_back(i);
//	}
//	return true;
//}


double CGAPython::GetDistance(int x, int y)
{
	int curX, curY;
	m_interface->GetMapXY(curX, curY);
	if (curY == y && curX == x)
		return 0;
	double fdistance = std::sqrt(std::pow(abs(curX - x), 2) + std::pow(abs(curY - y), 2));
	return fdistance;
}
bool CGAPython::MoveTo(int x, int y, int timeout)
{
	if (m_bMoveing)
		return false;
	int curX, curY;
	m_interface->GetMapXY(curX, curY);

	//判断当前坐标和目标坐标距离 超出8格  不执行
	if (GetDistance(x, y) > 8)
	{
		//if(m_bDebugLog)std::cout << std::string("目标x或者y超出范围,当前坐标(%1,%2) 目标坐标(%3,%4)").arg(curX).arg(curY).arg(x).arg(y);
		return false;
	}
	bool bRet = m_interface->WalkTo(x, y);
	if (bRet == false)
	{
		m_bMoveing = false;
		return false;
	}
	if (timeout < 1000)
		timeout = 10000;
	timeout = timeout / 1000;
	for (int i = 0; i < timeout; ++i)
	{
		if (m_bStop)
			return false;
		m_interface->GetMapXY(curX, curY);
		if (curX == x && curY == y)
		{
			m_bMoveing = false;
			return true;
		}
		//QApplication::processEvents();
		//Sleep(100);
	}
	m_bMoveing = false;
	return false;
}

std::vector<TMLPoint> CGAPython::GetMapEntranceList()
{
	std::vector<TMLPoint> enteranceList;
	CGA::cga_map_cells_t cells;
	if (m_interface->GetMapCollisionTable(true, cells))
	{
		CGA::cga_map_cells_t objCells;
		if (m_interface->GetMapObjectTable(true, objCells))
		{
			if (cells.x_size && cells.y_size)
			{
				for (int y = 0; y < cells.y_size; ++y)
				{
					for (int x = 0; x < cells.x_size; ++x)
					{
						if (x < objCells.x_size && y < objCells.y_size)
						{
							auto cellObject = objCells.cell.at((size_t)(x + y * objCells.x_size));
							if (cellObject & 0xff)
							{
								enteranceList.push_back(TMLPoint(x, y)); //蓝色 迷宫传送点
							}
						}
					}
				}
			}
		}
	}
	if (enteranceList.size() > 2)
	{
		//if(m_bDebugLog)std::cout << "获取到多个传送点";
	}
	return enteranceList;
}
TMLPoint CGAPython::GetMapCoordinate()
{
	TMLPoint pos;
	m_interface->GetMapXY(pos.x, pos.y);
	return pos;
}
bool CGAPython::IsInNormalState()
{
	int nWordStatus, nGameState;
	m_interface->GetWorldStatus(nWordStatus);
	m_interface->GetGameStatus(nGameState);
	return (nWordStatus == 9 && nGameState == 3) ? true : false;
}
//10秒？
bool CGAPython::WaitInNormalState(int timeout /*=10000*/)
{
	timeout = timeout == 0 ? 10 * 60 * 1000 : timeout;
	int timeoutNum = timeout / 1000; //10分钟 600秒 每次Sleep 1秒
	for (int i = 0; i < timeoutNum; ++i)
	{
		if (m_bStop)
			return false;
		if (IsInNormalState())
			return true;
		Sleep(1000);
	}
	return false;
}

bool CGAPython::IsInRandomMap()
{
	//index1  0固定地图 否则随机地图  index2 当前线路 index3地图编号 filemap地图文件名
	int index1, index2, index3;
	std::string filemap;
	m_interface->GetMapIndex(index1, index2, index3, filemap);
	if (index1 == 0)
		return false;
	return true;
}

//计算路径，修复路径上有传送点，会被传送的bug
A_FIND_PATH CGAPython::CalculatePath(int curX, int curY, int targetX, int targetY)
{
	A_FIND_PATH findPath;
	//1、下载地图
	//if (!IsMapDownload())
	//{
	//	if (!DownloadMap()) //下载失败退出？  暂不退出 能寻路就返回路径，不能就返回空
	//	{
	//		//return findPath;
	//	}
	//}
	//2、A*查找路径 返回路径列表
	try
	{
		CGA::cga_map_cells_t cells;
		if (m_interface->GetMapCollisionTable(true, cells))
		{
			CGA::cga_map_cells_t objCells;
			m_interface->GetMapObjectTable(true, objCells);
			AStar aStarFindPath(true, true);
			//x是列  y是行
			AStarGrid grid(cells.x_size, cells.y_size);
			int xtop = cells.y_size;
			int ytop = cells.x_size;
			for (int tmpx = 0; tmpx < xtop; ++tmpx) //行
			{
				for (int tmpy = 0; tmpy < ytop; ++tmpy) //列
				{
					auto cellWall = cells.cell.at((size_t)(tmpy + tmpx * cells.x_size)); //地图单位数据 行列
					auto cellObject = objCells.cell.at((size_t)(tmpy + tmpx * cells.x_size));
					if (cellWall == 1) //不可通行 1
					{
						grid.SetWalkableAt(tmpy, tmpx, false); //灰色 不可行
					}
					else
					{
						grid.SetWalkableAt(tmpy, tmpx, true);
						if (cellObject & 0xff) //路径上有传送门之类的 并且坐标不是目的坐标 跳过
						{
							if (tmpy != targetX || tmpx != targetY)
							{
								grid.SetWalkableAt(tmpy, tmpx, false); //灰色 不可行
							}
						}
					}
				}
			}
			TMLPoint frompos(curX - cells.x_bottom, curY - cells.y_bottom);
			TMLPoint topos(targetX - cells.x_bottom, targetY - cells.y_bottom);
			auto path = aStarFindPath.FindPath(frompos.x, frompos.y, topos.x, topos.y, &grid);
			findPath = AStarUtil::compressPath(path);
		}
		return findPath;
	}
	catch (const std::exception& e)
	{
		if(m_bDebugLog)std::cout << "Try catch CalculatePathEx Ero" << e.what();
	}
	catch (...)
	{
		if(m_bDebugLog)std::cout << "Try catch CalculatePathEx Ero";
	}
	return findPath;
}

//加载地图 判断坐标是否可达
bool CGAPython::IsReachableTargetEx(int sx, int sy, int tx, int ty)
{
	auto findPath = CalculatePath(sx, sy, tx, ty);
	if (findPath.size() < 1) //离线地图查找一波
	{
		//if(m_bDebugLog)std::cout << "目标不可达，加载离线地图尝试！";
		//int mapIndex = GetGameMapID();
		//QImage mapImage;
		//LoadOffLineMapImageData(mapIndex, mapImage);
		//findPath = CalculatePathEx(mapImage, sx, sy, tx, ty);
		//if (findPath.size() > 0)
		//	qDebug() << "离线地图查找路径成功，继续寻路";
	}
	if (findPath.size() > 0)
	{
		return true;
	}
	return false;
}

bool CGAPython::IsReachableTarget(int tx, int ty)
{
	//距离判断 虽然有些坐标很近，但有阻碍物，这里还是计算路径
	try
	{
		TMLPoint curPos = GetMapCoordinate();
		return IsReachableTargetEx(curPos.x, curPos.y, tx, ty);
	}
	catch (const std::exception& e)
	{
		if(m_bDebugLog)std::cout << "Try catch IsReachableTarget Ero" << e.what();
	}
	catch (...)
	{
		if(m_bDebugLog)std::cout << "Try Catch IsReachableTarget Ero";
	}
	return false;
}

TMLPoint CGAPython::GetRandomSpace(int x, int y, int distance, bool judgeReachTgt)
{
	int nTempX = 0;
	int nTempY = 0;
	auto warpPosList = GetMapEntranceList(); //传送点
	CGA::cga_map_cells_t cells;
	if (m_interface->GetMapCollisionTable(true, cells))
	{
		if (x > cells.x_size || y > cells.y_size)
		{
			return TMLPoint();
		}
		do
		{
			nTempX = x - distance;
			nTempY = y;
			auto FindFun = [&](int x, int y)
			{
				bool bAble = cells.cell.at((size_t)(x + y * cells.x_size)) == 0 && \
					std::find(std::begin(warpPosList), std::end(warpPosList), TMLPoint(x, y)) == std::end(warpPosList);
				if (bAble && judgeReachTgt)
				{
					return IsReachableTarget(x, y); //
				}
				return bAble;
			}; //不为传送点
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x + distance;
			nTempY = y;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x;
			nTempY = y - distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x;
			nTempY = y + distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x + distance;
			nTempY = y + distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x - distance;
			nTempY = y + distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x + distance;
			nTempY = y - distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x - distance;
			nTempY = y - distance;
			if (FindFun(nTempX, nTempY))
				break;
		} while (0);
	}
	return TMLPoint(nTempX, nTempY);
}

bool CGAPython::AutoNavigator(A_FIND_PATH path, bool isLoop)
{
	m_navigatorLoopCount += 1;
	auto backPath = path;
	int curX, curY;	  //当前坐标点
	int tarX, tarY;	  //当前目标坐标点
	int lastX, lastY; //最后一次移动点
	lastX = lastY = 0;
	int curMapIndex = GetGameMapID();
	std::string curMapName = GetMapName();
	unsigned long dwLastTime = 0; //第一次记录时间
	unsigned long dwCurTime = dwLastTime;
	int dwTimeoutTryCount = 0; //超时 重试次数
	bool isNormal = true;
	auto warpPosList = GetMapEntranceList(); //传送点
	auto startPos = GetMapCoordinate();
	if (path.front().first == startPos.x && path.front().second == startPos.y)
	{
		auto it = path.begin();
		path.erase(it);
	}
	TMLPoint walkprePos; //调用接口移动前坐标
	for (auto coor : path)
	{
		if (m_bStop)
			break;
		tarX = coor.first;
		tarY = coor.second;
		if(m_bDebugLog)std::cout << "目标:" << tarX << "," << tarY << std::endl;
		walkprePos = GetMapCoordinate(); //记录下移动前坐标
		m_interface->WalkTo(coor.first, coor.second);
		dwLastTime = 0;
		dwTimeoutTryCount = 0;
		while (!m_bStop) //for(int i = 0; i < 1000; ++i)
		{
			//1、判断战斗和切图
			while (!IsInNormalState() && !m_bStop) //战斗或者切图 等待完毕
			{
				//if(m_bDebugLog)std::cout << "AutoNavigator" << "战斗或者切图"<< std::endl;
				isNormal = false;
				Sleep(1000); //还是多等2秒吧  防止卡位
			}
			//2、判断地图是否发送变更 例如：迷宫送出来，登出，切到下个图
			if (curMapIndex != GetGameMapID() || curMapName != GetMapName())
			{
				if(m_bDebugLog)std::cout << "当前地图更改，寻路判断!" << std::endl;
				int gameStatus = 0;
				int lastWarpMap202 = 0;
				//战斗卡住 进行判断
				while (m_interface->GetGameStatus(gameStatus) && gameStatus == 202)
				{
					auto timestamp = GetTickCount();
					if (lastWarpMap202 == 0)
					{
						lastWarpMap202 = timestamp;
					}
					else if (timestamp - lastWarpMap202 >= 5)
					{
						m_interface->FixMapWarpStuck(0);
						lastWarpMap202 = timestamp + 8;
						if(m_bDebugLog)std::cout << ("切换地图 卡住 fix warp") << std::endl;
					}
					Sleep(1000);
				}
				WaitInNormalState();
				////再次判断 有些会卡回原图，这里再次判断
				//Sleep(2000);
				if (curMapIndex == GetGameMapID() && curMapName == GetMapName() && isLoop)
				{
					//m_interface->WalkTo(tarX, tarY); //重新执行一次移动
					//重新执行一次重新寻路
					if(m_bDebugLog)std::cout << "还在原图：重新查找路径 进行寻路" << std::endl;
					auto tgtPos = backPath.end();
					return AutoMoveInternal(tgtPos->first, tgtPos->second, false);
				}
				else
				{
					if(m_bDebugLog)std::cout << "地图更改，寻路结束!" << std::endl;
					if (IsInRandomMap())
						Sleep(m_mazeWaitTime);
					return false;
				}
			}
			//调换顺序 战斗或者切图后，还在本地图，再次执行 有问题 就是同一个地图 但是是个传送点
			if (!isNormal) //刚才战斗和切图了 现在重新执行最后一次坐标点任务
			{
				if(m_bDebugLog)std::cout << "战斗/切图等待，再次寻路！" << tarX << "," << tarY << std::endl;
				if (GetDistance(tarX, tarY) > 11)
				{
					if(m_bDebugLog)std::cout << "战斗/切图等待，再次寻路 距离大于11 刚才可能为传送 返回!" << tarX << "," << tarY << std::endl;
					//尝试自动寻路
					TMLPoint curPos = GetMapCoordinate();
					auto findPath = CalculatePath(curPos.x, curPos.y, tarX, tarY);
					if (findPath.size() > 0 && isLoop)
					{
						AutoNavigator(findPath, false);
					}
					else
					{
						if(m_bDebugLog)std::cout << "战斗/切图等待，再次寻路 失败!" << tarX << "," << tarY << std::endl;
						return false;
					}
				}
				else
				{
					if (!IsReachableTarget(walkprePos.x, walkprePos.y)) //用移动前点判断 不能到 说明换图成功，特别是ud这个图
					{
						if(m_bDebugLog)std::cout << "原坐标不可达，移动至目标点成功，寻路结束!" << std::endl;
						WaitInNormalState();
						return true;
					}
					else
					{
						m_interface->WalkTo(tarX, tarY);
						//Sleep(2000);
					}
				}
				dwLastTime = 0;
				isNormal = true;
			}
			//3、判断是否到达目的地
			m_interface->GetMapXY(curX, curY);
			if (curX == tarX && curY == tarY) //坐标一致
			{
				if (std::find(std::begin(warpPosList), std::end(warpPosList), TMLPoint(tarX, tarY)) != std::end(warpPosList)) //切一下 然后再移动一次
				{
					if(m_bDebugLog)std::cout << "目标为传送点,判断地图切换:" << tarX << "," << tarY << std::endl;
					int tryNum = 0;
					unsigned long fixWarpTime = GetTickCount(); //5秒判断
					while (tryNum < 3)
					{
						if ((GetTickCount() - fixWarpTime) > 5000) //5秒一轮 判断一次
						{
							int gameStatus = 0;
							int lastWarpMap202 = 0;
							//战斗卡住 进行判断 这里和自动战斗里默认500毫秒检测会冲突，但不影响
							while (m_interface->GetGameStatus(gameStatus) && gameStatus == 202)
							{
								auto timestamp = GetTickCount();
								if (lastWarpMap202 == 0)
								{
									lastWarpMap202 = timestamp;
								}
								else if (timestamp - lastWarpMap202 >= 5)
								{
									m_interface->FixMapWarpStuck(0);
									lastWarpMap202 = timestamp + 8;
									if(m_bDebugLog)std::cout << ("切换地图 卡住 fix warp 0") << std::endl;
								}
								Sleep(1000);
							}
							WaitInNormalState();
							tryNum++;
							fixWarpTime = GetTickCount();
							//传送的话，等待2秒 如果还是在本图 回退处理
							//Sleep(2000);
							//回退一个图 重试 如果不变的话，说明没卡 继续
							m_interface->GetMapXY(curX, curY);
							//如果是传送点 也增加切图判断，没切的话 重新执行一下
							if (curMapIndex != GetGameMapID() || curMapName != GetMapName())
							{
								if(m_bDebugLog)std::cout << "当前地图更改，寻路结束!" << std::endl;
								WaitInNormalState();
								if (IsInRandomMap())
									Sleep(m_mazeWaitTime);
								return true;
							}
							else if (curMapIndex == GetGameMapID() && curMapName == GetMapName() && (curX != tarX || curY != tarY)) //好多图 名字一样
							{
								if(m_bDebugLog)std::cout << "坐标切换，寻路结束!" << std::endl;
								WaitInNormalState();
								return true;
							}
							else if (curMapIndex == GetGameMapID() && curMapName == GetMapName() && curX == tarX && curY == tarY) //好多图 名字一样
							{
								//地图一致 目标一致，但是是传送点，	 判断原坐标是否可达
								if (!IsReachableTarget(walkprePos.x, walkprePos.y)) //用移动前点判断 不能到 说明换图成功，特别是ud这个图
								{
									if(m_bDebugLog)std::cout << "原坐标不可达，移动至目标点成功，寻路结束!" << std::endl;
									WaitInNormalState();
									return true;
								}
								if(m_bDebugLog)std::cout << "到达目标点，目标为传送点，地图卡住，切回地图,重新寻路" << tarX << "," << tarY << std::endl;
								m_interface->FixMapWarpStuck(1); //会切回上个图
								TMLPoint curPos = GetMapCoordinate();
								if (!isLoop)
								{
									m_interface->WalkTo(tarX, tarY);
									return true;
								}
								else
								{
									bool bTryRet = false;
									if (tarX == curPos.x && tarY == curPos.y)
									{
										auto tmpPos = GetRandomSpace(curPos.x, curPos.y, 1);
										AutoMoveInternal(tmpPos.x, tmpPos.y, false);
										bTryRet = AutoMoveInternal(tarX, tarY, false);
									}
									else
										bTryRet = AutoMoveInternal(tarX, tarY, false);
									return bTryRet;
								}
							}
							tryNum++;
						}
						else //没有5秒 判断是否切换成功 成功退出，否则继续
						{
							//如果是传送点 也增加切图判断，没切的话 重新执行一下
							m_interface->GetMapXY(curX, curY);
							if (curMapIndex != GetGameMapID() || curMapName != GetMapName()) //正常结束
							{
								if(m_bDebugLog)std::cout << "当前地图更改，寻路结束!" << std::endl;
								WaitInNormalState();
								if (IsInRandomMap())
									Sleep(m_mazeWaitTime);
								return true;
							}
							else if (curMapIndex == GetGameMapID() && curMapName == GetMapName() && (curX != tarX || curY != tarY)) //好多图 名字一样
							{
								if(m_bDebugLog)std::cout << "坐标切换，寻路结束!" << std::endl;
								WaitInNormalState();
								return true;
							}
						}
						Sleep(1000); //防止判断过快
					}
					if(m_bDebugLog)std::cout << "传送点切换返回,3次重试结束:" << tarX << "," << tarY << std::endl;
					if (!IsReachableTarget(walkprePos.x, walkprePos.y)) //用移动前点判断 不能到 说明换图成功，特别是ud这个图
					{
						if(m_bDebugLog)std::cout << "原坐标不可达，移动至目标点成功，寻路结束!" << std::endl;
						WaitInNormalState();
						return true;
					}
					else
					{
						//}
						if(m_bDebugLog)std::cout << "当前为传送点，但原坐标可达，重新移动至目标点，不判断目标点，寻路结束!" << tarX << "," << tarY << std::endl;
						m_interface->WalkTo(tarX, tarY); //重新执行一次移动 如果卡墙  也不管了 由脚本去判断
						WaitInNormalState();
						return true;
					}
				}
				break;
			}
			//4、判断玩家有没有自己点击坐标移动 有的话 等游戏人物不动时，重新执行最后一次移动
			dwCurTime = GetTickCount();
			if (lastX == curX && lastY == curY)
			{
				if (dwLastTime == 0)
				{
					dwLastTime = dwCurTime; //人物不动的状态
				}
				else
				{
					if (dwCurTime - dwLastTime > 10000) //10秒短路径寻路
					{
						dwTimeoutTryCount++;
						if (dwTimeoutTryCount >= 3)
						{
							if(m_bDebugLog)std::cout << "短坐标自动寻路次数超3次，返回!" << tarX << "," << tarY << std::endl;
							return false;
						}
						dwLastTime = dwCurTime;
						if(m_bDebugLog)std::cout << "卡墙，短坐标自动寻路!" << tarX  << "," << tarY << std::endl;
						//m_interface->FixMapWarpStuck(1); //会切回上个图
						WaitInNormalState();
						TMLPoint curPos = GetMapCoordinate();
						auto findPath = CalculatePath(curPos.x, curPos.y, tarX, tarY);
						if (findPath.size() == 1 || !isLoop)
						{
							if(m_bDebugLog)std::cout << "卡墙：WalkTo" << tarX << "," << tarY << std::endl;
							m_interface->WalkTo(tarX, tarY); //重新执行一次移动
						}
						else if (findPath.size() > 1 && isLoop)
						{
							if(m_bDebugLog)std::cout << "卡墙：AutoMoveInternal" << tarX << "," << tarY << std::endl;
							if (AutoMoveInternal(tarX, tarY, false) == false)
								return false;
							dwLastTime = dwCurTime;
						}
						else
						{
							if(m_bDebugLog)std::cout << "目标不可达，返回!" << tarX << "," << tarY << std::endl;
							return false;
						}
					}
				}
			}
			lastX = curX;
			lastY = curY;
			//QApplication::processEvents();
			//Sleep(1000);
			Sleep(10);
		}
	}
	WaitInNormalState();
	return true;
}

int CGAPython::GetTeammatesCount()
{
	CGA::cga_team_players_t plinfo;
	if (!m_interface->GetTeamPlayerInfo(plinfo))
	{
		return 0;
	}
	return plinfo.size();
}
//0正常完成 1退出中断  2地图变更中断 3地图下载失败
int CGAPython::AutoMoveTo(int x, int y, int timeout /*=100*/)
{
	if (m_bMoveing)
	{
		//if(m_bDebugLog)std::cout << "移动中";
		return 0;
	}
	m_navigatorLoopCount = 0;
	if (GetTeammatesCount() > 0 && !IsTeamLeader()) //队伍人数>0 并且不是队长的话  返回
	{
		if(m_bDebugLog)std::cout << "AutoMoveTo 队伍人数>0 并且自己不是队长" << std::endl;
		return 0;
	}
	bool bRet = AutoMoveInternal(x, y, timeout);
	if(m_bDebugLog)std::cout << "目标" << x << "," << y << " 寻路结束 " << std::endl;
	return bRet;
}
bool CGAPython::IsTeamLeader()
{
	CGA::cga_team_players_t teaminfo;
	m_interface->GetTeamPlayerInfo(teaminfo);

	CGA::cga_map_units_t units;
	m_interface->GetMapUnits(units);

	CGA::cga_player_info_t playerinfo;
	m_interface->GetPlayerInfo(playerinfo);

	for (int i = 0; i < teaminfo.size(); ++i)
	{
		auto tmpTeam = teaminfo[i];
		if (playerinfo.unitid == tmpTeam.unit_id && i == 0)
			return true;
	}
	return false;
}

void CGAPython::initCaclPetData(stringList petData)
{
	g_pGamePetCalc->initCaclPetData(petData);
}

std::tuple<int, int> CGAPython::ParsePetGrade(stringList inputData)
{
	ParsePetCalcData retData;
	g_pGamePetCalc->ParsePetGrade(inputData, retData);
	return std::make_tuple(retData.lossMin,retData.lossMax);
}

int CGAPython::AutoMoveToEx(int x, int y, std::string sMapName, int timeout /*= 100*/)
{
	if (sMapName.empty())
	{
		return AutoMoveTo(x, y, timeout);
	}
	else
	{
		int tryNum = 0;
		while (tryNum < 3)
		{
			AutoMoveTo(x, y, timeout);
			auto curMapName = GetMapName();
			auto curMapNum = GetGameMapID();
			if (!sMapName.empty())
			{
				if (IsInNormalState() && (curMapName == sMapName || curMapNum == std::stoi(sMapName)))
				{
					//到达目标地 返回1  否则尝试3次后返回0
					return 1;
				}
			}
			tryNum++;
		}
		if(m_bDebugLog)std::cout << "尝试3次后，到达目标地图失败:" << sMapName << " " << x << "," << y << std::endl;
	}
	return 0;
}

int CGAPython::AutoMoveToPath(std::vector<pair<int, int> > findPath, int timeout /*=100*/)
{
	if (findPath.size() > 0)
		AutoNavigator(findPath);
	else
		if(m_bDebugLog)std::cout << "未找到可通行路径!" << std::endl;
	return 0;
}

int CGAPython::AutoMoveInternal(int x, int y, int timeout /*= 100*/, bool isLoop)
{
	if (m_navigatorLoopCount >= 20)
		return 0;
	TMLPoint curPoint = GetMapCoordinate();
	if (curPoint.x == x && curPoint.y == y)
	{
		auto warpPosList = GetMapEntranceList(); //传送点		
		if (std::find(warpPosList.begin(), warpPosList.end(), TMLPoint(x, y)) != std::end(warpPosList))	 //切一下 然后再移动一次
		{
			if(m_bDebugLog)std::cout << "AutoMoveTo 坐标一样,目标为传送点，重新进入!" << std::endl;
			auto tmpPos = GetRandomSpace(x, y, 1);
			m_interface->WalkTo(tmpPos.x, tmpPos.y);
			Sleep(2000);
			m_interface->WalkTo(x, y);
		}
		if(m_bDebugLog)std::cout << "AutoMoveTo 坐标一样 返回!" << std::endl;
		return 1;
	}
	m_bMoveing = true;
	WaitInNormalState();
	//距离判断 虽然有些坐标很近，但有阻碍物，这里还是计算路径
	TMLPoint curPos = GetMapCoordinate();
	auto findPath = CalculatePath(curPos.x, curPos.y, x, y);
	if (findPath.size() < 1) //离线地图查找一波
	{
		/*	if(m_bDebugLog)std::cout << "未找到可通行路径，加载离线地图尝试！" << curPos.x << curPos.y << "tgt:" << x << "," << y<< std::endl;
			int mapIndex = GetGameMapID();
			QImage mapImage;
			LoadOffLineMapImageData(mapIndex, mapImage);
			findPath = CalculatePathEx(mapImage, curPos.x(), curPos.y(), x, y);
			if (findPath.size() > 0)
				if(m_bDebugLog)std::cout << "离线地图查找路径成功，继续寻路"<< std::endl;*/
	}
	bool bRet = false;
	if (findPath.size() > 0)
	{
		bRet = AutoNavigator(findPath, isLoop);
	}
	else
	{
		/*if(m_bDebugLog)std::cout << "未找到可通行路径！当前：" << curPos.x << ","
			<< curPos.y << "目标：" << x << "," << y << "Normal" << IsInNormalState()<< std::endl;*/
	}
	m_bMoveing = false;
	return bRet;
}

int CGAPython::GetGameMapID()
{
	int index1, index2, index3;
	std::string filemap;
	m_interface->GetMapIndex(index1, index2, index3, filemap);
	return index3;
}
