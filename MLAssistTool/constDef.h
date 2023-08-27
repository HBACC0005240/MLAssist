#pragma once
enum TObjectType
{
	TObject_None = 0,				//空
	TObject_AccountIdentity = 0x100, //账号集合 某个人证件
	TObject_Account = 0x110,		 //账号
	TObject_AccountGid = 0x211,		 //账号子ID
	TObject_GidRole = 0x300,		 //账号子ID角色 左右角色
	TObject_GidRoleLeft = 0x301,	 //账号子ID角色 左右角色
	TObject_GidRoleRight = 0x302,	 //账号子ID角色 左右角色
	TObject_CGPet = 0x1000,			//宠物
	TObject_Pet = 0x1001,			 //宠物
	TObject_BankPet = 0x1002,		//银行宠物
	TObject_CharPet = 0x1003,		//角色宠物

	TObject_CGItem = 0x1100,		 //道具
	TObject_Item = 0x1101,			//道具信息
	TObject_CharItem = 0x1110,		//角色道具信息
	TObject_CharBankItem = 0x1111,	//银行道具	

	TObject_CharGold = 0x1150,		//金币	
	TObject_CharBankGold = 0x1151,	//银行金币	

	TObject_CGSkill = 0x1200,		
	TObject_CharSkill = 0x1201,		
	TObject_CharPetSkill = 0x1202,		

	TObject_RoleRunConfig = 0x4000,	 //账号子ID角色 左右角色
	TObject_GateMap = 0x4200,		 //常用坐标
	TObject_Map = 0x4300,			 //地图信息
	TObject_PetBook = 0x4400,		//宠物图鉴
	TObject_CharServer = 0x4500,	//服务信息		


};
enum TBattleType
{
	dtNone = 0,

	dtCondition = 0x1000,						//判断条件
	dtCondition_Ignore = 0x1001,				//忽略
	dtCondition_EnemyCount = 0x1002,			//敌人数
	dtCondition_TeammateCount = 0x1003,			//队伍数
	dtCondition_EnemySingleRowCount = 0x1004,	//每行敌人数
	dtCondition_PlayerHp = 0x1005,				//人血
	dtCondition_PlayerMp = 0x1006,				//人魔
	dtCondition_PetHp = 0x1007,					//宠血
	dtCondition_PetMp = 0x1008,					//宠魔
	dtCondition_TeammateHp = 0x1009,			//队友血
	dtCondition_TeammateMp = 0x1010,			//队友魔
	dtCondition_EnemyMultiTargetHp = 0x1011,	//敌方T血量
	dtCondition_TeammateMultiTargetHp = 0x1012, //己方T血量
	dtCondition_EnemyAllHp = 0x1013,			//敌方全体血量
	dtCondition_TeammateAllHp = 0x1014,			//己方全体血量
	dtCondition_EnemyUnit = 0x1015,				//敌方单位 名称
	dtCondition_Round = 0x1016,					//回合
	dtCondition_DoubleAction = 0x1017,			//二动
	dtCondition_TeammateDebuff = 0x1018,		//敌人减益
	dtCondition_EnemyLevel = 0x1019,			//敌人等级
	dtCondition_EnemyAvgLevel = 0x1020,			//敌人平均等级
	dtCondition_InventoryItem = 0x1021,			//人物物品
	dtCondition_EnemyType = 0x1022,				//敌人类型 是否Boss战
	dtCondition_TeammateUnit = 0x1023,			//队友名称
	dtCondition_PlayerName = 0x1024,			//人物名称
	dtCondition_PlayerGold = 0x1025,			//人物金币
	dtCondition_PlayerJob = 0x1026,				//人物职业
	dtCondition_PlayerBGM = 0x1027,				//人物背景音乐

	dtCondition_Compare = 0x1028,		//比较基类
	dtCondition_EnemyLv1Hp = 0x1029,	//敌方1级单位个体血量
	dtCondition_EnemyLv1MaxHp = 0x1030, //敌方1级单位个体最大血量
	dtCondition_EnemyLv1MaxMp = 0x1031, //敌方1级单位个体最大魔量

