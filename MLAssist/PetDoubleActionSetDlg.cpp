#include "PetDoubleActionSetDlg.h"
#include "BattleSetDlg.h"

PetDoubleActionSetDlg::PetDoubleActionSetDlg(QWidget *parent) :
		QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui.checkBox_attack1->setChecked(g_pAutoBattleCtrl->m_bPetDoubleActionDefault1);
	ui.checkBox_attack2->setChecked(g_pAutoBattleCtrl->m_bPetDoubleActionDefault2);
}

PetDoubleActionSetDlg::~PetDoubleActionSetDlg()
{
}

void PetDoubleActionSetDlg::on_checkBox_attack1_stateChanged(int state)
{
	g_pAutoBattleCtrl->m_bPetDoubleActionDefault1 = (state == Qt::Checked) ? true : false;
}

void PetDoubleActionSetDlg::on_checkBox_attack2_stateChanged(int state)
{
	g_pAutoBattleCtrl->m_bPetDoubleActionDefault2 = (state == Qt::Checked) ? true : false;
}

void PetDoubleActionSetDlg::on_pushButton_attack1_clicked()
{
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->m_petDoubleAction1;
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();
		//人物无所谓 这里只取宝宝的
		CBattleCondition *pCondition = new CBattleCondition_Ignore();
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->m_petDoubleAction1 = ptr;
	}
}

void PetDoubleActionSetDlg::on_pushButton_attack2_clicked()
{
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->m_petDoubleAction2;
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();
		//人物无所谓 这里只取宝宝的
		CBattleCondition *pCondition = new CBattleCondition_Ignore();
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->m_petDoubleAction2 = ptr;
	}
}

void PetDoubleActionSetDlg::on_pushButton_ok_clicked()
{
	accept();
}

void PetDoubleActionSetDlg::on_pushButton_cancel_clicked()
{
	reject();
}
