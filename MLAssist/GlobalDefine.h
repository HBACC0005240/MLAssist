#pragma once
#include "../CGALib/gameinterface.h"
#include "constDef.h"
#include <QObject>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>

static QPoint g_defAddTroop(521, 11);
static QPoint g_defMapCtrl(624, 11);

static QPoint g_defSkill(112, 465);			 //技能
static QPoint g_defItem(190, 465);			 //物品
static QPoint g_defPet(265, 465);			 //宠物
static QPoint g_defSys(490, 465);			 //系统
static QPoint g_defGetOut(510, 158);		 //登出
static QPoint g_defGetOutEntrance(347, 140); //回到登入点

static QPoint g_defResumeHMP(302, 174);	   //恢复hp mp
static QPoint g_defResumePetHMP(285, 252); //恢复宠物hp mp
static QPoint g_defYes(250, 315);		   //是
static QPoint g_defNo(383, 315);		   //否
static QPoint g_defOk(313, 319);		   //确定

static QPoint g_defBattleLed(70, 8);	   //战斗
static QPoint g_defChatLed(90, 8);		   //聊天
static QPoint g_defTroopLed(105, 8);	   //队伍
static QPoint g_defCardLed(126, 8);		   //名片
static QPoint g_defTransactionLed(145, 8); //交易
static QPoint g_defHomeLed(160, 8);		   //家族

#define PI 3.141592653589793238462

#define GAME_X "0092BD28"
#define GAME_Y "0092BD30"
#define GAME_MOVE_X "008E543C"
#define GAME_MOVE_Y "008E5440"
#define GAME_MOVE_CALL "0044D000"

#define MAXBUFLEN 100
#define MOVE_STEP_DIAGONAL 64 //左右
#define MOVE_STEP_UPDOWN 48	  //上下
#define MOVE_STEP 32
#define GAME_CENTER_X 320
#define GAME_CENTER_Y 240
#define STEP_VAL 16 //斜边 16取值
#define MAX_STEP 4	//一次最多走4个坐标 不然就超出边界

struct GameInfo;	   //角色基类
struct GameSkill;	   //技能基类
struct Character;	   //人物角色
struct GamePet;		   //宠物
struct GameItem;	   //道具
struct GameAttrBase;   //人物属性信息
struct GameBattleUnit; //战斗信息 敌方 我方
struct GameSearch;	   //搜索信息
struct GameCompound;   //合成物品信息

typedef QSharedPointer<Character> CharacterPtr;
typedef QSharedPointer<GameItem> GameItemPtr;
typedef QSharedPointer<GameSkill> GameSkillPtr;
typedef QSharedPointer<GamePet> GamePetPtr;
typedef QSharedPointer<GameSearch> GameSearchPtr;
typedef QSharedPointer<GameBattleUnit> GameBattleUnitPtr;
typedef QSharedPointer<GameCompound> GameCompoundPtr;

