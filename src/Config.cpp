#include "Config.h"


namespace Config
{
	Boolean EnableDE{ "EnableDisenchantEverything" };
	Boolean EnableUE{ "EnableUnlimitedEnchantment" };
	Integer UEMax{ "EnchantmentMaxSlots" };


	void Load()
	{
		auto Main = COMPILE_PROXY("NoEnchantmentRestrictionRemake.toml"sv);

		Main.Bind(EnableDE, true);
		Main.Bind(EnableUE, true);
		Main.Bind(UEMax, 8);

		Main.Load();

		INFO("Config Loaded"sv);
	}
}