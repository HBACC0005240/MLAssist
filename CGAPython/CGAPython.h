#include "./pybind11/include/pybind11/pybind11.h"
#include "./pybind11/include/pybind11/functional.h"
#include "../CGALib/gameinterface.h"

namespace py = pybind11;
using PyCallbackFunc = std::function<void(py::object)>;
using namespace CGA;
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
private:
	CGA::CGAInterface* m_interface;
};
