#pragma execution_character_set("utf-8")
#include "GPCalc.h"
#include <math.h>
#include <algorithm>
#include <cmath>
#include <iostream>

GPCalc::GPCalc()
{
	m_specialPetBase.insert(std::make_pair("小蝙蝠", 0.3));
	m_specialPetBase.insert(std::make_pair("使魔", 0.3));
	m_specialPetBase.insert(std::make_pair("英普", 0.3));
	m_specialPetBase.insert(std::make_pair("大地牛头怪", 0.3));
	m_specialPetBase.insert(std::make_pair("烈风牛头怪", 0.3));
	m_specialPetBase.insert(std::make_pair("火焰牛头怪", 0.3));
	m_specialPetBase.insert(std::make_pair("寒冰牛头怪", 0.3));
	m_specialPetBase.insert(std::make_pair("大地牛头怪", 0.3));
	m_specialPetBase.insert(std::make_pair("小鸭子", 0.22));
	
}
//回复
double GPCalc::CalcRev(double revVals[5])
{
	return ((revVals[0] * 8 - revVals[1] - revVals[2] + revVals[3] * 2 - revVals[4] * 3) / 10);
}

//精神
double GPCalc::CalcSpi(double spiVals[5])
{
	return ((-spiVals[0] * 3 - spiVals[1] + spiVals[2] * 2 - spiVals[3] * 1 + spiVals[4] * 8) / 10);
}

//类型 五维 根据体力 魔法 敏捷等值 返回bp
double GPCalc::CalcStat(int type, double dV[5])
{
	double a = dV[0];
	double b = dV[1];
	double c = dV[2];
	double d = dV[3];
	double e = dV[4];
	switch (type)
	{
		case 0: return a * 8 + b * 2 + c * 3 + d * 3 + e * 1;
		case 1: return a * 1 + b * 2 + c * 2 + d * 2 + e * 10;
		case 2: return (a * 2 + b * 27 + c * 3 + d * 3 + e * 2) / 10;
		case 3: return (a * 2 + b * 3 + c * 30 + d * 3 + e * 2) / 10;
		default: return (a * 1 + b * 2 + c * 2 + d * 20 + e * 1) / 10;
	}
}
//解决方案
std::shared_ptr<GamePetSolution> GPCalc::CreateSol(double solution[5], int grIndex, int level, int base)
{
	std::shared_ptr<GamePetSolution> pSol= std::make_shared<GamePetSolution>(); 
	pSol->gr = grIndex;
	for (int cla = 0; cla < 5; ++cla)
		pSol->cl[cla] = solution[cla];
	double solBase = (base * 1000 + (40 + pSol->gr) * (level - 1)) / 1000;
	memset(pSol->cfLoss, 0, sizeof(double) * 5);
	for (int ba = 0; ba < 5; ++ba)
	{
		pSol->BP[ba] = pSol->cl[ba] * solBase;
		pSol->stat[ba] = CalcStat(ba, pSol->cl) * solBase + 20;
	}
	pSol->spi = CalcSpi(pSol->cl) * solBase + 100;
	pSol->rev = CalcRev(pSol->cl) * solBase + 100;
	return pSol;
}

GPCalc *GPCalc::GetInstance()
{
	static GPCalc instance;
	return &instance;
}

//Pts格式為 (圖鑑編號,寵物名,種族編號,體頂檔,力頂檔,強頂檔,速頂檔,魔頂檔,屬性,屬性量,卡色,卡等,技能欄,出沒地點,一級點,出現率[分鐘],可否封印)
//获取用户选取的宠物类型
//根据宠物编号 设置计算倍率
double GPCalc::CalcBaseBP(int petNumber, int level)
{
	//#37使魔, #46小蝙蝠, #9001~#9004牛鬼
	if (petNumber == 37 || petNumber == 46 || petNumber == 9001 || petNumber == 9002 || petNumber == 9003 || petNumber == 9004) return 0.3;
	//#221小鴨子, 非一級時, 每檔BP=0.22
	else if (petNumber == 221 || petNumber == 46 && level > 1)
		return 0.22;
	//#2307二改愛絲波波
	else if (petNumber == 2307 || petNumber == 239)
		return 0.25;
	//其他所有寵
	return 0.2;
}

