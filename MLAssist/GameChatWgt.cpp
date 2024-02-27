#include "GameChatWgt.h"
#include "../include/ITPublic.h"
#include "GameData.h"
#include "UserDefDialog.h"
#include "YunLai.h"
#include <QDateTime>
#include <QScrollBar>
#include <QMenu>
GameChatWgt::GameChatWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);

	init();
	connect(g_pGameCtrl, SIGNAL(signal_addOneChat(const QString &)), this, SLOT(doAddOneChat(const QString &)), Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyChatMsg, this, &GameChatWgt::OnNotifyChatMsg, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGameCharacterInfo, this, &GameChatWgt::OnNotifyGetPlayerInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyFillChatSettings, this, &GameChatWgt::OnNotifyFillChatSettings, Qt::ConnectionType::QueuedConnection);

	m_nLastIndex = 0;
	m_chatStoreCount = ui.lineEdit_chatCount->text().toInt();

	ui.textEdit->document()->setMaximumBlockCount(m_chatStoreCount);
	connect(ui.checkBox_timer, SIGNAL(stateChanged(int)), this, SLOT(OnCtrlTimer(int)), Qt::ConnectionType::QueuedConnection);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimeChat()));
	connect(&m_friendCardTimer, SIGNAL(timeout()), this, SLOT(OnUpdateFriendCard()));
	connect(&m_blockChatTimer, SIGNAL(timeout()), this, SLOT(OnSyncBlockChatMsgs()));
	m_friendCardTimer.start(10000); //100秒刷一次
	m_blockChatTimer.setSingleShot(true);
	connect(ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(doItemDoubleClicked(QListWidgetItem *)));
	ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::PartiallyChecked);
	//connect(ui.listWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_listWidget_customContextMenuRequested(const QPoint &)));
}

GameChatWgt::~GameChatWgt()
{
}

void GameChatWgt::init()
{
	ui.comboBox_chatColor->clear();
	QList<QColor> gameColorList;
	QMap<int, QColor> colorMap;
	colorMap.insert(0, QColor(255, 255, 255));
	colorMap.insert(1, QColor(0, 255, 255));   //湖蓝色
	colorMap.insert(2, QColor(255, 0, 255));   /*紫色*/
	colorMap.insert(3, QColor(0, 0, 255));	   /*蓝色*/
	colorMap.insert(5, QColor(0, 255, 0));	   /*绿色*/
	colorMap.insert(6, QColor(255, 0, 0));	   /*红色*/
	colorMap.insert(7, QColor(160, 160, 164)); /*灰色*/
	colorMap.insert(8, QColor(166, 202, 240)); /*亮灰*/
	colorMap.insert(9, QColor(192, 220, 192)); /*绿灰*/

	for (auto it = colorMap.begin(); it != colorMap.end(); ++it)
	{
		QPixmap pix(QSize(70, 20));
		pix.fill(QColor(it.value()));
		ui.comboBox_chatColor->addItem(QIcon(pix), "", it.key());
		ui.comboBox_chatColor->setIconSize(QSize(70, 20));
		ui.comboBox_chatColor->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	}
	QStringList oftenMsgs;
	oftenMsgs << "呦呦 切克闹，煎饼果子来一套！";
	oftenMsgs << "你是风儿，我是沙，我们一切沙沙沙~";
	for (auto msg : oftenMsgs)
	{
		ui.comboBox_OftenMsg->addItem(msg);
	}
}

void GameChatWgt::on_lineEdit_chat_returnPressed()
{
	auto saystring = ui.lineEdit_chat->text();
	doGameChat(saystring);
	ui.lineEdit_chat->setText("");
}

void GameChatWgt::on_lineEdit_chatCount_editingFinished()
{
	m_chatStoreCount = ui.lineEdit_chatCount->text().toInt();
	ui.textEdit->document()->setMaximumBlockCount(m_chatStoreCount);
}

void GameChatWgt::OnCtrlTimer(int nFlag)
{
	if (nFlag == Qt::Checked)
	{
		bool bOk = false;
		int val = ui.lineEdit_Timer->text().toInt(&bOk);
		if (bOk)
		{
			m_timer.setInterval(val * 1000);
			m_timer.start();
		}
	}
	else
	{
		m_timer.stop();
	}
}

