#include "FZParseScript.h"
#include "GameData.h"
#include "MLAssist.h"
#include <windows.h>
#include <QFile>
#include <QtConcurrent>
FZParseScript::FZParseScript()
{
	m_tScriptTimerInterval = 320;
	m_scriptCtrl = 0;
	m_scriptRunRow = 0;
	m_recvHeadMap.insert(RECV_HEAD_lo, "RECV_HEAD_lo");
	m_recvHeadMap.insert(RECV_HEAD_DRQU, "RECV_HEAD_DRQU");
	m_recvHeadMap.insert(RECV_HEAD_GUZ, "RECV_HEAD_GUZ");
	m_recvHeadMap.insert(RECV_HEAD_iqQs, "RECV_HEAD_iqQs");
	m_recvHeadMap.insert(RECV_HEAD_Xv, "RECV_HEAD_Xv");
	m_recvHeadMap.insert(RECV_HEAD_ZJ, "RECV_HEAD_ZJ");
	m_recvHeadMap.insert(RECV_HEAD_adZ, "RECV_HEAD_adZ");
	m_recvHeadMap.insert(RECV_HEAD_kG, "RECV_HEAD_kG");
	m_recvHeadMap.insert(RECV_HEAD_Bgfl, "RECV_HEAD_Bgfl");
	m_recvHeadMap.insert(RECV_HEAD_xG, "RECV_HEAD_xG");
	m_recvHeadMap.insert(RECV_HEAD_rYTy, "RECV_HEAD_rYTy");
	m_recvHeadMap.insert(RECV_HEAD_ik, "RECV_HEAD_ik");
	m_recvHeadMap.insert(RECV_HEAD_LvD, "RECV_HEAD_LvD");
	m_recvHeadMap.insert(RECV_HEAD_OEJd, "RECV_HEAD_OEJd");
	m_recvHeadMap.insert(RECV_HEAD_PxU, "RECV_HEAD_PxU");
	m_recvHeadMap.insert(RECV_HEAD_tn, "RECV_HEAD_tn");
	m_recvHeadMap.insert(RECV_HEAD_JM, "RECV_HEAD_JM");
	m_recvHeadMap.insert(RECV_HEAD_iVy, "RECV_HEAD_iVy");
	m_recvHeadMap.insert(RECV_HEAD_yPJ, "RECV_HEAD_yPJ");
	m_recvHeadMap.insert(RECV_HEAD_crXf, "RECV_HEAD_crXf");
}

FZParseScript::~FZParseScript()
{
}

bool FZParseScript::CheckStop()
{
	if (m_bStopFunRun)
		return true;
	if (m_scriptCtrl == SCRIPT_CTRL_STOP || m_scriptCtrl == SCRIPT_CTRL_PAUSE)
		return true;
	return false;
}

bool FZParseScript::Compare(QVariant v1, QVariant v2, int nType /*= dtCompare_Equal*/)
{
	switch (nType)
	{
		case dtCompare_Equal: return v1 == v2;
		case dtCompare_NotEqual: return v1 != v2;
		case dtCompare_GreaterThan: return v1 > v2;
		case dtCompare_LessThan: return v1 < v2;
		case dtCompare_GreaterEqual: return v1 >= v2;
		case dtCompare_LessEqual: return v1 <= v2;
		default:
			break;
	}
	return false;
}

bool FZParseScript::CompareEx(QVariant v1, QVariant v2, QString sVal /*= "="*/)
{
	int compareType = dtCompare_Equal;
	if (sVal == "=")
	{
		compareType = dtCompare_Equal;
	}
	else if (sVal == "!=")
	{
		compareType = dtCompare_NotEqual;
	}
	else if (sVal == ">")
	{
		compareType = dtCompare_GreaterThan;
	}
	else if (sVal == "<")
	{
		compareType = dtCompare_LessThan;
	}
	else if (sVal == ">=")
	{
		compareType = dtCompare_GreaterEqual;
	}
	else if (sVal == "<=")
	{
		compareType = dtCompare_LessEqual;
	}
	return Compare(v1, v2, compareType);
}

