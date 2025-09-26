// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ContainerComponent.h"

#include "Items/ItemBase.h"

// Sets default values for this component's properties
UContainerComponent::UContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

int32 UContainerComponent::FindFirstFreeSlotIndex()
{
	int32 IndexToReturn = INDEX_NONE;

	for (int32 IndexToTest = 0; IndexToTest < GetSlotsCapacity(); ++IndexToTest)
	{
		bool bTestedIndexFound = false;
		for (const UItemBase* ItemInArray : ContainerContents)
		{
			if (ItemInArray->InventorySlotIndex == IndexToTest)
			{
				bTestedIndexFound = true;
				break;
			}
		}
		
		if (!bTestedIndexFound)
		{
			IndexToReturn = IndexToTest;
			break;
		}
	}

	return IndexToReturn;
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
	if (!(ContainerContents.Num() + 1 > ContainerSlotsCapacity))
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

void UContainerComponent::TryMoveOrSwapOrMerge(UItemBase* ItemToTry, UItemBase* CurrentItemAtIndex,
	const int32 TargetIndex)
{
	if (!ItemToTry || !CheckIfIndexIsValid(TargetIndex)) return;
	
	if (!CurrentItemAtIndex)
	{
		// no item at target index
		ItemToTry->InventorySlotIndex = TargetIndex;
	}
	else
	{
		// item at target index -> check if same item type and if yes, can merge?
		if (ItemToTry->ID == CurrentItemAtIndex->ID)
		{
			const int32 MaxAmountToMerge = CalculateNumberForFullStack(CurrentItemAtIndex, ItemToTry->Quantity);
			const int32 ActualAmountToMerge = FMath::Min(ItemToTry->Quantity, MaxAmountToMerge);
			
			CurrentItemAtIndex->SetQuantity(CurrentItemAtIndex->Quantity + ActualAmountToMerge);
			ItemToTry->SetQuantity(ItemToTry->Quantity - ActualAmountToMerge);
		}
		// else swap
		else
		{
			CurrentItemAtIndex->InventorySlotIndex = ItemToTry->InventorySlotIndex;
			ItemToTry->InventorySlotIndex = TargetIndex;
		}
	}

	OnContainerUpdated.Broadcast();
}

FItemAddResult UContainerComponent::HandleNonStackableItems(UItemBase* InputItem)
{
	// check if the input item has valid weight
	if (FMath::IsNearlyZero(InputItem->GetItemSingleWeight()) || InputItem->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item has invalid weight value"), InputItem->ItemTextData.Name));
	}

	// adding one more item would overflow slot capacity
	if (ContainerContents.Num() + 1 > ContainerSlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. All inventory slots are full"), InputItem->ItemTextData.Name));
	}

	AddNewItem(InputItem);
	return FItemAddResult::AddedAll(1, FText::Format(
			FText::FromString("Successfully added a single {0} to the inventory."), InputItem->ItemTextData.Name));
}

int32 UContainerComponent::HandleStackableItems(UItemBase* InputItem, int32 RequestedAddAmount)
{
    if (!InputItem || RequestedAddAmount <= 0) return 0;
	
    int32 AmountToDistribute = RequestedAddAmount;

    // first fill existing stack to the max
    UItemBase* ExistingItemStack = FindNextPartialStack(InputItem);
	
    while (ExistingItemStack && AmountToDistribute > 0)
    {
        // how many items can be transfered to the existing stack
        const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
        if (AmountToMakeFullStack <= 0)
        {
            // this stack is already full, find next stack
            ExistingItemStack = FindNextPartialStack(InputItem);
            continue;
        }

        // increase amount in stack
        ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + AmountToMakeFullStack);

        // reduce the rest
        AmountToDistribute -= AmountToMakeFullStack;
        InputItem->SetQuantity(AmountToDistribute);

        // if everything was distributed
        if (AmountToDistribute <= 0)
        {
            OnContainerUpdated.Broadcast();
            return RequestedAddAmount;
        }

        // else find next stack
        ExistingItemStack = FindNextPartialStack(InputItem);
    }

    // 2) no more partial but still more to distribute -> check if there is another free slot to use
    if (AmountToDistribute > 0)
    {
        if (ContainerContents.Num() + 1 <= ContainerSlotsCapacity)
        {
            // don`t place more than max quantity in one slot
            const int32 ToPlace = InputItem->ItemNumericData.bIsStackable
                ? FMath::Min(AmountToDistribute, InputItem->ItemNumericData.MaxStackSize)
                : 1;

            // Wenn nicht der ganze Rest reinpasst, Item-Quantity entsprechend kürzen,
            // damit AddNewItem nur "ToPlace" übernimmt.
            if (ToPlace < AmountToDistribute)
            {
                AmountToDistribute -= ToPlace;
                InputItem->SetQuantity(AmountToDistribute);

                // Kopie erzeugen, damit der neue Stack unabhängig ist
                AddNewItem(InputItem->CreateItemCopy(), ToPlace);

                OnContainerUpdated.Broadcast();
                return RequestedAddAmount - AmountToDistribute; // ein Teil passte in den neuen Slot
            }

            // Ganzen Rest passt in den einen neuen Slot
            AddNewItem(InputItem, AmountToDistribute);

            OnContainerUpdated.Broadcast();
            return RequestedAddAmount;
        }

        // Kein Slot mehr frei – nur der zuvor in Partials verteilte Anteil zählt
        OnContainerUpdated.Broadcast();
        return RequestedAddAmount - AmountToDistribute;
    }

    // Es gab Rest=0 (alles in Partials untergebracht)
    OnContainerUpdated.Broadcast();
    return RequestedAddAmount;
}

FItemAddResult UContainerComponent::HandleAddItem(UItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		// handle non-stackable items
		if (!InputItem->ItemNumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem);
		}

		// handle stackable
		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
			FText::FromString("Successfully added {0} {1} to the inventory."), InitialRequestedAddAmount, InputItem->ItemTextData.Name));
		}

		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
			FText::FromString("Partial amount ({0}) of {1} added to the inventory."), StackableAmountAdded, InputItem->ItemTextData.Name));
		}

		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. No remaining inventory slots, or invalid item,"), InputItem->ItemTextData.Name));
		}
	}
	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error. GetOwner() check somehow failed."));
}

void UContainerComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem;

	if (Item->bIsCopy || Item->bIsPickup)
	{
		// if the item is already a copy, or is a world pickup
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// used when splitting or dragging to/from another inventory
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);
	NewItem->InventorySlotIndex = FindFirstFreeSlotIndex();

	ContainerContents.Add(NewItem);
	OnContainerUpdated.Broadcast();
}