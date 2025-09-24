// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryPanel.h"

#include "Components/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Items/ItemBase.h"
#include "Player/AetherworksCharacter.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<AAetherworksCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory();
		if (InventoryReference)
		{
			InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
			SetInfoText();
		}
	}
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryPanel::RefreshInventory()
{
	if (InventoryReference && InventorySlotClass)
	{
		InventoryWrapBox->ClearChildren();
		
		for (UItemBase* const& InventoryItem : InventoryReference->GetInventoryContents())
		{
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
			ItemSlot->SetItemReference(InventoryItem);

			InventoryWrapBox->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}

void UInventoryPanel::SetInfoText() const
{
	const FString WeightInfoValue {FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight()) + "/" + FString::SanitizeFloat(InventoryReference->GetWeightCapacity())};
	const FString CapacityInfoValue {FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/" + FString::FromInt(InventoryReference->GetSlotsCapacity())};
	
	WeightInfo->SetText(FText::FromString(WeightInfoValue));
	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}
