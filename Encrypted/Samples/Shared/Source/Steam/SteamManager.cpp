// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"

#ifdef EOS_STEAM_ENABLED
#include "DebugLog.h"
#include "StringUtils.h"
#include "Game.h"
#include "GameEvent.h"
#include "Authentication.h"
#include "Users.h"
#include "SteamManager.h"
#include "CommandLine.h"

#include "steam/steam_api.h"

// Steam Manager Implementation
class FSteamManager::FImpl
{
public:
	FImpl()
	{
		
	}

	~FImpl()
	{
		
	}

	void Init();
	void Update();
	void RetrieveEncryptedAppTicket();
	void OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t* pEncryptedAppTicketResponse, bool bIOFailure);
	void StartLogin();

private:
	CCallResult<FSteamManager::FImpl, EncryptedAppTicketResponse_t> SteamCallResultEncryptedAppTicket;
	std::wstring EncryptedSteamAppTicket;
	bool bIsInitialized = false;
};

void FSteamManager::FImpl::Init()
{
	if (SteamAPI_Init())
	{
		FDebugLog::Log(L"Steam - Initialized");

		bIsInitialized = true;

		RetrieveEncryptedAppTicket();
	}
	else
	{
		FDebugLog::LogError(L"Steam must be running to play this game (SteamAPI_Init() failed)");
	}
}

void FSteamManager::FImpl::Update()
{
	if (!bIsInitialized)
	{
		return;
	}

	// Run Steam client callbacks
	SteamAPI_RunCallbacks();
}

void FSteamManager::FImpl::RetrieveEncryptedAppTicket()
{
	if (!bIsInitialized)
	{
		return;
	}

	if (FCommandLine::Get().HasParam(CommandLineConstants::SteamAppTicket))
	{
		// Attempt to use app ticket string from command line
		EncryptedSteamAppTicket = FCommandLine::Get().GetParamValue(CommandLineConstants::SteamAppTicket);
	}

	if (!EncryptedSteamAppTicket.empty())
	{
		StartLogin();
	}
	else
	{
		FDebugLog::Log(L"Steam - Requesting Encrypted App Ticket ...");
		
		SteamAPICall_t SteamAPICallHandle = SteamUser()->RequestEncryptedAppTicket(nullptr, 0);

		SteamCallResultEncryptedAppTicket.Set(SteamAPICallHandle, this, &FSteamManager::FImpl::OnRequestEncryptedAppTicket);
	}
}

void FSteamManager::FImpl::OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t* EncryptedAppTicketResponse, bool bIOFailure)
{
	FDebugLog::Log(L"Steam - OnRequestEncryptedAppTicket Callback");

	if (bIOFailure)
	{
		FDebugLog::LogError(L"Steam - OnRequestEncryptedAppTicket Callback - Failure Requesting Ticket");
		return;
	}

	if (EncryptedAppTicketResponse->m_eResult == k_EResultOK)
	{
		// Get ticket size
		uint32 TicketSize = 0;
		SteamUser()->GetEncryptedAppTicket(nullptr, 0, &TicketSize);

		// Get encrypted app ticket
		uint32 BufSize = TicketSize;
		uint8* SteamAppTicket = new uint8[BufSize];
		if (!SteamUser()->GetEncryptedAppTicket(SteamAppTicket, BufSize, &TicketSize))
		{
			FDebugLog::LogError(L"Steam App Ticket not available");
			delete[] SteamAppTicket;
			return;
		}

		uint32 StringBufSize = (TicketSize * 2) + 1;
		char* SteamAppTicketString = new char[StringBufSize];
		uint32_t OutLen = StringBufSize;
		EOS_EResult ConvResult = EOS_ByteArray_ToString(SteamAppTicket, BufSize, SteamAppTicketString, &OutLen);
		if (ConvResult != EOS_EResult::EOS_Success)
		{
			delete[] SteamAppTicket;
			delete[] SteamAppTicketString;
			FDebugLog::LogError(L"Steam - OnRequestEncryptedAppTicket Callback - EOS_ByteArray_ToString Failed - Result: %ls", FStringUtils::Widen(EOS_EResult_ToString(ConvResult)).c_str());
			return;
		}

		std::string NarrowSteamAppTicketString = SteamAppTicketString;
		EncryptedSteamAppTicket = FStringUtils::Widen(NarrowSteamAppTicketString);

		delete[] SteamAppTicket;
		delete[] SteamAppTicketString;

		StartLogin();
	}
	else if (EncryptedAppTicketResponse->m_eResult == k_EResultLimitExceeded)
	{
		FDebugLog::LogError(L"Steam - OnRequestEncryptedAppTicket Callback - Calling RequestEncryptedAppTicket more than once per minute returns this error");
	}
	else if (EncryptedAppTicketResponse->m_eResult == k_EResultDuplicateRequest)
	{
		FDebugLog::LogError(L"Steam - OnRequestEncryptedAppTicket Callback - Calling RequestEncryptedAppTicket while there is already a pending request results in this error");
	}
	else if (EncryptedAppTicketResponse->m_eResult == k_EResultNoConnection)
	{
		FDebugLog::LogError(L"Steam - OnRequestEncryptedAppTicket Callback - Calling RequestEncryptedAppTicket while not connected to steam results in this error");
	}
}

void FSteamManager::FImpl::StartLogin()
{
	if (!EncryptedSteamAppTicket.empty())
	{
		FDebugLog::Log(L"Steam - StartLogin - Ticket: %ls", EncryptedSteamAppTicket.c_str());

		FGameEvent Event(EGameEventType::StartUserLogin, EncryptedSteamAppTicket, (int)ELoginMode::ExternalAuth, (int)ELoginExternalType::Steam);
		FGame::Get().OnGameEvent(Event);
	}
	else
	{
		FDebugLog::LogError(L"Steam - StartLogin - Invalid Steam App Ticket");
	}
}

std::unique_ptr<FSteamManager> FSteamManager::Instance;

FSteamManager::FSteamManager()
	: Impl(new FImpl())
{

}

FSteamManager::~FSteamManager()
{

}

FSteamManager& FSteamManager::GetInstance()
{
	if (!Instance)
	{
		Instance = std::unique_ptr<FSteamManager>(new FSteamManager());
	}

	return *Instance;
}

void FSteamManager::ClearInstance()
{
	Instance.reset();
}

void FSteamManager::Init()
{
	Impl->Init();
}

void FSteamManager::Update()
{
	Impl->Update();
}

void FSteamManager::RetrieveEncryptedAppTicket()
{
	Impl->RetrieveEncryptedAppTicket();
}

void FSteamManager::StartLogin()
{
	Impl->StartLogin();
}

void FSteamManager::OnGameEvent(const FGameEvent& Event)
{
	if (Event.GetType() == EGameEventType::UserInfoRetrieved)
	{
		FEpicAccountId UserId = Event.GetUserId();

		// Log Steam Display Name
		std::wstring DisplayName = FGame::Get().GetUsers()->GetExternalAccountDisplayName(UserId, UserId, EOS_EExternalAccountType::EOS_EAT_STEAM);
		if (!DisplayName.empty())
		{
			FDebugLog::Log(L"[EOS SDK] External Account Display Name: %ls", DisplayName.c_str());
		}
		else
		{
			FDebugLog::LogError(L"[EOS SDK] External Account Display Name Not Found");
		}
	}
}
#endif //EOS_STEAM_ENABLED