// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"
#include "VoiceRoomMemberTableRowView.h"
#include "TextLabel.h"
#include "Button.h"
#include "Checkbox.h"
#include "Voice.h"
#include "Game.h"

FVoiceRoomMemberTableRowView::FVoiceRoomMemberTableRowView(Vector2 Pos, Vector2 Size, UILayer Layer, const std::wstring& InAssetFile, const FVoiceRoomMemberTableRowData& InData, FColor InBackgroundColor, FColor InTextColor)
	: FDialog(Pos, Size, Layer), Data(InData), AssetFile(InAssetFile), BackgroundColor(InBackgroundColor), TextColor(InTextColor)
{
	static_assert(std::tuple_size<FVoiceRoomMemberTableRowView::FRowWidgets>::value == std::tuple_size<FVoiceRoomMemberTableRowData::FValues>::value, "FRowWidgets must be the same size as the number of values.");
	static_assert(std::tuple_size<FVoiceRoomMemberTableRowView::FActionButtons>::value == std::tuple_size<FVoiceRoomMemberTableRowData::FActionsAvailable>::value, "FActionButtons must be the same size as the number of actions.");
	static_assert(std::tuple_size<FVoiceRoomMemberTableRowView::FActionIcons>::value == std::tuple_size<FVoiceRoomMemberTableRowData::FActionsAvailable>::value, "FActionIcons must be the same size as the number of actions.");

	Vector2 LabelSize;
	Vector2 ActionSize;
	CalcSizes(LabelSize, ActionSize);

	//create labels
	for (size_t i = 0; i < RowWidgets.size(); ++i)
	{
		std::shared_ptr<FTextLabelWidget> NextLabel = std::make_shared<FTextLabelWidget>(
			Vector2(Pos.x + LabelSize.x * i, Pos.y),
			LabelSize,
			Layer - 1,
			Data.Values[i],
			AssetFile,
			BackgroundColor,
			TextColor);

		RowWidgets[i] = NextLabel;
		AddWidget(NextLabel);
	}

	//create action buttons
	std::array<std::wstring, FVoiceRoomMemberTableRowData::EAction::Count> actionLabels;
	std::array<FColor, FVoiceRoomMemberTableRowData::EAction::Count> actionColors;
	std::array<std::wstring, FVoiceRoomMemberTableRowData::EAction::Count> actionIconEnabled;
	std::array<std::wstring, FVoiceRoomMemberTableRowData::EAction::Count> actionIconDisabled;

	actionLabels[FVoiceRoomMemberTableRowData::EAction::Status] = L"";
	actionColors[FVoiceRoomMemberTableRowData::EAction::Status] = Color::Black;
	actionIconEnabled[FVoiceRoomMemberTableRowData::EAction::Status] = L"Assets/audio_on.dds";
	actionIconDisabled[FVoiceRoomMemberTableRowData::EAction::Status] = L"Assets/audio_off.dds";

	actionLabels[FVoiceRoomMemberTableRowData::EAction::Mute] = L"MUTE";
	actionColors[FVoiceRoomMemberTableRowData::EAction::Mute] = Color::DarkGreen;

	actionLabels[FVoiceRoomMemberTableRowData::EAction::RemoteMute] = L"REMOTE MUTE";
	actionColors[FVoiceRoomMemberTableRowData::EAction::RemoteMute] = Color::DarkBlue;

	actionLabels[FVoiceRoomMemberTableRowData::EAction::Kick] = L"KICK";
	actionColors[FVoiceRoomMemberTableRowData::EAction::Kick] = Color::DarkRed;

	for (size_t Ix = 0; Ix < ActionButtons.size(); ++Ix)
	{
		if (Ix != FVoiceRoomMemberTableRowData::EAction::Status)
		{
			std::shared_ptr<FButtonWidget> NextButton = std::make_shared<FButtonWidget>(
				Vector2(Pos.x + Data.Values.size() * LabelSize.x + Ix * ActionSize.x, Pos.y),
				ActionSize,
				Layer - 1,
				actionLabels[Ix],
				assets::DefaultButtonAssets,
				Font,
				actionColors[Ix]);
			NextButton->SetOnPressedCallback([this, Ix]() { this->OnPressed(Ix); });
			if (!Data.bActionsAvailable[Ix])
			{
				NextButton->Disable();
			}

			ActionButtons[Ix] = NextButton;
			AddWidget(NextButton);
		}
	}

	for (size_t Ix = 0; Ix < ActionIcons.size(); ++Ix)
	{
		if (Ix == FVoiceRoomMemberTableRowData::EAction::Status)
		{
			std::shared_ptr<FCheckboxWidget> NextCheckbox = std::make_shared<FCheckboxWidget>(
				Vector2(Pos.x + Data.Values.size() * LabelSize.x + Ix * ActionSize.x, Pos.y),
				ActionSize,
				Layer - 1,
				actionLabels[Ix],
				L"",
				nullptr,
				actionIconDisabled[Ix],
				actionIconEnabled[Ix]);
			NextCheckbox->SetOnTickedCallback([this, Ix](bool bIsPressed) { this->OnPressed(Ix); });
			NextCheckbox->Disable();
			NextCheckbox->Show();
			ActionIcons[Ix] = NextCheckbox;
			AddWidget(NextCheckbox);
		}
	}
}

