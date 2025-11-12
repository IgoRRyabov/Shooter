#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/ItemData.h"
#include "InventoryItemWidget.generated.h"


UCLASS()
class SHOOTER_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Инициализировать UI элемент */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetupItem(const FItemData& ItemData, int32 Quantity);

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemName;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemQuantity;
};