FZParseScript &FZParseScript::getInstance()
{
	static FZParseScript g_pFZParseScript;
	return g_pFZParseScript;
}

void FZParseScript::ParseGameScript(const QString &szPath)
{
	if (!QFile::exists(szPath))
		return;
	QFile file(szPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	m_scriptDataList.clear();
	int nRowIndex = 0;
	//?????????????????? ??????  ????????????
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	m_szScriptDesc = tc->toUnicode(file.readLine());
	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		QString szData = tc->toUnicode(line);
		//		QString szData = QString::fromLocal8Bit(line);
		szData = szData.simplified();
		szData = szData.remove(QRegExp("\\s"));
		int nIndex = szData.indexOf("//");
		szData = szData.mid(0, nIndex);
		nIndex = szData.indexOf(";");
		szData = szData.mid(0, nIndex);
		if (szData.isEmpty()) //???????????????
			continue;
		//		m_script.insert(++nRowIndex, szData);
		m_scriptDataList.append(szData);
	}
	file.close();
	//???????????? ????????????  ?????????????????? ??????
	for (int i = 0; i < m_scriptDataList.size(); ++i)
	{
		QString szData = m_scriptDataList.at(i);
		//?????? ????????????
		if (szData.contains(":")) //goto??????
		{
			int nIndex = szData.indexOf(":");
			m_mark.insert(i, szData.mid(0, nIndex)); //???????????????????????????
		}
	}
}

void FZParseScript::SetScriptRunRow(int nRow)
{
	m_bStopFunRun = true;
	m_scriptRunRow = nRow;
}

bool FZParseScript::RunScript()
{
	m_scriptRunRow = 0;
	//????????????
	if (m_scriptDataList.size() < 1)
		return false;
	for (int i = 0; i < m_scriptDataList.size(); ++i)
	{
		QString szData = m_scriptDataList.at(i);
		szData = szData.simplified(); //??????????????????
		if (szData.startsWith("set("))
		{
			ParseSet(szData);
		}
		else if (szData.startsWith("InputVal("))
		{
			ParseInputVal(szData);
		}
		//?????? ????????????
		if (szData.contains(":")) //goto??????
		{
			m_scriptRunRow = i;
			break; //?????????goto??????  ????????????
		}
	}
	m_scriptCtrl = SCRIPT_CTRL_RUN;
	QtConcurrent::run(RunScriptThread, this);
	return true;
}

bool FZParseScript::StopScript()
{
	m_scriptCtrl = SCRIPT_CTRL_STOP;
	return true;
}

bool FZParseScript::PauseScript()
{
	m_scriptCtrl = SCRIPT_CTRL_PAUSE;
	return true;
}

void FZParseScript::ParseSet(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() == 2)
	{
		if (args[0] == "timer")
		{
			setTimerCmd(SCRIPT_TIMER_RUN, args.at(1).toInt());
		}
		else if (args[0] == "timer1")
		{
			setTimerCmd(SCRIPT_TIMER_SYS, args.at(1).toInt());
		}
	}
	else if (args.size() == 3)
	{
	}
	else if (args.size() == 13)
	{
		if (args[0].contains("????????????"))
		{
			GameData::getInstance().SetCharacterSwitch(args[2].toInt(), args[4].toInt(), args[6].toInt(), args[8].toInt(), args[10].toInt(), args[12].toInt());
		}
	}
}

void FZParseScript::setTimerCmd(int nType, int nTimer)
{
	switch (nType)
	{
		case SCRIPT_TIMER_RUN: m_tScriptTimerInterval = nTimer; break;
		case SCRIPT_TIMER_SYS: m_tSysCmdTimerInterval = nTimer; break;
		default: break;
	}
}
//????????????
void FZParseScript::setAutoActionCmd(int nType, int nVal)
{
}

