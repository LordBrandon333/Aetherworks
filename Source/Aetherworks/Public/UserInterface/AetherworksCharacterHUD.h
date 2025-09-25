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
	
	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
public:
	
	AAetherworksCharacterHUD();

	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	void ShowCrosshair() const;
	void HideCrosshair() const;

	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

protected:
	
	virtual void BeginPlay() override;
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	bool bIsMenuVisible = false;

protected:
	
	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	UPROPERTY()
	UUserWidget* CrosshairWidget;
	
};