typedef QList<GameItemPtr> GameItemList;
typedef QList<GameSkillPtr> GameSkillList;
typedef QList<GamePetPtr> GamePetList;
typedef QList<GameSearchPtr> GameSearchList;
typedef QList<GameBattleUnitPtr> GameBattleUnitList;
typedef QList<GameCompoundPtr> GameCompoundList;
//
//Q_DECLARE_METATYPE(QSharedPointer<GameItem>)
//Q_DECLARE_METATYPE(QSharedPointer<GameSkill>)
//Q_DECLARE_METATYPE(QSharedPointer<GamePet>)
//Q_DECLARE_METATYPE(QSharedPointer<GameSearch>)
//Q_DECLARE_METATYPE(QSharedPointer<GameBattleUnit>)
//Q_DECLARE_METATYPE(QSharedPointer<GameCompound>)
Q_DECLARE_METATYPE(GameItemPtr)
Q_DECLARE_METATYPE(GamePetPtr)
Q_DECLARE_METATYPE(GameSearchPtr)
Q_DECLARE_METATYPE(GameBattleUnitPtr)
Q_DECLARE_METATYPE(GameCompoundPtr)
Q_DECLARE_METATYPE(CharacterPtr)
Q_DECLARE_METATYPE(GameSkillPtr)
//游戏人物 队友 宠物 怪物信息基类
struct GameInfo
{
	QString name;	//名称
	int hp = 0;		//血量
	int mp = 0;		//魔力
	int maxhp = 0;	//总血量
	int maxmp = 0;	//总魔力
	int level = 0;	//等级
	int xp = 0;		//当前经验
	int maxxp = 0;	//最大经验
	int health = 0; //健康值0-100
	int id = -1;
	QString realname; //真实名称 比如黄蜂
	QString showname; //设置名称
};
struct GameTeamPlayer : public GameInfo
{
	int unit_id = 0;
	int x = 0;
	int y = 0;
	int injury;
	bool is_me = false;
	QString nick_name;
	QString title_name;
};
struct GameSearch
{
	QString name; //搜索名称
	bool bSearch; //是否搜索
};
struct GameBattleUnit : public GameInfo
{
	bool exist = false;	 //站位是否存在战斗单位
	int flags = 0;		 //还不知道是啥
	int pos = 0;		 //站位
	bool isback = false; //
	int debuff = 0;		 //中毒酒醉 buff状态
	int multi_hp = 0;
	int multi_maxhp = 0;
	int petriding_modelid = 0; //骑乘宠物id
};
//游戏技能基类
struct GameSkill
{
	bool exist = false; //是否存在
	QString name;		//技能名称
	QString info;		//技能介绍
	int id = 0;			//技能id
	int cost = 0;		//耗魔
	int flags = 0;		//
	int index = 0;		//技能位置
	int pos = 0;
	int level = 0;			//等级
	int maxLevel = 0;		//最高等级
	bool available = false; //是否可用
	int xp = 0;
	int maxxp = 0;
	GameSkillList subskills; //技能子项
};

//合成材料
struct GameMaterial
{
	int itemid = 0;
	int count = 0;
	QString name;
	int pos = 0; //合成时候用
};
//合成物品列表
struct GameCompound
{
	int id;
	int cost;
	int level;
	int itemid;
	int index;
	QString name;
	QString info;
	GameMaterial materials[5];
	bool available;
};
//技能项
struct GameCompoundSkill : public GameSkill
{
	GameCompound compound; //可合成物品信息项
};
struct GameAttrBase
{
	int points_remain = 0;	  //未加点数
	int points_endurance = 0; //体
	int points_strength = 0;  //力
	int points_defense = 0;	  //强
	int points_agility = 0;	  //敏
	int points_magical = 0;	  //魔
	int value_attack = 0;	  //攻击
	int value_defensive = 0;  //防御
	int value_agility = 0;	  //敏捷
	int value_spirit = 0;	  //精神
	int value_recovery = 0;	  //回复
	int resist_poison = 0;	  //抵抗-毒
	int resist_sleep = 0;	  //抵抗-昏睡
	int resist_medusa = 0;	  //抵抗-石化
	int resist_drunk = 0;	  //抵抗-酒醉
	int resist_chaos = 0;	  //抵抗-混乱
	int resist_forget = 0;	  //抵抗-遗忘
	int fix_critical = 0;	  //修正-必杀
	int fix_strikeback = 0;	  //修正-反击
	int fix_accurancy = 0;	  //修正-命中
	int fix_dodge = 0;		  //修正-闪躲
	int element_earth = 0;	  //地
	int element_water = 0;	  //水
	int element_fire = 0;	  //火
	int element_wind = 0;	  //风
};
struct PlayerDesc
{
	int changeBits;
	int sellIcon;
	std::string sellString;
	int buyIcon;
	std::string buyString;
	int wantIcon;
	std::string wantString;
	std::string descString;
};

