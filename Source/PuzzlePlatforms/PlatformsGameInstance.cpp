// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformsGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "UI/MenuWidget.h"
#include "UI/MainMenu.h"

const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");
const static bool USING_LAN = false;

UPlatformsGameInstance::UPlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
	
}

void UPlatformsGameInstance::Init()
{
	Super::Init();

	SubSystem = IOnlineSubsystem::Get();
	if (SubSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("subsystem found"))
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPlatformsGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPlatformsGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPlatformsGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPlatformsGameInstance::OnJoinSessionComplete);
		}
	}

	if (GEngine)
	{
		GEngine->NetworkFailureEvent.AddUObject(this, &UPlatformsGameInstance::OnNetworkFailure);
	}

	Login();
}

void UPlatformsGameInstance::Shutdown()
{
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		DestroySession();
	}
}

void UPlatformsGameInstance::Host(FString InServerName)
{
	ServerName = InServerName;

	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		DestroySession();
	}
	else
	{
		CreateSession();
	}
	
}

void UPlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (!bSuccess)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("Hosting"));
	}

	//ServerTravelTo(UGameplayStatics::GetCurrentLevelName(this) + TEXT("?listen"));
	ServerTravelTo(TEXT("Lobby?listen"));
}

void UPlatformsGameInstance::DestroySession()
{
	if (!bIsLoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not logged in, cannot destroy session"))
		return;
	}

	if (SessionInterface.IsValid())
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
}

void UPlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("Session destroyed"))
}

void UPlatformsGameInstance::CreateSession()
{
	if (!bIsLoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not logged in, cannot create session"))
		return;
	}

	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = USING_LAN;
		SessionSettings.bIsDedicated = false;
		SessionSettings.NumPublicConnections = 5;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(SEARCH_KEYWORDS, FString("Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);
		SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
	}
}

void UPlatformsGameInstance::OnFindSessionsComplete(bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("Finished searching for sessions"))
	if (bSuccess && SessionSearch.IsValid() && MainMenuWidget)
	{
		TArray<FServerData> ServerData;
		if (SessionSearch->SearchResults.Num() > 0)
		{
			for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
			{
				UE_LOG(LogTemp, Warning, TEXT("Found Session ID %s"), *Result.Session.GetSessionIdStr())
				FServerData Data;
				Data.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
				Data.CurrentPlayers = Data.MaxPlayers - Result.Session.NumOpenPublicConnections;
				Data.HostUserName = Result.Session.OwningUserName;
				FString Name;
				if (!Result.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, Name))
				{
					Name = Result.GetSessionIdStr();
				}
				Data.Name = Name;
				ServerData.Add(Data);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Sessions found"))
		}
		MainMenuWidget->SetServerList(ServerData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Searching"))
	}
}

void UPlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error getting connect string"))
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void UPlatformsGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureTpe, const FString& ErrorString)
{
	LoadMainMenuLevel();
}

void UPlatformsGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{

	UE_LOG(LogTemp, Warning, TEXT("Logged in %d"), bWasSuccessful)
	bIsLoggedIn = bWasSuccessful;

	if (SubSystem)
	{
		IOnlineIdentityPtr OnlineIdentity = SubSystem->GetIdentityInterface();
		if (OnlineIdentity.IsValid())
		{
			OnlineIdentity->ClearOnLoginCompleteDelegates(0, this);
		}
	}
}

void UPlatformsGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (!SessionSearch.IsValid())
	{
		return;
	}

	SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[Index]);
}

void UPlatformsGameInstance::LoadMainMenuLevel()
{
	ClientTravelTo(TEXT("MainMenu"));
}

void UPlatformsGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Searching for sessions"))
		SessionSearch->bIsLanQuery = USING_LAN;
		SessionSearch->MaxSearchResults = 5000;
		SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FString("Lobby"), EOnlineComparisonOp::Equals);
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
		//SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		//SessionSearch->QuerySettings.Set(SEARCH_EMPTY_SERVERS_ONLY, false, EOnlineComparisonOp::Equals);
	
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UPlatformsGameInstance::ServerTravelTo(const FString& MapName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(TEXT("/Game/Maps/") + MapName);
	}
}

void UPlatformsGameInstance::ClientTravelTo(const FString& MapName)
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(TEXT("/Game/Maps/") + MapName, ETravelType::TRAVEL_Absolute);
	}
}

void UPlatformsGameInstance::LoadMainMenu()
{
	if (!MainMenuWidgetClass)
	{
		return;
	}

	MainMenuWidget = CreateWidget<UMainMenu>(this, MainMenuWidgetClass);
	if (!MainMenuWidget)
	{
		return;
	}

	MainMenuWidget->Setup();
	MainMenuWidget->SetMenuInterface(this);
}

void UPlatformsGameInstance::InGameLoadMenu()
{
	if (!InGameMenuWidgetClass)
	{
		return;
	}

	InGameMenuWidget = CreateWidget<UMenuWidget>(this, InGameMenuWidgetClass);
	if (!InGameMenuWidget)
	{
		return;
	}

	InGameMenuWidget->Setup();
	InGameMenuWidget->SetMenuInterface(this);
}

void UPlatformsGameInstance::LobbyLoadMenu()
{
	if (!LobbyMenuWidgetClass)
	{
		return;
	}

	LobbyMenuWidget = CreateWidget<UMenuWidget>(this, LobbyMenuWidgetClass);
	if (!LobbyMenuWidget)
	{
		return;
	}

	LobbyMenuWidget->Setup();
	LobbyMenuWidget->SetMenuInterface(this);
}

void UPlatformsGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(NAME_GameSession);
	}
}

void UPlatformsGameInstance::Login()
{
	if (SubSystem)
	{
		IOnlineIdentityPtr OnlineIdentity = SubSystem->GetIdentityInterface();
		if (OnlineIdentity.IsValid())
		{	/*
			FOnlineAccountCredentials Credentials;				//dev											//accountportal
			Credentials.Id = FString("127.0.0.1:8081");			//for dev use IP and port
			Credentials.Token = FString("EOSTestCredential");	//token for dev is credential name
			Credentials.Type = FString("developer");			//developer for dev								// accountportal only uses web for login
			OnlineIdentity->OnLoginCompleteDelegates->AddUObject(this, &UPlatformsGameInstance::OnLoginComplete);
			OnlineIdentity->Login(0, Credentials);
			*/

			//Using command line args
			OnlineIdentity->OnLoginCompleteDelegates->AddUObject(this, &UPlatformsGameInstance::OnLoginComplete);
			OnlineIdentity->AutoLogin(0);
		}
	}
}