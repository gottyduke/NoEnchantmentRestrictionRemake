#include "Settings.h"


bool Settings::LoadSettings(const bool a_dumpParse)
{
	auto [log, success] = J2S::load_settings(FILE_NAME, a_dumpParse);

	if (!log.empty()) {
		_ERROR("%s", log.c_str());
	}
	
	return success;
}


decltype(Settings::disenchantEverything)	Settings::disenchantEverything("disenchantEverything", true);
decltype(Settings::unlimitedEnchantment)	Settings::unlimitedEnchantment("unlimitedEnchantment", false);
decltype(Settings::enchantsSlotOverride)	Settings::enchantsSlotOverride("enchantsSlotOverride", 0);