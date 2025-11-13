#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/InventorySlot.h"
#include "InventorySlotWidget.generated.h"

UCLASS()
class SHOOTER_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupSlot(const FInventorySlot& SlotData);

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* QuantityText;
};