void FVoiceRoomMemberTableRowView::SetFocused(bool bValue)
{
	FDialog::SetFocused(bValue);

	if (bValue)
	{
		SetBorderColor(Color::UIDarkGrey);
	}
	else
	{
		ClearBorderColor();
	}
}

void FVoiceRoomMemberTableRowView::SetPosition(Vector2 Pos)
{
	FDialog::SetPosition(Pos);

	ReadjustLayout();
}

void FVoiceRoomMemberTableRowView::SetSize(Vector2 NewSize)
{
	FDialog::SetSize(NewSize);

	ReadjustLayout();
}

void FVoiceRoomMemberTableRowView::Enable()
{
	FDialog::Enable();

	//Disable action buttons
	for (size_t Ix = 0; Ix < FVoiceRoomMemberTableRowData::EAction::Count; ++Ix)
	{
		if (!Data.bActionsAvailable[Ix] && ActionButtons.size() > Ix)
		{
			std::shared_ptr<FButtonWidget> ActionButton = ActionButtons[Ix];
			if (ActionButton)
			{
				ActionButton->Disable();
			}
		}
		if (!Data.bActionsAvailable[Ix] && ActionIcons.size() > Ix)
		{
			std::shared_ptr<FCheckboxWidget> ActionIcon = ActionIcons[Ix];
			if (ActionIcon)
			{
				ActionIcon->Disable();
			}
		}
	}
}

void FVoiceRoomMemberTableRowView::SetOnActionPressedCallback(std::function<void(size_t)> Callback)
{
	ActionPressedCallback = Callback;
}

void FVoiceRoomMemberTableRowView::HideActions()
{
	for (auto ActionButton : ActionButtons)
	{
		if (ActionButton)
		{
			ActionButton->Hide();
		}
	}
	for (auto ActionIcon : ActionIcons)
	{
		if (ActionIcon)
		{
			ActionIcon->Hide();
		}
	}
}

void FVoiceRoomMemberTableRowView::SetData(const FVoiceRoomMemberTableRowData& InData)
{
	Data = InData;

	for (size_t i = 0; i < RowWidgets.size(); ++i)
	{
		std::wstring DataString = (i < InData.Values.size()) ? InData.Values[i] : L"-";
		RowWidgets[i]->SetText(DataString);
	}

	for (size_t Ix = 0; Ix < ActionButtons.size(); ++Ix)
	{
		if (ActionButtons[Ix])
		{
			if (Data.bActionsAvailable[Ix])
			{
				ActionButtons[Ix]->Enable();
			}
			else
			{
				ActionButtons[Ix]->Disable();
			}
		}
	}

	for (size_t Ix = 0; Ix < ActionIcons.size(); ++Ix)
	{
		if (ActionIcons[Ix])
		{
			if (Ix == FVoiceRoomMemberTableRowData::EAction::Status)
			{
				bool bTicked = Data.bIsMuted || !Data.bIsSpeaking;
				ActionIcons[Ix]->SetTicked(bTicked);
				if (bTicked)
				{
					ActionIcons[Ix]->SetTickedColor(Data.bIsMuted ? Color::Red : Color::White);
				}
				else
				{
					ActionIcons[Ix]->SetUntickedColor(Data.bIsSpeaking ? Color::Green : Color::White);
				}
			}
		}
	}

	ReadjustLayout();
}

