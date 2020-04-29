#include "Hooks.h"
#include "Settings.h"

#include "REL/Relocation.h"
#include "SKSE/CodeGenerator.h"
#include "SKSE/RegistrationSet.h"
#include "SKSE/SafeWrite.h"
#include "SKSE/Trampoline.h"


namespace
{
	constexpr auto NOP = 0x90;
	constexpr auto JMP = 0xE9;

	constexpr auto FUNC_ID = 50565;
	constexpr auto START = 0x24;
	constexpr auto END = 0x2A;
}


namespace Hooks
{
	void InstallHooks()
	{
		auto success = true;

		const auto codeSize = END - START;
		const auto injectAddr = REL::ID(FUNC_ID).GetAddress() + START;
		const auto continueAddr = injectAddr + codeSize;

		const auto allocationSize = 12 + 4 + 5;
		const auto trampoline = SKSE::GetTrampoline()->Allocate(allocationSize);
		auto trampolinePtr = reinterpret_cast<std::uintptr_t>(trampoline);

		for (auto i = 0; i < codeSize; ++i) {
			success &=
				SKSE::SafeWrite8(injectAddr + i, NOP);
		}

		success &=
			SKSE::SafeWrite8(injectAddr, JMP) &&
			SKSE::SafeWrite32(injectAddr + 1, trampolinePtr - injectAddr - 5);

		success &=
			SKSE::SafeWriteBuf(trampolinePtr, "\x31\xED\xC7\x86\x98\x01\x00\x00", 8);
		trampolinePtr += 8;

		success &=
			SKSE::SafeWrite32(trampolinePtr, *Settings::enchantSlotsOverride ? *Settings::enchantSlotsOverride : 0x64);
		trampolinePtr += 4;
		
		success &=
			SKSE::SafeWriteBuf(trampolinePtr, "\x48\x89\x69\xE0", 4);
		trampolinePtr += 4;

		success &=
			SKSE::SafeWrite8(trampolinePtr, JMP) &&
			SKSE::SafeWrite32(trampolinePtr + 1, continueAddr - trampolinePtr - 5);
		trampolinePtr += 5;

		if (success) {
			_MESSAGE("Successfully installed hook");
		} else {
			_FATALERROR("Failed to install hook for unlimited enchantment slots");
		}
	}
}