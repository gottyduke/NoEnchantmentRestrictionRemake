#pragma once

#include "Json2Settings.h"


namespace J2S = Json2Settings;

class Settings
{
public:
	Settings() = delete;

	static bool LoadSettings(bool a_dumpParse = false);
	
	static J2S::bSetting	disenchantEverything;
	static J2S::bSetting	unlimitedEnchantment;
	static J2S::iSetting	enchantsSlotOverride;
	

private:
	static constexpr char* FILE_NAME = "Data\\SKSE\\Plugins\\NoEnchantmentRestrictionsSSE.json";
};