void GameChatWgt::OnTimeChat()
{
	auto saystring = ui.lineEdit_chat->text();
	if (saystring.isEmpty())
	{
		m_timer.stop();
		return;
	}
	doGameChat(saystring);
}

void GameChatWgt::doLoadUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("chat");
	ui.checkBox_BlockChatMsgs->setCheckState((Qt::CheckState)iniFile.value("blockState").toInt());
	iniFile.endGroup();
}

void GameChatWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("chat");
	iniFile.setValue("blockState", (int)ui.checkBox_BlockChatMsgs->checkState()); 
	iniFile.endGroup();
}

void GameChatWgt::doLoadJsConfig(QJsonObject &obj)
{
	if (obj.contains("blockchatmsgs"))
	{
		int val = obj.take("blockchatmsgs").toInt();
		if (val == 2)
		{
			ui.checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Checked);
		}
		else if (val == 1)
		{
			ui.checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::PartiallyChecked);
		}
		else if (val == 0)
		{
			ui.checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Unchecked);
		}
	}
}

void GameChatWgt::doSaveJsConfig(QJsonObject &obj)
{
	obj.insert("blockchatmsgs", (int)ui.checkBox_BlockChatMsgs->checkState());
}

void GameChatWgt::on_comboBox_chatColor_currentIndexChanged(int index)
{
	m_nChatColorIndex = ui.comboBox_chatColor->currentData().toInt();
}

void GameChatWgt::on_radioButton_big_toggled(bool checked)
{
	if (checked)
	{
		m_nChatSize = 2;
	}
}

void GameChatWgt::on_radioButton_mid_toggled(bool checked)
{
	if (checked)
	{
		m_nChatSize = 0;
	}
}

void GameChatWgt::on_radioButton_small_toggled(bool checked)
{
	if (checked)
	{
		m_nChatSize = 1;
	}
}

void GameChatWgt::OnUpdateFriendCard()
{
	if (!this->isVisible())
		return;
	if (!g_pGameFun->IsOnline())
		return;

	CGA::cga_cards_info_t cards;
	g_CGAInterface->GetCardsInfo(cards);
	if (ui.listWidget->count() == cards.size())
		return;
	CGA::cga_cards_info_t onlinecards;
	CGA::cga_cards_info_t offlinecards;
	for (auto info : cards)
	{
		if (info.server == 0)
			offlinecards.push_back(info);
		else
			onlinecards.push_back(info);
	}
	qSort(onlinecards.begin(), onlinecards.end(), [&](auto a, auto b)
			{ return a.server < b.server; });
	auto createListItemFun = [&](const CGA::cga_cards_info_t &tmpCards)
	{
		for (auto info : tmpCards)
		{
			QString sInfo = QString("%1 Lv:%2 %3线").arg(QString::fromStdString(info.name)).arg(info.level).arg(info.server);
			auto pItem = m_cardForItem.value(QString::fromStdString(info.name));
			if (pItem == nullptr)
			{
				pItem = new QListWidgetItem();
				pItem->setData(Qt::UserRole, info.index);
				ui.listWidget->addItem(pItem);
				m_cardForItem.insert(QString::fromStdString(info.name), pItem);
			}
			pItem->setText(sInfo);
			if (info.server != 0)
				pItem->setTextColor(QColor(0, 0, 255));
			else
				pItem->setTextColor(QColor(0, 0, 0));
			pItem->setToolTip(QString::fromStdString(info.nickname));
		}
	};
	createListItemFun(onlinecards);
	createListItemFun(offlinecards);
}

void GameChatWgt::doItemDoubleClicked(QListWidgetItem *pItem)
{
	if (!pItem)
		return;
	auto name = m_cardForItem.key(pItem);
	UserDefDialog dlg(this);
	dlg.setWindowTitle("发送邮件");
	dlg.setLabelText("信息:");
	if (dlg.exec() == QDialog::Accepted)
	{
		int index = pItem->data(Qt::UserRole).toInt();

		bool bRet = false;
		g_CGAInterface->SendMail(index, dlg.getVal().toStdString(), bRet);
	}
}

