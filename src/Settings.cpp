#include "Settings.h"


bool Settings::LoadSettings(const bool a_dumpParse)
{
	auto [log, success] = J2S::load_settings(FILE_NAME, a_dumpParse);

	if (!log.empty()) {
		_ERROR("%s", log.c_str());
	}
	
	return success;
}


decltype(Settings::disenchantEverything)	Settings::disenchantEverything("DisenchantEverything", true);
decltype(Settings::disenchantNotDestory)	Settings::disenchantNotDestory("DisenchantNotDestroy", true);
decltype(Settings::unlimitedEnchantment)	Settings::unlimitedEnchantment("UnlimitedEnchantment", true);
decltype(Settings::enchantSlotsOverride)	Settings::enchantSlotsOverride("EnchantSlotsOverride", 0);