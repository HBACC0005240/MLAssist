#include "GameScriptSetWgt.h"
#include "CGFunction.h"
#include "GameCtrl.h"
GameScriptSetWgt::GameScriptSetWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	ui.checkBox_hpPercentageUI->hide();
	ui.checkBox_mpPercentageUI->hide();
	ui.checkBox_petHpPercentageUI->hide();
	ui.checkBox_petMpPercentageUI->hide();
	connect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doCustomScriptItemChanged(QListWidgetItem *)));
	readCustomJson();
	initListWidget();
}

GameScriptSetWgt::~GameScriptSetWgt()
{
}

bool GameScriptSetWgt::readCustomJson()
{
	ui.listWidget->clear();
	QString path = QApplication::applicationDirPath() + "//db//scriptUiCustom.json";
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << path << "File open failed!";
		return false;
	}
	else
	{
		qDebug() << "File open successfully!";
	}
	QJsonParseError *error = new QJsonParseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), error);
	if (!jsonDoc.isNull() && (error->error == QJsonParseError::NoError))
	{
		QJsonArray userCustomScriptData = jsonDoc.toVariant().toJsonArray();
		m_userCustomScriptData.clear();
		for (auto tScriptData : userCustomScriptData)
		{
			QJsonObject tp = tScriptData.toObject();
			auto name = tp.value("name").toString();
			auto default = tp.value("default").toString();
			QString type = tp.value("type").toString();
			if (type.isEmpty())
			{
				type = "string";
			}
			QVariantList sData;
			sData << name << default << type;
			m_userCustomScriptData.insert(name, sData);
			createListWidgetItem(sData);
		}
		/*QJsonObject jsonObj = jsonDoc.object();
		if (!jsonObj.isEmpty())
		{
			m_professions = jsonObj.value("Professions").toArray();
		}*/
	}
	file.close();
	return true;
}

void GameScriptSetWgt::initListWidget()
{
	/*ui.listWidget->clear();
	for (auto it = m_userCustomScriptData.begin(); it != m_userCustomScriptData.end(); ++it)
	{
		QVariantList tScriptData = it.value();
		QString name = tScriptData[0].toString();
		QVariant default = tScriptData[1].toString();
		QVariant type = tScriptData[2].toString();

		QListWidgetItem *pItem = new QListWidgetItem();
		pItem->setSizeHint(QSize(150, 30));
		pItem->setToolTip(pItem->text());
		QWidget *pWidget = new QWidget();
		QLabel *pLabel = new QLabel(name, pWidget);
		pLabel->setMinimumSize(50, 26);
		QLineEdit *pEdit = new QLineEdit(pWidget);
		pEdit->setMinimumSize(100, 20);
		pEdit->setText(default.toString());
		QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
		pLayout->addWidget(pLabel);
		pLayout->addWidget(pEdit);
		pLayout->addStretch();
		pLayout->setContentsMargins(0, 0, 0, 0);
		pWidget->setLayout(pLayout);
		ui.listWidget->addItem(pItem);
		ui.listWidget->setItemWidget(pItem, pWidget);
		g_pGameFun->SetScriptUiSetData(name, default);
		m_nameForEdit.insert(name, pEdit);
		connect(pEdit, SIGNAL(editingFinished()), this, SLOT(doCustomScript_editingFinished()));
	}

	connect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doCustomScriptItemChanged(QListWidgetItem *)));*/
}

void GameScriptSetWgt::createListWidgetItem(QVariantList userData)
{
	QVariantList tScriptData = userData;
	QString name = tScriptData[0].toString();
	QVariant default = tScriptData[1].toString();
	QVariant type = tScriptData[2].toString();

	QListWidgetItem *pItem = new QListWidgetItem();
	pItem->setSizeHint(QSize(150, 30));
	pItem->setToolTip(pItem->text());
	QWidget *pWidget = new QWidget();
	QLabel *pLabel = new QLabel(name, pWidget);
	pLabel->setMinimumSize(50, 26);
	QLineEdit *pEdit = new QLineEdit(pWidget);
	pEdit->setMinimumSize(100, 20);
	pEdit->setText(default.toString());
	QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
	pLayout->addWidget(pLabel);
	pLayout->addWidget(pEdit);
	pLayout->addStretch();
	pLayout->setContentsMargins(0, 0, 0, 0);
	pWidget->setLayout(pLayout);
	ui.listWidget->addItem(pItem);
	ui.listWidget->setItemWidget(pItem, pWidget);
	g_pGameFun->SetScriptUiSetData(name, default);
	m_nameForEdit.insert(name, pEdit);
	connect(pEdit, SIGNAL(editingFinished()), this, SLOT(doCustomScript_editingFinished()));
}

