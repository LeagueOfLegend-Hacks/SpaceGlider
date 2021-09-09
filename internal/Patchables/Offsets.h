#pragma once
namespace Offsets {
	enum class SpellInfo {
		Slot = 0x4,
		StartTime = 0x8,
		SpellIndex = 0xC,
		Level = 0x58,
		source_id = 0x64,
		SourceNetworkID = 0x6C,
		StartPosition = 0x7C,
		EndPosition = 0x88,
		HasTarget = 0xB4,
		TargetID = 0xB8
	};

	enum class Spell {
		kInvalid = 0x4
	};
	enum class Functions {
		GetPing = 0x322500,								// 11.18		55 8B EC 83 EC 08 0F B6
		OnProcessSpell = 0x51D350,						// 11.18		E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 80 BE ?? ?? ?? ?? ?? D8
		WorldToScreen = 0x972BA0,						// 11.18		83 EC 10 56 E8 ? ? ? ? 8B 08
		GetFirstObject = 0x27A880,						// 11.18		8B 44 24 04 56 8B 71 18
		GetNextObject = 0x27B280,						// 11.18		E8 ? ? ? ? 8B F0 85 F6 74 21 0F 1F 44 00 ?
		IsAlive = 0x13EDF0,								// 11.18		56 8B F1 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 19
		IsHero = 0x1A94C0,								// 11.18		E8 ? ? ? ? 83 C4 04 84 C0 74 52
		IsMissile = 0x1A9520,							// 11.18		E8 ? ? ? ? 83 C4 04 84 C0 74 3F
		IsMinion = 0x1a70c0,							// 11.17		E8 ? ? ? ? 83 C4 04 80 7F 26 06
		IsInhib = 0x1a6f00,								// 11.17		E8 ? ? ? ? 55 88 44 24 1F
		IsBaron = 0x165B20,								// 11.17		56 81 C1 ? ? ? ? E8 ? ? ? ? 68
		IsNexus = 0x1a7000,								// 11.17		E8 ? ? ? ? 55 88 44 24 20
		IsTurret = 0x1a6ec0,							// 11.17		E8 ? ? ? ? 83 C4 04 84 C0 75 ?
		GetAttackDelay = 0x2781A0,						// 11.18		8B 44 24 04 51 F3
		GetAttackCastDelay = 0x2780a0,					// 11.18		83 EC 0C 53 8B 5C 24 14 8B CB 56
		IssueClick = 0x5F5B40,							// 11.18		E8 ? ? ? ? 84 C0 74 12 5F
	};
	enum class Data {
		NetClient = 0x30dab58,							// 11.18		8B 0D ? ? ? ? 85 C9 74 07 8B 01 6A 01 FF 50 08 8B
		LocalPlayer = 0x30D2FD4,						// 11.18		A1 ?? ?? ?? ?? 85 C0 74 07 05 ?? ?? ?? ?? EB 02 33 C0 56
		GameTime = 0x30CB3AC,							// 11.18		F3 0F 11 05 ? ? ? ? 8B 49
		ObjectManager = 0x18363C0,						// 11.18		8B 0D ? ? ? ? 89 74 24 14
		ManagerTemplate_Minions = 0x30CD588,			// 11.17		A1 ?? ?? ?? ?? 53 55 8B 6C 24 1C
		ManagerTemplate_AttackableUnits = 0x30CD584,	// 11.17
		ManagerTemplate_Turrets = 0x30D1AF8,			// 11.17		A1 ? ? ? ? 53 56 8B 70 04
		ManagerTemplate_Shops = 0x30CE500,				// 11.17
		ManagerTemplate_Inhibitors = 0x30DB7F8,			// 11.17
		MangerTemplate_Buildings = 0x183B9DC,			// 11.17
		ManagerTemplate_Heros = 0x1833D88,				// 11.18		8B 0D ?? ?? ?? ?? 50 8D 44 24 18
		HudInstance = 0x18363F8,						// 11.18		8B 0D ? ? ? ? 6A 00 8B 49 34 E8 ? ? ? ? B0 01 C2
		D3DRender = 0x30FA8D4							// 11.18		8B 15 ? ? ? ? 83 EC 08
	};
	enum class GameObject {
		Index = 0x20,
		TeamID = 0x4c,
		Position = 0x220,
		mPAR = 0x298,
		mMaxPAR = 0x2a8,
		mPAREnabled = 0x2b8,
		mPARState = 0x2e8,
		mSAR = 0x308,
		mMaxSAR = 0x318,
		mSAREnabled = 0x328,
		LargePipBitField = 0x338,
		MediumPipBitField = 0x348,
		mSARState = 0x358,
		mLifetime = 0xc6c,
		mLifetimeTicks = 0xc8c,
		PhysicalDamagePercentageModifier = 0xCE0,
		mIsTargetable = 0xD00,
		HP = 0xd98,
		MaxHP = 0xDA8,
		HPMaxPenalty = 0xDB8,
		AllShield = 0xDD8,
		PhysicalShield = 0xDE8,
		MagicalShield = 0xDF8,
		ChampSpecificHealth = 0xE08,
		IncomingHealingAllied = 0xE18,
		IncomingHealingEnemy = 0xE28,
		StopShieldFade = 0xE38,
		ActionState = 0x106C,
		ActionState2 = 0x107C,
		PercentCooldownMod = 0x16b0,
		AbilityHasteMod = 0x16c0,
		PercentCooldownCapMod = 0x16d0,
		PassiveCooldownEndTime = 0x16e0,
		PassiveCooldownTotalTime = 0x16f0,
		FlatPhysicalDamageMod = 0x1630,
		PercentPhysicalDamageMod = 0x1740,
		PercentBonusPhysicalDamageMod = 0x1750,
		PercentBasePhysicalDamageAsFlatBonusMod = 0x1760,
		FlatMagicDamageMod = 0x1770,
		PercentMagicDamageMod = 0x16af,
		FlatMagicReduction = 0x1790,
		PercentMagicReduction = 0x17a0,
		FlatCastRangeMod = 0x17b0,
		AttackSpeedMod = 0x17c0,
		PercentAttackSpeedMod = 0x17d0,
		PercentMultiplicativeAttackSpeedMod = 0x17e0,
		BaseAttackDamage = 0x17f0,
		BonusAttackDamage = 0x121C,
		ChampionName = 0x2BB4,
		BaseAttackDamageSansPercentScale = 0x1800,
		FlatBaseAttackDamageMod = 0x1810,
		PercentBaseAttackDamageMod = 0x1820,
		BaseAbilityDamage = 0x1830,
		CritDamageMultiplier = 0x1840,
		ScaleSkinCoef = 0x1850,
		Dodge = 0x16af,
		Crit = 0x1870,
		FlatBaseHPPoolMod = 0x1880,
		Armor = 0x1890,
		BonusArmor = 0x18a0,
		SpellBlock = 0x18b0,
		BonusSpellBlock = 0x18c0,
		HPRegenRate = 0x18d0,
		BaseHPRegenRate = 0x18e0,
		MoveSpeed = 0x18f0,
		AttackRange = 0x1910,
		FlatBubbleRadiusMod = 0x1920,
		PercentBubbleRadiusMod = 0x1930,
		FlatArmorPenetration = 0x1940,
		PhysicalLethality = 0x1950,
		PercentArmorPenetration = 0x1960,
		PercentBonusArmorPenetration = 0x1970,
		PercentCritBonusArmorPenetration = 0x1980,
		PercentCritTotalArmorPenetration = 0x1990,
		FlatMagicPenetration = 0x19a0,
		MagicLethality = 0x19b0,
		PercentMagicPenetration = 0x19c0,
		PercentBonusMagicPenetration = 0x19d0,
		PercentLifeStealMod = 0x19e0,
		PercentSpellVampMod = 0x19f0,
		PercentOmnivampMod = 0x1a00,
		PercentPhysicalVamp = 0x1a10,
		PathfindingRadiusMod = 0x1a20,
		PercentCCReduction = 0x1a30,
		PercentEXPBonus = 0x1a40,
		PARRegenRate = 0x1a50,
		PrimaryARBaseRegenRateRep = 0x1a60,
		SecondaryARRegenRateRep = 0x1a70,
		SecondaryARBaseRegenRateRep = 0x1a80,
		Gold = 0x1b80,
		GoldTotal = 0x1b90,
		MinimumGold = 0x0,
		CombatType = 0x20d4,
		SpellBook = 0x2350,
		Exp = 0x3344,
		mLevelRef = 0x3354,
		SkillUpLevelDeltaReplicate = 0x3364,
		VisionScore = 0x39EC,
		mNumNeutralMinionsKilled = 0x3a1c,
		InputLocks = 0x3a74,
		ShutdownValue = 0x39fc,
		BaseGoldGivenOnDeath = 0x3a0c,

		MissileSpellInfo = 0x258,
		MissileSrcIndex = 0x2BC,
		MissileStartPos = 0x2D4,
		MissileEndPos = 0x2E0,
		MissileDestIndex = 0x314,
		BaseCharacterData = 0x2B24,
	};

	enum class AIManager
	{
		NavStart = 0x1cc,
		NavEnd = 0x224,
		ServerPos = 0x2e4,
		Velocity = 0x2f0,
		Moving = 0x1c0,
		CurrentSegment = 0x1c4,
		Dashing = 0x214,
		DashingSpeed = 0x1f8,
		NavArray = 0x1e4,
		NavArrayEnd = 0x1e8
	};
	
	enum class CharacterData {
		SkinHash = 0x10,
	};
}
//yer