//游戏人物信息
struct Character : public GameInfo
{
	int souls = 0;		//掉魂
	int level = 0;		//等级
	int gold = 0;		//金钱
	int unitid = 0;		//id
	int image_id = 0;	//人物模型图片id
	int score = 0;		//战绩？
	int skillslots = 0; //技能格
	int use_title = 0;	//当前使用的称号
	int avatar_id = 0;
	bool petriding = false;		  //骑乘中？
	int petid = -1;				  //当前出战宠物id
	int default_petid = -1;		  //默认出战宠物
	int direction = 0;			  //面对的方向
	int punchclock = 0;			  //卡时
	bool usingpunchclock = false; //是否打卡中
	QString job;				  //职业名称
	QStringList titles;			  //称号
	QString prestige;			  //声望称号
	GameAttrBase detail;		  //属性
	int manu_endurance = 0;		  //耐力
	int manu_skillful = 0;		  //灵巧
	int manu_intelligence = 0;	  //智力
	int value_charisma = 0;		  //魅力
	int serverindex = -1;		  //当前服务器线路
								  ////当前坐标
								  //int x = 0;
								  //int y = 0;
								  //QString mapName;  //地图名称
								  //int mapIndex = 0; //地图index
	QString sGid;				  //游戏id
	int player_index = 0;			//左右角色
	PlayerDesc persdesc;
};

//游戏宠物信息
struct GamePet : public GameInfo
{
	int index = -1; //顺序位置
	int flags = 0;
	int battle_flags = 0;
	int loyality = 0;			 //忠诚
	bool default_battle = false; //当前战斗宠物
	GameSkillList skills;		 //技能列表
	GameAttrBase detail;		 //宠物属性
	bool exist = false;			 //是否存在
	int state = 0;
	int grade = -1; //档次
	int lossMinGrade = -1;
	int lossMaxGrade = -1;
	bool bCalcGrade = true; //是否计算档次
	int race = 0;			//种族
	int skillslots = 0;
};
//人物物品信息
struct GameItem
{
	GameItem() {}
	QString name;				//名称
	QString maybeName;			//没鉴定时 数据库查询可能名称
	QString info;				//物品介绍
	QString attr;				//物品属性
	int image_id = 0;			//图片id
	int count = -1;				//数量
	int pos = -1;				//位置
	int id = -1;				//id
	int type = -1;				//物品类型
	int level = 0;				//等级
	bool assessed = false;		//是否已鉴定
	bool exist = false;			//存在
	int maxCount = -1;			//物品堆叠上限
	bool isDrop = false;		//是否扔
	bool isPile = false;		//是否叠加
	bool isSale = false;		//是否卖
	bool isPick = false;		//是否捡
	int nCurDurability = 0;		//当前耐久度
	int nMaxDurability = 0;		//最大耐久度
	double nDurabilityRate = 0; //当前耐久率

