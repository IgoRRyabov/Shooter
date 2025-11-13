#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "InventorySlot.generated.h"

USTRUCT(BlueprintType)
struct SHOOTER_API FInventorySlot
{
	GENERATED_BODY()

public:
	// Пустой слот
	FInventorySlot()
		: ItemID(NAME_None), Quantity(0)
	{}

	// Занятый слот
	FInventorySlot(FName InItemID, int32 InQuantity)
		: ItemID(InItemID), Quantity(InQuantity)
	{}

	// Проверка, пустой ли слот
	bool IsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;
};
