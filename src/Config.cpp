#include "Config.h"


namespace Config
{
	Boolean EnableDE{ "EnableDisenchantEverything" };
	Boolean EnableUE{ "EnableUnlimitedEnchantment" };
	Integer UEMax{ "EnchantmentMaxSlots" };


	void Load()
	{
		auto mainConfig = COMPILE_PROXY("NoEnchantmentRestrictionRemake.toml"sv);

		mainConfig.Bind(EnableDE, true);
		mainConfig.Bind(EnableUE, true);
		mainConfig.Bind(UEMax, 3);

		mainConfig.Load();

		INFO("Config Loaded"sv);
	}
}