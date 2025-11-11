#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

UCLASS()
class SHOOTER_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void UpdateHealthDisplay(float NewHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void UpdateStaminaDipslay(float NewStamina);
};
