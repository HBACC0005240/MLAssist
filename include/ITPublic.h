#pragma once
#include <windows.h>
#include <QTextCodec>
//ANSIת����UNICODE
static QTextCodec* g_codecGBK = QTextCodec::codecForName("GBK");
static LPWSTR ANSITOUNICODE1(const char* pBuf)
{
	int lenA = lstrlenA(pBuf);
	int lenW = 0;
	LPWSTR lpszFile = nullptr;
	lenW = MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, 0, 0);
	if (lenW > 0)
	{
		lpszFile = SysAllocStringLen(0, lenW);
		MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, lpszFile, lenW);
	}
	return lpszFile;
}
static QString GBK2UTF8(const QString& str)
{
	QTextCodec* utf8 = QTextCodec::codecForName("UTF-8");
	return utf8->toUnicode(str.toUtf8());
}

static QString UTF82GBK(const QString& str)
{
	QTextCodec* gbk = QTextCodec::codecForName("GB18030");
	return gbk->toUnicode(str.toLocal8Bit());
}

static std::string GBK2UTF8(std::string& str)
{
	QString temp = QString::fromLocal8Bit(str.c_str());
	std::string ret = temp.toUtf8().data();
	return ret;
}

static std::string UTF82GBK(std::string& str)
{
	QString temp = QString::fromUtf8(str.c_str());
	std::string ret = temp.toLocal8Bit().data();
	return ret;
}
static QString ConvertFileName(const QString& sFileName)
{
	QString sNewFileName = sFileName;
	QHash<QString, QString> mapEscape;
	mapEscape.insert("\\", "%5C");
	mapEscape.insert("/", "%2F");
	mapEscape.insert(":", "%3A");
	mapEscape.insert("*", "%2A");
	mapEscape.insert("?", "%3F");
	mapEscape.insert("\"", "%22");
	mapEscape.insert("<", "%3C");
	mapEscape.insert(">", "%3E");
	mapEscape.insert("|", "%7C");
	//把不符合要求的以其他替换掉  %22123%2A45%7C67%3E8%3C9%3F10%7C11%3A12
	for (auto it = mapEscape.begin(); it != mapEscape.end(); ++it)
	{
		sNewFileName = sNewFileName.replace(it.key(), it.value());
	}
	////这个是还原 还原完  "123*45|67>8<9?10|11:12
	//for (auto it = mapEscape.begin(); it != mapEscape.end(); ++it)
	//{
	//	name = name.replace(it.value(), it.key());
	//}
	return sNewFileName;
}