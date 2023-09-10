#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

typedef std::vector<std::string> stringList;
struct GamePetSolution
{
	int gr;
	double cl[5];
	double BP[5];
	double stat[5];
	double loss; 
	double cfLoss[5];
	double maxLoss[5];
	double spi; //精神 = spi(this.cl) * solBase + 100
	double rev; //回复 = rev(this.cl) * solBase + 100
};
class GamePetCalcData
{
public:
	int number = 0;							   //图鉴编号
	std::string petName;					   //宠物名字
	int petRaceType = -1;					   //种族编号
	int level = 1;							   //宠物默认等级
	double ability[5] = { 0, 0, 0, 0, 0 };	   //能力值 当前宠物 血魔攻防敏 减去20初始值的数
	double curShowBp[5] = { 0, 0, 0, 0, 0 };   //bp 当前宠物显示的bp
	double calcBp[5] = { 0, 0, 0, 0, 0 };	   //计算的bp
	bool bInputBp = false;					   //是否有输入bp
	double bpGrowing[5] = { 0, 0, 0, 0, 0 };   //bp成长档
	double bestBPGrade[5] = { 0, 0, 0, 0, 0 }; //bp满档成长档
	double bpMax[5] = { 0, 0, 0, 0, 0 };	   //bp当前宠最大成长档
	double bpMin[5] = { 0, 0, 0, 0, 0 };	   //bp当前宠最小成长档
	int totalGrade = 0;						   //满档成长综合档次
	double cfLoss[5] = { 0, 0, 0, 0, 0 };
	double maxLoss[5] = { 0, 0, 0, 0, 0 };
	double baseBP;								  //能力倍率
	double spirit = 0;							  //精神
	double recovery = 0;						  //恢复
	double lossMin = -1;						  //最少掉几档
	double lossMax = -1;						  //最多掉几档
	double cfLossSummary[5] = { 0, 0, 0, 0, 0 };  //每一项最少掉档可能
	double maxLossSummary[5] = { 0, 0, 0, 0, 0 }; //每一项最大掉档可能
	std::vector<std::shared_ptr<GamePetSolution>> sols;
};
struct ParsePetCalcData
{
	double lossMin = -1;				//最少掉几档
	double lossMax = -1;				//最多掉几档
};
class CGPetPictorialBook
{
public:
	int number;			   //编号
	std::string name;		   //宠物名称
	std::string raceTypeName;  //种族名称
	double bestBPGrade[5]; //满档成长档次
	double baseBP;		   //能力倍率
	int totalGrade;		   //综合档次

	int can_catch;		//0x0001 0=不可捕捉，1=可以捕捉
	int card_type;		//0x0002 0=无，1=银卡，2=金卡
	int raceType;		//0x0003 种族
	int index;			//0x0004
	int image_id;		//0x0008
	int rate_endurance; //0x000C 0=半颗星，3=2颗星，5=3颗星，9=5颗星
	int rate_strength;	//0x0010 力量
	int rate_defense;	//0x0014 防御
	int rate_agility;	//0x0018 敏捷
	int rate_magical;	//0x001C 魔法
	int element_earth;	//0x0020 地
	int element_water;	//0x0024 水
	int element_fire;	//0x0028 火
	int element_wind;	//0x002C 风
	int skill_slots;	//0x0030 技能栏
};
class GPCalc 
{	
public:
	static GPCalc *GetInstance();

	double CalcBaseBP(int petNumber,int level=1);
	double CalcBaseBP(const std::string& petName,int level=1);
	std::shared_ptr<GamePetSolution> CreateSol(double solution[5], int grIndex, int level, int base);
	double CalcStat(int type, double dV[5]);
	double CalcSpi(double spiVals[5]);
	double CalcRev(double revVals[5]);
	std::shared_ptr<GamePetCalcData> CreatePetData(stringList inputData);
	std::shared_ptr<GamePetCalcData> ParseLine(stringList inputData);
	bool ParsePetGrade(stringList inputData, ParsePetCalcData& retData);
	std::string ReadIniValString(std::string strSection, std::string strKeyword, std::string strDefault, std::string strFileName);
	
	void initCaclPetData(stringList petData);// { m_cgPetDef = petData; }
	std::vector<std::string> string_split(std::string str, std::string pattern);

private:
	GPCalc();
	std::map<std::string, std::shared_ptr<CGPetPictorialBook> > m_cgPetDef;

	stringList args;
	double baseBP = 0;
	std::map<std::string, double> m_specialPetBase;
};
#define g_pGamePetCalc GPCalc::GetInstance()