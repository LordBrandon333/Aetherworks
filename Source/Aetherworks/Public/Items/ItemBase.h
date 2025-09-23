// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/ItemDataStructs.h"
#include "ItemBase.generated.h"


UCLASS()
class AETHERWORKS_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================

	UItemBase();

	UFUNCTION(Category = "Item")
	UItemBase* CreateItemCopy() const;

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * ItemNumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return ItemNumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullStackSize() const { return Quantity == ItemNumericData.MaxStackSize; }

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION(Category = "Item")
	virtual void Use(AActor* Actor);
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
	
	//UPROPERTY()
	//UInventoryComponent* OwningInventory;
	
	UPROPERTY(VisibleAnywhere, Category = "ItemData", meta = (UIMin = 1))
	int32 Quantity;
	
	UPROPERTY(EditAnywhere, Category = "ItemData")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemAssetData ItemAssetData;

protected:
	bool operator==(const FName& OtherID) const
	{
		return ID == OtherID;
	}
};
