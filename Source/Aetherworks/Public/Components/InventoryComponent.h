#pragma once

#include "CoreMinimal.h"
#include "ContainerComponent.h"
#include "InventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AETHERWORKS_API UInventoryComponent : public UContainerComponent
{
	GENERATED_BODY()

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
public:
	
	UFUNCTION(Category = "Inventory") FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };
	UFUNCTION(Category = "Inventory") FORCEINLINE float GetWeightCapacity() const {return InventoryWeightCapacity; };
	UFUNCTION(Category = "Inventory") FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };
	virtual int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove) override;
	
	

protected:

	virtual FItemAddResult HandleNonStackableItems(UItemBase* InputItem) override;
	virtual int32 HandleStackableItems(UItemBase* InputItem, int32 RequestedAddAmount) override;
	int32 CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAmount);

	virtual void AddNewItem(UItemBase* Item, const int32 AmountToAdd = 1) override;

	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================
protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float InventoryTotalWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	float InventoryWeightCapacity = 50.f;
};