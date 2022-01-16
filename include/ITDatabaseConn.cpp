#include "ITDatabaseConn.h"
#include "time.h"
#include <QDebug>
//#include <QSettings>
//#include <QMessageBox>


ITRecordSet::ITRecordSet()
{
	m_sqlquery = NULL;
}
ITRecordSet::~ITRecordSet()
{

}

QString ITRecordSet::getStrValue(const QString& strName)
{
	if (m_sqlquery != NULL)
	{
		int nameCol = m_sqlquery->record().indexOf(strName);
		if (nameCol != -1)
		{
			return m_sqlquery->value(nameCol).toString();
		}
	}
	return "";
}

int ITRecordSet::getIntValue(const QString& strName)
{
	if (m_sqlquery != NULL)
	{
		int nameCol = m_sqlquery->record().indexOf(strName);
		if (nameCol != -1)
		{
			return m_sqlquery->value(nameCol).toInt();
		}
	}
	return 0;
}

int ITRecordSet::getBitValue(const QString& strName)
{
	if (m_sqlquery != NULL)
	{
		int ret = 0;
		int nameCol = m_sqlquery->record().indexOf(strName);
		if (nameCol != -1)
		{
			QVariant var = m_sqlquery->value(nameCol);
			QByteArray bytearray = var.toByteArray();
			char* szChar = bytearray.data();
			if (szChar)
			{
				ret = (int)((*szChar) & 0x0f);
				ret = ret != 0 ? 1 : 0;
			}
			return ret;
		}
	}
	return 0;
}

float ITRecordSet::getFloatValue(const QString& strName)
{
	if (m_sqlquery != NULL)
	{
		int nameCol = m_sqlquery->record().indexOf(strName);
		if (nameCol != -1)
		{
			return m_sqlquery->value(nameCol).toFloat();
		}
	}
	return 0;
}
double ITRecordSet::getDoubleValue(const QString& strName)
{
	if (m_sqlquery != NULL)
	{
		int nameCol = m_sqlquery->record().indexOf(strName);
		if (nameCol != -1)
		{
			return m_sqlquery->value(nameCol).toDouble();
		}
	}
	return 0;
}

QByteArray ITRecordSet::getByteData(const QString& strName)
{
	if (m_sqlquery != NULL)
	{
		int nameCol = m_sqlquery->record().indexOf(strName);
		if (nameCol != -1)
		{
			return m_sqlquery->value(nameCol).toByteArray();
		}
	}
	return 0;
}
bool ITRecordSet::next()
{
	return m_sqlquery->next();
}
ITDataBaseConn::ITDataBaseConn(QString strdbtype)
{
	if (strdbtype.contains("SQLServer", Qt::CaseInsensitive) || strdbtype.contains("ODBC", Qt::CaseInsensitive))
	{
		m_db = QSqlDatabase::addDatabase("QODBC");
		m_nDBType = DB_TYPE_SQLSERVER;
	}
	else if (strdbtype.contains("Oracle", Qt::CaseInsensitive))
	{
		m_db = QSqlDatabase::addDatabase("QOCI");
		m_nDBType = DB_TYPE_ORACLE;
	}
	else if (strdbtype.contains("MYSQL", Qt::CaseInsensitive))
	{
		m_db = QSqlDatabase::addDatabase("QMYSQL");
		m_nDBType = DB_TYPE_MYSQL;
	}
	else if (strdbtype.contains("SQLITE", Qt::CaseInsensitive))
	{
		m_db = QSqlDatabase::addDatabase("QSQLITE");
		m_nDBType = DB_TYPE_SQLITE;
	}
	else if (strdbtype.contains("m", Qt::CaseInsensitive))
		m_db = QSqlDatabase::addDatabase("QMYSQL");
	else if (strdbtype.contains("O", Qt::CaseInsensitive))
		m_db = QSqlDatabase::addDatabase("QOCI");
	else if (strdbtype.contains("postgres", Qt::CaseInsensitive))
		m_db = QSqlDatabase::addDatabase("QPSQL", "postgres");
	else
		m_db = QSqlDatabase::addDatabase("QODBC");
	m_strDBType = strdbtype;
	m_pQuery = NULL;
}