void GameChatWgt::OnNotifyGetPlayerInfo(CharacterPtr charinfo)
{
	m_player = charinfo;
}

void GameChatWgt::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
	QMenu menu;
	menu.addAction("刷新邮件列表", this, SLOT(OnUpdateFriendCard()));
	menu.exec(QCursor::pos());
}

void GameChatWgt::on_checkBox_BlockChatMsgs_stateChanged(int state)
{
	if (g_CGAInterface->IsConnected())
	{
		switch (state)
		{
			case Qt::CheckState::Checked:
				g_CGAInterface->SetBlockChatMsgs(2);
				break;
			case Qt::CheckState::PartiallyChecked:
				g_CGAInterface->SetBlockChatMsgs(1);
				break;
			case Qt::CheckState::Unchecked:
				g_CGAInterface->SetBlockChatMsgs(0);
				break;
		}
	}else
	{
		m_blockChatTimer.start(1000);
	}
}

void GameChatWgt::addOneChatData(const QString &chatData, int nType, int color)
{
	//QMutexLocker locker(&m_tableMutex);

	QString line;
	QString msg = chatData;
	msg = msg.replace("\\c", ",");
	line.append(QString("[%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

	line.append(QString("<font color=\"#0000FF\">%1: %2</font><br>").arg("").arg(msg));

	QTextCursor txtcur = ui.textEdit->textCursor();
	txtcur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	txtcur.insertHtml(line);
	txtcur.insertBlock();
}

void GameChatWgt::doGameChat(const QString &chatData)
{
	int ingame = 0;
	if (g_CGAInterface->IsInGame(ingame) && ingame)
	{ //color range size
		g_pGameFun->Chat(chatData, m_nChatColorIndex, 3, m_nChatSize);
	}
}

void GameChatWgt::doUpdateGameChat()
{
	return;
	//聊天基址005A91C4  差值 134
	//005A7014基址 005D218C 数组索引
	int nCurChatIndex = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "005D218C");
	//比对索引 当前是2说明有1条数据  3有2条
	if (m_nLastIndex != (nCurChatIndex - 1)) //说明有新的聊天记录
	{
		for (int i = m_nLastIndex; i < (nCurChatIndex - 1); ++i)
		{
			DWORD pAddress = 0x005A7014;
			DWORD offset = i * 0x134;
			pAddress += offset;
			QString chatData = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 300)));
			if (!chatData.isEmpty()) //不为空 记录时间  2秒定时器 只能这样来记录大概时间了
			{
				chatData = chatData.trimmed();
				//	ui.textEdit->append(QString("%1 %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd")).arg(chatData));
				addOneChatData(chatData);
			}
		}
		//ui.tableWidget->verticalScrollBar()->setSliderPosition(ui.tableWidget->verticalScrollBar()->maximum());
	}
	m_nLastIndex = (nCurChatIndex - 1);
}

void GameChatWgt::on_comboBox_OftenMsg_currentIndexChanged(int index)
{
	ui.lineEdit_chat->setText(ui.comboBox_OftenMsg->currentText());
}

void GameChatWgt::on_pushButton_AddOftenMsg_clicked()
{
	QString sText = ui.comboBox_OftenMsg->currentText();
	int index = ui.comboBox_OftenMsg->findText(sText);
	if (index < 0)
	{
		ui.comboBox_OftenMsg->addItem(sText);
	}
}

void GameChatWgt::on_pushButton_chat_clicked()
{
	/*QString szChat = ui.textEdit->toPlainText();
	GameData::getInstance().Chat(szChat);*/
}

void GameChatWgt::on_pushButton_clearSendChat_clicked()
{
	ui.lineEdit_chat->setText("");
}

