#include "LuaCodeEditorDlg.h"
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QTextCodec>

LuaCodeEditorDlg::LuaCodeEditorDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}

LuaCodeEditorDlg::~LuaCodeEditorDlg()
{
}

void LuaCodeEditorDlg::SetOpenLuaScriptPath(const QString &sPath)
{
	openScriptFile(sPath);	
}

void LuaCodeEditorDlg::on_toolButton_open_clicked()
{	
	QString sPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开"), "./", "*.lua"); //*.script;
	if (sPath.isEmpty())
		return;
	openScriptFile(sPath);
}

void LuaCodeEditorDlg::on_toolButton_save_clicked()
{
	QFile tmpFile(m_sOpenPath);
	if (!tmpFile.open(QIODevice::ReadWrite))
	{
		QString text = QString("打开a%1文件失败,可能不存在该文件!").arg(m_sOpenPath);
		qDebug() << text;
		return;
	}
	QTextStream readstream(&tmpFile);					   //构建文本流
	QTextCodec *codec = QTextCodec::codecForName("UTF-8"); //设置编码
	readstream.setCodec(codec);
	tmpFile.resize(0);
	auto fileData = ui.plainTextEdit->toPlainText();
	readstream << fileData;
	tmpFile.close();
}

void LuaCodeEditorDlg::on_toolButton_saveAs_clicked()
{
	QString savePath = QFileDialog::getSaveFileName(NULL, "另存为", "./", ".lua");
	if (savePath.isEmpty())
		return;
	if (!savePath.endsWith(".lua"))
	{
		savePath += ".lua";
	}
	QFile tmpFile(savePath);
	if (!tmpFile.open(QIODevice::ReadWrite))
	{
		QString text = QString("打开a%1文件失败,可能不存在该文件!").arg(m_sOpenPath);
		qDebug() << text;
		return;
	}
	QTextStream readstream(&tmpFile);					   //构建文本流
	QTextCodec *codec = QTextCodec::codecForName("UTF-8"); //设置编码
	readstream.setCodec(codec);
	tmpFile.resize(0);
	auto fileData = ui.plainTextEdit->toPlainText();
	readstream << fileData;
	tmpFile.close();
}

bool LuaCodeEditorDlg::openScriptFile(const QString &sPath)
{
	QFile tmpFile(sPath);
	if (!tmpFile.open(QIODevice::ReadOnly))
	{
		QString text = QString("打开a%1文件失败,可能不存在该文件!").arg(sPath);
		qDebug() << text;
		return false;
	}
	m_sOpenPath = sPath;
	QTextStream readstream(&tmpFile); //构建文本流
	QString szCodec = "UTF-8";
	QTextCodec *codec = QTextCodec::codecForName(szCodec.toStdString().c_str()); //设置编码
	readstream.setCodec(codec);
	auto fileData = readstream.readAll();
	tmpFile.close();
	m_operationData = fileData;
	ui.plainTextEdit->setPlainText(fileData);
	return true;
}
