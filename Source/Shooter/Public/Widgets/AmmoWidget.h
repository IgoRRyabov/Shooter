#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

UCLASS()
class SHOOTER_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void UpdateAmmoDisplay(int32 AmmoInClip, int32 SpareAmmo);
};