bool FZParseScript::nop()
{
	while (g_pGameFun->IsInBattle()) //????????? ??????
	{
		Sleep(m_tScriptTimerInterval);
	}
	return true;
}

bool FZParseScript::Npc(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	szScript = szScript.remove("\"");
	QStringList args = szScript.split(",");
	if (args.size() == 3)
	{
		int index = 0;
		if (args[1].contains("null"))
			index = 0;
		else
			index = args[1].toInt();
		g_pGameFun->Npc(args[0].toInt(), index);
	}
	return true;
}

bool FZParseScript::nowhile(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	szScript = szScript.replace("\\S", " ");
	szScript = szScript.replace("\\S", " ");
	szScript = szScript.remove("\"");
	QStringList args = szScript.split(",");
	if (args.size() == 3)
	{
		while (!CheckStop()) //????????? ??????
		{
			QString szMap = g_pGameFun->GetMapName();
			if (szMap == args[0])
			{
				QPoint gamePoint = g_pGameFun->GetMapCoordinate();
				if (gamePoint.x() == args[1].toInt() && gamePoint.y() == args[2].toInt())
					break;
			}
			Sleep(m_tScriptTimerInterval);
		}
	}
	else if (args.size() == 2)
	{
		while (!CheckStop()) //????????? ??????
		{
			QString szMap = g_pGameFun->GetMapName();
			qDebug() << szMap << args[0];

			if (szMap == args[0])
			{
				break;
			}
			Sleep(m_tScriptTimerInterval);
		}
	}
	else if (args.size() == 1)
	{
		if (args[0] == "renew")
		{
			g_pGameCtrl->OnSetAutoSupply(true); //??????
												//			GameData::getInstance().selectRenew();
		}
	}

	return true;
}

bool FZParseScript::moveto(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() != 2)
		return false;
	int nTimeOut = 60000; //1??????????????? ??????
	QPoint lastPoint;	  //????????????????????? ???????????????????????????????????????????????? ????????????
	lastPoint = g_pGameFun->GetMapCoordinate();
	while (!CheckStop())
	{
		if (m_scriptCtrl == SCRIPT_CTRL_PAUSE)
		{
			Sleep(m_tScriptTimerInterval);
			continue;
		}
		//	GameData::getInstance().moveToPoint(args[0].toInt(), args[1].toInt());
		QPoint gamePoint = g_pGameFun->GetMapCoordinate();
		//qDebug() << szData << "???????????????"<< gamePoint.x << gamePoint.y;
		if (gamePoint.x() == args[0].toInt() && gamePoint.y() == args[1].toInt())
			break;
		if (lastPoint.x() == gamePoint.x() && lastPoint.y() == gamePoint.y()) //??????????????? ????????? ??????????????????
		{																	  //????????????????????? ?????????
			g_pGameFun->AutoMoveTo(args[0].toInt(), args[1].toInt());
		}
		lastPoint = gamePoint;
		Sleep(m_tScriptTimerInterval);
	}
	Sleep(m_tScriptTimerInterval); //????????? ????????? ????????????
	return true;
}

bool FZParseScript::turn_about(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() != 2)
		return false;
	int direction = args[0].toInt();
	g_pGameFun->TurnAbout(direction + 1);
	//	GameData::getInstance().turn_about(direction);
	return true;
}

bool FZParseScript::high(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() != 3)
		return false;
	args.removeAt(0);	  //???????????????
	int nTimeOut = 60000; //1??????????????? ??????
	QPoint lastPoint;	  //????????????????????? ???????????????????????????????????????????????? ????????????
	lastPoint = g_pGameFun->GetMapCoordinate();
	while (m_scriptCtrl != SCRIPT_CTRL_STOP)
	{
		if (m_scriptCtrl == SCRIPT_CTRL_PAUSE)
		{
			Sleep(m_tScriptTimerInterval);
			continue;
		}
		//	GameData::getInstance().moveToPoint(args[0].toInt(), args[1].toInt());
		QPoint gamePoint = g_pGameFun->GetMapCoordinate();
		if (gamePoint.x() == args[0].toInt() && gamePoint.y() == args[1].toInt())
			break;
		if (lastPoint.x() == gamePoint.x() && lastPoint.y() == gamePoint.y()) //??????????????? ????????? ??????????????????
			g_pGameFun->AutoMoveTo(args[0].toInt(), args[1].toInt());
		lastPoint = gamePoint;
		Sleep(m_tScriptTimerInterval);
	}
	Sleep(m_tScriptTimerInterval); //????????? ????????? ????????????
	return true;
}

