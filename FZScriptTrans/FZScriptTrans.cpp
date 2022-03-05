#include "FZScriptTrans.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTextCodec>
#include <QScrollBar>
FZScriptTrans::FZScriptTrans(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	//ui.lineEdit_fz->setText("F:\\MyPro\\CG\\ml\\5zhuan\\★百人.script");
	QString sScriptPath = QCoreApplication::applicationDirPath() + "//脚本";
	ui.lineEdit_fz->setText(sScriptPath);
	m_szFzPath = ui.lineEdit_fz->text();
	m_szSaveDir = sScriptPath;
	ui.lineEdit_ml->setText(m_szSaveDir);
	//QString sTest = "goto(\"法兰到西门\")\ngoto(\"法兰到南门\")";
	//sTest = sTest.replace(QRegExp("goto\\(([^<\\)]*)\\)+"), "goto \\1 ");
	//ui.textEdit->setText(sTest);
	auto scroBar = ui.textEdit->verticalScrollBar();
	auto tgtBar = ui.textEdit_2->verticalScrollBar();

	connect(scroBar, SIGNAL(valueChanged(int)), this, SLOT(on_textEdit_cursorPositionChanged()));
	connect(tgtBar, SIGNAL(valueChanged(int)), this, SLOT(on_textEdit_2_cursorPositionChanged()));



}
void FZScriptTrans::on_pushButton_clicked()
{
	m_szFzPath = ui.lineEdit_fz->text();
	m_szFzPath = QFileDialog::getOpenFileName(NULL, "打开文件", m_szFzPath, "*.script");//第一个是基于那个窗口，第二个参数是描述信息，第三个打开路径，第四个过滤掉其他格式
	ui.lineEdit_fz->setText(m_szFzPath);
}

void FZScriptTrans::on_pushButton_2_clicked()
{
	m_szSaveDir = ui.lineEdit_ml->text();
	m_szSaveDir = QFileDialog::getExistingDirectory(NULL, "保存路径", m_szSaveDir);//第一个是基于那个窗口，第二个参数是描述信息，第三个打开路径，第四个过滤掉其他格式
	ui.lineEdit_ml->setText(m_szSaveDir);

}
void FZScriptTrans::on_pushButton_openFz_clicked()
{
	QFile xmlFile(m_szFzPath);
	if (!xmlFile.open(QIODevice::ReadOnly))
	{

		QString title = QString("打开a%1文件").arg(m_szFzPath);
		QString text = QString("打开a%1文件失败,可能不存在该文件!").arg(m_szFzPath);
		QMessageBox::warning(NULL, title, text);
	}
	QTextStream readstream(&xmlFile);    //构建文本流
	QString szCodec = "GB2312";
	if (ui.radioButton_utf8->isChecked())
	{
		szCodec = "UTF-8";
	}
	QTextCodec* codec = QTextCodec::codecForName(szCodec.toStdString().c_str()); //设置编码
	readstream.setCodec(codec);
	auto fileData = readstream.readAll();
	ui.textEdit->setText(fileData);
	xmlFile.close();
	m_operationData = fileData;

}

void FZScriptTrans::on_pushButton_saveML_clicked()
{
	QFileInfo fileinfo(m_szFzPath);
	QString fileName = fileinfo.baseName();
	QString savePath = m_szSaveDir + "//" + fileName + ".lua";

	QFile xmlFile(savePath);
	if (!xmlFile.open(QIODevice::ReadWrite))
	{

		QString title = QString("打开a%1文件").arg(savePath);
		QString text = QString("打开a%1文件失败,可能不存在该文件!").arg(savePath);
		QMessageBox::warning(NULL, title, text);
	}
	QTextStream readstream(&xmlFile);    //构建文本流
	QTextCodec* codec = QTextCodec::codecForName("UTF-8"); //设置编码
	readstream.setCodec(codec);
	xmlFile.resize(0);
	auto fileData = ui.textEdit_2->toPlainText();
	readstream << fileData;
	xmlFile.close();
}

