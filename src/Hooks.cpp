#include "Hooks.h"
#include "Settings.h"

#include "DKUtil/Hook.h"

#include "RE/Skyrim.h"


namespace
{
	constexpr std::uint64_t UES_FUNC_ID = 50565;
	constexpr std::uintptr_t UES_OFFSET_START = 0x24;
	constexpr std::uintptr_t UES_OFFSET_END = 0x2A;

	constexpr BranchInstruction INSTRUCTION = {
		0,
		"\x31\xED\xC7\x86\x98\x01\x00\x00\x00\x00\x00\x00",
		12,
		"\x48\x89\x69\xE0",
		4
	};
}


namespace Hooks
{
	bool InstallHooks()
	{
		auto success = true;
		
		if (*Settings::unlimitedEnchantment) {
			success &= DKUtil::Hook::BranchToFunction<UES_FUNC_ID, UES_OFFSET_START, UES_OFFSET_END>(INSTRUCTION);

			success &= 
				WRITE_32(DKUtil::Hook::GetCurrentPtr() - 4 - 4 - 5, *Settings::enchantSlotsOverride ? *Settings::enchantSlotsOverride : 0x64);
		}

		return success;
	}
}
