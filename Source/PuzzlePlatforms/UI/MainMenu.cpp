// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

#include "MenuInterface.h"
#include "ServerRow.h"

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success)
	{
		return false;
	}

	if (HostBtn)
	{
		HostBtn->OnClicked.AddDynamic(this, &UMainMenu::OnHostBtnClicked);
	}

	if (HostMenuBtn)
	{
		HostMenuBtn->OnClicked.AddDynamic(this, &UMainMenu::OnHostMenuBtnClicked);
	}

	if (CancelHostBtn)
	{
		CancelHostBtn->OnClicked.AddDynamic(this, &UMainMenu::OnCancelBtnClicked);
	}

	if (JoinBtn)
	{
		JoinBtn->OnClicked.AddDynamic(this, &UMainMenu::OnJoinBtnClicked);
	}

	if (JoinMenuBtn)
	{
		JoinMenuBtn->OnClicked.AddDynamic(this, &UMainMenu::OnJoinMenuBtnClicked);
	}
	
	if (CancelJoinBtn)
	{
		CancelJoinBtn->OnClicked.AddDynamic(this, &UMainMenu::OnCancelBtnClicked);
	}

	if (QuitBtn)
	{
		QuitBtn->OnClicked.AddDynamic(this, &UMainMenu::OnQuitBtnClicked);
	}

	if (RefreshBtn)
	{
		RefreshBtn->OnClicked.AddDynamic(this, &UMainMenu::OnRefreshBtnClicked);
	}

	return true;
}

void UMainMenu::OnHostMenuBtnClicked()
{
	if (MenuSwitcher && HostMenu)
	{
		MenuSwitcher->SetActiveWidget(HostMenu);
	}
}

void UMainMenu::OnHostBtnClicked()
{
	if (MenuInterface)
	{
		FString ServerName = TEXT("Game Server");
		if (ServerNameField)
		{
			ServerName = ServerNameField->Text.ToString();
		}
		MenuInterface->Host(ServerName);
	}
}

void UMainMenu::OnJoinMenuBtnClicked()
{
	if (MenuSwitcher && JoinMenu)
	{
		MenuSwitcher->SetActiveWidget(JoinMenu);
		RefreshServerList();
	}
}

void UMainMenu::SetServerList(const TArray<FServerData>& ServerData)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ServerList->ClearChildren();

	uint32 Index = 0;
	for (const FServerData& Data : ServerData)
	{
		UServerRow* TempRow = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!TempRow)
		{
			return;
		}

		TempRow->Setup(Data, this, Index);
		ServerList->AddChild(TempRow);
		Index++;
	}
}

void UMainMenu::SetSelectedRow(uint32 InIndex, bool bInSelected)
{
	UServerRow* SelectedRow = Cast<UServerRow>(ServerList->GetChildAt(InIndex));
	if (SelectedRow)
	{
		SelectedRow->SetSelected(bInSelected);
	}
}

void UMainMenu::SetSelectedIndex(uint32 Index)
{
	if (SelectedIndex.IsSet())
	{
		SetSelectedRow(SelectedIndex.GetValue(), false);
	}
	SelectedIndex = Index;
	SetSelectedRow(SelectedIndex.GetValue(), true);
}

void UMainMenu::OnJoinBtnClicked()
{
	if (!MenuInterface)
	{
		return;
	}

	if (SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Index is %d"), SelectedIndex.GetValue())
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Index is not set"))
	}
}

void UMainMenu::OnCancelBtnClicked()
{
	if (MenuSwitcher && MainMenu)
	{
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UMainMenu::OnQuitBtnClicked()
{
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

	PlayerController->ConsoleCommand(TEXT("Quit"));
}

void UMainMenu::OnRefreshBtnClicked()
{
	if (JoinMenu)
	{
		RefreshServerList();
	}
}

void UMainMenu::RefreshServerList()
{
	if (MenuInterface)
	{
		MenuInterface->RefreshServerList();
	}
}
