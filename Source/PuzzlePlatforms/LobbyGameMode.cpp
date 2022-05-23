// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "PlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	NumberOfPlayers++;
	if (NumberOfPlayers >= 2)
	{
		UPlatformsGameInstance* GameInstance = GetGameInstance<UPlatformsGameInstance>();
		if (GameInstance)
		{
			GameInstance->ServerTravelTo(TEXT("Level1"));
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	NumberOfPlayers--;
}