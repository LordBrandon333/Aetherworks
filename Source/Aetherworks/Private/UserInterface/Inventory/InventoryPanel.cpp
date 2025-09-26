// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryPanel.h"

#include "Components/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Items/ItemBase.h"
#include "Player/AetherworksCharacter.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"

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
			RefreshInventory();
		}
	}
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on InventoryPanel."));

		// returning true will stop the drop operation at this widget
		return true;
	}
	// returning false will cause the drop operation to fall through to underlying widgets (if any)
	return false;
}

void UInventoryPanel::BuildSlotsIfNeeded()
{
	if (!InventoryReference || !InventoryWrapBox || !InventorySlotClass) return;

	const int32 Capacity = InventoryReference->GetSlotsCapacity();
	if (Capacity == CachedCapacity && InventorySlots.Num() == Capacity && InventoryWrapBox->GetChildrenCount() == Capacity)
	{
		// all slots are already build
		return;
	}

	CachedCapacity = Capacity;
	InventorySlots.Reset(Capacity);
	InventoryWrapBox->ClearChildren();
	InventorySlots.Reserve(Capacity);

	for (int32 i = 0; i < Capacity; ++i)
	{
		UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
		ItemSlot->InitializeAsEmptyInventorySlot(InventoryReference, i);
		InventorySlots.Add(ItemSlot);
		InventoryWrapBox->AddChild(ItemSlot);
	}
}

void UInventoryPanel::ClearSlotVisuals()
{
	for (UInventoryItemSlot* InvSlot : InventorySlots)
	{
		InvSlot->ResetToEmptySlot();
	}
}

void UInventoryPanel::FillSlotVisuals()
{
	const TArray<UItemBase*>& Items = InventoryReference->GetInventoryContents();

	for (UItemBase* Item : Items)
	{
		if (!Item) continue;

		const int32 Idx = Item->InventorySlotIndex;
		if (!CheckIfIndexIsValid(Idx)) continue;

		InventorySlots[Idx]->InitializeVisualization(Item);
	}
}

void UInventoryPanel::RefreshInventory()
{
	if (InventoryReference && InventorySlotClass)
	{
		// Only (re)builds Slots if necessary, like Init or SlotCapacity resize
		BuildSlotsIfNeeded();
		// Resets every existing SlotWidget to empty version
		ClearSlotVisuals();
		// Fills the necessary slots with info of items in the inventory
		FillSlotVisuals();
		// Sets weight & capacity info
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
