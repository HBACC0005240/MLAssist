#include "./pybind11/include/pybind11/pybind11.h"
#include "./pybind11/include/pybind11/functional.h"
#include "./pybind11/include/pybind11/stl.h"
#include "./pybind11/include/pybind11/complex.h"
#include "./pybind11/include/pybind11/stl_bind.h"
#include "./pybind11/include/pybind11/chrono.h"
#include "./pybind11/include/pybind11/iostream.h"

#include "../CGALib/gameinterface.h"
#include "./Astar/AStar.h"

#define A_FIND_PATH std::vector<std::pair<int, int> >

namespace py = pybind11;
using PyCallbackFunc = std::function<void(py::object)>;
using namespace CGA;

typedef std::vector<cga_craft_material_t> cga_crafts_material_t;
typedef struct pcga_craft_info_s
{
	pcga_craft_info_s()
	{
		id = 0;
		cost = 0;
		level = 0;
		itemid = 0;
		index = -1;
		available = false;
	}
	pcga_craft_info_s(int a1, int a2, int a3, int a4, int a5, std::string& a6, std::string& a7, bool a8) : id(a1), cost(a2), level(a3), itemid(a4), index(a5), name(a6), info(a7), available(a8)
	{

	}
	int id;
	int cost;
	int level;
	int itemid;
	int index;
	std::string name;
	std::string info;
	cga_crafts_material_t materials;
	bool available;
}pcga_craft_info_t;
typedef std::vector<pcga_craft_info_t> pcga_crafts_info_t;

typedef std::vector<tCard_Recv_Msg> tCard_Recvs_Msg;

typedef struct pcga_card_recv_msg_s
{
	pcga_card_recv_msg_s()
	{
		index = 0;
	}
	pcga_card_recv_msg_s(int a1, std::string& a2)
		: index(a1), name(a2)
	{
	}
	int index;
	std::string name;
	tCard_Recvs_Msg msgs;
}pcga_card_recv_msg_t;
typedef std::vector<pcga_card_recv_msg_t> pcga_cards_recv_msg_t;


typedef struct pcga_craft_item_s
{
	pcga_craft_item_s()
	{
		skill_index = 0;
		subskill_index = 0;
		sub_type = 0;
		for (int i = 0; i < 6; ++i)
			itempos.push_back(-1);
	}
	pcga_craft_item_s(int a1, int a2, int a3) : skill_index(a1), subskill_index(a2), sub_type(a3)
	{
	}
	pcga_craft_item_s(int a1, int a2, int a3, int a4[]) : skill_index(a1), subskill_index(a2), sub_type(a3)
	{
		for (int i = 0; i < 6; ++i)
		{
			itempos[i] = a4[i];
		}
	}
	int skill_index;
	int subskill_index;
	int sub_type;
	std::vector<int> itempos;
}pcga_craft_item_t;
typedef struct MLPoint
{
	MLPoint() { x = y = 0; }
	MLPoint(int a1, int a2) :x(a1), y(a2) {}
	bool operator==(const MLPoint& p1) { return p1.x == x && p1.y == y; }
	int x;
	int y;
}TMLPoint;
class CGAPython
{
public:
	CGAPython();
	bool IsConnected();
	void Disconnect();
	bool Connect(int port);
	bool Initialize(cga_game_data_t& data);

	int IsInGame();
	int GetWorldStatus();
	int GetGameStatus();
	int GetBGMIndex();
	cga_sys_time_t GetSysTime();
	cga_player_info_t GetPlayerInfo();
	bool SetPlayerFlagEnabled(int index, bool enable);
	bool IsPlayerFlagEnabled(int index);

	bool IsPetValid(int petid);
	cga_pet_info_t GetPetInfo(int petid);
	cga_pets_info_t GetPetsInfo();
	cga_pets_info_t GetBankPetsInfo();
	bool IsPetSkillValid(int petid, int skillid);
	cga_pet_skill_info_t GetPetSkillInfo(int petid, int skillid);
	//bool GetPetSkillsInfo(int petid, cga_pet_skills_info_t& skills);	//test1 这种需要.def("__iter__")  python部分遍历也难受
	//bool GetPetSkillsInfo1(std::vector<int>& tests);					//test2  和上面一样
	cga_pet_skills_info_t GetPetSkillsInfo(int petid);					//默认返回时候 直接转为python格式了 python方式访问