void FZScriptTrans::on_pushButton_trans_clicked()
{
	m_operationData = ui.textEdit->toPlainText();

	if (m_operationData.isEmpty())
		return;
	m_operationData.replace("//", "--");
	//m_operationData.replace("\"=\"", "\"==\"");	
	//m_operationData.replace("\"!=\"", "\"~=\"");

	m_operationData.replace("=", "==");
	m_operationData.replace("!=", "~=");

	m_operationData.replace("\\\\z3", "");
	m_operationData.replace("\\\\z6", "");
	m_operationData.replace("\\\\z12", "");
	m_operationData.replace("\\S", " ");

	m_operationData.replace(QRegExp("set\\(timer1[^,]*,.?\\d+\\)"), "");
	m_operationData.replace(QRegExp("set\\(auto_protect[^,]*,.?AutoMoveType[^,]*,.?\\d+\\)"), "");
	m_operationData.replace(QRegExp("set\\(auto_protect[^,]*,.?AutoMoveType[^,]*,[^\)]*\\)"), "");
	m_operationData.replace(QRegExp("set\\(auto_protect[^,]*,.?AutoMove[^,]*,.?\\d+\\)"), "");
	m_operationData.replace(QRegExp("set\\(auto_protect[^,]*,.?SpeedSpc[^,]*,.?\\d+\\)"), "");
	m_operationData.replace(QRegExp("set\\(auto_protect[^,]*,.?RandX[^,]*,.?\\d+\\)"), "");
	m_operationData.replace(QRegExp("set\\(auto_protect[^,]*,.?RandY[^,]*,.?\\d+\\)"), "");
	m_operationData.replace(QRegExp("ShowMessage\\(([^\)]*)\\)"), "");


	m_operationData.replace(QRegExp("InputVal\\(\"([^,]*)\",.?([^,]*),.?([^,]*)\\)"), "\\1=用户输入框(\\2,\\3)");
	m_operationData.replace(QRegExp("InputVal\\(([^,]*),.?([^,]*),.?([^,]*)\\)"), "\\1=用户输入框(\\2,\\3)");

	m_operationData.replace(QRegExp("脚本参数\\(.?编辑框.?,.?\"([^,]*)\",.?([^,]*)\\)"), "\\1=用户输入框(\"\\1\",\\2)");


	m_operationData.replace("set(timer", "设置(\"timer\"");
	m_operationData.replace("set(auto_action, 自动战斗", "设置(\"自动战斗\"");
	m_operationData.replace("set(auto_action, 高速战斗", "设置(\"高速战斗\"");
	m_operationData.replace("set(auto_action, 高速延迟", "设置(\"高速延时\"");
	m_operationData.replace("set(auto_action, Auto", "设置(\"自动战斗\"");
	m_operationData.replace("set(auto_action, ActionCount", "设置(\"高速战斗\"");
	m_operationData.replace("set(auto_action, ActionSpeed", "设置(\"高速延时\"");


	m_operationData.replace("nowhile(renew)", "");
	m_operationData.replace("nowhile(sale)", "");
	m_operationData.replace("begin_auto_action", "开始遇敌()");
	m_operationData.replace("end_auto_action", "停止遇敌()");

	m_operationData.replace("renew(1001)", "回复(0)");
	m_operationData.replace("renew(1002)", "回复(1)");
	m_operationData.replace("renew(1003)", "回复(2)");
	m_operationData.replace("renew(1004)", "回复(3)");
	m_operationData.replace("renew(1005)", "回复(4)");
	m_operationData.replace("renew(1006)", "回复(5)");
	m_operationData.replace("renew(1007)", "回复(6)");
	m_operationData.replace("renew(1008)", "回复(7)");
	m_operationData.replace("renew(east)", "回复(\"east\")");
	m_operationData.replace("renew(north)", "回复(\"north\")");
	m_operationData.replace("renew(south)", "回复(\"south\")");
	m_operationData.replace("renew(west)", "回复(\"west\")");
	m_operationData.replace("renew", "回复");
	m_operationData.replace("sale(1001", "卖(0");
	m_operationData.replace("sale(1002", "卖(1");
	m_operationData.replace("sale(1003", "卖(2");
	m_operationData.replace("sale(1004", "卖(3");
	m_operationData.replace("sale(1005", "卖(4");
	m_operationData.replace("sale(1006", "卖(5");
	m_operationData.replace("sale(1007", "卖(6");
	m_operationData.replace("sale(1008", "卖(7");
	m_operationData.replace("Npc", "对话选择");
	m_operationData.replace("JianDongXi", "转向");

	m_operationData.replace("movego(1001", "移动一格(0");
	m_operationData.replace("movego(1002", "移动一格(1");
	m_operationData.replace("movego(1003", "移动一格(2");
	m_operationData.replace("movego(1004", "移动一格(3");
	m_operationData.replace("movego(1005", "移动一格(4");
	m_operationData.replace("movego(1006", "移动一格(5");
	m_operationData.replace("movego(1007", "移动一格(6");
	m_operationData.replace("movego(1008", "移动一格(7");
	

	m_operationData.replace(QRegExp("ShuangDian\\(\"([^,]*)\"\\)"), "使用物品(\"\\1\")");
	m_operationData.replace(QRegExp("ShuangDian\\(([^,]*)\\)"), "使用物品(\"\\1\")");
	m_operationData.replace(QRegExp("equip\\(\"([^,]*)\"\\)*"), "装备物品(\"\\1\"");//装备物品 标签替换
	m_operationData.replace(QRegExp("equip\\(([^,]*)\\)*"), "装备物品(\"\\1\"");//装备物品 标签替换

	m_operationData.replace(QRegExp("ren\\(\"([^)]*)\"\\)*"), "扔(\"\\1\")");	//带引号的
	m_operationData.replace(QRegExp("ren\\(([^)]*)\\)*"), "扔(\"\\1\")");//不带引号的	
	m_operationData.replace(QRegExp("moveres\\(\"([^)]*)\"\\)*"), "叠(\"\\1\")");	//带引号的
	m_operationData.replace(QRegExp("moveres\\(([^)]*)\\)*"), "叠(\"\\1\")");//不带引号的
	m_operationData.replace(QRegExp("Compound\\(\"([^,)]*)\"\,*"), "合成(\"\\1\")");//带引号的
	m_operationData.replace(QRegExp("Compound\\(([^,)]*)\,*"), "合成(\"\\1\")");//不带引号的
	m_operationData.replace(QRegExp("([^:\\s]*)\:"), "::\\1::");//go 标签替换

	m_operationData.replace(QRegExp("chat\\(\"([^,]*)\",([^,]*),v?([0-9]*),v?([0-9]*)\\)"), "喊话(\"\\1\",0\\2,0\\3,0\\4)");//喊话 标签替换
	m_operationData.replace(QRegExp("chat\\(([^,]*),([^,]*),v?([0-9]*),v?([0-9]*)\\)"), "喊话(\"\\1\",0\\2,0\\3,0\\4)");//喊话 标签替换

	m_operationData.replace(QRegExp("chat\\(\"([^,]*)\",([^,]*),([0-9]*),([0-9]*)\\)"), "喊话(\"\\1\",0\\2,0\\3,0\\4)");//喊话 标签替换
	m_operationData.replace(QRegExp("chat\\(([^,]*),([^,]*),([0-9]*),([0-9]*)\\)"), "喊话(\"\\1\",0\\2,0\\3,0\\4)");//喊话 标签替换
	m_operationData.replace(QRegExp("chat\\(.?([^,]*).?,.?\"([^,]*)\".?,.?\"([0-9]*)\".?,.?\"([0-9]*)\".?\\)"), "喊话(\"\\1\",0\\2,0\\3,0\\4)");//喊话 标签替换


	//物品 带引号
	m_operationData.replace(QRegExp("if\\(.?\"物品\".?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,([^\)]*)\\)"), "if(取物品数量(\"\\1\") \\2 \\3)then goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?\"物品\".?,.?([^,]*).?,.?\"([^,]*)\".?,.?([^,]*).?,([^\)]*)\\)"), "if(取物品数量(\\1) \\2 \\3)then goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?\"物品\".?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,([^\)]*)\\)"), "if(取物品数量(\\1) \\2 \\3)then goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?\"物品\".?,.?([^,]*).?,.?([^,]*).?,.?([^,]*).?,([^\)]*)\\)"), "if(取物品数量(\\1) \\2 \\3)then goto \\4 end");
	//不带引号
	m_operationData.replace(QRegExp("if\\(.?物品.?,.?([^,]*).?,.?\"([^,]*)\".?,.?([^,]*).?,([^\)]*)\\)"), "if(取物品数量(\\1) \\2 \\3)then goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?物品.?,.?([^,]*).?,.?([^,]*).?,.?([^,]*).?,([^\)]*)\\)"), "if(取物品数量(\\1) \\2 \\3)then goto \\4 end");

	//系统消息
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"SystemCue\".?,[^,]*,.?([^,]*),([^\)]*)\\)"), "if(string.find(最新系统消息(),\"\\1\")~=nil)then goto \\2	end");
	m_operationData.replace(QRegExp("if\\(.?\"状态\".?,.?SystemCue.?,[^,]*,.?([^,]*),([^\)]*)\\)"), "if(string.find(最新系统消息(),\"\\1\")~=nil)then goto \\2	end");

	m_operationData.replace(QRegExp("if\\(.?聊天.?,.?\"内容\".?,[^,]*,.?([^,]*),([^\)]*)\\)"), "if(string.find(聊天(50),\"\\1\")~=nil)then goto \\2	end");
	m_operationData.replace(QRegExp("if\\(.?聊天.?,.?内容.?,[^,]*,.?([^,]*),([^\)]*)\\)"), "if(string.find(聊天(50),\"\\1\")~=nil)then goto \\2	end");

	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"Character._loc\".?,.?\"([^,]*)\".?,.?([^,]*),([^\)]*)\\)"), "if(取当前地图名() \\1 \\2)then goto \\3 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?Character._loc.?,.?\"([^,]*)\".?,.?([^,]*),([^\)]*)\\)"), "if(取当前地图名() \\1 \\2)then goto \\3 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?Character._loc.?,.?([^,]*).?,.?([^,]*),([^\)]*)\\)"), "if(取当前地图名() \\1 \"\\2\")then goto \\3 end");



	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"GameCommand.ActionStat\".?,.?([^,]*).?,.?([^,]*),([^\)]*)\\)"), "if(是否战斗中() \\1 \\2)then goto \\3 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"GameCommand.ActionStat\".?,.?\"([^,]*)\".?,.?([^,]*),([^\)]*)\\)"), "if(是否战斗中() \\1 \\2)then goto \\3 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"GameCommand.ActionStat\".?,.?\"([^,]*)\".?,.?\"([^,]*)\",([^\)]*)\\)"), "if(是否战斗中() \\1 \\2)then goto \\3 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?GameCommand.ActionStat.?,.?([^,]*).?,.?([^,]*),([^\)]*)\\)"), "if(是否战斗中() \\1 \\2)then goto \\3 end");


	/*m_operationData.replace(QRegExp("if\\(.?状态.?,.?GameMap.x.y.?,.?\"([^,]*)\".?,.?\"([^,]*),.?([^,]*)\",([^\)]*)\\)"), "if((x,y=取当前坐标()) and x\\1\\2 and y\\1\\3)then	goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"GameMap.x.y\".?,.?\"([^,]*)\".?,.?\"([^,]*),.?([^,]*)\",([^\)]*)\\)"), "if((x,y=取当前坐标()) and x\\1\\2 and y\\1\\3)then goto \\4 end");	*/
	//if("状态", GameMap.x.y, "=" , "130,187", yudi)
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?GameMap.x.y.?,.?\"([^,]*)\".?,.?\"([^,]*),.?([^,]*)\",([^\)]*)\\)"), "if(人物(\"坐标\")  \\1 \"\\2,\\3\")then	goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?GameMap.x.y.?,.?([^,]*).?,.?\"([^,]*),.?([^,]*)\",([^\)]*)\\)"), "if(人物(\"坐标\")  \\1 \"\\2,\\3\")then	goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?GameMap.x.y.?,.?([^,]*).?,.?([^,]*),.?([^,]*),([^\)]*)\\)"), "if(人物(\"坐标\")  \\1 \"\\2,\\3\")then	goto \\4 end");
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"GameMap.x.y\".?,.?\"([^,]*)\".?,.?\"([^,]*),.?([^,]*)\",([^\)]*)\\)"), "if(人物(\"坐标\")  \\1 \"\\2,\\3\")then	goto \\4 end");
	//if("状态", "GameMap.x.y", = , "130,187", yudi)
	m_operationData.replace(QRegExp("if\\(.?状态.?,.?\"GameMap.x.y\".?,.?([^,]*).?,.?\"([^,]*),.?([^,]*)\",([^\)]*)\\)"), "if(人物(\"坐标\")  \\1 \"\\2,\\3\")then	goto \\4 end");


	//m_operationData.replace(QRegExp("chat\\(\"([^,]*)\"\,)*"), "喊话(\"\\1\"");//喊话 标签替换
	//m_operationData.replace(QRegExp("chat\\(([^,]*)\\)*"), "喊话(\"\\1\"");//喊话 标签替换

	m_operationData.replace("nop", "");
	m_operationData.replace("ClearSysCue", "清除系统消息()");
	m_operationData.replace("WaitTime", "等待");
	m_operationData.replace("getout", "回城()");
	QString sReg = "nowhile\\(([^,]*)\\)*";
	QString sRep = "等待到指定地图(\"\\1\"";
	m_operationData.replace(QRegExp("nowhile\\(\"([^,]*)\"\\)*"), "等待到指定地图(\"\\1\"");
	m_operationData.replace(QRegExp("nowhile\\(([^,]*)\\)*"), "等待到指定地图(\"\\1\"");
	m_operationData.replace("nowhile", "等待到指定地图");
	m_operationData.replace("等待到指定地图(renew)", "");

	m_operationData.replace("set_wait(RECV_HEAD_crXf)", "");
	m_operationData.replace("wait(RECV_HEAD_crXf)", "等待服务器返回()");
	m_operationData.replace("turn_about", "转向");
	m_operationData.replace("moveto", "移动");
	m_operationData.replace(QRegExp("goto\\(\"([^<\\)]*)\"\\)+"), "goto \\1 ");
	m_operationData.replace(QRegExp("goto\\(([^<\\)]*)\\)+"), "goto \\1 ");
	m_operationData.replace(QRegExp("high\\(([^,]*),"), "移动(");
	m_operationData.replace(QRegExp("Work\\(.?101,([^\)]*)\\)"), "工作(\"鉴定\",\\1)");
	m_operationData.replace(QRegExp("Work\\(.?100,([^\)]*)\\)"), "");

	//有引号
	m_operationData.replace(QRegExp("if\\(.?人.?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,([^\)]*)\\)"), "if(人物(\"\\1\") \\2 \\3)then goto \\4 end");
	//没有引号
	m_operationData.replace(QRegExp("if\\(.?人.?,.?([^,]*).?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,([^\)]*)\\)"), "if(人物(\"\\1\") \\2 \\3)then goto \\4 end");

	//有引号
	m_operationData.replace(QRegExp("if\\(.?宠.?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,([^\)]*)\\)"), "if(宠物(\"\\1\") \\2 \\3)then goto \\4 end");
	//没有引号
	m_operationData.replace(QRegExp("if\\(.?宠.?,.?([^,]*).?,.?\"([^,]*)\".?,.?\"([^,]*)\".?,([^\)]*)\\)"), "if(宠物(\"\\1\") \\2 \\3)then goto \\4 end");
	ui.textEdit_2->setText(m_operationData);

}

