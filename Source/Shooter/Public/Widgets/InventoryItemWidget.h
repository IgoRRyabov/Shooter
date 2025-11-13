#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

UCLASS()
class SHOOTER_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetupItem(FName ItemID, int32 Quantity);

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemQuantity;

private:
	FName CurrentItemID;
	int32 CurrentQuantity;
};
