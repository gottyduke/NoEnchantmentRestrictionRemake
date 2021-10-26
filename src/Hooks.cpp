#include "Hooks.h"


namespace
{
	constexpr std::uint64_t UES_FUNC_ID = 50565;
	constexpr std::uintptr_t UES_OFFSET_START = 0x24;
	constexpr std::uintptr_t UES_OFFSET_END = 0x2A;

	constexpr BRANCH_INSTRUCTION INSTRUCTION = {
		// Last 4 bytes placeholding cuz it's cool
		"\x31\xED\xC7\x86\x98\x01\x00\x00\x00\x00\x00\x00",
		12,
		"\x48\x89\x69\xE0",
		4
	};

	// placeholding bytes | post patch | op code | relative displacement
	constexpr std::uintptr_t REGISTER_OFFSET = sizeof(std::int32_t) + sizeof(std::int32_t) + sizeof(std::uint8_t) + sizeof(std::int32_t);
}


namespace Hooks
{
	void InstallHooks()
	{
		DKUtil::Hook::BranchToID<UES_FUNC_ID, UES_OFFSET_START, UES_OFFSET_END>(DKUTIL_HOOK_NO_FUNCTION, INSTRUCTION);

		unsigned int max = 0x64;
		WRITE(CURRENT_PTR - REGISTER_OFFSET, max);
		
		INFO("Hooks installed"sv);
	}
}
