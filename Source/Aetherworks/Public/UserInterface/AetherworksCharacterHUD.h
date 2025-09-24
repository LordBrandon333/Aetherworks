// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AetherworksCharacterHUD.generated.h"

struct FInteractableData;
class UInteractionWidget;
class UMainMenu;

UCLASS()
class AETHERWORKS_API AAetherworksCharacterHUD : public AHUD
{
	GENERATED_BODY()

public:

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================

	AAetherworksCharacterHUD();

	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	bool bIsMenuVisible = false;

protected:
	
	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
	
	virtual void BeginPlay() override;
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================

	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;
	
};
