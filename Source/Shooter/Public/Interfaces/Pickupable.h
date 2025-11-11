#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickupable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

class SHOOTER_API IPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	void OnPickedUp(class AMyCharacter* Picker);

	/** Текст для UI (например: "[E] Поднять аптечку") */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	FText GetPickupText() const;

	/** Позиция, где отображается подсказка (например, над предметом) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	FVector GetPickupLocation() const;
};
