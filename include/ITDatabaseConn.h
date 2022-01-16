#ifndef ITDATABASECONN_H
#define ITDATABASECONN_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlError>
#include <QSharedPointer>

class ITRecordSet;
class ITDataBaseConn;
typedef QSharedPointer<ITRecordSet> ITRecordSetPtr;
typedef QSharedPointer<ITDataBaseConn> ITDataBaseConnPtr;
typedef QSharedPointer<QSqlQuery> QSqlQueryPtr;

enum DataBaseDefine
{
	DB_TYPE_NONE = 0,
	DB_TYPE_SQLSERVER = 1,
	DB_TYPE_MYSQL = 2,
	DB_TYPE_ORACLE = 3,
	DB_TYPE_SQLITE = 4,
};
class ITRecordSet
{
public:
	ITRecordSet();
	~ITRecordSet();

	QString getStrValue(const QString& strName);
	int getIntValue(const QString& strName);
	float getFloatValue(const QString& strName);
	double getDoubleValue(const QString& strName);
	QByteArray getByteData(const QString& strName);
	int getBitValue(const QString& strName);
	bool next();
public:
	QSqlQueryPtr m_sqlquery;
};

class ITDataBaseConn : public QObject
{
	Q_OBJECT

public:
	~ITDataBaseConn();
	ITDataBaseConn(QString strdbtype);

	bool isValid() { return m_db.isValid(); }
	bool openDataBase(const QString& strDBName,						//´ò¿ªÊý¾Ý¿â
		const QString& hostname = "",
		const QString& strUser = "",
		const QString& strPwd = "",
		int nport = 0);

	bool execSql(const QString& strSql);							//Ö´ÐÐSQLÓï¾ä

	ITRecordSetPtr execQuerySql(const QString& strSql);
	QSqlDatabase& getDataBase() { return m_db; }
	QSqlQueryPtr getSqlQuery();
	int getCurrentDBType() { return m_nDBType; }
	QString getCurrentDBTypeText() { return m_strDBType; }
	QString getCurrentDBUser() { return m_strDBUser; }
	QString getCurrentDBPwd() { return m_strDBPwd; }
	QString getCurrentDBIpAddr() { return m_strDBIpAddr; }
	QString getLastError() { return m_strLastError; }
private:
	QSqlDatabase	m_db;
	QString			m_strDBType;	//数据库类型
	QString			m_strDBIpAddr;	//数据库地址
	QString			m_strDBUser;	//数据库用户
	QString			m_strDBPwd;		//数据库密码
	QSqlQueryPtr	m_pQuery;
	QString         m_strLastError; //error info
	int				m_nDBType;
};
#endif // ITDATABASECONN_H
