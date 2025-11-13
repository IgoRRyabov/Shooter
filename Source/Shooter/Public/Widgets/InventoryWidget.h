#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
class SHOOTER_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void InitializeInventory(class UInventoryComponent* InInventory);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool IsInitialized() const { return bInitialized; }
	
	UFUNCTION()
	void RefreshInventory();
	
	UPROPERTY(meta = (BindWidget))
	class UImage* CharacterImage;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* GridPanel;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
private:
	UPROPERTY()
	class UInventoryComponent* Inventory;

	bool bInitialized = false;
};