void GameChatWgt::on_pushButton_mail_clicked()
{
	CGA::cga_cards_recv_msg_t cardMsgs;
	g_CGAInterface->GetCardsRecvMsg(cardMsgs);
	for (int i = 0; i < cardMsgs.size(); ++i)
	{
		auto tMsg = cardMsgs[i];
		qDebug() << tMsg.index << QString::fromStdString(tMsg.name);

		for (int n = 0; n < 10; ++n)
		{
			qDebug() << n << tMsg.msgs[n].state << QString::fromStdString(tMsg.msgs[n].date) << QString::fromStdString(tMsg.msgs[n].msg);
		}
	}
	return;
	struct MyStruct
	{
		short mailstate;
		char sdate[18];
		char smsg[300];
	};
	MyStruct tStru;
	if (g_pGameCtrl->getGamePID() != 0 && g_pGameCtrl->getGameBaseAddr() != 0)
	{
		for (int i = 0; i < 60; ++i)
		{
			for (int n = 0; n < 10; ++n) //0xC67B28
			{
				DWORD pAddress = 0x10623A8 - 0x400000;
				//0xC623A8;

				//0xC67B28;
				DWORD offset = n * 0x140;
				DWORD offsetCard = i * 0xC80;
				pAddress += offset;
				pAddress += offsetCard;
				memset(&tStru, 0, sizeof(MyStruct));
				char *pData = YunLai::ReadMemoryStrFromProcessID(g_pGameCtrl->getGamePID(), (ULONG_PTR)g_pGameCtrl->getGameBaseAddr() + pAddress, 320); //
				memcpy(&tStru, pData, 320);
				//if (tStru.sdate)
				qDebug() << i << n << tStru.mailstate << tStru.sdate << QString::fromWCharArray(ANSITOUNICODE1(tStru.smsg));
			}
		}
	}
}

void GameChatWgt::doAddOneChat(const QString &chatData)
{
	addOneChatData(chatData);
}

void GameChatWgt::OnNotifyChatMsg(int unitid, QString msg, int size, int color)
{
	if (m_player && !m_player->name.isEmpty())
	{
		int type = -1;
		QString name;
		if (unitid > 0)
		{
			if (m_player->unitid == unitid)
			{
				type = 1;
				name = m_player->name;
			}
			else
			{
				CGA::cga_map_units_t units;
				if (g_CGAInterface->GetMapUnits(units))
				{
					for (const auto &u : units)
					{
						if (u.valid == 2 && ((u.flags & 256) || (u.flags & 4096)) && u.unit_id == unitid)
						{
							type = (u.flags & 256) ? 2 : 3;
							name = QString::fromStdString(u.unit_name);
							break;
						}
					}
				}
			}
		}
		else
		{
			type = 0;
		}

		if (type == -1)
			return;

		if (type >= 1 && type <= 3)
		{
			auto header = QString("%1: ").arg(name);
			if (msg.indexOf(header) == 0)
			{
				msg = msg.mid(header.length());
			}
			else
			{
				auto header2 = QString("[GP]%1: ").arg(name);
				if (msg.indexOf(header2) == 0)
				{
					msg = msg.mid(header2.length());
				}
			}
		}

		QString line;

		msg = msg.replace("\\c", ",");
		line.append(QString("[%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

		if (type == 0)
		{
			line.append(QString("<font color=\"#FF0000\">%1</font><br>").arg(msg));
		}
		else if (type == 1)
		{
			line.append(QString("<font color=\"#0000FF\">%1: %2</font><br>").arg(name).arg(msg));
		}
		else
		{
			line.append(QString("%1: %2<br>").arg(name).arg(msg));
		}

		QTextCursor txtcur = ui.textEdit->textCursor();
		txtcur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		txtcur.insertHtml(line);
		txtcur.insertBlock();
	}
}

void GameChatWgt::OnNotifyFillChatSettings(int blockchatmsgs)
{
	if (blockchatmsgs == 2)
	{
		ui.checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Checked);
	}
	else if (blockchatmsgs == 1)
	{
		ui.checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::PartiallyChecked);
	}
	else if (blockchatmsgs == 0)
	{
		ui.checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Unchecked);
	}
}

void GameChatWgt::OnSyncBlockChatMsgs()
{
	m_blockChatTimer.stop();
	if (g_CGAInterface->IsConnected())
	{
		g_CGAInterface->SetBlockChatMsgs((int)ui.checkBox_BlockChatMsgs->checkState());
	}else
	{
		m_blockChatTimer.start(1000);
	}
}