	bool IsSkillValid(int skillid);
	cga_skill_info_t GetSkillInfo(int skillid);
	cga_skills_info_t GetSkillsInfo();
	cga_subskill_info_t GetSubSkillInfo(int id, int stage);
	cga_subskills_info_t GetSubSkillsInfo(int id);

	cga_picbooks_info_t GetPicBooksInfo();
	cga_cards_info_t GetCardsInfo();
	pcga_cards_recv_msg_t GetCardsRecvMsg();
	bool SetCardRecvMsgState(int index, int item, int state);
	bool IsItemValid(int itempos);
	cga_item_info_t GetItemInfo(int itempos);
	cga_items_info_t GetItemsInfo();
	cga_items_info_t GetBankItemsInfo();
	int GetBankGold();
	bool MoveItem(int itempos, int dstpos, int count);
	bool MovePet(int itempos, int dstpos);
	bool MoveGold(int gold, int opt);
	bool DropItem(int itempos);
	bool DropPet(int petpos);
	bool ChangePetState(int petpos, int state);
	std::tuple<int, int, int, std::string> GetMapIndex();
	std::map<std::string, int> GetMapXY();
	std::map<std::string, float> GetMapXYFloat();
	std::map<std::string, float> GetMoveSpeed();
	std::map<std::string, int> GetMouseXY();
	int GetMouseOrientation();
	std::string GetMapName();
	cga_map_units_t GetMapUnits();
	cga_map_cells_t GetMapCollisionTable(bool loadall);
	cga_map_cells_t GetMapCollisionTableRaw(bool loadall);
	cga_map_cells_t GetMapObjectTable(bool loadall);
	cga_map_cells_t GetMapTileTable(bool loadall);

	bool WalkTo(int x, int y);
	bool TurnTo(int x, int y);
	bool SetMoveSpeed(int speed);
	bool ForceMove(int dir, bool show);
	bool ForceMoveTo(int x, int y, bool show);
	bool IsMapCellPassable(int x, int y);
	bool LogBack();
	bool LogOut();
	bool BackSelectServer();
	bool SayWords(const std::string& str, int, int, int);
	bool ChangeNickName(std::string& str);
	bool ChangeTitleName(int titleId);
	CGA::cga_pers_desc_t ChangePersDesc();
	bool ChangePetName(int petId, const std::string& name);

	bool UseItem(int itempos);
	bool ClickNPCDialog(int option, int index);
	bool SellNPCStore(cga_sell_items_t& items);
	bool BuyNPCStore(cga_buy_items_t& items);
	bool PlayerMenuSelect(int menuindex, const std::string& menustring);
	bool UnitMenuSelect(int menuindex);
	bool UpgradePlayer(int attr);
	bool UpgradePet(int petid, int attr);

	bool IsBattleUnitValid(int pos);
	cga_battle_unit_t GetBattleUnit(int pos);
	cga_battle_units_t GetBattleUnits();
	cga_battle_context_t GetBattleContext();

	bool BattleNormalAttack(int target);
	bool BattleSkillAttack(int skillpos, int skilllv, int target, bool packetOnly);
	bool BattleRebirth();
	bool BattleGuard();
	bool BattleEscape();
	bool BattleExchangePosition();
	bool BattleChangePet(int petid);
	bool BattleUseItem(int itempos, int target);
	bool BattlePetSkillAttack(int skillpos, int target, bool packetOnly);
	bool BattleDoNothing();
	bool BattleSetHighSpeedEnabled(bool enable);
	bool SetGameTextUIEnabled(bool enable);
	bool SetGameTextUICurrentScript(const std::string& script);
	int GetBattleEndTick();
	bool SetBattleEndTick(int msec);
	bool SetWorkDelay(int delay);
	bool SetWorkAcceleration(int percent);
	bool SetImmediateDoneWork(bool enable);
	int GetImmediateDoneWorkState();
	bool StartWork(int skill_index, int sub_index);
	bool CraftItem(pcga_craft_item_t& craft);
	bool AssessItem(int skill_index, int itempos);
	pcga_craft_info_t GetCraftInfo(int skill_index, int sub_index);
	pcga_crafts_info_t GetCraftsInfo(int skill_index);

