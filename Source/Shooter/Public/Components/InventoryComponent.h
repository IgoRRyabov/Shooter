#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/ItemData.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void AddItem_Server(const FItemData& NewItem);

	UFUNCTION(BlueprintCallable)
	bool HasItem(FName ItemID) const;

	UFUNCTION(BlueprintCallable)
	const TArray<FItemData>& GetItems() const { return Items; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Items, VisibleAnywhere, BlueprintReadOnly)
	TArray<FItemData> Items;

	UFUNCTION()
	void OnRep_Items();

	void AddItem_Internal(const FItemData& NewItem);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