	bool compare(GameItem *t)
	{
		if (this->name != t->name)
			return false;
		if (this->info != t->info)
			return false;
		if (this->attr != t->attr)
			return false;
		if (this->count != t->count)
			return false;
		if (this->pos != t->pos)
			return false;
		if (this->id != t->id)
			return false;
		if (this->type != t->type)
			return false;
		if (this->level != t->level)
			return false;
		if (this->assessed != t->assessed)
			return false;
		if (this->exist != t->exist)
			return false;
		if (this->maxCount != t->maxCount)
			return false;
		if (this->isDrop != t->isDrop)
			return false;
		if (this->isPile != t->isPile)
			return false;
		if (this->isSale != t->isSale)
			return false;
		return true;
	}
};
//游戏升级数据
struct GameUpgradeData
{
	bool bEnabled = false;	 //启用
	int addBpType = -1;		 //混加	0体 1力 2强 3敏 4魔 混加检查下面值
	int vigorVal = 0;		 //指定体力多少点
	int strengthVal = 0;	 //指定体力多少点力
	int enduranceVal = 0;	 //指定强度多少点
	int agilityVal = 0;		 //指定敏捷多少点敏
	int magicalVal = 0;		 //指定魔力多少点
	bool bVigor = false;	 //加体力
	bool bStrength = false;	 //加力量
	bool bEndurance = false; //加强度
	bool bAgility = false;	 //加敏捷
	bool bMagical = false;	 //加魔法
};
struct GamePlayerUpgradeCfg
{
	bool bEnabled = false;		//启用自动加点
	QString playerName;			//指定玩家名字加点
	GameUpgradeData specialCfg; //指定加点
	GameUpgradeData normalCfg;	//正常加点
	uint64_t lastCallTime = 0;	//最后一次调用的时间
};
struct GameUpgradeCfg
{
	bool bEnabled = false;		 //启用宠物自动加点
	QString petName;			 //指定宠物名字加点
	QString petRealName;		 //指定宠物真实名字加点
	GameUpgradeData normalCfg;	 //正常加点
	GameUpgradeData unnormalCfg; //爆点加点
	uint64_t lastCallTime = 0;	 //最后一次调用的时间
	int oldVigorVal = 0;		 //加点前体力
	int oldStrengthVal = 0;		 //加点前力量
	int oldEnduranceVal = 0;	 //加点前强度
	int oldAgilityVal = 0;		 //加点前敏捷
	int oldMagicalVal = 0;		 //加点前魔法
};

typedef struct CGA_BattleContext_s
{
	CGA_BattleContext_s()
	{
		m_bRoundZeroNotified = false;
		m_bIsPetDoubleAction = false;
		m_bIsPlayerForceAction = false;
		m_bIsPlayerActionPerformed = false;
		m_bIsPlayerEscaped = false;
		m_bIsHaveLv1Enemy = false;
		m_bIsSealLv1Enemy = false;
		m_bIsFirstDouble = false;
		m_bIsPetRiding = false;
		m_iPlayerStatus = 0;
		m_iPlayerPosition = 0;
		m_iRoundCount = 0;
		m_iLastRound = 0;
		m_iPetPosition = 0;
		m_iEnemyCount = 0;
		m_iTeammateCount = 0;
		m_iFrontCount = 0;
		m_iBackCount = 0;
		m_iWeaponAllowBit = 0;
		m_iSkillAllowBit = 0;
		m_iPetSkillAllowBit = 0;
		m_iConditionTarget = -1;
		m_nLv1MaxHp = 0;
		m_nLv1MaxMp = 0;
		m_nCurDoubleAction = 0;
	}

	bool m_bIsPlayer;				 //是否人物
	bool m_bIsDouble;				 //人物没带宠 是否二动
	bool m_bIsFirstDouble;			 //是否第一动使用
	int m_nCurDoubleAction;			 //当前是1动还是2动	0一动 1二动 2结束
	bool m_bIsSkillPerformed;		 //是否技能
	bool m_bRoundZeroNotified;		 //第0回合 1回合从0开始
	bool m_bIsPetRiding;			 //是否骑乘中
	bool m_bIsPetDoubleAction;		 //宠物是否二动
	bool m_bIsPlayerForceAction;	 //人物强制行动
	bool m_bIsPlayerActionPerformed; //是否人物动作回合
	bool m_bIsPlayerEscaped;		 //人物是否逃跑
	bool m_bIsHaveLv1Enemy;			 //是否包含1级怪
	bool m_bIsSealLv1Enemy;			 //是否打开了1级宠设置 默认封印
	int m_iPlayerStatus;			 //人物状态
	int m_iPlayerPosition;			 //人物站位
	int m_iRoundCount;				 //回合总数
	int m_iLastRound;				 //最后一回合 当前回合数
	int m_iBGMIndex;				 //战斗音乐index
	int m_iPetId;					 //宠物id
	int m_iPetPosition;				 //宠物站位
	int m_iEnemyCount;				 //敌人总数
	int m_iTeammateCount;			 //己方总数
	int m_iFrontCount;				 //前排敌人数
	int m_iBackCount;				 //后排敌人数
	int m_iWeaponAllowBit;			 //武器是否允许
	int m_iSkillAllowBit;			 //技能是否允许
	int m_iPetSkillAllowBit;		 //宠物技能是否允许
	int m_iConditionTarget;			 //判断目标
	int m_nLv1MaxHp;				 //1级怪血
	int m_nLv1MaxMp;				 //1级怪魔

	CharacterPtr m_character;
	GameSkillList m_PlayerSkills;
	GamePetList m_Pets;
	GameItemList m_Items;
	GameBattleUnitList m_UnitGroup;

} CGA_BattleContext_t;
typedef struct CGA_NPCDialog_s
{
	CGA_NPCDialog_s()
	{
		type = 0;
		options = 0;
		dialog_id = 0;
		npc_id = 0;
	}
	int type;
	int options;
	int dialog_id;
	int npc_id;
	QString message;
} CGA_NPCDialog_t;