double GPCalc::CalcBaseBP(const std::string &petName, int level /*=1*/)
{
	/*std::cout << "m_specialPetBase size :" << m_specialPetBase.size() << petName << std::endl;
	for(auto it= m_specialPetBase.begin();it!= m_specialPetBase.end();++it)
		std::cout << "m_specialPetBase content :" << it->first << ":"  << it->second << std::endl;*/
	auto it = m_specialPetBase.find(petName);
	if (it != m_specialPetBase.end())
	{
		return it->second;
	}
	return 0.2;
}

//名称 血 魔 攻 防 敏 精神 回复 体 力 强 敏 魔
std::shared_ptr<GamePetCalcData> GPCalc::CreatePetData(stringList inputData)
{
	int vLen = inputData.size();
	if (vLen != 13)
		return nullptr;
	std::shared_ptr<GamePetCalcData> pet(new GamePetCalcData);
	for (int i = 0; i < 5; ++i)
	{
		pet->ability[i] = (std::stoi( inputData[i + 1]) - 20);
	}
	//精神
	pet->spirit = std::stoi(inputData[6]);
	//恢复
	pet->recovery = std::stoi(inputData[7]);

	//有1级的bp
	for (int b = 0; b < 5; b++) //当前用户输入的1级游戏bp数据
		pet->curShowBp[b] = std::stoi(inputData[b + 8]);
	auto pDefPet = m_cgPetDef[inputData[0]]; //宠物图鉴
	if (pDefPet == nullptr)
		return nullptr;
	pet->petName = pDefPet->name;
	pet->number = pDefPet->number;
	pet->baseBP = CalcBaseBP(pet->petName, 1); //先用这个 后面预置	
	//std::cout << "CalcBaseBP :" <<pet->baseBP << std::endl;

	int totalGrade = 0;
	for (size_t i = 0; i < 5; i++)
	{
		pet->bestBPGrade[i] = pDefPet->bestBPGrade[i];
		totalGrade += pDefPet->bestBPGrade[i];
	}
	pet->totalGrade = totalGrade;
	return pet;
}
//血 魔 攻 防 敏 精 回  bp 体 力 强 敏 魔
//默认按12项处理
std::shared_ptr<GamePetCalcData> GPCalc::ParseLine(stringList inputData)
{
	if (inputData.size() < 5)
		return nullptr;
	auto pet = CreatePetData(inputData);
	if (pet == nullptr)
		return nullptr;
	double base = pet->baseBP; //能力倍率
	int numOfResults = 0;
	int BPClSum = 0; //成长档和 例如黄蜂满档加起来110档
	double BPCl[5];	 //满档成长档

	double BPMax[5] = { 0, 0, 0, 0, 0 };	 //(5)最高成长档
	double BPMin[5] = { 0, 0, 0, 0, 0 };	 //5  最低成长档
	std::vector<double> cfLoss({ 0, 0, 0, 0, 0 }); //掉档数
	std::vector<double> maxLoss({ 0, 0, 0, 0, 0 });
	//最大掉档
	double totalLoss = 0;
	double lossMin = 20;
	double lossMax = 0;
	std::vector<double> cfLossSummarycfLoss({ 4, 4, 4, 4, 4 });
	std::vector<double> maxLossSummary({ 0, 0, 0, 0, 0 });
	for (int b = 0; b < 5; b++)
	{
		BPCl[b] = pet->bestBPGrade[b]; //  满档 成长档
		BPClSum = BPClSum + BPCl[b];
	}
	double errorMargin = 0;
	int loop = 6;
	if (pet->level == 1)
	{
		loop = 1;
		errorMargin = 0;
	}
	//loop就是次数  式中seed被我们称为“系”，取值范围是0.040和0.045之间的6个步差为0.001的数。 所以最多6次循环计算即可
	for (int gr = 0; gr < loop; gr++)
	{
		//以满档数据计算，一个宠物最多+10能力点，最多掉4能力
		//依据噬生理论：对于一个确定的宠物，其某项能力的表观档次的所有取值可能与其最高成长档的差值范围是 -4~+10
		for (int b = 0; b < 5; b++)
		{
			BPMax[b] = BPCl[b] + 10; //10点隐藏bp 随机档？ BPMax:20, 48, 23, 53, 16
			BPMin[b] = BPCl[b] - 4;	 //最多掉4档？	BPMin:6, 34, 9, 39, 2]
		}
		//未饲养升级的n级宠物的BP是按照(0.2+(n-1)*seed)为单位进行分配的。
		//宠物的某项BP上的成长档是多少，该BP上就会分到多少个单位的(0.2+(n-1)*seed)
		//之外，还有10个单位的(0.2+(n-1)*seed)会被随机分配到五项BP上，这10个单位的(0.2+(n-1)*seed)就是随机10档。
		//base就是0.2 是20倍率/100得到   40就是系对应0.040有6个级别 gr其实就是40到-45
		double baseBP = (base * 1000 + (40 + gr) * (pet->level - 1)) / 1000; //0.2
		if (pet->bInputBp)													 //如果用户有输入1级的bp数据 则计算
		{
			//formula is: (base+(40+gr)/1000*(level-1)), gr=0,1,2,3,4,5
			for (int b = 0; b < 5; ++b)
			{
				BPMax[b] = std::min(BPMax[b], std::floor((pet->curShowBp[b] + 1 + errorMargin) / baseBP));
				BPMin[b] = std::max(BPMin[b], std::ceil((pet->curShowBp[b] - errorMargin) / baseBP));
				//计算完成后，变为 相当于结合实际bp值，排除了一些数据？
				//BPMax: 15, 40, 20, 45, 10]
				//BPMin: 10, 35, 15, 40, 5]
			}
		}
		BPMax[0] = std::min(BPMax[0], std::floor((pet->ability[0] + 1 + errorMargin) / 8 / baseBP));
		BPMax[1] = std::min(BPMax[1], std::floor((pet->ability[2] + 1 + errorMargin) / 2.7 / baseBP));
		BPMax[2] = std::min(BPMax[2], std::floor((pet->ability[3] + 1 + errorMargin) / 3 / baseBP));
		BPMax[3] = std::min(BPMax[3], std::floor((pet->ability[4] + 1 + errorMargin) / 2 / baseBP));
		BPMax[4] = std::min(BPMax[4], std::floor((pet->ability[1] + 1 + errorMargin) / 10 / baseBP));
		//BPMax: (5) [15, 40, 20, 45, 10] 这个是把当前宠物的血数据以及影响关系，代入计算，不知道作用
		//add('Early BPMax = ' + BPMax + '<BR>')
		//add('Early BPMin = ' + BPMin + '<BR>')
		int breaker = 0;
		//循环碰撞20次 最接近就是实际数 20档 所以20次
		while (true)
		{
			breaker++;
			int total = 0;
			for (int q = 0; q < 5; ++q)
				total = total + BPMax[q] - BPMin[q];
			//total=25
			//baseBP=0.2 把其余部分对当前部分的影响乘起来，是计算得到的血量值  当前宠血-计算值，是体力的影响值 /8 /baseBp就是实际宠成长档值？
			BPMin[0] = std::min(BPMax[0], std::max(BPMin[0], std::ceil((pet->ability[0] - errorMargin - (BPMax[1] * 2 + BPMax[2] * 3 + BPMax[3] * 3 + BPMax[4] * 1) * baseBP) / 8 / baseBP)));
			BPMin[1] = std::min(BPMax[1], std::max(BPMin[1], std::ceil((pet->ability[2] - errorMargin - (BPMax[0] * 2 + BPMax[2] * 3 + BPMax[3] * 3 + BPMax[4] * 2) / 10 * baseBP) / 2.7 / baseBP)));
			BPMin[2] = std::min(BPMax[2], std::max(BPMin[2], std::ceil((pet->ability[3] - errorMargin - (BPMax[0] * 2 + BPMax[1] * 3 + BPMax[3] * 3 + BPMax[4] * 2) / 10 * baseBP) / 3 / baseBP)));
			BPMin[3] = std::min(BPMax[3], std::max(BPMin[3], std::ceil((pet->ability[4] - errorMargin - (BPMax[0] * 1 + BPMax[1] * 2 + BPMax[2] * 2 + BPMax[4] * 1) / 10 * baseBP) / 2 / baseBP)));
			BPMin[4] = std::min(BPMax[4], std::max(BPMin[4], std::ceil((pet->ability[1] - errorMargin - (BPMax[0] * 1 + BPMax[1] * 2 + BPMax[2] * 2 + BPMax[3] * 2) * baseBP) / 10 / baseBP)));
			BPMax[0] = std::max(BPMin[0], std::min(BPMax[0], std::floor(((pet->ability[0] + 1 + errorMargin) - (BPMin[1] * 2 + BPMin[2] * 3 + BPMin[3] * 3 + BPMin[4] * 1) * baseBP) / 8 / baseBP)));
			BPMax[1] = std::max(BPMin[1], std::min(BPMax[1], std::floor(((pet->ability[2] + 1 + errorMargin) - (BPMin[0] * 2 + BPMin[2] * 3 + BPMin[3] * 3 + BPMin[4] * 2) / 10 * baseBP) / 2.7 / baseBP)));
			BPMax[2] = std::max(BPMin[2], std::min(BPMax[2], std::floor(((pet->ability[3] + 1 + errorMargin) - (BPMin[0] * 2 + BPMin[1] * 3 + BPMin[3] * 3 + BPMin[4] * 2) / 10 * baseBP) / 3 / baseBP)));
			BPMax[3] = std::max(BPMin[3], std::min(BPMax[3], std::floor(((pet->ability[4] + 1 + errorMargin) - (BPMin[0] * 1 + BPMin[1] * 2 + BPMin[2] * 2 + BPMin[4] * 1) / 10 * baseBP) / 2 / baseBP)));
			BPMax[4] = std::max(BPMin[4], std::min(BPMax[4], std::floor(((pet->ability[1] + 1 + errorMargin) - (BPMin[0] * 1 + BPMin[1] * 2 + BPMin[2] * 2 + BPMin[3] * 2) * baseBP) / 10 / baseBP)));
			//BPMax: (5) [12, 40, 17, 43, 7]
			//BPMin: (5) [10, 38, 15, 41, 5]
			for (int q = 0; q < 5; ++q)
				total = total - BPMax[q] + BPMin[q];
			//total=15
			if (total > 70 || total == 0 || breaker > 20)
				break;
		}
		//add('Gr    = ' + gr    + ' | ')
		//add('BPMax = ' + BPMax + ' | ')
		//add('BPMin = ' + BPMin + '<BR>')
		bool doCheckBP = true; //这个应该是输入后才进行计算？
		for (auto a = BPMin[0]; a <= BPMax[0]; a++)
		{
			for (auto b = BPMin[1]; b <= BPMax[1]; b++)
			{
				for (auto c = BPMin[2]; c <= BPMax[2]; c++)
				{
					for (auto d = BPMin[3]; d <= BPMax[3]; d++)
					{
						for (auto e = BPMin[4]; e <= BPMax[4]; e++)
						{
							auto cor = true;
							double combo[5] = { a, b, c, d, e };
							//分别用最小bp和最大bp 通过计算值和实际成长的值比对 达到算档
							for (int co = 0; co < 5; ++co)
							{
								//计算的bp值对血魔攻防敏的影响 得到预计值
								auto thisStat = std::floor(CalcStat(co, combo) * baseBP);
								if (pet->level == 1)							 //1级直接和实际宠值比对
									cor = (cor && thisStat == pet->ability[co]); //如果
								else											 //不是1级的话，算差值
									cor = (cor && (std::abs(thisStat - pet->ability[co]) <= errorMargin));
								//add((std::floor(stat(co,a,b,c,d,e)*baseBP) + 'vs' + p.stat[co]) + ' | ')
							}
							//判断档次范围是否在正常档次 正负10范围内
							double sum = a + b + c + d + e;
							cor = cor && ((BPClSum + 10) >= sum);
							cor = cor && ((BPClSum - 10) <= sum);
							if (cor)
							{
								auto sureLoss = std::max(0.0, BPCl[0] - a) + std::max(0.0, BPCl[1] - b) +
												std::max(0.0, BPCl[2] - c) + std::max(0.0, BPCl[3] - d) + std::max(0.0, BPCl[4] - e);
								totalLoss = BPClSum + 10 - sum;
								if (sureLoss > totalLoss)
									cor = false;
							}
							if (cor && pet->spirit > 0) //输入了精神 进行精神判断
							{
								double thisSpi = std::floor(CalcSpi(combo) * baseBP + 100);
								if (pet->level == 1)
									cor = cor && (pet->spirit == thisSpi);
								else
									cor = cor && (std::abs(thisSpi - pet->spirit) <= errorMargin);
							}
							if (cor && pet->recovery > 0) //输入了回复 加上回复判断
							{
								double thisRev = std::floor(CalcRev(combo) * baseBP + 100);
								if (pet->level == 1)
									cor = cor && (pet->recovery == thisRev);
								else
									cor = cor && (std::abs(thisRev - pet->recovery) <= errorMargin);
							}
							if (cor)
							{
								double solution[5] = { a, b, c, d, e };
								auto newSol = CreateSol(solution, gr, pet->level, base);
								pet->sols.push_back(newSol);
								numOfResults++;
							}
						}
					}
				}
			}
		}
	}
	//END OF INDEX USE
	//结果
	if (numOfResults > 0)
	{
		pet->lossMin = 20;
		pet->lossMax = 0;
		double tmpVal[5] = { 4, 4, 4, 4, 4 };
		memcpy(pet->cfLossSummary, tmpVal, sizeof(double) * 5);
		memset(pet->maxLossSummary, 0, sizeof(double) * 5);
		if (pet->spirit <= 0)
			pet->spirit = std::floor(pet->sols[0]->spi);
		if (pet->recovery <= 0)
			pet->recovery = std::floor(pet->sols[0]->rev);
		{
			if (!pet->bInputBp)
			{
				for (int st = 0; st < 5; st++)
					pet->calcBp[st] = std::floor(pet->sols[0]->BP[st]);
			}
		}
	}
	for (int r = 0; r < numOfResults; r++)
	{
		if (pet->spirit <= 0 || (std::floor(pet->sols[r]->spi) != pet->spirit))
			pet->spirit = 0;
		if (pet->recovery <= 0 || (std::floor(pet->sols[r]->rev) != pet->recovery))
			pet->recovery = 0;
		if (!pet->bInputBp)
		{
			//possible performance improvement here?
			for (int st = 0; st < 5; st++)
			{
				if (pet->calcBp[st] <= 0)
					break;
				else if (pet->calcBp[st] != std::floor(pet->sols[r]->BP[st]))
					pet->calcBp[st] = 0;
			}
		}
		totalLoss = BPClSum + 10;
		for (int st = 0; st < 5; st++)
		{
			pet->sols[r]->cfLoss[st] = std::max(0.0, BPCl[st] - pet->sols[r]->cl[st]);
			totalLoss -= pet->sols[r]->cl[st];
		}
		double minLoss = std::max(0.0, totalLoss - 16);
		for (int st = 0; st < 5; st++)
		{
			pet->sols[r]->cfLoss[st] = std::max(pet->sols[r]->cfLoss[st], minLoss);
			pet->cfLossSummary[st] = std::min(pet->sols[r]->cfLoss[st], pet->cfLossSummary[st]);
		}
		pet->sols[r]->loss = totalLoss;
		pet->lossMin = std::min(pet->lossMin, totalLoss);
		pet->lossMax = std::max(pet->lossMax, totalLoss);
		auto unCfLoss = totalLoss;
		for (int st = 0; st < 5; st++)
			unCfLoss -= pet->sols[r]->cfLoss[st];
		for (int st = 0; st < 5; st++)
		{
			pet->sols[r]->maxLoss[st] = std::min(unCfLoss + pet->sols[r]->cfLoss[st], 4.0);
			pet->maxLossSummary[st] = std::max(pet->sols[r]->maxLoss[st], pet->maxLossSummary[st]);
		}
	}
	for (int st = 0; st < 5; st++)
		pet->ability[st] += 20;
	return pet;
}

bool GPCalc::ParsePetGrade(stringList inputData, ParsePetCalcData& retData)
{
	auto pCalcData = ParseLine(inputData);
	if (!pCalcData)
		return false;
	retData.lossMin = pCalcData->lossMin;
	retData.lossMax = pCalcData->lossMax;
	return true;
}

std::vector<std::string> GPCalc::string_split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern; //扩展字符串以方便操作
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

void GPCalc::initCaclPetData(stringList petData)
{
	m_cgPetDef.clear();
	for (std::string petInfo : petData)
	{
		stringList petVal = string_split(petInfo,";");
		std::shared_ptr<CGPetPictorialBook> pPet= std::make_shared<CGPetPictorialBook>(); 
		//pPet->number = tmpPet.toInt();
		if (petVal.size() >= 7)
		{
			pPet->name = petVal.at(0);
			pPet->raceType = std::stoi(petVal.at(1));
			for (int i = 0; i < 5; ++i)
			{
				pPet->bestBPGrade[i] = std::stoi(petVal.at(i + 2));
			}
		}
		m_cgPetDef.insert(std::make_pair(pPet->name, pPet));
	}
}
