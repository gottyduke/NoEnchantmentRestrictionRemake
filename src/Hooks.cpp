#include "Hooks.h"
#include "Config.h"

namespace Hooks
{
	// 1-6-323: 0x894EE0 + 0x212
	constexpr std::uint64_t AE_FuncID = 51242;
	constexpr std::ptrdiff_t AE_OffsetL = 0x212;
	constexpr std::ptrdiff_t AE_OffsetH = 0x243;

	// 1-5-97: 0x866E20 + 0x1CD
	constexpr std::uint64_t SE_FuncID = 50329;
	constexpr std::ptrdiff_t SE_OffsetL = 0x1CD;
	constexpr std::ptrdiff_t SE_OffsetH = 0x1FF;

	constexpr OpCode AsmSrc[]{
		0xB8,                    // mov eax,
		0x00, 0x00, 0x00, 0x00,  // Imm32
	};

	// opcode
	constexpr std::ptrdiff_t ImmediateOffset = sizeof(OpCode);

	HookHandle _Hook_UES;

	void Install()
	{
		_Hook_UES = DKUtil::Hook::AddASMPatch(
			DKUtil::Hook::IDToAbs(AE_FuncID, SE_FuncID),
			DKUtil::Hook::RuntimeOffset(AE_OffsetL, AE_OffsetH, SE_OffsetL, SE_OffsetH),
			{ &AsmSrc, std::extent_v<decltype(AsmSrc)> });

		_Hook_UES->Enable();

		DKUtil::Hook::WriteImm(_Hook_UES->TramPtr + ImmediateOffset, static_cast<Imm32>(*Config::UEMax));

		INFO("Hooks installed"sv);
	}

	void Uninstall()
	{
		_Hook_UES->Disable();

		INFO("Hooks uninstalled"sv);
	}
}
