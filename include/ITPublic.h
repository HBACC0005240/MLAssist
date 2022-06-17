#pragma once
#include <windows.h>
#include <QTextCodec>
#include <QSettings>
#include <QIODevice>
#include <QTextStream>
//ANS To UNICODE
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

static bool IniWriteFunc(QIODevice& device, const QSettings::SettingsMap& settingsMap)
{
	QString lastSection;
	const char* const eol = "\r\n";
	bool writeError = false;
	QMapIterator<QString, QVariant> it(settingsMap);
	while (it.hasNext() && !writeError)
	{
		it.next();
		QString key = it.key();
		QString section;
		//qDebug() << "key: " << key;
		int idx = key.lastIndexOf(QChar('/'));
		if (idx == -1)
		{
			section = QString("[General]");
		}
		else
		{
			section = key.left(idx);
			key = key.mid(idx + 1);
			if (section.compare(QString("General"), Qt::CaseInsensitive) == 0)
			{
				section = QString("[%General]");
			}
			else
			{
				section.prepend(QChar('['));
				section.append(QChar(']'));
			}
		}
		if (section.compare(lastSection, Qt::CaseInsensitive))
		{
			if (!lastSection.isEmpty())
			{
				device.write(eol);
			}
			lastSection = section;
			if (device.write(section.toUtf8() + eol) == -1)
			{
				writeError = true;
			}
		}
		QByteArray block = key.toUtf8();
		block += " = ";
		if (it.value().type() == QVariant::StringList)
		{
			foreach(QString s, it.value().toStringList())
			{
				block += s;
				block += ", ";
			}
			if (block.endsWith(", "))
			{
				block.chop(2);
			}
		}
		else if (it.value().type() == QVariant::List)
		{
			foreach(QVariant v, it.value().toList())
			{
				block += v.toString();
				block += ", ";
			}
			if (block.endsWith(", "))
			{
				block.chop(2);
			}
		}
		else
		{
			block += it.value().toString();
		}
		block += eol;
		if (device.write(block) == -1)
		{
			writeError = true;
		}
	}
	return writeError;
}
static bool IniReadFunc(QIODevice& device, QSettings::SettingsMap& settingsMap)
{
	QString currentSection;
	QTextStream stream(&device);
	stream.setCodec("GB2312");
	QString data;
	bool ok = true;
	while (!stream.atEnd())
	{
		data = stream.readLine();
		if (data.trimmed().isEmpty())
		{
			continue;
		}
		if (data[0] == QChar('['))
		{
			QString iniSection;
			int inx = data.indexOf(QChar(']'));
			if (inx == -1)
			{
				ok = false;
				iniSection = data.mid(1);
			}
			else
			{
				iniSection = data.mid(1, inx - 1);
			}
			iniSection = iniSection.trimmed();
			if (iniSection.compare(QString("general"), Qt::CaseInsensitive) == 0)
			{
				currentSection.clear();
			}
			else
			{
				if (iniSection.compare(QString("%general"), Qt::CaseInsensitive) == 0)
				{
					currentSection = QString("general");
				}
				else
				{
					currentSection = iniSection;
				}
				currentSection += QChar('/');
			}
		}
		else
		{
			bool inQuotes = false;
			int equalsPos = -1;
			QList<int> commaPos;
			int i = 0;
			while (i < data.size())
			{
				QChar ch = data.at(i);
				if (ch == QChar('='))
				{
					if (!inQuotes && equalsPos == -1)
					{
						equalsPos = i;
					}
				}
				else if (ch == QChar('"'))
				{
					inQuotes = !inQuotes;
				}
				else if (ch == QChar(','))
				{
					if (!inQuotes && equalsPos != -1)
					{
						commaPos.append(i);
					}
				}
				/*	else if (ch == QChar(';') || ch == QChar('#')) {
						if (!inQuotes) {
							data.resize(i);
							break;
						}
					}*/
				else if (ch == QChar('\\'))
				{
					if (++i < data.size())
					{
					}
					else
					{
						ok = false;
						break;
					}
				}
				i++;
			}
			if (equalsPos == -1)
			{
				break;
			}
			else
			{
				QString key = data.mid(0, equalsPos).trimmed();
				if (key.isEmpty())
				{
					break;
				}
				else
				{
					key = currentSection + key;
				}
				if (commaPos.isEmpty())
				{ //value
					QString v = data.mid(equalsPos + 1).trimmed();
					if (v.startsWith("\"") && v.endsWith("\"") && v.length() > 1)
					{
						v = v.mid(1, v.length() - 2);
					}
					settingsMap[key] = (v);
				}
				else
				{ //value list
					commaPos.prepend(equalsPos);
					commaPos.append(-1);
					QVariantList vals;
					for (int i = 1; i < commaPos.size(); ++i)
					{
						QString d = data.mid(commaPos.at(i - 1) + 1, commaPos.at(i) - commaPos.at(i - 1) - 1);
						QString v = d.trimmed();
						if (v.startsWith("\"") && v.endsWith("\"") && v.length() > 1)
						{
							v = v.mid(1, v.length() - 2);
						}
						vals.append(v);
					}
					settingsMap[key] = vals;
				}
			}
		}
	}
	return ok;
}