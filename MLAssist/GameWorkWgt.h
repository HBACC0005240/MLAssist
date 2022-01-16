#pragma once

#include "GlobalDefine.h"
#include "ui_GameWorkWgt.h"
#include <QSettings>
#include <QTimer>
#include <QWidget>

class GameWorkWgt : public QWidget
{
	Q_OBJECT

public:
	GameWorkWgt(QWidget *parent = Q_NULLPTR);
	~GameWorkWgt();

	enum ShowType
	{
		SHOW_Name = 1,
		SHOW_Count = 2,
		SHOW_NameCount = 3,
		SHOW_Type = 4,
		SHOW_Code = 5
	};
	void initSkillComboBox();
	void initSkillComboBox2();
	void GetRowColFromItemPos(int itempos, int &row, int &col);
	QString GetItemShowText(GameItemPtr pItem);
	static void StartWorkThread(GameWorkWgt *pThis);

protected:
	void ResetWorkUI();
	static bool SortItem(QSharedPointer<GameItem> p1, QSharedPointer<GameItem> p2);
public slots:
	void on_pushButton_useSkill_clicked();
	void Active();
	void doUpdateItemList(GameItemList items); //更新物品栏显示
	void on_radioButton_name_clicked();
	//void on_radioButton_count_clicked();
	void on_radioButton_code_clicked();
	void on_radioButton_nameCount_clicked();
	void on_radioButton_type_clicked();
	void on_customContextMenu(const QPoint &pos);
	void doDropItem();
	void doUseItem();
	void doAddDieList();
	void doAddRenList();
	void doAddRenCodeList();
	void doAddSaleList();
	void doTableCellClicked(int nRow, int nCol);
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void on_pushButton_compound_clicked(bool checked);
	void on_pushButton_Identify_clicked(bool checked);	   //高速鉴定
	void on_pushButton_hunt_clicked(bool checked);		   //高速狩猎
	void on_pushButton_digging_clicked(bool checked);	   //高速挖掘
	void on_pushButton_lumber_clicked(bool checked);	   //高速伐木
	void on_pushButton_RepairArmor_clicked(bool checked);  //全防修
	void on_pushButton_RepairWeapon_clicked(bool checked); //全武修
	void ResetWorkText();								   //重置所有按钮提示信息
	void StartWork();
	void on_lineEdit_compound_editingFinished();
	void on_lineEdit_digging_editingFinished();
	void on_lineEdit_hunt_editingFinished();
	void on_lineEdit_lumber_editingFinished();
	void doStartProgress();
	void doFiniProgress();
	void doUpdateProgress();
	void doUpdateCompoundList(GameCompoundList compound); //更新可以合成物品的列表
	void doShowMaterialInfo(QListWidgetItem *pItem);
	void doWorkThreadFini(const QString &sMsg);
	void on_pushButton_arrange_clicked(); //一键整理
	void on_pushButton_pullCompound_clicked();

private:
	Ui::GameWorkWgt ui;
	bool m_bStopWork = false;
	QStringList m_szPickSkillList;
	int m_nType = SHOW_Name;
	quint64 m_dLastTime;
	int m_nCurrentWorkType = TWork_None; //当前工作类型
	int m_nCurrentWorkSkillIndex = -1;	 //当前工作技能index
	int m_nCurrentWorkDelayTime = 6500;	 //当前工作延时
	QMap<int, QString> m_workTypeForText;
	QMap<int, int> m_workTypeForTime; //工作类型对应时间
	QTimer m_progressTimer;
	QString m_sCurrentCompoundName; //合成名称
};
