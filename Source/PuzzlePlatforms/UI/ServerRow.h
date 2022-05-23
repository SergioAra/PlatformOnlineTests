// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.h"
#include "ServerRow.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class PUZZLEPLATFORMS_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	void Setup(const FServerData& ServerData, UMainMenu* InParent, uint32 InIndex);

	void SetSelected(bool bInSelected);

protected:

	UFUNCTION()
	void OnRowBtnClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void Select();

	UFUNCTION(BlueprintImplementableEvent)
	void DeSelect();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UserName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ConnectionFraction;

	UPROPERTY(meta = (BindWidget))
	UButton* RowBtn;

	UPROPERTY(Transient)
	UMainMenu* Parent;

	uint32 Index = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bSelected = false;
};
