// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzlePlatformsGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API ALobbyGameMode : public APuzzlePlatformsGameMode
{
	GENERATED_BODY()
	
public:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

protected:

	FTimerHandle GameStartTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float GameStartDelay = 5.f;

	uint32 NumberOfPlayers = 0;

	UFUNCTION(Exec)
	void StartGame();
};
