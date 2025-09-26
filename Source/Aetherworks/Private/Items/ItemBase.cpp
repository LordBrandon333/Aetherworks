// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"
#include "Components/ContainerComponent.h"

UItemBase::UItemBase() : bIsCopy(false), bIsPickup(false)
{
}

void UItemBase::ResetItemFlags()
{
	bIsCopy = false;
	bIsPickup = false;
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

	ItemCopy->ID = this->ID;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemQuality = this->ItemQuality;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->ItemTextData = this->ItemTextData;
	ItemCopy->ItemNumericData = this->ItemNumericData;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->ItemAssetData = this->ItemAssetData;
	
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity == Quantity) return;
	Quantity = FMath::Clamp(NewQuantity, 0, ItemNumericData.bIsStackable ? ItemNumericData.MaxStackSize : 1);

	
	if (this->OwningInventory)
	{
		if (this->Quantity <= 0)
		{
			this->OwningInventory->RemoveSingleInstanceOfItem(this);
		}
	}
	else
	{
		{
			UE_LOG(LogTemp, Warning, TEXT("ItemBase OwningInventory was null (item may be a pickup)."))
		}
	}
}

void UItemBase::Use(AActor* Actor)
{
}
