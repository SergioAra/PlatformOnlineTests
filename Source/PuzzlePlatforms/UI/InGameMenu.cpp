// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu.h"
#include "Components/Button.h"
#include "MenuInterface.h"

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success)
	{
		return false;
	}

	if (CancelBtn)
	{
		CancelBtn->OnClicked.AddDynamic(this, &UInGameMenu::OnCancelBtnClicked);
	}

	if (QuitBtn)
	{
		QuitBtn->OnClicked.AddDynamic(this, &UInGameMenu::OnQuitBtnClicked);
	}
	return true;
}

void UInGameMenu::OnCancelBtnClicked()
{
	OnLevelRemovedFromWorld(GetWorld()->GetCurrentLevel(), GetWorld());
}

void UInGameMenu::OnQuitBtnClicked()
{
	if (MenuInterface)
	{
		OnLevelRemovedFromWorld(GetWorld()->GetCurrentLevel(), GetWorld());
		MenuInterface->LoadMainMenuLevel();
	}
}
