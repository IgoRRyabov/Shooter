#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/ItemData.h"
#include "Structs/InventoryItem.h"
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
	void ServerRemoveItem(FName ItemID, int32 Quantity = 1);

	/** Получить все предметы */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	const TArray<FInventoryItem>& GetItems() const { return Items; }

	/** Проверить, есть ли предмет */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool HasItem(FName ItemID) const;

	/** Делегат для обновления UI */
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

protected:
	/** Список предметов */
	UPROPERTY(ReplicatedUsing=OnRep_Items, VisibleAnywhere, BlueprintReadOnly)
	TArray<FInventoryItem> Items;

	UFUNCTION()
	void OnRep_Items();

	void AddItem_Internal(FName ItemID, int32 Quantity);
	void RemoveItem_Internal(FName ItemID, int32 Quantity);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
