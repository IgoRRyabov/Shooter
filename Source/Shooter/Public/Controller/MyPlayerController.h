#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/InventoryWidget.h"
#include "MyPlayerController.generated.h"

UCLASS()
class SHOOTER_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	AMyPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	void ToggleInventory();
private:
	UPROPERTY()
	UInventoryWidget* InventoryWidget;
};
