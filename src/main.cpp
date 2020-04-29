#include "FormManipulator.h"
#include "Hooks.h"
#include "Settings.h"
#include "Validator.h"
#include "version.h"

#include "SKSE/API.h"


namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			FormManipulator::TryReplaceForm();
		}
	}
}


extern "C"
{
bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\NoEnchantmentRestrictionRemake.log");
	SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
	SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
	SKSE::Logger::UseLogStamp(true);
	SKSE::Logger::TrackTrampolineStats(true);

	_MESSAGE("NoEnchantmentRestrictionRemake v%s", NERR_VERSION_VERSTRING);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "NoEnchantmentRestrictionRemake";
	a_info->version = NERR_VERSION_MAJOR;

	if (a_skse->IsEditor()) {
		_FATALERROR("Loaded in editor, marking as incompatible!\n");
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver <= SKSE::RUNTIME_1_5_39) {
		_FATALERROR("Unsupported runtime version %s!", ver.GetString().c_str());
		return false;
	}

	return true;
}


bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	_MESSAGE("NoEnchantmentRestrictionRemake loaded");

	if (!Init(a_skse)) {
		return false;
	}

	if (!Settings::LoadSettings()) {
		_FATALERROR("Failed to load settings");
		return false;
	}
	
	const auto messaging = SKSE::GetMessagingInterface();
	if (messaging->RegisterListener("SKSE", MessageHandler)) {
		_MESSAGE("Messaging interface registration successful");
	} else {
		_FATALERROR("Messaging interface registration failed!\n");
		return false;
	}

	if (!SKSE::AllocTrampoline(1 << 5)) {
		_FATALERROR("Failed to allocate trampoline");
		return false;
	}
	
	Hooks::InstallHooks();
	
	_MESSAGE("Finished");
	return true;
}
}