bool FZParseScript::work(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	szScript = szScript.remove("\"");
	QStringList args = szScript.split(",");
	if (args.size() == 2)
	{
		//	while(1)
		{
			g_pGameFun->Work(args.at(0).toInt(), args.at(1).toInt());
			Sleep(m_tScriptTimerInterval);
			/*if (GameData::getInstance().isWorking() == false)
			{
				GameData::getInstance().Work(args.at(0), args.at(1));
				Sleep(m_tScriptTimerInterval);
			}
			else
				break;*/
		}
	}
	else if (args.size() == 1) //?????????????????? ???????????????
	{
	}
	return true;
}

bool FZParseScript::renew(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() != 1)
		return false;
	int direction = args.at(0).toInt();
	GameData::getInstance().turn_about(direction);
	int nHp = GameData::getInstance().getPersionHP();
	int nMp = GameData::getInstance().getPersionMP();
	GameData::getInstance().selectRenew();
	int nRenewHp = GameData::getInstance().getPersionHP();
	int nRenewMp = GameData::getInstance().getPersionMP();
	if (nHp == nRenewHp && nMp == nRenewMp)
	{
		renew(szData); //??????
	}
	return true;
}

bool FZParseScript::set_wait(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	g_pGameFun->SetWaitRecv(m_recvHeadMap.key(szScript));
	return true;
}

bool FZParseScript::Wait(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	return g_pGameFun->WaitRecvHead(m_recvHeadMap.key(szScript));
}

QString FZParseScript::MidBrackets(const QString &szData)
{
	QString szScript = szData.mid(szData.indexOf("(") + 1);
	szScript = szScript.mid(0, szScript.lastIndexOf(")"));
	return szScript;
}

//????????????
void FZParseScript::ParseInputVal(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(","); //??????
	if (args.size() != 3)
		return;
	m_userDefVariable.insert(args[0], args[2].remove("\"").toInt());
}

void FZParseScript::ParseIFCmd(const QString &szData)
{
	QString szScript = MidBrackets(szData);
	szScript = szScript.remove("\""); //???????????????
	QStringList args = szScript.split(",");
	if (args.size() < 5)
		return;
	auto player = g_pGameFun->GetGameCharacter();;
	if (player == nullptr)
		return;
	if (args[0].contains("??????"))
	{
		if (args[1].contains("GameMap.x.y")) //????????????????????????
		{
			if (args[2] == "=")
			{ //??????????????????
				QPoint curPoint = g_pGameFun->GetMapCoordinate();
				//????????????????????? ???x y??????????????? ????????????,???????????? ???6?????????
				if (curPoint.x() == args[3].toInt() && curPoint.y() == args[4].toInt())
				{ //??????
					int nRow = m_mark.key(args[5]);
					m_scriptRunRow = nRow;
				}
			}
		}
		else if (args[1].contains("Character._health")) //????????????
		{
			int nHealth = player->health; //g_pGameFun->GetCharacterHealth();
			bool bRet = CompareEx(nHealth, args[3].toInt(), args[2]);
			if (bRet)
			{
				int nRow = m_mark.key(args[5]);
				m_scriptRunRow = nRow;
			}
		}
		else if (args[1].contains("Character._loc")) //??????
		{
			QString curMap = g_pGameFun->GetMapName();
			bool bRet = CompareEx(curMap, args[3].toInt(), args[2]);
			if (bRet)
			{
				int nRow = m_mark.key(args[4]);
				m_scriptRunRow = nRow;
			}
		}
	}
	else if (args[0].contains("???"))
	{
		if (args[1].contains("???"))
		{
			int nSetHp = args[3].toInt();
			int nHp = player->hp;
			bool bRet = CompareEx(nHp, nSetHp, args[2]);
			if (bRet)
			{
				int nRow = m_mark.key(args[4]);
				m_scriptRunRow = nRow;
			}
		}
		else if (args[1].contains("???"))
		{
			int nSetMp = args[3].toInt();
			int nMp = player->mp;
			bool bRet = CompareEx(nMp, nSetMp, args[2]);
			{
				//??????
				int nRow = m_mark.key(args[4]);
				m_scriptRunRow = nRow;
			}
		}
	}
	else if (args[0].contains("???"))
	{
		if (args[1].contains("???"))
		{
		}
		else if (args[1].contains("???"))
		{
		}
	}
}

