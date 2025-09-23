// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/AetherworksCharacterHUD.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"

AAetherworksCharacterHUD::AAetherworksCharacterHUD()
{
}

void AAetherworksCharacterHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AAetherworksCharacterHUD::DisplayMenu()
{
	if (!MainMenuWidget) return;

	bIsMenuVisible = true;
	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
}

void AAetherworksCharacterHUD::HideMenu()
{
	if (!MainMenuWidget) return;
	
	bIsMenuVisible = false;
	MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
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
