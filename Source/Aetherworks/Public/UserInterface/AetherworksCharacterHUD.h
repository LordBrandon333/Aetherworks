// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AetherworksCharacterHUD.generated.h"

class UInGameUI;
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
	
	void ToggleMenu() const;
	bool GetIsMenuVisible() const;

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
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInGameUI> InGameUIClass;

protected:
	
	UPROPERTY()
	UInteractionWidget* InteractionWidget = nullptr;

	UPROPERTY()
	UUserWidget* CrosshairWidget = nullptr;
	
	UPROPERTY()
	UInGameUI* InGameUI = nullptr;
};
