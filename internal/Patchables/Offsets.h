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
		TargetID = 0x74
	};

	enum class Spell {
		kInvalid = 0x4
	};
	enum class Functions {
		GetPing = 0x322500,								// 11.18
		OnProcessSpell = 0x4FC190,						// 11.18
		OnDeleteObject = 0x278DE0,
		WorldToScreen = 0x972BA0,						// 11.18
		GetFirstObject = 0x27A880,						// 11.18
		GetNextObject = 0x27B280,						// 11.18
		IsAlive = 0x13EDF0,								// 11.18 vtable[127]
		IsHero = 0x1A94C0,								// 11.18
		IsMissile = 0x1A9520,							// 11.18
		IsMinion = 0x1a9500,							// 11.17
		IsInhib = 0x1a6f00,								// 11.17
		IsBaron = 0x165B20,								// 11.17
		IsNexus = 0x1a9440,								// 11.17
		IsTurret = 0x1a96b0,							// 11.17
		GetAttackDelay = 0x2781A0,						// 11.18
		GetAttackCastDelay = 0x2780a0,					// 11.18
		IssueClick = 0x5F5B40,							// 11.18
		IsNotWall = 0x8F7CA0,							// 11.18
		GetHeightForPosition = 0x8f3180,				// 11.18
		GetBasicAttack = 0x1209c0,						// 11.18
	};
	enum class Data {
		NetClient = 0x30DA3EC,							// 11.18
		LocalPlayer = 0x30D2FD4,						// 11.18
		GameTime = 0x30CB3AC,							// 11.18
		ObjectManager = 0x18363C0,						// 11.18
		ManagerTemplate_Minions = 0x1836454,			// 11.18
		ManagerTemplate_AttackableUnits = 0x30C5994,	// 11.18
		ManagerTemplate_Turrets = 0x30C5990,			// 11.18
		ManagerTemplate_Shops = 0x30C67C8,				// 11.18
		ManagerTemplate_Inhibitors = 0x30D3714,			// 11.18
		MangerTemplate_Buildings = 0x1833C4C,			// 11.18
		ManagerTemplate_Heros = 0x18343AC,				// 11.18
		HudInstance = 0x18363F8,						// 11.18
		D3DRender = 0x30FA8D4							// 11.18
	};
	enum class GameObject {
		Index = 0x20,									// 11.18
		TeamID = 0x4c,									// 11.18
		Position = 0x23c,								// 11.18
		SpellBook = 0x2370,
		mPAR = 0x2b4,
		mMaxPAR = 0x2c4,
		mPAREnabled = 0x2d4,
		mPARState = 0x304,
		mSAR = 0x324,
		mMaxSAR = 0x334,
		mSAREnabled = 0x344,
		mSARState = 0x374,
		mLifetime = 0xc88,
		mLifetimeTicks = 0xca8,
		mIsTargetable = 0xD1C,
		mIsInvulnearable = 0x3EC,
		HP = 0xdb4,
		MaxHP = HP + 0x10,
		Mana = 0x2B4,
		MaxMana = Mana + 0x10,
		PercentCooldownMod = 0x16c8,
		AbilityHasteMod = 0x16d8,
		PercentCooldownCapMod = 0x16e8,
		PassiveCooldownEndTime = 0x16f8,
		PassiveCooldownTotalTime = 0x1708,
		FlatPhysicalDamageMod = 0x1648,
		PercentPhysicalDamageMod = 0x1758,
		PercentBonusPhysicalDamageMod = 0x1768,
		PercentBasePhysicalDamageAsFlatBonusMod = 0x1778,
		FlatMagicDamageMod = 0x1788,
		PercentMagicDamageMod = 0x16c7,
		FlatMagicReduction = 0x17a8,
		PercentMagicReduction = 0x17b8,
		FlatCastRangeMod = 0x17c8,
		AttackSpeedMod = 0x17d8,
		PercentAttackSpeedMod = 0x17e8,
		PercentMultiplicativeAttackSpeedMod = 0x17f8,
		BaseAttackDamage = 0x1808,
		BaseAttackDamageSansPercentScale = 0x1818,
		FlatBaseAttackDamageMod = 0x1828,
		PercentBaseAttackDamageMod = 0x1838,
		BaseAbilityDamage = 0x1848,
		CritDamageMultiplier = 0x1858,
		ScaleSkinCoef = 0x1868,
		Dodge = 0x16c7,
		Crit = 0x1888,
		FlatBaseHPPoolMod = 0x1898,
		Armor = 0x18a8,
		BonusArmor = 0x18b8,
		SpellBlock = 0x18c8,
		BonusSpellBlock = 0x18d8,
		HPRegenRate = 0x18e8,
		BaseHPRegenRate = 0x18f8,
		MoveSpeed = 0x1908,
		AttackRange = 0x1928,
		FlatBubbleRadiusMod = 0x1938,
		PercentBubbleRadiusMod = 0x1948,
		FlatArmorPenetration = 0x1958,
		PhysicalLethality = 0x1968,
		PercentArmorPenetration = 0x1978,
		PercentBonusArmorPenetration = 0x1988,
		PercentCritBonusArmorPenetration = 0x1998,
		PercentCritTotalArmorPenetration = 0x19a8,
		FlatMagicPenetration = 0x19b8,
		MagicLethality = 0x19c8,
		PercentMagicPenetration = 0x19d8,
		PercentBonusMagicPenetration = 0x19e8,
		PercentLifeStealMod = 0x19f8,
		PercentSpellVampMod = 0x1a08,
		PercentOmnivampMod = 0x1a18,
		PercentPhysicalVamp = 0x1a28,
		PathfindingRadiusMod = 0x1a38,
		PercentCCReduction = 0x1a48,
		PercentEXPBonus = 0x1a58,
		PARRegenRate = 0x1a68,
		PrimaryARBaseRegenRateRep = 0x1a78,
		SecondaryARRegenRateRep = 0x1a88,
		SecondaryARBaseRegenRateRep = 0x1a98,
		Gold = 0x1b98,
		GoldTotal = 0x1ba8,
		MinimumGold = 0x0,
		CombatType = 0x20ec,
		Exp = 0x3364,
		mLevelRef = 0x3374,
		mNumNeutralMinionsKilled = 0x3a3c,
		InputLocks = 0x3a94,
		ShutdownValue = 0x3a1c,
		BaseGoldGivenOnDeath = 0x3a2c,

		BonusAttackDamage = 0x1234,
		ChampionName = 0x2BD4,

		MissileSpellInfo = 0x258,
		MissileSrcIndex = 0x2BC,
		MissileStartPos = 0x2D4,
		MissileEndPos = 0x2E0,
		MissileDestIndex = 0x314,
		BaseCharacterData = 0x2B44,
	};

	enum class BuffManager
	{
		Instance = 0x21BC,
		ArrayStart = 0x10,
		ArrayEnd = 0x14,
		StartTime = 0xC,
		BuffScript = 0x8,
		EndTime = 0x10,
		BuffType = 0x4,
		BuffDuration = 0x14,
		BuffCount = 0x74,
		BuffCountAlt = 0x20,
		BuffCountAlt2 = 0x24,
		BuffName = 0x8,
		BuffHash = 0x88,  
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