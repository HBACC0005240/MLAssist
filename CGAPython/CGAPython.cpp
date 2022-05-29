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
	m_interface->Initialize(data);
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
