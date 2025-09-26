// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/HotbarPanel.h"

#include "Components/HorizontalBox.h"
#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Player/AetherworksCharacter.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"

void UHotbarPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<AAetherworksCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory();
		if (InventoryReference)
		{
			InventoryReference->OnContainerUpdated.AddUObject(this, &UHotbarPanel::RefreshHotbar);
			RefreshHotbar();
		}
	}
}

bool UHotbarPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (!ItemDragDrop) return false;

	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on HotbarPanel."));

		// returning true will stop the drop operation at this widget
		return true;
	}
	// returning false will cause the drop operation to fall through to underlying widgets (if any)
	return false;
}

bool UHotbarPanel::CheckIfIndexIsValid(const int32 Index) const
{
	return InventoryReference 
		&& Index >= InventoryReference->GetHotbarStart() 
		&& Index <  InventoryReference->GetHotbarEnd(); 
}

void UHotbarPanel::BuildSlotsIfNeeded()
{
	if (!InventoryReference || !HotbarHorizontalBox || !InventorySlotClass) return;

	const int32 Capacity = InventoryReference->GetHotbarEnd() - InventoryReference->GetHotbarStart();
	if (Capacity == CachedCapacity && HotbarSlots.Num() == Capacity && HotbarHorizontalBox->GetChildrenCount() == Capacity)
	{
		// all slots are already build
		return;
	}

	CachedCapacity = Capacity;
	HotbarSlots.Reset(Capacity);
	HotbarHorizontalBox->ClearChildren();
	HotbarSlots.Reserve(Capacity);

	for (int32 i = InventoryReference->GetHotbarStart(); i < InventoryReference->GetHotbarEnd(); ++i)
	{
		UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
		ItemSlot->InitializeAsEmptyInventorySlot(InventoryReference, i);
		HotbarSlots.Add(ItemSlot);
		HotbarHorizontalBox->AddChild(ItemSlot);
	}
}

void UHotbarPanel::ClearSlotVisuals()
{
	for (UInventoryItemSlot* InvSlot : HotbarSlots)
	{
		InvSlot->ResetToEmptySlot();
	}
}

void UHotbarPanel::FillSlotVisuals()
{
	const TArray<UItemBase*>& Items = InventoryReference->GetContainerRegionContents(EContainerRegion::Hotbar);
	const int32 Start = InventoryReference->GetHotbarStart();

	for (UItemBase* Item : Items)
	{
		if (!Item) continue;

		const int32 AbsIdx = Item->InventorySlotIndex;
		const int32 RelIdx = AbsIdx - Start;
		if (RelIdx < 0 || RelIdx >= HotbarSlots.Num()) continue;

		HotbarSlots[RelIdx]->InitializeVisualization(Item);
	}
}

void UHotbarPanel::RefreshHotbar()
{
	if (InventoryReference && InventorySlotClass)
	{
		// Only (re)builds Slots if necessary, like Init or SlotCapacity resize
		BuildSlotsIfNeeded();
		// Resets every existing SlotWidget to empty version
		ClearSlotVisuals();
		// Fills the necessary slots with info of items in the inventory
		FillSlotVisuals();
	}
}
