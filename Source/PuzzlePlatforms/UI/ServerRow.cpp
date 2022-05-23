// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ServerRow.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


void UServerRow::Setup(const FServerData& ServerData, UMainMenu* InParent, uint32 InIndex)
{
	ServerName->SetText(FText::FromString(ServerData.Name));
	UserName->SetText(FText::FromString(ServerData.HostUserName));
	ConnectionFraction->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers)));
	Index = InIndex;
	Parent = InParent;
	RowBtn->OnClicked.AddDynamic(this, &UServerRow::OnRowBtnClicked);

}

void UServerRow::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	if (bSelected)
	{
		Select();
	}
	else
	{
		DeSelect();
	}
}

void UServerRow::OnRowBtnClicked()
{
	if (Parent)
	{
		Parent->SetSelectedIndex(Index);
	}
}