	dtCompare = 0x1100,				 //判断
	dtCompare_Equal = 0x1101,		 //等于
	dtCompare_NotEqual = 0x1102,	 //不等于
	dtCompare_GreaterThan = 0x1103,	 //大于
	dtCompare_LessThan = 0x1104,	 //小于
	dtCompare_GreaterEqual = 0x1105, //大于等于
	dtCompare_LessEqual = 0x1106,	 //小于等于
	dtCompare_Contain = 0x1107,		 //包含
	dtCompare_NotContain = 0x1108,	 //不包含

	dtAction = 0x2000,
	dtAction_Player = 0x2100,				  //动作
	dtAction_PlayerIgnore = 0x2100,			  //人物忽略
	dtAction_PlayerAttack = 0x2101,			  //人物攻击
	dtAction_PlayerGuard = 0x2102,			  //人物防御
	dtAction_PlayerEscape = 0x2103,			  //人物逃跑
	dtAction_PlayerExchangePosition = 0x2104, //人物交换位置
	dtAction_PlayerChangePet = 0x2105,		  //人物交换宠物
	dtAction_PlayerSkillAttack = 0x2106,	  //人物技能攻击
	dtAction_PlayerUseItem = 0x2107,		  //人物使用物品
	dtAction_PlayerLogBack = 0x2108,		  //人物登出
	dtAction_PlayerRebirth = 0x2109,		  //人物气绝 精灵变身
	dtAction_PlayerDoNothing = 0x2110,		  //人物什么也不做

	dtAction_Pet = 0x2200,			  //宠物动作
	dtAction_PetIgnore = 0x2200,	  //宠物忽略
	dtAction_PetDoNothing = 0x2201,	  //宠物什么也不做
	dtAction_PetSkillAttack = 0x2202, //宠物技能攻击

	dtTarget = 0x3000,			 //目标
	dtTarget_Enemy = 0x3001,	 //敌方
	dtTarget_Teammate = 0x3002,	 //队友
	dtTarget_Self = 0x3003,		 //人物
	dtTarget_Pet = 0x3004,		 //宠物
	dtTarget_Condition = 0x3005, //条件

	dtTargetCondition = 0x4000,				   //目标筛选
	dtTargetCondition_Random = 0x4001,		   //随机
	dtTargetCondition_Front = 0x4002,		   //前排
	dtTargetCondition_Back = 0x4003,		   //后排
	dtTargetCondition_LowHP = 0x4004,		   //血少优先
	dtTargetCondition_HighHP = 0x4005,		   //血多优先
	dtTargetCondition_LowHPPercent = 0x4006,   //低血量比例
	dtTargetCondition_HighHPPercent = 0x4007,  //高血量比例
	dtTargetCondition_LowLv = 0x4008,		   //级低优先
	dtTargetCondition_HighLv = 0x4009,		   //级高优先
	dtTargetCondition_SingleDebuff = 0x4010,   //单体异常状态
	dtTargetCondition_MulTDebuff = 0x4011,	   //T字形异常状态
	dtTargetCondition_Goatfarm = 0x4012,	   //砍牛
	dtTargetCondition_Boomerang = 0x4013,	   //回力标(人多的一排)
	dtTargetCondition_LessUnitRow = 0x4014,	   //人少的一排
	dtTargetCondition_MultiMagic = 0x4015,	   //强力魔法
	dtTargetCondition_AimOrderUpDown = 0x4016, //集火上到下
	dtTargetCondition_AimOrderDownUp = 0x4017, //集火下到上
	dtTargetCondition_AimOrderCenter = 0x4018, //集火中间