void GameScriptSetWgt::doLoadUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("ScriptUi");
	ui.comboBox_leaderName->setCurrentText(iniFile.value("TeamLeaderName").toString());
	ui.lineEdit_playerHpUI->setText(iniFile.value("PlayerHp").toString());
	ui.lineEdit_playerMpUI->setText(iniFile.value("PlayerMp").toString());
	ui.lineEdit_petHpUI->setText(iniFile.value("PetHp").toString());
	ui.lineEdit_petMpUI->setText(iniFile.value("PetMp").toString());
	ui.lineEdit_troopCountUI->setText(iniFile.value("TroopCount").toString());
	ui.lineEdit_teammateName->setText(iniFile.value("TeammateName").toString());

	QMap<QString, QVariant> existData = g_pGameFun->GetScriptUiSet();
	int count = iniFile.value("CustomCount").toInt();
	int index = 1;
	if (count > 0)
	{
		ui.listWidget->clear();
		m_nameForEdit.clear();
	//	m_userCustomScriptData.clear();
	}
	for (int i = 0; i < count; ++i)
	{
		QString name = iniFile.value(QString("customName%1").arg(index)).toString();
		auto val = iniFile.value(QString("customVal%1").arg(index)).toString();
		auto type = iniFile.value(QString("customValType%1").arg(index));

		if (m_nameForEdit.contains(name))
		{
			auto pEdit = m_nameForEdit.value(name);
			pEdit->setText(val);
		}
		else
		{
			QListWidgetItem *pItem = new QListWidgetItem();
			pItem->setSizeHint(QSize(150, 30));
			pItem->setToolTip(pItem->text());
			QWidget *pWidget = new QWidget();
			QLabel *pLabel = new QLabel(name, pWidget);
			pLabel->setMinimumSize(50, 26);
			QLineEdit *pEdit = new QLineEdit(pWidget);
			pEdit->setMinimumSize(100, 20);
			pEdit->setText(val);
			QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
			pLayout->addWidget(pLabel);
			pLayout->addWidget(pEdit);
			pLayout->addStretch();
			pLayout->setContentsMargins(0, 0, 0, 0);
			pWidget->setLayout(pLayout);
			ui.listWidget->addItem(pItem);
			ui.listWidget->setItemWidget(pItem, pWidget);
			m_nameForEdit.insert(name, pEdit);
			m_userCustomScriptData.insert(name, QVariantList() << name << val << type);
			connect(pEdit, SIGNAL(editingFinished()), this, SLOT(doCustomScript_editingFinished()));
		}
		index++;
		g_pGameFun->SetScriptUiSetData(name, GetUserDefineData(name, val));

	}
	iniFile.endGroup();
	on_pushButton_fetchTeamData_clicked();
	on_lineEdit_playerHpUI_editingFinished();
	on_lineEdit_playerMpUI_editingFinished();
	on_lineEdit_petMpUI_editingFinished();
	on_lineEdit_petHpUI_editingFinished();
	on_lineEdit_troopCountUI_editingFinished();
	on_lineEdit_teammateName_editingFinished();
}

void GameScriptSetWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("ScriptUi");
	iniFile.setValue("TeamLeaderName", ui.comboBox_leaderName->currentText());
	iniFile.setValue("PlayerHp", ui.lineEdit_playerHpUI->text());
	iniFile.setValue("PlayerMp", ui.lineEdit_playerMpUI->text());
	iniFile.setValue("PetHp", ui.lineEdit_petHpUI->text());
	iniFile.setValue("PetMp", ui.lineEdit_petMpUI->text());
	iniFile.setValue("TroopCount", ui.lineEdit_troopCountUI->text());
	iniFile.setValue("TeammateName", ui.lineEdit_teammateName->text());

	iniFile.setValue("CustomCount", m_userCustomScriptData.size());
	int index = 1;
	for (auto it = m_userCustomScriptData.begin(); it != m_userCustomScriptData.end(); ++it)
	{
		auto tScriptData = it.value();
		auto name = tScriptData[0].toString();
		auto type = tScriptData[2].toString();
		auto val = g_pGameFun->GetScriptUiSetData(name); //tScriptData.second;
		iniFile.setValue(QString("customName%1").arg(index), name);
		iniFile.setValue(QString("customVal%1").arg(index), val);
		iniFile.setValue(QString("customValType%1").arg(index), type);
		index++;
	}
	iniFile.endGroup();
}

void GameScriptSetWgt::doCustomScript_editingFinished()
{
	QLineEdit *pObj = (QLineEdit *)sender();
	if (!pObj)
		return;
	QString sName = m_nameForEdit.key(pObj);
	if (sName.isEmpty())
		return;
	g_pGameFun->SetScriptUiSetData(sName, GetUserDefineData(sName, pObj->text()));
}