ITDataBaseConn::~ITDataBaseConn()
{
	m_db.close();
}

bool ITDataBaseConn::openDataBase(const QString& strDBName, const QString& strhostname, const QString& strUser, const QString& strPwd, int nport)
{
	QString strDriver;
	if (m_strDBType.contains("SQLServer", Qt::CaseInsensitive) || m_strDBType.contains("ODBC", Qt::CaseInsensitive))
	{
		strDriver = QString("DRIVER={SQL SERVER};port=1433;SERVER=%1;DATABASE=%2").arg(strhostname).arg(strDBName);
		nport = 1433;
	}
	else if (m_strDBType.contains("Oracle", Qt::CaseInsensitive))
	{
		strDriver = strDBName;
		nport = 1521;
	}
	else if (m_strDBType.contains("MYSQL", Qt::CaseInsensitive))
	{
		strDriver = strDBName;
		nport = 3306;
	}
	else if (m_strDBType.contains("SQLITE", Qt::CaseInsensitive))
	{
		strDriver = strDBName;
	}
	else
	{
		strDriver = QString("DRIVER={SQL SERVER};port=1433;SERVER=%1;DATABASE=%2").arg(strhostname).arg(strDBName);
		nport = 1433;
	}
	m_strDBPwd = strPwd;
	m_strDBUser = strUser;
	m_strDBIpAddr = strhostname;

	m_db.setDatabaseName(strDriver);
	m_db.setHostName(strhostname);
	m_db.setUserName(strUser);
	m_db.setPassword(strPwd);
	if (m_strDBType == "ORACLE")
		m_db.setPort(nport);

	if (m_db.isOpen())
	{
		if (m_pQuery)
		{
			m_pQuery.clear();
			/*delete m_pQuery;
			m_pQuery = nullptr;*/
		}
		if (m_pQuery == nullptr)
			m_pQuery = QSqlQueryPtr(new QSqlQuery(m_db));
		m_strLastError.clear();
		return true;
	}
	else
	{
		if (m_db.open() == false)	//open后，立即调用isOpen是判断不出来的，必须等到会
		{
			m_strLastError.clear();
			m_strLastError = m_db.lastError().text();
			qDebug() << m_strLastError;
			return false;
		}
		else
		{
			if (m_pQuery)
			{
				m_pQuery.clear();
				/*delete m_pQuery;
				m_pQuery = nullptr;*/
			}
			if (m_pQuery == nullptr)
				m_pQuery = QSqlQueryPtr(new QSqlQuery(m_db));
			return true;
		}
	}
	return false;
}

ITRecordSetPtr ITDataBaseConn::execQuerySql(const QString& strSql)
{
	QSqlQueryPtr pSqlQuery(new QSqlQuery(m_db));
	if (pSqlQuery)
	{
		pSqlQuery->clear();
		if (pSqlQuery->exec(strSql))
		{
			ITRecordSetPtr recordset(new ITRecordSet);
			recordset->m_sqlquery = pSqlQuery;
			return recordset;
		}
		else
		{
			m_strLastError.clear();
			m_strLastError = pSqlQuery->lastError().text();
			qDebug() << m_strLastError;
		}
	}
	return NULL;
}
bool ITDataBaseConn::execSql(const QString& strSql)
{
	bool bSuc = false;
	if (!m_pQuery)
		return false;
	m_pQuery->clear();
	if (m_pQuery->exec(strSql))
	{
		bSuc = true;
	}
	else
	{
		m_strLastError.clear();
		m_strLastError = m_pQuery->lastError().text();
		qDebug() << m_strLastError;
	}
	return bSuc;
}

QSqlQueryPtr ITDataBaseConn::getSqlQuery()
{
	if (m_pQuery == NULL)
		return NULL;
	m_pQuery->clear();
	return m_pQuery;
}