#pragma once

namespace Config
{
	using namespace DKUtil::Alias;

	extern Boolean EnableDE;
	extern Boolean EnableUE;
	extern Integer UEMax;
	extern String Exclusion;

	void Load();
}