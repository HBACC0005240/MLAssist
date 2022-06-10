#pragma once

#include "GlobalDefine.h"
#include "ui_GameOftenFunctionWgt.h"
#include <QSettings>
#include <QWidget>

class GameOftenFunctionWgt : public QWidget
{
	Q_OBJECT

public:
	GameOftenFunctionWgt(QWidget *parent = Q_NULLPTR);
	~GameOftenFunctionWgt();

	void init();
	void initSearchItems(GameSearchList itemList);
	void initSaleItems(GameItemList itemList);
	static void SwitchMapThread(GameOftenFunctionWgt *pThis, int nDir, int nVal);

protected:
	void AppendDataToEdit(const QString &sText, const QString &sFilte);

public slots:
	void on_pushButton_addSearch_clicked();
	void on_pushButton_addSale_clicked();
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void doLoadJsConfig(QJsonObject &obj);
	void doSaveJsConfig(QJsonObject &obj);
	void doSearchItemChanged(QListWidgetItem *pItem);
	void doSaleItemChanged(QListWidgetItem *pItem);

	void on_pushButton_LeftUp_clicked();
	void on_pushButton_Left_clicked();
	void on_pushButton_LeftDown_clicked();
	void on_pushButton_Up_clicked();
	void on_pushButton_RightUp_clicked();
	void on_pushButton_Right_clicked();
	void on_pushButton_RightDown_clicked();
	void on_pushButton_Down_clicked();
	void on_pushButton_SwitchMap_clicked();
	void doAddSaleItem(GameItemPtr pItem);
	void doAddSeachItem(GameItemPtr pItem);
	void OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	bool judgeIsCrate(CGA_MapUnit_t u);
	bool judgeIsEntries(CGA_MapUnit_t u);
	void on_groupBox_search_clicked();

	void on_groupBox_autoTalk_toggled(bool checked);
	void on_radioButton_talkNo_clicked(bool checked);
	void on_radioButton_talkYes_clicked(bool checked);

private:
	Ui::GameOftenFunctionWgt ui;
	quint64 m_lastTime = 0;
	bool m_bEnableSearch = true;
	bool m_bIsSearchAll = false;
	QStringList m_pSearchNameList;
	bool m_bSearchCrate = false;
	bool m_bSearchEntries = false;
};
