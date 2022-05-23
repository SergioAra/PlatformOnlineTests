// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuWidget.h"
#include "MenuInterface.h"

void UMenuWidget::Setup()
{
	AddToViewport();
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	if (!InWorld)
	{
		return;
	}

	APlayerController* PlayerController = InWorld->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
	RemoveFromViewport();

	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}