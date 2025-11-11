#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickUpWidget.generated.h"

UCLASS()
class SHOOTER_API UPickUpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void SetPickUpText(const FText&  PickUpText);
};
