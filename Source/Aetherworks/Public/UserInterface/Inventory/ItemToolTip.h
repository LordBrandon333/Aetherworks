// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

class UTextBlock;
class UInventoryItemSlot;
class UItemBase;


UCLASS()
class AETHERWORKS_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
public:

	void SetItemToolTipText(const UItemBase* InItem);
	void ClearItemToolTipText();
	
protected:
	
	virtual void NativeConstruct() override;

	void SetToolTipVariablesToVisibilityState(ESlateVisibility TargetState);

	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
public:
	
	UPROPERTY(VisibleAnywhere);
	UInventoryItemSlot* InventorySlotBeingHovered;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ArmorRating;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UsageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxStackSize;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackWeight;
};
