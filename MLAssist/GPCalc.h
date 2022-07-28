#pragma once

#include "GlobalDefine.h"
#include <QStringList>
#include <QMap>
#include <QSharedPointer>
class GPCalc : public QObject
{
	Q_OBJECT

public:
	static GPCalc *GetInstance();

	double CalcBaseBP(int petNumber,int level=1);
	double CalcBaseBP(const QString& petName,int level=1);
	QSharedPointer<GamePetSolution> CreateSol(double solution[5], int grIndex, int level, int base);
	double CalcStat(int type, double dV[5]);
	double CalcSpi(double spiVals[5]);
	double CalcRev(double revVals[5]);
	void Output(QSharedPointer<GamePetCalcData> pet);
	QSharedPointer<GamePetCalcData> CreatePetData(QStringList inputData);
	QSharedPointer<GamePetCalcData> ParseLine(QStringList inputData);
	QString ReadIniValString(QString strSection, QString strKeyword, QString strDefault, QString strFileName);
	void setCaclPetData(QMap<QString, QSharedPointer<CGPetPictorialBook> > petData) { m_cgPetDef = petData; }

public slots:

private:
	GPCalc();
	QMap<QString, QSharedPointer<CGPetPictorialBook> > m_cgPetDef;

	QStringList args;
	double baseBP = 0;
	QMap<QString, double> m_specialPetBase;
};
#define g_pGamePetCalc GPCalc::GetInstance()