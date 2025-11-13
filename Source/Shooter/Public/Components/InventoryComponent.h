#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/ItemData.h"
#include "Structs/InventoryItem.h"
#include "Structs/InventorySlot.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	/** Добавить предмет (только сервер) */
	UFUNCTION(Server, Reliable)
	void ServerAddItem(FName ItemID, int32 Quantity = 1);

	/** Удалить предмет (только сервер) */
	UFUNCTION(Server, Reliable)
	void ServerRemoveItem(int32 ItemID, int32 Quantity = 1);

	/** Делегат для обновления UI */
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UFUNCTION(BlueprintPure)
	const TArray<FInventorySlot>& GetSlots() const { return Slots; }

	UFUNCTION(BlueprintPure)
	int32 GetNumRows() const { return NumRows; }

	UFUNCTION(BlueprintPure)
	int32 GetNumColumns() const { return NumColumns; }
protected:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<FInventorySlot> Slots;

	UFUNCTION()
	void OnRep_Slots();

	// Размер сетки
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 NumRows = 5;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 NumColumns = 6;
	
	UFUNCTION()
	void OnRep_Items();

	void AddItem_Internal(FName ItemID, int32 Quantity);
	void RemoveItem_Internal(int32 SlotIndex, int32 Quantity);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
