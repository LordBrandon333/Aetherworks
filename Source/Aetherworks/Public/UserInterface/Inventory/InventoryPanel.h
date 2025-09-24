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

	void SetInfoText() const;
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
public:

	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryWrapBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	AAetherworksCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent*  InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;
};