	int GetCraftStatus();
	bool DoRequest(int request_type);
	bool EnableFlags(int type, bool enable);
	bool TradeAddStuffs(cga_sell_items_t items, cga_sell_pets_t pets, int gold);
	bool AddAllTradeItems(int step);
	cga_team_players_t GetTeamPlayerInfo(cga_team_players_t& result);

	bool FixMapWarpStuck(int type);
	bool SetNoSwitchAnim(bool enable);
	bool SetSwitchAnimForceWait(int state, int ticks);
	//bool GetMoveHistory(std::vector<unsigned long>& v);
	bool SetWindowResolution(int w, int h);
	bool RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize);
	double GetNextAnimTickCount();
	bool LoginGameServer(const std::string& gid, const std::string& glt, int serverid, int bigServerIndex, int serverIndex, int character);
	bool CreateCharacter(const CGA::cga_create_chara_t& req);
	bool IsUIDialogPresent(int dialog);
	bool PlayGesture(int index);
	bool DeleteCard(int index, bool packetonly);
	bool SendMail(int index, const std::string& msg);
	bool SendPetMail(int index, int petid, int itempos, const std::string& msg);

	bool RegisterChatMsgNotify(const std::function<void(CGA::cga_chat_msg_t)>& f);
	bool RegisterServerShutdownNotify(const std::function<void(int)>& callback);
	bool RegisterGameWndKeyDownNotify(const std::function<void(unsigned int)>& callback);
	bool RegisterBattleActionNotify(const std::function<void(int)>& callback);
	bool RegisterPlayerMenuNotify(const std::function<void(cga_player_menu_items_t)>& callback);
	bool RegisterUnitMenuNotify(const std::function<void(cga_unit_menu_items_t)>& callback);
	bool RegisterNPCDialogNotify(const std::function<void(cga_npc_dialog_t)>& callback);
	bool RegisterWorkingResultNotify(const std::function<void(cga_working_result_t)>& callback);
	bool RegisterTradeStuffsNotify(const std::function<void(cga_trade_stuff_info_t)>& callback);
	bool RegisterTradeDialogNotify(const std::function<void(cga_trade_dialog_t)>& callback);
	bool RegisterTradeStateNotify(const std::function<void(int)>& callback);
	bool RegisterDownloadMapNotify(const std::function<void(cga_download_map_t)>& callback);
	bool RegisterConnectionStateNotify(const std::function<void(cga_conn_state_t)>& callback);

	////移动到指定坐标 超时时间 秒  封装下吧  python也可以自己实现寻路
	bool IsInNormalState();
	bool IsInRandomMap();
	bool WaitInNormalState(int timeout = 10000);
	TMLPoint CGAPython::GetMapCoordinate();
	std::vector<TMLPoint> GetMapEntranceList();
	double GetDistance(int x, int y);
	bool MoveTo(int x, int y, int timeout = 10000);
	//根据路径寻路
	bool AutoNavigator(A_FIND_PATH path, bool isLoop = true);
	bool isMoveing() { return m_bMoveing; }
	//本地自动寻路
	int AutoMoveTo(int x, int y, int timeout = 10000);
	int AutoMoveToEx(int x, int y, std::string sMapName = "", int timeout = 10000);
	int AutoMoveToPath(std::vector<pair<int, int> > findPath, int timeout = 10000);
	int AutoMoveInternal(int x, int y, int timeout = 10000, bool isLoop = true);
	int GetGameMapID();
	bool IsReachableTargetEx(int sx, int sy, int tx, int ty);
	bool IsReachableTarget(int tx, int ty);
	A_FIND_PATH CalculatePath(int curX, int curY, int targetX, int targetY);
	TMLPoint GetRandomSpace(int x, int y, int distance, bool judgeReachTgt = true);
	int GetTeammatesCount();
	bool IsTeamLeader();


private:
	CGA::CGAInterface* m_interface;
	bool m_bMoveing = false;
	bool m_bStop = false;
	int m_navigatorLoopCount = 0;
	double m_mazeWaitTime = 5000;
};
