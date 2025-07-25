// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "UI/MenuInterface.h"
#include "PlatformsGameInstance.generated.h"

/**
 * 
 */

class UMenuWidget;
class UMainMenu;
class FOnlineSessionSearch;
class UNetDriver;

UCLASS()
class PUZZLEPLATFORMS_API UPlatformsGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:

	UPlatformsGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;

	UFUNCTION(Exec)
	virtual void Host(FString InServerName) override;

	UFUNCTION(Exec)
	virtual void Join(uint32 Index) override;

	UFUNCTION(BlueprintCallable)
	virtual void LoadMainMenuLevel() override;

	virtual void RefreshServerList() override;

	UFUNCTION(Exec)
	void ServerTravelTo(const FString& MapName);

	UFUNCTION(Exec)
	void ClientTravelTo(const FString& MapName);

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
	void InGameLoadMenu();

	UFUNCTION(BlueprintCallable)
	void LobbyLoadMenu();

	void StartSession();

protected:

	void OnCreateSessionComplete(FName SessionName, bool bSuccess);

	void OnDestroySessionComplete(FName SessionName, bool bSuccess);

	void CreateSession();

	void OnFindSessionsComplete(bool bSuccess);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureTpe, const FString& ErrorString) ;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainMenu> MainMenuWidgetClass;

	UMainMenu* MainMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMenuWidget>InGameMenuWidgetClass;

	UMenuWidget* InGameMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMenuWidget>LobbyMenuWidgetClass;

	UMenuWidget* LobbyMenuWidget;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FString ServerName;
};
