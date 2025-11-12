#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.generated.h"

USTRUCT(BlueprintType)
struct SHOOTER_API FInventoryItem
{
	GENERATED_BODY()

public:
	// ID предмета, совпадает с ItemID в базе данных
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	// Количество этого предмета
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;

	FInventoryItem() {}
	FInventoryItem(FName InItemID, int32 InQuantity = 1)
		: ItemID(InItemID), Quantity(InQuantity)
	{}
};