#include "Components/ContainerComponent.h"
#include "Items/ItemBase.h"

UContainerComponent::UContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UContainerComponent::GetAmountOfUsedSlotsInContainer()
{
	int32 ItemAmount = 0;

	for (const UItemBase* Item : ContainerContents)
	{
		if (Item->InventorySlotIndex >= 0 && Item->InventorySlotIndex < ContainerSlotsCapacity) ++ItemAmount;
	}

	return ItemAmount;
}

void UContainerComponent::BeginPlay()
{
	Super::BeginPlay();
}

UItemBase* UContainerComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (ContainerContents.Contains(ItemIn)) return ItemIn;
	}
	return nullptr;
}

UItemBase* UContainerComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = ContainerContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UContainerComponent::FindNextPartialStack(UItemBase* ItemIn) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = ContainerContents.FindByPredicate([&ItemIn](const UItemBase* InventoryItem)
		{
			return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullStackSize();
		}))
	{
		return *Result;
	}
	return nullptr;
}

int32 UContainerComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->ItemNumericData.MaxStackSize - StackableItem->Quantity;
	return FMath::Min(InitialRequestAddAmount, AddAmountToMakeFullStack);
}

int32 UContainerComponent::FindFirstFreeSlotIndexInRange(int32 StartIndexInclusive, int32 EndIndexExclusive)
{
	int32 IndexToReturn = INDEX_NONE;

	StartIndexInclusive = FMath::Max(0, StartIndexInclusive);
	EndIndexExclusive   = FMath::Min(GetTotalSlotsCapacity(), EndIndexExclusive);

	for (int32 IndexToTest = StartIndexInclusive; IndexToTest < EndIndexExclusive; ++IndexToTest)
	{
		bool bOccupied = false;
		for (const UItemBase* ItemInArray : ContainerContents)
		{
			if (ItemInArray && ItemInArray->InventorySlotIndex == IndexToTest)
			{
				bOccupied = true;
				break;
			}
		}
		if (!bOccupied)
		{
			IndexToReturn = IndexToTest;
			break;
		}
	}

	return IndexToReturn;
}

int32 UContainerComponent::FindFirstFreeSlotIndex()
{
	// Standard: gesamter Bereich
	return FindFirstFreeSlotIndexInRange(0, GetTotalSlotsCapacity());
}

void UContainerComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	ContainerContents.RemoveSingle(ItemToRemove);
	OnContainerUpdated.Broadcast();
}

int32 UContainerComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);
	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);
	OnContainerUpdated.Broadcast();
	return ActualAmountToRemove;
}

void UContainerComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	if (!(ContainerContents.Num() + 1 > GetTotalSlotsCapacity()))
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

void UContainerComponent::TryMoveOrSwapOrMerge(UItemBase* ItemToTry, UItemBase* CurrentItemAtIndex, const int32 TargetIndex)
{
	if (!ItemToTry || !CheckIfIndexIsValid(TargetIndex)) return;
	
	if (!CurrentItemAtIndex)
	{
		// move
		ItemToTry->InventorySlotIndex = TargetIndex;
	}
	else
	{
		// merge?
		if (ItemToTry->ID == CurrentItemAtIndex->ID)
		{
			const int32 MaxAmountToMerge = CalculateNumberForFullStack(CurrentItemAtIndex, ItemToTry->Quantity);
			const int32 ActualAmountToMerge = FMath::Min(ItemToTry->Quantity, MaxAmountToMerge);
			
			CurrentItemAtIndex->SetQuantity(CurrentItemAtIndex->Quantity + ActualAmountToMerge);
			ItemToTry->SetQuantity(ItemToTry->Quantity - ActualAmountToMerge);
		}
		else
		{
			// swap
			const int32 OldIdx = ItemToTry->InventorySlotIndex;
			CurrentItemAtIndex->InventorySlotIndex = OldIdx;
			ItemToTry->InventorySlotIndex = TargetIndex;
		}
	}

	OnContainerUpdated.Broadcast();
}

FItemAddResult UContainerComponent::HandleNonStackableItems(UItemBase* InputItem)
{
	// Slots voll?
	if (ContainerContents.Num() + 1 > GetTotalSlotsCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0}. All slots are full."), InputItem->ItemTextData.Name));
	}

	AddNewItem(InputItem);
	return FItemAddResult::AddedAll(1, FText::Format(
			FText::FromString("Added a single {0}."), InputItem->ItemTextData.Name));
}

int32 UContainerComponent::HandleStackableItems(UItemBase* InputItem, int32 RequestedAddAmount)
{
	if (!InputItem || RequestedAddAmount <= 0) return 0;
	
	int32 AmountToDistribute = RequestedAddAmount;

	// 1) vorhandene Teil-Stacks auffüllen
	UItemBase* ExistingItemStack = FindNextPartialStack(InputItem);
	while (ExistingItemStack && AmountToDistribute > 0)
	{
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
		if (AmountToMakeFullStack <= 0)
		{
			ExistingItemStack = FindNextPartialStack(InputItem);
			continue;
		}

		ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + AmountToMakeFullStack);

		AmountToDistribute -= AmountToMakeFullStack;
		InputItem->SetQuantity(AmountToDistribute);

		if (AmountToDistribute <= 0)
		{
			OnContainerUpdated.Broadcast();
			return RequestedAddAmount;
		}

		ExistingItemStack = FindNextPartialStack(InputItem);
	}

	// 2) Rest in EINEN neuen Slot (falls frei)
	if (AmountToDistribute > 0)
	{
		if (ContainerContents.Num() + 1 <= GetTotalSlotsCapacity())
		{
			const int32 ToPlace = InputItem->ItemNumericData.bIsStackable
				? FMath::Min(AmountToDistribute, InputItem->ItemNumericData.MaxStackSize)
				: 1;

			if (ToPlace < AmountToDistribute)
			{
				AmountToDistribute -= ToPlace;
				InputItem->SetQuantity(AmountToDistribute);

				AddNewItem(InputItem->CreateItemCopy(), ToPlace);
				OnContainerUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			AddNewItem(InputItem, AmountToDistribute);
			OnContainerUpdated.Broadcast();
			return RequestedAddAmount;
		}

		OnContainerUpdated.Broadcast();
		return RequestedAddAmount - AmountToDistribute;
	}

	OnContainerUpdated.Broadcast();
	return RequestedAddAmount;
}

FItemAddResult UContainerComponent::HandleAddItem(UItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		if (!InputItem->ItemNumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem);
		}

		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
			FText::FromString("Successfully added {0} {1}."), InitialRequestedAddAmount, InputItem->ItemTextData.Name));
		}
		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
			FText::FromString("Partial amount ({0}) of {1} added."), StackableAmountAdded, InputItem->ItemTextData.Name));
		}
		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0}. No remaining slots or invalid item."), InputItem->ItemTextData.Name));
		}
	}
	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error."));
}

void UContainerComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
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
	NewItem->InventorySlotIndex = FindFirstFreeSlotIndex(); // Basis: irgendwo

	ContainerContents.Add(NewItem);
	OnContainerUpdated.Broadcast();
}
