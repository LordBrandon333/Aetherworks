// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryItemSlot.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Items/ItemBase.h"
#include "UserInterface/Inventory/DragItemVisual.h"
#include "UserInterface/Inventory/InventoryTooltip.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "Engine/Engine.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (TooltipClass)
	{
		InventoryTooltip = CreateWidget<UInventoryTooltip>(this, TooltipClass);
		InventoryTooltip->InventorySlotBeingHovered = this;
		SetToolTip(InventoryTooltip);
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	// submenu on right click will happen here 

	return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!ItemReference) return;

	if (DragItemVisualClass)
	{
		const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());

		ItemReference->ItemNumericData.bIsStackable
			? DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity))
			: DragVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

		UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
		DragItemOperation->SourceItem = ItemReference;
		DragItemOperation->SourceInventory = ItemReference->OwningInventory;

		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		OutOperation = DragItemOperation;
	}
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		InventoryReference->TryMoveOrSwapOrMerge(ItemDragDrop->SourceItem, this->ItemReference, this->SlotIndex);
		// returning true will stop the drop operation at this widget
		return true;
	}
	
	// returning false will cause the drop operation to fall through to underlying widgets (if any)
	return false;
}

void UInventoryItemSlot::InitializeAsEmptyInventorySlot(UInventoryComponent* InInventory, int32 InIndex)
{
	if (InInventory && InInventory->CheckIfIndexIsValid(InIndex))
	{
		ItemBorder->SetBrushColor(FLinearColor::Gray);
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		SlotIndex = InIndex;
		InventoryReference = InInventory;
	}
}

void UInventoryItemSlot::InitializeVisualization(UItemBase* ItemIn)
{
	if (!ItemIn) return;
	
	ItemReference = ItemIn;

	switch (ItemReference->ItemQuality) {
	case EItemQuality::Shoddy:
		ItemBorder->SetBrushColor(FLinearColor(0.82f, 0.7f, 0.54f, 1.f));
		break;
	case EItemQuality::Common:
		ItemBorder->SetBrushColor(FLinearColor::White);
		break;
	case EItemQuality::Quality:
		ItemBorder->SetBrushColor(FLinearColor(0.f, 0.51f, 0.169f, 1.f));
		break;
	case EItemQuality::Masterwork:
		ItemBorder->SetBrushColor(FLinearColor(0.f, 0.4f, 0.75f, 1.f));
		break;
	case EItemQuality::Grandmaster:
		ItemBorder->SetBrushColor(FLinearColor(1.f, 0.45f, 0.f, 1.0f)); // orange color
		break;
	}

	ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);
	ItemIcon->SetVisibility(ESlateVisibility::Visible);

	if (ItemReference->ItemNumericData.bIsStackable)
	{
		ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		ItemQuantity->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InventoryTooltip) InventoryTooltip->SetTooltipText(ItemReference);
}

void UInventoryItemSlot::ResetToEmptySlot()
{
	ItemBorder->SetBrushColor(FLinearColor::Gray);
	ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	ItemReference = nullptr;
	InventoryTooltip->ClearTooltipText();
}