	dtSettingType_NoPetDoubleAction = 0x5001, //不带宠二动

};
//比较级：higher
//最高级：highest
//low的比较级: lower
//low最高级: lowest
enum TChangePetType
{
	TChangePet_Call = 0,	   //换指定宠
	TChangePet_Recall = 1,	   //收回
	TChangePet_HighestLv,	   //最高等级
	TChangePet_HighestHp,	   //血最多
	TChangePet_HighestMp,	   //魔最多
	TChangePet_LowestLv,	   //等级最低
	TChangePet_LowestHp,	   //血最低
	TChangePet_LowestMp,	   //魔最低
	TChangePet_HighestLoyalty, //最高忠诚
	TChangePet_LowestLoyalty,  //最低忠诚
	TChangePet_SpecialIndex0,  //指定宠物
	TChangePet_SpecialIndex1,  //指定宠物
	TChangePet_SpecialIndex2,  //指定宠物
	TChangePet_SpecialIndex3,  //指定宠物
	TChangePet_SpecialIndex4,  //指定宠物

};
//预置的保护设置
enum TProtectSet
{
	TProtectSet_PlayerHp1 = 1,	//人物血少保护1
	TProtectSet_PlayerHp2 = 2,	//人物血少保护2
	TProtectSet_PlayerMp1 = 3,	//人物魔保护1
	TProtectSet_PlayerMp2 = 4,	//人物魔保护2
	TProtectSet_TeammateHp = 5, //队友血少保护
	TProtectSet_PetHp = 6,		//宠物血少保护
	TProtectSet_PetMp = 7,		//宠物魔少保护
	TProtectSet_TroopHp = 8		//队伍血少保护
};

//逃跑设置
enum TEscapeSet
{
	TEscapeSet_All = 1,			  //全跑
	TEscapeSet_No1Lv = 2,		  //无1级逃跑
	TEscapeSet_SpecialEnemy = 3,  //指定敌人逃跑
	TEscapeSet_EnemyAvgLv = 4,	  //平均等级
	TEscapeSet_EnemyCount = 5,	  //敌人个数
	TEscapeSet_NoBoss = 6,		  //非Boss逃跑
	TEscapeSet_TeammateCount = 7, //队伍人数逃跑

};
//1级宠设置
enum TLv1EnemySet
{
	TLv1EnemySet_FilterHP = 1,	  //过滤1级怪血不达标
	TLv1EnemySet_FilterMP = 2,	  //过滤1级怪魔不达标
	TLv1EnemySet_RecallPet = 3,	  //收回宠物
	TLv1EnemySet_RoundOne = 4,	  //第一回合
	TLv1EnemySet_Lv1HpAction = 5, //1级怪血>多少 继续打怪
	TLv1EnemySet_CleanNoLv1 = 6,  //清除非1级怪
	TLv1EnemySet_LastSet = 7	  //最后设置

};

