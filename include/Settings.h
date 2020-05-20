#pragma once

#include "Json2Settings.h"


namespace J2S = Json2Settings;

class Settings
{
public:
	Settings() = delete;

	static bool LoadSettings(bool a_dumpParse = false);

	static J2S::bSetting disenchantEverything;
	//static J2S::bSetting	disenchantNotDestory;
	static J2S::bSetting unlimitedEnchantment;
	static J2S::iSetting enchantSlotsOverride;

private:
	static constexpr auto FILE_NAME = R"(Data\SKSE\Plugins\NoEnchantmentRestrictionRemake.json)";
};
