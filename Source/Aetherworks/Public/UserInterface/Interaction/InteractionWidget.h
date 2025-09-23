// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UProgressBar;
class UTextBlock;
class AAetherworksCharacter;
struct FInteractableData;
/**
 * 
 */
UCLASS()
class AETHERWORKS_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================

	void UpdateWidget(const FInteractableData* InteractableData) const;
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
	
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Player Reference")
	AAetherworksCharacter* PlayerCharacter;

protected:

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================

	UFUNCTION(Category = "Interaction Widget | Interactable Data")
	float UpdateInteractionProgress();
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
	
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* ActionText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* QuantityText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* KeyPressText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UProgressBar* InteractionProgressBar;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	float CurrentInteractionDuration;
};