void GameScriptSetWgt::doCustomScriptItemChanged(QListWidgetItem *pItem)
{
	//if (!pItem)
	//	return;
	//int ntype = pItem->data(Qt::UserRole).toInt();
	//auto pCfg = g_pGameFun->GetStopScriptCfg(ntype);
	//if (pCfg)
	//{
	//	pCfg->bChecked = (pItem->checkState() == Qt::Checked ? true : false);
	//	pCfg->bPercentage = false; //是否百分比判断
	//}
}

//刷新下拉队伍信息
void GameScriptSetWgt::on_pushButton_fetchTeamData_clicked()
{
	QString sCurName = ui.comboBox_leaderName->currentText();
	ui.comboBox_leaderName->clear();
	auto allTeamPlayers = g_pGameFun->GetTeamPlayers();
	for (int i = 0; i < allTeamPlayers.size(); ++i)
	{
		auto teamPlayer = allTeamPlayers[i];
		ui.comboBox_leaderName->addItem(teamPlayer->name);
	}
	int index = ui.comboBox_leaderName->findText(sCurName);
	if (index < 0)
	{
		ui.comboBox_leaderName->addItem(sCurName);
		index = ui.comboBox_leaderName->findText(sCurName);
	}
	ui.comboBox_leaderName->setCurrentIndex(index);
}

void GameScriptSetWgt::on_pushButton_fetchTeammateName_clicked()
{
	QStringList sTeammateNameList;
	auto allTeamPlayers = g_pGameFun->GetTeamPlayers();
	for (int i = 0; i < allTeamPlayers.size(); ++i)
	{
		auto teamPlayer = allTeamPlayers[i];
		if (teamPlayer->is_me == 0)
		{
			sTeammateNameList.append(teamPlayer->name);
		}
	}
	ui.lineEdit_teammateName->setText(sTeammateNameList.join("|"));
}

void GameScriptSetWgt::on_comboBox_leaderName_currentTextChanged(const QString &text)
{
	g_pGameFun->SetScriptUiSetData("队长名称", text);
}

void GameScriptSetWgt::on_lineEdit_teammateName_editingFinished()
{
	QString sText = ui.lineEdit_teammateName->text();

	g_pGameFun->SetScriptUiSetData("队员列表", sText);
}

void GameScriptSetWgt::on_lineEdit_playerHpUI_editingFinished()
{
	QString sText = ui.lineEdit_playerHpUI->text();
	g_pGameFun->SetScriptUiSetData("人补血", QVariant(sText.toInt()));
}

void GameScriptSetWgt::on_lineEdit_playerMpUI_editingFinished()
{
	QString sText = ui.lineEdit_playerMpUI->text();
	g_pGameFun->SetScriptUiSetData("人补魔", QVariant(sText.toInt()));
}

void GameScriptSetWgt::on_lineEdit_petMpUI_editingFinished()
{
	QString sText = ui.lineEdit_petMpUI->text();
	g_pGameFun->SetScriptUiSetData("宠补魔", QVariant(sText.toInt()));
}

void GameScriptSetWgt::on_lineEdit_petHpUI_editingFinished()
{
	QString sText = ui.lineEdit_petHpUI->text();
	g_pGameFun->SetScriptUiSetData("宠补血", QVariant(sText.toInt()));
}

void GameScriptSetWgt::on_lineEdit_troopCountUI_editingFinished()
{
	QString sText = ui.lineEdit_troopCountUI->text();
	g_pGameFun->SetScriptUiSetData("队伍人数", QVariant(sText.toInt()));
}

void GameScriptSetWgt::on_checkBox_hpPercentageUI_stateChanged(int state)
{
	//百分比依赖于当前人物数据，只能是脚本获取时候进行百分比判断，或者连接成功后判断，暂时不加
	//if (state == Qt::Checked)
	//{
	//	int nVal = ui.lineEdit_playerHpUI->text().toInt();
	//
	//	g_pGameFun->SetScriptUiSetData(TScriptUI_PlayerHp, sText);
	//}
}

void GameScriptSetWgt::on_checkBox_mpPercentageUI_stateChanged(int state)
{
}

void GameScriptSetWgt::on_checkBox_petMpPercentageUI_stateChanged(int state)
{
}

void GameScriptSetWgt::on_checkBox_petHpPercentageUI_stateChanged(int state)
{
}

QVariant GameScriptSetWgt::GetUserDefineData(const QString &name, QString sVal)
{
	if (!m_userCustomScriptData.contains(name))
		return QVariant(sVal);

	auto userDefine = m_userCustomScriptData.value(name);
	if (userDefine[2] == "int")
		return QVariant(sVal.toInt());
	else
		return QVariant(sVal);
}
