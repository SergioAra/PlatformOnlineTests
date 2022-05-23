// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */

class UButton;

UCLASS()
class PUZZLEPLATFORMS_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()
	

protected:

	virtual bool Initialize() override;

	UFUNCTION()
	void OnCancelBtnClicked();

	UFUNCTION()
	void OnQuitBtnClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* CancelBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitBtn;
};
