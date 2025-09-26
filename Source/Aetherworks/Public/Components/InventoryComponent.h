#pragma once

#include "CoreMinimal.h"
#include "ContainerComponent.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EContainerRegion : uint8
{
	Inventory		UMETA(DisplayName = "Inventory"),
	Hotbar			UMETA(DisplayName = "Hotbar"),
	Equipment		UMETA(DisplayName = "Equipment"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AETHERWORKS_API UInventoryComponent : public UContainerComponent
{
	GENERATED_BODY()

public:
	// Gewicht
	UFUNCTION(Category = "Container | Inventory") FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; }
	UFUNCTION(Category = "Container | Inventory") FORCEINLINE float GetWeightCapacity() const { return InventoryWeightCapacity; }
	UFUNCTION(Category = "Container | Inventory") FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; }

	// WICHTIG: Gesamtzahl der Slots = InventorySlots + Hotbar + Equipment
	virtual int32 GetTotalSlotsCapacity() const override { return ContainerSlotsCapacity + HotbarSlotCapacity + EquipmentSlotCapacity; }
	virtual int32 GetAmountOfUsedSlotsInContainer() override;

	// Bereichs-Helper (Indizes)
	FORCEINLINE int32 GetInventoryStart()  const { return 0; }
	FORCEINLINE int32 GetInventoryEnd()    const { return ContainerSlotsCapacity; }

	FORCEINLINE int32 GetHotbarStart()     const { return ContainerSlotsCapacity; }
	FORCEINLINE int32 GetHotbarEnd()       const { return ContainerSlotsCapacity + HotbarSlotCapacity; }

	FORCEINLINE int32 GetEquipmentStart()  const { return ContainerSlotsCapacity + HotbarSlotCapacity; }
	FORCEINLINE int32 GetEquipmentEnd()    const { return GetTotalSlotsCapacity(); }

	// Freien Slot im jeweiligen Bereich suchen
	int32 FindFirstFreeInventorySlot() { return FindFirstFreeSlotIndexInRange(GetInventoryStart(), GetInventoryEnd()); }
	int32 FindFirstFreeHotbarSlot()    { return FindFirstFreeSlotIndexInRange(GetHotbarStart(), GetHotbarEnd()); }
	int32 FindFirstFreeEquipSlot()     { return FindFirstFreeSlotIndexInRange(GetEquipmentStart(), GetEquipmentEnd()); }

	// Override Remove-Amount (Gewicht)
	virtual int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove) override;

protected:
	// Gewichtete Varianten
	virtual FItemAddResult HandleNonStackableItems(UItemBase* InputItem) override;
	virtual int32 HandleStackableItems(UItemBase* InputItem, int32 RequestedAddAmount) override;
	int32 CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAmount);

	// NEU: Player-Add legt standardmäßig NUR in den normalen Inventarbereich ab
	virtual void AddNewItem(UItemBase* Item, const int32 AmountToAdd = 1) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Container | Inventory")
	float InventoryTotalWeight = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Container | Inventory")
	float InventoryWeightCapacity = 50.f;

	// Wichtig: Diese beiden addieren wir hinter ContainerSlotsCapacity
	UPROPERTY(EditDefaultsOnly, Category = "Container | Inventory")
	int32 HotbarSlotCapacity = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Container | Inventory")
	int32 EquipmentSlotCapacity = 12;
};
