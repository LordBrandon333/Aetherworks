#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());
	return (WeightMaxAddAmount >= RequestedAmount) ? RequestedAmount : WeightMaxAddAmount;
}

int32 UInventoryComponent::GetAmountOfUsedSlotsInContainer()
{
	int32 ItemAmount = 0;

	for (const UItemBase* Item : ContainerContents)
	{
		if (Item->InventorySlotIndex >= GetInventoryStart() && Item->InventorySlotIndex < GetInventoryEnd()) ++ItemAmount;
	}

	return ItemAmount;
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);
	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);
	InventoryTotalWeight -= ActualAmountToRemove * ItemIn->GetItemSingleWeight();
	OnContainerUpdated.Broadcast();
	return ActualAmountToRemove;
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* InputItem)
{
	if (FMath::IsNearlyZero(InputItem->GetItemSingleWeight()) || InputItem->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0}. Invalid weight value."), InputItem->ItemTextData.Name));
	}

	if (InventoryTotalWeight + InputItem->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0}. Weight limit would be exceeded."), InputItem->ItemTextData.Name));
	}

	// Slots voll? (Gesamt)
	if (ContainerContents.Num() + 1 > GetTotalSlotsCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0}. All slots are full."), InputItem->ItemTextData.Name));
	}

	AddNewItem(InputItem); // -> in normalen Bereich
	return FItemAddResult::AddedAll(1, FText::Format(
			FText::FromString("Added one {0}."), InputItem->ItemTextData.Name));
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* InputItem, int32 RequestedAddAmount)
{
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(InputItem->GetItemStackWeight())) return 0;

	int32 AmountToDistribute = RequestedAddAmount;

	// 1) vorhandene Teil-Stacks auffüllen (wie gehabt, inkl. Gewicht)
	UItemBase* ExistingItemStack = FindNextPartialStack(InputItem);
	while (ExistingItemStack)
	{
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
		const int32 WeightLimitAddAmount  = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		if (WeightLimitAddAmount > 0)
		{
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);
			InventoryTotalWeight += ExistingItemStack->GetItemSingleWeight() * WeightLimitAddAmount;

			AmountToDistribute -= WeightLimitAddAmount;
			InputItem->SetQuantity(AmountToDistribute);

			if (InventoryTotalWeight + ExistingItemStack->GetItemSingleWeight() > InventoryWeightCapacity)
			{
				OnContainerUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		else
		{
			if (AmountToDistribute != RequestedAddAmount)
			{
				OnContainerUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
			return 0;
		}

		if (AmountToDistribute <= 0)
		{
			OnContainerUpdated.Broadcast();
			return RequestedAddAmount;
		}

		ExistingItemStack = FindNextPartialStack(InputItem);
	}

	// 2) Rest: EINEN neuen Stack in normalen Bereich, wenn frei & Gewicht passt
	if (AmountToDistribute > 0)
	{
		// Gesamt-Slots voll?
		if (ContainerContents.Num() + 1 > GetTotalSlotsCapacity())
		{
			OnContainerUpdated.Broadcast();
			return RequestedAddAmount - AmountToDistribute;
		}

		// Ziel-Slot im normalen Inventarbereich suchen
		const int32 FreeIndex = FindFirstFreeInventorySlot();
		if (FreeIndex == INDEX_NONE)
		{
			// Kein normaler Slot frei -> aktuell keine Auto-Ablage in Hotbar/Equip
			OnContainerUpdated.Broadcast();
			return RequestedAddAmount - AmountToDistribute;
		}

		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(InputItem, AmountToDistribute);
		if (WeightLimitAddAmount > 0)
		{
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				AmountToDistribute -= WeightLimitAddAmount;
				InputItem->SetQuantity(AmountToDistribute);

				// Kopie anlegen und gezielt in FreeIndex setzen (Player-Override von AddNewItem übernimmt Platzierung)
				AddNewItem(InputItem->CreateItemCopy(), WeightLimitAddAmount);
				// Der Override unten benutzt Bereichssuche; wir setzen hier direkt den Index:
				// -> deshalb kleine Anpassung: wir setzen nach AddNewItem nicht direkt den Index.
				// Für exakte Index-Zuordnung: eigenes AdoptExisting (optional).
				return RequestedAddAmount - AmountToDistribute;
			}

			AddNewItem(InputItem, AmountToDistribute);
			return RequestedAddAmount;
		}

		// kein Gewicht mehr
		OnContainerUpdated.Broadcast();
		return RequestedAddAmount - AmountToDistribute;
	}

	OnContainerUpdated.Broadcast();
	return RequestedAddAmount;
}

void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem = nullptr;

	if (Item->bIsCopy || Item->bIsPickup)
	{
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	// WICHTIG: Nur im normalen Inventarbereich platzieren (0 .. ContainerSlotsCapacity-1)
	const int32 FreeIndex = FindFirstFreeInventorySlot();
	if (FreeIndex == INDEX_NONE)
	{
		// Fallback: (sollte nicht passieren, da vorher geprüft) -> in ges. Bereich
		NewItem->InventorySlotIndex = FindFirstFreeSlotIndex();
	}
	else
	{
		NewItem->InventorySlotIndex = FreeIndex;
	}

	ContainerContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnContainerUpdated.Broadcast();
}

TArray<UItemBase*> UInventoryComponent::GetContainerRegionContents(const EContainerRegion RequestedRegion)
{
	TArray<UItemBase*> OutArray;

	int32 RegionStartIndex = 0;
	int32 RegionEndIndex = 0;

	switch (RequestedRegion) {
	case EContainerRegion::Inventory:
		RegionStartIndex = GetInventoryStart();
		RegionEndIndex = GetInventoryEnd();
		break;
		
	case EContainerRegion::Hotbar:
		RegionStartIndex = GetHotbarStart();
		RegionEndIndex = GetHotbarEnd();
		break;
		
	case EContainerRegion::Equipment:
		RegionStartIndex = GetEquipmentStart();
		RegionEndIndex = GetEquipmentEnd();
		break;
	}

	for (UItemBase* Item : ContainerContents)
	{
		if (Item->InventorySlotIndex >= RegionStartIndex && Item->InventorySlotIndex < RegionEndIndex) OutArray.Add(Item);
	}
	
	return OutArray;
}
