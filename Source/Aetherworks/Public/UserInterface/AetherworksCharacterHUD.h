// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AetherworksCharacterHUD.generated.h"

class UHotbarPanel;
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

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UHotbarPanel> HotbarPanelClass;

	bool bIsMenuVisible = false;

protected:
	
	UPROPERTY()
	UMainMenu* MainMenuWidget = nullptr;

	UPROPERTY()
	UInteractionWidget* InteractionWidget = nullptr;

	UPROPERTY()
	UUserWidget* CrosshairWidget = nullptr;

	UPROPERTY()
	UHotbarPanel* HotbarWidget = nullptr;
	
};