typedef struct CGA_MapCellData_s
{
	CGA_MapCellData_s()
	{
		xbottom = 0;
		ybottom = 0;
		xsize = 0;
		ysize = 0;
	}
	int xbottom;
	int ybottom;
	int xsize;
	int ysize;
	std::vector<short> cells;
	int mapindex;
} CGA_MapCellData_t;

typedef struct CGA_MapUnit_s
{
	CGA_MapUnit_s()
	{
		valid = 0;
		type = 0;
		unit_id = 0;
		model_id = 0;
		xpos = 0;
		ypos = 0;
		item_count = 0;
		injury = 0;
		icon = 0;
		level = 0;
		flags = 0;
	}
	int valid;
	int type;
	int unit_id;
	int model_id;
	int xpos;
	int ypos;
	int item_count;
	int injury;
	int icon;
	int level;
	int flags;
	QString unit_name;
	QString nick_name;
	QString title_name;
	QString item_name;
} CGA_MapUnit_t;

typedef QVector<CGA_MapUnit_t> CGA_MapUnits_t;

struct GameCfgBase
{
	bool bChecked = false;
	bool bSelf = false;		   //自己
	bool bPet = false;		   //宠
	bool bTeammate = false;	   //队友
	int nLv = -1;			   //等级 默认最高级
	QString selectName;		   //人物名称
	QString selectSubName;	   //二级选择名称
	uint64_t lastUseSkill = 0; //最后一次调用的时间
	double dCost = 0;		   //技能耗魔
};
//游戏治疗设置
struct GameHealCfg : public GameCfgBase
{
	int nHurtVal = 100; //治疗 默认治疗全部
};
//游戏急救设置
struct GameFirstAidCfg : public GameCfgBase
{
	double dFirstAidHp = 20; //急救值 默认20%
};
struct GameBattlePetCfg
{
	bool bRecallLoyality = false;			   //收回忠诚小于
	bool bRecallMp = false;					   //收回魔小于
	bool bSummonPet = false;				   //召唤宠物
	int nRecallLoyality = 0;				   //忠诚值
	int nRecallMp = 0;						   //魔法值
	int nSummonPetType = TChangePet_HighestLv; //召唤宠物类型 默认最高等级
	int nSummonPetIndex = -1;				   //召唤宠物 指定index
};
struct GameConditionCfg : GameCfgBase
{
	bool bChecked = false;	  //是否判断
	double dVal = 0;		  //判断值
	bool bPercentage = false; //是否百分比
	int nType = 0;			  //类型
};
//装备保护
struct GameEquipProtectCfg : GameConditionCfg
{
	int nSwapEquip = 0;			  //0  更换同名装备 1 更换同类装备 2直接下线
	bool bNoEquipOffLine = false; //没有装备 下线
	bool bRenEquip = false;		  //扔无耐久装备
};
struct GameTradeStuff
{
	int gold = 0;
	CGA::cga_items_info_t items;
	CGA::cga_trade_pet_info_t pets[5];
	CGA::cga_trade_pet_skill_info_t petskills[5];
};
class CGPetPictorialBook
{
public:
	int number;			   //编号
	QString name;		   //宠物名称
	QString raceTypeName;  //种族名称
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
typedef QSharedPointer<CGPetPictorialBook> CGPetPictorialBookPtr;

struct GamePetSolution
{
	int gr;
	double cl[5];
	/*for (var cla in classes) this.cl[cla] = classes[cla]*/
	//formula is: (baseBP+(40+gr)/1000*(level-1)), gr=0,1,2,3,4,5
	//solBase = (base * 1000 + (40 + this.gr) * (level - 1)) / 1000
	double BP[5];
	double stat[5];
	double loss; // = undefined
	double cfLoss[5];
	double maxLoss[5];
	/*		for (var ba in this.cl) {
				this.BP[ba] = this.cl[ba] * solBase
					this.stat[ba] = stat(ba, this.cl) * solBase + 20
			}*/
	double spi; //精神 = spi(this.cl) * solBase + 100
	double rev; //回复 = rev(this.cl) * solBase + 100
};
class GamePetCalcData
{
public:
	int number = 0;							   //图鉴编号
	QString petName;						   //宠物名字
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
	QList<QSharedPointer<GamePetSolution> > sols;
};
struct GamePickCfg
{
	bool bPicked = true;		 //是否捡物
	bool bFollow = false;		 //本地图跟随
	bool bFixedPoint = false;	 //定点
	int east = 0;				 //东
	int south = 0;				 //南
	uint64_t lastPickTime = 0;	 //最后一次捡物时间
	QList<QPoint> filterPosList; //过滤捡取失败物品坐标	貌似得每个物品加个时间 才行 一段时间回复
};
class CProcessItem
{
public:
	CProcessItem(quint32 pid, quint32 tid, quint32 hWnd, const QString &szTitle, bool bAttached, const QString &sGid)
	{
		m_ProcessId = pid;
		m_ThreadId = tid;
		m_hWnd = hWnd;
		m_Title = szTitle; //QString::fromWCharArray(szTitle);
		m_bAttached = bAttached;
		m_sLoginUserID = sGid;
	}
	quint32 m_ProcessId;
	quint32 m_ThreadId;
	quint32 m_hWnd;
	QString m_Title;
	QString m_sLoginUserID;
	bool m_bAttached;
};

typedef QSharedPointer<CProcessItem> CProcessItemPtr;
typedef QList<CProcessItemPtr> CProcessItemList;
struct GameGoodsFetch
{
	QString sGID;			//游戏子账号
	int character = 0;		//0左 1右
	bool bFetched = false;	//是否抓取过
	QString erMsg = "";		//错误信息 例如没有游戏人物 或 已经登录
	QStringList playerInfo; //人物信息 名称 等级 称号 技能
	QStringList ownItems;	//身上物品信息 名称 数量 代码
	QStringList ownPets;	//身上宠物信息 名称 等级 代码 属性
	QStringList bankItems;	//银行物品信息
	QStringList bankPets;	//银行宠物信息
	int ownGold = 0;		//身上金币
	int bankGold = 0;		//银行金币
};
typedef QSharedPointer<GameGoodsFetch> GameGoodsFetchPtr;
typedef QList<GameGoodsFetchPtr> GameGoodsFetchList;
#pragma pack(1)
struct tMapHead
{
	char cFlag[12];
	unsigned int w;
	unsigned int h;
}; // 后续紧跟w*h
#pragma pack()
//#include <opencv2/opencv.hpp>
class TSearchRect
{
public:
	QRect _rect;
	QPoint _centrePos;
	QList<QPoint> _rectPosList;
	//std::vector<cv::Point> _cvRectPosList;

	double _distance = 0; //初始位置和当前中心点距离
};
typedef QSharedPointer<TSearchRect> TSearchRectPtr;
typedef QList<TSearchRectPtr> TSearchRectList;
#define SafeDelete(p) \
	if (nullptr != p) \
	{                 \
		delete p;     \
		p = nullptr;  \
	}
