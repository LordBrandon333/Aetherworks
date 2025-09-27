// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/AetherworksCharacterHUD.h"
#include "UserInterface/Interaction/InteractionWidget.h"
#include "UserInterface/Inventory/HotbarPanel.h"
#include "UserInterface/InGameUI.h"

AAetherworksCharacterHUD::AAetherworksCharacterHUD()
{
}

void AAetherworksCharacterHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		CrosshairWidget->AddToViewport();
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InGameUIClass)
	{
		InGameUI = CreateWidget<UInGameUI>(GetWorld(), InGameUIClass);
		InGameUI->AddToViewport(5);
		InGameUI->SetPlayerController(GetOwningPlayerController());
		InGameUI->SetVisibility(ESlateVisibility::Visible);
	}
}

void AAetherworksCharacterHUD::ToggleMenu() const
{
	InGameUI->ToggleInventory();
}

bool AAetherworksCharacterHUD::GetIsMenuVisible() const
{
	if (InGameUI)
		return InGameUI->GetIsMenuVisible();
	return false;
}

void AAetherworksCharacterHUD::ShowCrosshair() const
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AAetherworksCharacterHUD::HideCrosshair() const
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AAetherworksCharacterHUD::ShowInteractionWidget() const
{
	if (!InteractionWidget) return;
	
	InteractionWidget->SetVisibility(ESlateVisibility::Visible);
}

void AAetherworksCharacterHUD::HideInteractionWidget() const
{
	if (!InteractionWidget) return;
	
	InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void AAetherworksCharacterHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (!InteractionWidget) return;

	if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}

	InteractionWidget->UpdateWidget(InteractableData);
}
