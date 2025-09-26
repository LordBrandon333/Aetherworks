// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ContainerComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnContainerUpdated);

class UItemBase;

UENUM(BlueprintType)
enum EItemAddResult : uint8
{
	IAR_NoItemAdded			UMETA(DisplayName = "No item added"),
	IAR_PartialAmountAdded	UMETA(DisplayName = "Partial amount of item added"),
	IAR_AllItemAdded		UMETA(DisplayName = "All item added"),
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() : ActualAmountAdded(0), OperationResult(EItemAddResult::IAR_NoItemAdded), ResultMessage(FText::GetEmpty()) {}

	// Actual amount of item that was added to the inventory
	int32 ActualAmountAdded;
	// Enum representing the end state of an add item operation
	EItemAddResult OperationResult;
	// Informational message that can be passed with the result
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	}
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::IAR_PartialAmountAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	}
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AETHERWORKS_API UContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================

	UContainerComponent();

	UFUNCTION(Category = "Container") FItemAddResult HandleAddItem(UItemBase* InputItem);

	UFUNCTION(Category = "Container") UItemBase* FindMatchingItem(UItemBase* ItemIn) const;
	UFUNCTION(Category = "Container") UItemBase* FindNextItemByID(UItemBase* ItemIn) const;
	UFUNCTION(Category = "Container") UItemBase* FindNextPartialStack(UItemBase* ItemIn) const;
	
	UFUNCTION(Category = "Container") void RemoveSingleInstanceOfItem(UItemBase* ItemToRemove);
	UFUNCTION(Category = "Container") int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);
	UFUNCTION(Category = "Container") void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);
	UFUNCTION(Category = "Container") void TryMoveOrSwapOrMerge(UItemBase* ItemToTry, UItemBase* CurrentItemAtIndex, const int32 TargetIndex);

	//=== Getters ===
	UFUNCTION(Category = "Container") FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };
	UFUNCTION(Category = "Container") FORCEINLINE float GetWeightCapacity() const {return InventoryWeightCapacity; };
	UFUNCTION(Category = "Container") FORCEINLINE int32 GetSlotsCapacity() const {return ContainerSlotsCapacity; };
	UFUNCTION(Category = "Container") FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return ContainerContents; };
	UFUNCTION(Category = "Container") FORCEINLINE bool CheckIfIndexIsValid(const int32 Index) const { return Index >= 0 && Index < ContainerSlotsCapacity; }

	//=== Setters ===
	UFUNCTION(Category = "Container") FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { ContainerSlotsCapacity = NewSlotsCapacity; };
	UFUNCTION(Category = "Container") FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };

	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================

	FOnContainerUpdated OnContainerUpdated;

protected:
	
	//============================================================================================================
	//	FUNCTIONS
	//============================================================================================================
	
	virtual void BeginPlay() override;

	FItemAddResult HandleNonStackableItems(UItemBase* InputItem);
	int32 HandleStackableItems(UItemBase* InputItem, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAmount);
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestAddAmount);

	int32 FindFirstFreeSlotIndex();
	
	void AddNewItem(UItemBase* Item, const int32 AmountToAdd = 1);

	//============================================================================================================
	//	PROPERTIES & VARIABLES
	//============================================================================================================

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float InventoryTotalWeight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Container")
	int32 ContainerSlotsCapacity = 28;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	float InventoryWeightCapacity = 50.f;

	UPROPERTY(VisibleAnywhere, Category = "Container")
	TArray<TObjectPtr<UItemBase>> ContainerContents;
};
