// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryItemSlot;
class UInventoryComponent;
class UWrapBox;
class AAetherworksCharacter;
class UTextBlock;
/**
 * 
 */
UCLASS()
class AETHERWORKS_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()
	
	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
public:
	
	UFUNCTION()
	void RefreshInventory();

protected:
	
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION(Category = "Inventory") FORCEINLINE bool CheckIfIndexIsValid(const int32 Index) const { return Index >= 0 && Index < CachedCapacity; }

	void BuildSlotsIfNeeded();
	void ClearSlotVisuals();
	void FillSlotVisuals();
	void SetInfoText() const;
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
public:

	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryWrapBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo = nullptr;

	UPROPERTY()
	AAetherworksCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent*  InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	UPROPERTY()
	TArray<UInventoryItemSlot*> InventorySlots;

	UPROPERTY()
	int32 CachedCapacity = -1;
};