void FZScriptTrans::on_pushButton_replaceRex_clicked()
{
	QString srcText = ui.textEdit_srcRex->toPlainText();
	QString regText = ui.lineEdit_rex->text();
	QString repText = ui.lineEdit_replaceText->text();
	srcText = srcText.replace(QRegExp(regText), repText);
	ui.textEdit_dstRex->setText(srcText);
}

void FZScriptTrans::on_textEdit_cursorPositionChanged()
{
	//QTextCursor cusor;
	//ui.textEdit->setTextCursor();
	auto scroBar = ui.textEdit->verticalScrollBar();
	auto curVal = scroBar->value();
	auto tgtBar = ui.textEdit_2->verticalScrollBar();
	tgtBar->setValue(curVal);
}
//void textEdit::gotoline(int line)
//{
//    QTextCursor tc = textCursor();
//    int position = document()->findBlockByNumber(line - 1).position();
//    tc.setPosition(position, QTextCursor::MoveAnchor);
//    setTextCursor(tc);
//}
void FZScriptTrans::on_textEdit_2_cursorPositionChanged()
{
	auto scroBar = ui.textEdit->verticalScrollBar();
	auto tgtBar = ui.textEdit_2->verticalScrollBar();
	auto curVal = tgtBar->value();
	scroBar->setValue(curVal);

}
