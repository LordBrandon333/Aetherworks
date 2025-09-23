// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class AAetherworksCharacter;
/**
 * 
 */
UCLASS()
class AETHERWORKS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
	
	UPROPERTY()
	AAetherworksCharacter* PlayerCharacter;

protected:

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
