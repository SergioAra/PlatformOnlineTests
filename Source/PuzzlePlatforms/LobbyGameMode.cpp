// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "PlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	NumberOfPlayers++;
	if (NumberOfPlayers > 1 && !GetWorldTimerManager().IsTimerActive(GameStartTimerHandle))
	{
		GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ALobbyGameMode::StartGame, GameStartDelay);
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	NumberOfPlayers--;
}

void ALobbyGameMode::StartGame()
{
	UPlatformsGameInstance* GameInstance = GetGameInstance<UPlatformsGameInstance>();
	if (GameInstance)
	{
		GameInstance->StartSession();
		bUseSeamlessTravel = true;
		GameInstance->ServerTravelTo(TEXT("Level1"));
	}
}
