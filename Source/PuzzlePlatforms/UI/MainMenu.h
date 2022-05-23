// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()
	
	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};

class UButton;
class UWidgetSwitcher;
class UEditableTextBox;
class UPanelWidget;

UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:

	void SetServerList(const TArray<FServerData>& ServerData);

	void SetSelectedIndex(uint32 Index);

protected:

	virtual bool Initialize() override;

	UFUNCTION()
	void OnHostMenuBtnClicked();

	UFUNCTION()
	void OnHostBtnClicked();

	UFUNCTION()
	void OnJoinMenuBtnClicked();

	UFUNCTION()
	void OnJoinBtnClicked();

	UFUNCTION()
	void OnCancelBtnClicked();

	UFUNCTION()
	void OnQuitBtnClicked();

	UFUNCTION()
	void OnRefreshBtnClicked();

	void RefreshServerList();

	void SetSelectedRow(uint32 InIndex, bool bInSelected);

	UPROPERTY(meta = (BindWidget))
	UButton* HostBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HostMenuBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinMenuBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelJoinBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelHostBtn;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* RefreshBtn;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ServerList;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ServerNameField;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ServerRowClass;

	TOptional<uint32> SelectedIndex;
};
