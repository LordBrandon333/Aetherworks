// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/InGameUI.h"

#include "Components/CanvasPanel.h"
#include "Items/ItemBase.h"
#include "Player/AetherworksCharacter.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AAetherworksCharacter>(GetOwningPlayerPawn());

	HideMenu();
}

bool UInGameUI::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragDrop) return false;

	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		return true;
	}
	return false;
}

void UInGameUI::ToggleInventory()
{
	if (!PlayerController) return;
	
	if (bIsMenuVisible)
	{
		HideMenu();
		
		const FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();

		const FInputModeGameAndUI InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
	}
}

void UInGameUI::HideMenu()
{
	bIsMenuVisible = false;
	MenuLayer->SetVisibility(ESlateVisibility::Collapsed);
}

void UInGameUI::DisplayMenu()
{
	bIsMenuVisible = true;
	MenuLayer->SetVisibility(ESlateVisibility::Visible);
}
