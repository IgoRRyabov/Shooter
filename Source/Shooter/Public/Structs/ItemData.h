#pragma once

#include "CoreMinimal.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct SHOOTER_API FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	int32 MaxStackSize = 99;

	FItemData()
		: ItemID(NAME_None), Icon(nullptr), MaxStackSize(99)
	{}
};
