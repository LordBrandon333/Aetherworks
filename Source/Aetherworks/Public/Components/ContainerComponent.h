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

	int32 ActualAmountAdded;
	EItemAddResult OperationResult;
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
	UContainerComponent();

	UFUNCTION(Category = "Container") FItemAddResult HandleAddItem(UItemBase* InputItem);

	UFUNCTION(Category = "Container") UItemBase* FindMatchingItem(UItemBase* ItemIn) const;
	UFUNCTION(Category = "Container") UItemBase* FindNextItemByID(UItemBase* ItemIn) const;
	UFUNCTION(Category = "Container") UItemBase* FindNextPartialStack(UItemBase* ItemIn) const;
	
	UFUNCTION(Category = "Container") void RemoveSingleInstanceOfItem(UItemBase* ItemToRemove);
	UFUNCTION(Category = "Container") virtual int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);
	UFUNCTION(Category = "Container") void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);
	UFUNCTION(Category = "Container") void TryMoveOrSwapOrMerge(UItemBase* ItemToTry, UItemBase* CurrentItemAtIndex, const int32 TargetIndex);

	//=== Getters ===
	// WICHTIG: virtuell gemacht, damit InventoryComponent die Gesamt-Kapazität (Inv+Hotbar+Equip) zurückgeben kann
	UFUNCTION(Category = "Container") virtual int32 GetTotalSlotsCapacity() const { return ContainerSlotsCapacity; };
	UFUNCTION(Category = "Container") FORCEINLINE TArray<UItemBase*> GetContainerContents() const { return ContainerContents; };
	UFUNCTION(Category = "Container") FORCEINLINE bool CheckIfIndexIsValid(const int32 Index) const { return Index >= 0 && Index < GetTotalSlotsCapacity(); }
	UFUNCTION(Category = "Container") virtual int32 GetAmountOfUsedSlotsInContainer();

	//=== Setters ===
	UFUNCTION(Category = "Container") FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { ContainerSlotsCapacity = NewSlotsCapacity; };

public:
	FOnContainerUpdated OnContainerUpdated;

protected:
	virtual void BeginPlay() override;

	virtual FItemAddResult HandleNonStackableItems(UItemBase* InputItem);
	virtual int32 HandleStackableItems(UItemBase* InputItem, int32 RequestedAddAmount);
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestAddAmount);

	// Standard: sucht im gesamten [0..GetSlotsCapacity)
	int32 FindFirstFreeSlotIndex();

	// Neu: sucht in einem expliziten Bereich [Start, End)
	int32 FindFirstFreeSlotIndexInRange(int32 StartIndexInclusive, int32 EndIndexExclusive);

	// Basis-AddNewItem (legt in ersten freien Slot, kann von abgeleiteten Klassen überschrieben werden)
	virtual void AddNewItem(UItemBase* Item, const int32 AmountToAdd = 1);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Container")
	int32 ContainerSlotsCapacity = 28; // „normales“ Inventar / Kisten-Kapazität (erste Indizes)

	UPROPERTY(VisibleAnywhere, Category = "Container")
	TArray<TObjectPtr<UItemBase>> ContainerContents;
};
