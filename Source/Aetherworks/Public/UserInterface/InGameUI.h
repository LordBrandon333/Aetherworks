// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"


class AAetherworksCharacter;
class UInventoryPanel;
class UHotbarPanel;
class UCanvasPanel;

UCLASS()
class AETHERWORKS_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
public:

	UFUNCTION(BlueprintCallable) void ToggleInventory();
	//UFUNCTION(BlueprintCallable) void ShowContainer(class UContainerComponent* Container);
	//UFUNCTION(BlueprintCallable) void CloseContainer();

	UFUNCTION() void SetPlayerController(APlayerController* InPlayerController) { PlayerController = InPlayerController; }
	UFUNCTION() FORCEINLINE bool GetIsMenuVisible() const { return this->bIsMenuVisible; }

protected:
	
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	
	void HideMenu();
	void DisplayMenu();
	
	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
public:
	
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* HudLayer;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* MenuLayer;

	UPROPERTY(meta=(BindWidget)) UHotbarPanel* HotbarPanel;
	UPROPERTY(meta=(BindWidget)) UInventoryPanel* InventoryPanel;
	//UPROPERTY(meta=(BindWidget)) UUserWidget*   EquipmentPanel;
	//UPROPERTY(meta=(BindWidget)) UUserWidget*   BackpackPanel;
	//UPROPERTY(meta=(BindWidget)) UContainerPanel* ContainerPanel;

private:
	
	//UPROPERTY() TObjectPtr<UContainerComponent> CurrentContainer;
	bool bIsMenuVisible = false;
	AAetherworksCharacter* PlayerCharacter = nullptr;
	APlayerController* PlayerController = nullptr;
};
