#pragma once


#include "DKUtil/Config.hpp"


namespace Config
{
	using namespace DKUtil::Alias;

	extern Boolean EnableDE;
	extern Boolean EnableUE;
	extern Integer UEMax;


	void Load();
}