//转向方向
enum MOVE_DIRECTION
{
	MOVE_DIRECTION_Origin = 0,	  //原地
	MOVE_DIRECTION_North = 0,	  //MOVE_DIRECTION_North			北
	MOVE_DIRECTION_NorthEast = 1, //MOVE_DIRECTION_NorthEast		东北
	MOVE_DIRECTION_East = 2,	  //MOVE_DIRECTION_RIGH				东
	MOVE_DIRECTION_SouthEast = 3, //MOVE_DIRECTION_SouthEast		东南
	MOVE_DIRECTION_South = 4,	  //MOVE_DIRECTION_South			南
	MOVE_DIRECTION_SouthWest = 5, //MOVE_DIRECTION_WestDOWN			西南
	MOVE_DIRECTION_West = 6,	  //MOVE_DIRECTION_West				西
	MOVE_DIRECTION_NorthWest = 7, //MOVE_DIRECTION_WestUP			西北
};
//enum CHARACTER_STATE
//{
//	CHARACTER_Troop = 1,		//队
//	CHARACTER_Battle = 4,		//战
//	CHARACTER_Chat = 8,			//聊
//	CHARACTER_Card = 16,		//名
//	CHARACTER_Exchange = 32,	//易
//	CHARACTER_Home = 64,		//家
//};
enum CHARACTER_STATE
{
	CHARACTER_Battle = 0,			 //战
	CHARACTER_Chat = 1,				 //聊
	CHARACTER_Troop = 2,			 //队
	CHARACTER_Card = 3,				 //名
	CHARACTER_Trade = 4,			 //易
	CHARACTER_Family = 5,			 //家
	CHARACTER_Avatar_Public = 100,	 //公共
	CHARACTER_Battle_Position = 101, //摆摊
};
enum TCharacter_Action
{
	TCharacter_Action_PK = 1,			  //PK
	TCharacter_Action_JOINTEAM = 3,		  //加入队伍
	TCharacter_Action_EXCAHNGECARD = 4,	  //交换名片
	TCharacter_Action_TRADE = 5,		  //交易
	TCharacter_Action_KICKTEAM = 11,	  //剔除队伍
	TCharacter_Action_LEAVETEAM = 12,	  //离开队伍
	TCharacter_Action_TRADE_CONFIRM = 13, //交易确定
	TCharacter_Action_TRADE_REFUSE = 14,  //交易取消
	TCharacter_Action_TEAM_CHAT = 15,	  //队聊
	TCharacter_Action_REBIRTH_ON = 16,	  //开始摆摊
	TCharacter_Action_REBIRTH_OFF = 17,	  //停止摆摊
};
////交易状态
//enum TTradeState
//{
//	TTRADE_STATE_CANCEL = 0,	 //交易取消
//	TTRADE_STATE_READY = 1,	 //交易准备
//	TTRADE_STATE_CONFIRM = 2, //交易确定
//	TTRADE_STATE_SUCCEED = 3, //交易成功
//};
//遇敌方式
enum TEncounterEnemyType
{
	TEncounter_Origin_North = 0,	 //		北
	TEncounter_Origin_NorthEast = 1, //		东北
	TEncounter_Origin_East = 2,		 //		东
	TEncounter_Origin_SouthEast = 3, //		东南
	TEncounter_Origin_South = 4,	 //		南
	TEncounter_Origin_SouthWest = 5, //		西南
	TEncounter_Origin_West = 6,		 //		西
	TEncounter_Origin_NorthWest = 7, //		西北
	TEncounter_Origin_Random = 8,	 //		原地随机
};
//扔宠设置
enum TDropPetType
{
	TDropPetType_PLevel = 1, //等级
	TDropPetType_PHp,		 //生命
	TDropPetType_PMp,		 //魔力
	TDropPetType_PVigor,	 //体力
	TDropPetType_PStrength,	 //力量
	TDropPetType_PEndurance, //防御
	TDropPetType_PAgility,	 //敏捷
	TDropPetType_PMagical,	 //魔法
	TDropPetType_VAttack,	 //攻击力
	TDropPetType_VDefensive, //防御力
	TDropPetType_VAgility,	 //敏捷力
	TDropPetType_VSpirit,	 //精神力
	TDropPetType_VRecovery,	 //回复力
	TDropPetType_Grade,		 //档次
	TDropPetType_RealName,	 //真实名称

};
//未启用
enum TRecvType
{
	RECV_HEAD_lo = 1, //进入登录选择画面，返回剩余点数
	RECV_HEAD_DRQU,	  //登录成功
	RECV_HEAD_GUZ,	  //登录的城市, 登录点
	RECV_HEAD_iqQs,	  //本人代码
	RECV_HEAD_Xv,	  //人物状态
	RECV_HEAD_ZJ,	  //技能
	RECV_HEAD_adZ,	  //聊天
	RECV_HEAD_kG,	  //名片表
	RECV_HEAD_Bgfl,	  //名片
	RECV_HEAD_xG,	  //接收到的邮件
	RECV_HEAD_rYTy,	  //一个宠物的基本资料
	RECV_HEAD_ik,	  //一个宠物的扩展资料
	RECV_HEAD_LvD,	  //一个宠物的技能资料
	RECV_HEAD_OEJd,	  //战斗资料
	RECV_HEAD_PxU,	  //物品表
	RECV_HEAD_tn,	  //移动物品
	RECV_HEAD_JM,	  //战斗开始
	RECV_HEAD_iVy,	  //队伍资料
	RECV_HEAD_yPJ,	  //战斗结束
	RECV_HEAD_crXf	  //与NPC说话后的选择菜单
};
//脚本状态
enum TScriptCtrl
{
	SCRIPT_CTRL_STOP = 0,  //停止
	SCRIPT_CTRL_RUN = 1,   //执行
	SCRIPT_CTRL_PAUSE = 2, //暂停
};
//工作类型定义
enum TWorkType
{
	TWork_None = 0,					 //无
	TWork_Heal = 1,					 //治疗
	TWork_FirstAid = 2,				 //急救
	TWork_Identify = 101,			 //鉴定
	TWork_TransIdentify = 201,		 //变身鉴定
	TWork_Digging = 102,			 //挖掘
	TWork_TransDigging = 202,		 //变身挖掘
	TWork_Hunt = 103,				 //狩猎
	TWork_TransHunt = 203,			 //高速狩猎
	TWork_Lumber = 104,				 //伐木
	TWork_TransLumber = 204,		 //变身伐木
	TWork_RepairWeapon = 105,		 //修理武器
	TWork_TransRepairWeapon = 205,	 //变身修理武器
	TWork_RepairArmor = 106,		 //修理防具
	TWork_TransRepairArmor = 206,	 //变身修理防具
	TWork_TransHuntExperience = 207, //变身狩猎体验
	TWork_HuntExperience = 107,		 //狩猎体验
	TWork_Compound = 300,			 //合成
	TWork_Transformation = 400,		 //变身
	TWork_Cosplay = 500,			 //变装
};
//遇敌停止判断
enum TConditionType
{
	TCondition_None = 0,				//无
	TCondition_PlayerEatMagic,			//人物吃料理
	TCondition_PlayerEatMedicament,		//人物吃血瓶
	TCondition_PetEatMagic,				//宠物吃料理
	TCondition_PetEatMedicament,		//宠物吃血瓶
	TCondition_StopEncounterPlayerHp,	//人物血少停止遇敌
	TCondition_StopEncounterPlayerMp,	//人物魔少停止遇敌
	TCondition_StopEncounterPetHp,		//宠物血少停止遇敌
	TCondition_StopEncounterPetMp,		//宠物魔少停止遇敌
	TCondition_StopEncounterTeammateHp, //队友血少停止遇敌
	TCondition_StopEncounterTroopCount, //队伍数量停止遇敌
	TCondition_PlayerInjuredWhite,		//人物白伤停止遇敌
	TCondition_PlayerInjuredYellow,		//人物黄伤停止遇敌
	TCondition_PlayerInjuredPurple,		//人物紫伤停止遇敌
	TCondition_PlayerInjuredRed,		//人物红伤停止遇敌
	TCondition_PetCountFull,			//宠物满停止遇敌
	TCondition_ItemListFull,			//物品满停止遇敌
	TCondition_DontHaveMagicFood,		//无料理停止遇敌
	TCondition_DontHaveMedicament,		//无血瓶停止遇敌
	TCondition_BattlePetLoyaltyTooLow,	//出战宠物忠诚低于60停止遇敌
	TCondition_PlayeLossSoul,			//掉魂停止遇敌
	TCondition_EquipProtcet,			//装备保护
};
//系统设置
enum TSysConfigSet
{
	TSysConfigSet_None = 0,
	TSysConfigSet_AutoDrop,			  //自动扔
	TSysConfigSet_AutoPile,			  //自动叠
	TSysConfigSet_AutoPick,			  //自动捡
	TSysConfigSet_AutoSale,			  //自动卖
	TSysConfigSet_AutoSupply,		  //自动加血
	TSysConfigSet_AutoEncounter,	  //自动遇敌
	TSysConfigSet_EncounterType,	  //自动遇敌类型
	TSysConfigSet_EncounterSpeed,	  //自动遇敌速度
	TSysConfigSet_Timer,			  //脚本定时器
	TSysConfigSet_AutoBattle,		  //自动战斗
	TSysConfigSet_HighSpeedBattle,	  //高速战斗
	TSysConfigSet_HighSpeedDelay,	  //高速延时
	TSysConfigSet_AllEncounterEscape, //遇敌全跑
	TSysConfigSet_CharacterSwitch,	  //人物开关
	TSysConfigSet_MoveSpeed,		  //行走速度
	TSysConfigSet_FollowPickItem,	  //跟随捡物
	TSysConfigSet_NoLv1Escape,		  //无1级逃跑
	TSysConfigSet_NoPetDoubleAction,  //不带宠二动
	TSysConfigSet_NoPetAttack,		  //不带宠二动攻击
	TSysConfigSet_NoPetGuard,		  //不带宠二动防御
	TSysConfigSet_PlayerTitle,		  //设置人物称号

};
//命令行参数解析
enum TCmdConfig
{
	TCmdConfig_None = 0,
	TCmdConfig_GameType,
	TCmdConfig_LoginUser,
	TCmdConfig_LoginPwd,
	TCmdConfig_LoginGid,
	TCmdConfig_BigServer,
	TCmdConfig_Server,
	TCmdConfig_Character,
	TCmdConfig_AutoLogin,
	TCmdConfig_SkipUpdate,
	TCmdConfig_AutoChangeServer,
	TCmdConfig_AutoKillGame,
	TCmdConfig_AutoCreateChara,
	TCmdConfig_CreateCharaChara,
	TCmdConfig_CreateCharaEye,
	TCmdConfig_CreateCharaMouth,
	TCmdConfig_CreateCharaColor,
	TCmdConfig_CreateCharaPoints,
	TCmdConfig_CreateCharaElements,
	TCmdConfig_CreateCharaName,
	TCmdConfig_LoadScript,
	TCmdConfig_ScriptAutoRestart,
	TCmdConfig_ScriptFreezeStop,
	TCmdConfig_ScriptAutoTerm,
	TCmdConfig_InjuryProtect,
	TCmdConfig_SoulProtect,
	TCmdConfig_LoadSettings,
	TCmdConfig_KillFreeze,
	TCmdConfig_ConsoleMaxLines,
	TCmdConfig_ChatMaxLines,
	TCmdConfig_LogBackRestart,
	TCmdConfig_TransUserInput,
};
//宠物状态
enum TPetState
{
	TPET_STATE_NONE = 0,
	TPET_STATE_READY = 1,  //待命
	TPET_STATE_BATTLE = 2, //战斗
	TPET_STATE_REST = 3,   //休息
	TPET_STATE_WALK = 16,  //散步
};
//宠物状态
enum TPetAddPointType
{
	TPET_POINT_TYPE_None = -1,
	TPET_POINT_TYPE_Vigor = 0,	   //体力
	TPET_POINT_TYPE_Strength = 1,  //力量
	TPET_POINT_TYPE_Endurance = 2, //强度
	TPET_POINT_TYPE_Agility = 3,   //敏捷
	TPET_POINT_TYPE_Magical = 4,   //魔力
};
//返回人物和宠物信息的一些定义
enum TReturnGameData
{
	TRet_Game_Hp = 1,
	TRet_Game_Mp,
	TRet_Game_MaxHp,
	TRet_Game_MaxMp,
	TRet_Game_Level,
	TRet_Game_Exp,
	TRet_Game_MaxExp,
	TRet_Game_Health,
	TRet_Game_Soul,
	TRet_Game_Name,
	TRet_Game_Gold,
	TRet_Game_Punchclock,
	TRet_Game_PunchclockState,
	TRet_Game_Job,
	TRet_Game_Profession,
	TRet_Game_Prestige,
	TRet_Game_Loc,
	TRet_Game_PetCount,
	TRet_Game_CurLine,		 //当前游戏线路
	TRet_Game_CurServelLine, //当前游戏大线路
	TRet_Game_Loyality,
	TRet_Game_PetGrade,
	TRet_Game_PetState,
	TRet_Game_PetChangeState,
	TRet_Game_PetChangeName,
};
//自定义快捷键
enum TDefQuickKey
{
	TDef_Quick_Key_Logback = 1,
	TDef_Quick_Key_Logout,
	TDef_Quick_Key_Cross,
	TDef_Quick_Key_Trade,
	TDef_Quick_Key_TradeNoAccept,
	TDef_Quick_Key_SaveAll,
	TDef_Quick_Key_FetchAll,
	TDef_Quick_Key_CallFz,
	TDef_Quick_Key_Encounter,
};
enum TScriptUIData
{
	TScriptUI_TeamLeaderName = 1, //队长名称
	TScriptUI_TeammateNameList,	  //队员列表
	TScriptUI_PlayerHp,			  //人物血
	TScriptUI_PlayerMp,			  //人物魔
	TScriptUI_PetHp,			  //宠血
	TScriptUI_PetMp,			  //宠魔
	TScriptUI_TroopCount,		  //队伍人数
	TScriptUI_UsetCustom1,		  //自定义1
	TScriptUI_UsetCustom2,		  //自定义2
	TScriptUI_UsetCustom3,		  //自定义3

};
#define GETDEVSUBCLASS(n) (n & 0xfffffff0)
#define GETDEVCLASS(n) (n & 0xffffff00)
#define GETDEVSUPPERCLASS(n) (n & 0xfffff000)
