#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryComponent.h"
#include "InventoryWidget.generated.h"

UCLASS()
class SHOOTER_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Привязать компонент инвентаря */
	void InitializeInventory(UInventoryComponent* InInventory);

	/** Показать/скрыть UI */
	void SetInventoryVisible(bool bVisible);

	/** Проверить, открыт ли UI */
	bool IsInventoryVisible() const { return bIsVisible; }
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox_Items;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<class UInventoryItemWidget> ItemWidgetClass;

	UFUNCTION()
	void RefreshInventory();

private:
	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	bool bIsVisible = false;
};