void FZParseScript::RunScriptThread(FZParseScript *pThis)
{
	while (pThis->m_scriptCtrl != SCRIPT_CTRL_STOP) //??????????????? ??????
	{
		if (pThis->m_scriptCtrl == SCRIPT_CTRL_PAUSE)
		{
			Sleep(pThis->m_tScriptTimerInterval);
			continue;
		}
		if (pThis->m_scriptRunRow > (pThis->m_scriptDataList.size() - 1))
		{
			pThis->m_scriptCtrl = SCRIPT_CTRL_STOP;
			emit pThis->refreshScriptUI();
			return;
		}
		QString szScriptData = pThis->m_scriptDataList.at(pThis->m_scriptRunRow);
		szScriptData = szScriptData.simplified(); //??????????????????
		qDebug() << szScriptData;
		//??????????????????
		if (szScriptData == "nop")
		{
			pThis->nop();
		}
		else if (szScriptData == "getout")
		{
			g_pGameFun->Logout();
		}
		else if (szScriptData.startsWith("WaitTime("))
		{
			int nVal = pThis->MidBrackets(szScriptData).toInt();
			Sleep(nVal);
		}
		else if (szScriptData.startsWith("if("))
		{
			pThis->ParseIFCmd(szScriptData);
		}
		else if (szScriptData.startsWith("goto("))
		{
			pThis->m_scriptRunRow = pThis->m_mark.key(pThis->MidBrackets(szScriptData));
		}
		else if (szScriptData.startsWith("moveto("))
		{
			pThis->moveto(szScriptData);
		}
		else if (szScriptData.startsWith("turn_about("))
		{
			pThis->turn_about(szScriptData);
		}
		else if (szScriptData.startsWith("high("))
		{
			pThis->high(szScriptData);
		}
		else if (szScriptData.startsWith("nowhile("))
		{
			pThis->nowhile(szScriptData);
		}
		else if (szScriptData.startsWith("Work("))
		{
			pThis->work(szScriptData);
		}
		else if (szScriptData.startsWith("end_auto_action"))
		{
			g_pGameFun->end_auto_action();
		}
		else if (szScriptData.startsWith("begin_auto_action"))
		{
			g_pGameFun->begin_auto_action();
		}
		else if (szScriptData.startsWith("Npc"))
		{
			pThis->Npc(szScriptData);
			;
		}
		else if (szScriptData.startsWith("ClearSysCue"))
		{
			g_pGameFun->ClearSysCue();
		}
		else if (szScriptData.startsWith("set_wait"))
		{
			pThis->set_wait(szScriptData);
			;
		}
		else if (szScriptData.startsWith("wait"))
		{
			pThis->Wait(szScriptData);
			;
		}
		if (pThis->m_bStopFunRun) //????????????????????????
		{
			pThis->m_bStopFunRun = false;
		}
		pThis->m_scriptRunRow = pThis->m_scriptRunRow + 1;
		emit pThis->updateScriptRow(pThis->m_scriptRunRow);
		Sleep(pThis->m_tScriptTimerInterval);
	}
}
