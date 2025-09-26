// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotbarPanel.generated.h"

class UHorizontalBox;
class UInventoryItemSlot;
class UInventoryComponent;
class AAetherworksCharacter;

UCLASS()
class AETHERWORKS_API UHotbarPanel : public UUserWidget
{
	GENERATED_BODY()

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
public:
	
	UFUNCTION()
	void RefreshHotbar();

protected:
	
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION(Category = "Inventory") FORCEINLINE bool CheckIfIndexIsValid(const int32 Index) const;

	void BuildSlotsIfNeeded();
	void ClearSlotVisuals();
	void FillSlotVisuals();
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
public:

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HotbarHorizontalBox = nullptr;

	UPROPERTY()
	AAetherworksCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent*  InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	UPROPERTY()
	TArray<UInventoryItemSlot*> HotbarSlots;

	UPROPERTY()
	int32 CachedCapacity = -1;
};