void FVoiceRoomMemberTableRowView::SetFont(FontPtr InFont)
{
	Font = InFont;

	for (auto Widget : RowWidgets)
	{
		Widget->SetFont(InFont);
	}

	for (auto Button : ActionButtons)
	{
		if (Button)
		{
			Button->SetFont(InFont);
		}
	}

	for (auto Icon : ActionIcons)
	{
		if (Icon)
		{
			Icon->SetFont(InFont);
		}
	}
}

void FVoiceRoomMemberTableRowView::CalcSizes(Vector2& LabelSize, Vector2& ActionSize)
{
	// Fit the action buttons into "2" columns.
	float ActionSizeRatio = 2.3f / FVoiceRoomMemberTableRowData::EAction::Count;
	float ColumnRatio = Data.Values.size() + ActionSizeRatio * FVoiceRoomMemberTableRowData::EAction::Count;
	LabelSize = Vector2(Size.x / ColumnRatio, Size.y);
	ActionSize = Vector2(LabelSize.x * ActionSizeRatio, Size.y);
}

void FVoiceRoomMemberTableRowView::ReadjustLayout()
{
	Vector2 LabelSize;
	Vector2 ActionSize;
	CalcSizes(LabelSize, ActionSize);

	//resize labels
	for (size_t i = 0; i < RowWidgets.size(); ++i)
	{
		if (RowWidgets[i])
		{
			RowWidgets[i]->SetPosition(Vector2(Position.x + LabelSize.x * i, Position.y));
			RowWidgets[i]->SetSize(LabelSize);
		}
	}

	for (size_t i = 0; i < ActionButtons.size(); ++i)
	{
		if (ActionButtons[i])
		{
			ActionButtons[i]->SetPosition(Vector2(Position.x + Data.Values.size() * LabelSize.x + i * ActionSize.x, Position.y));
			ActionButtons[i]->SetSize(ActionSize);
		}
	}

	for (size_t i = 0; i < ActionIcons.size(); ++i)
	{
		if (ActionIcons[i])
		{
			ActionIcons[i]->SetPosition(Vector2(Position.x + Data.Values.size() * LabelSize.x + i * ActionSize.x, Position.y));
			ActionIcons[i]->SetSize(ActionSize);
		}
	}
}

void FVoiceRoomMemberTableRowView::OnPressed(size_t ActionIndex)
{
	if (ActionPressedCallback)
	{
		ActionPressedCallback(ActionIndex);
	}

	FProductUserId UserId  = Data.UserId;

	if (ActionIndex == FVoiceRoomMemberTableRowData::EAction::Mute)
	{
		FGame::Get().GetVoice()->LocalToggleMuteMember(UserId);
	}
	else if (ActionIndex == FVoiceRoomMemberTableRowData::EAction::RemoteMute)
	{
		FGame::Get().GetVoice()->RemoteToggleMuteMember(UserId);
	}
	else if (ActionIndex == FVoiceRoomMemberTableRowData::EAction::Kick)
	{
		FGame::Get().GetVoice()->KickMember(UserId);
	}
}

template<>
std::shared_ptr<FVoiceRoomMemberTableRowView> CreateListEntry(Vector2 Pos, Vector2 Size, UILayer Layer, const FVoiceRoomMemberTableRowData& Data)
{
	return std::make_shared<FVoiceRoomMemberTableRowView>(Pos, Size, Layer, L"", Data, Color::DarkGray, Color::White);
}