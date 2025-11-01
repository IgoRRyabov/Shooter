#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Widgets/AmmoWidget.h"
#include "ShooterHUD.generated.h"


UCLASS()
class SHOOTER_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UAmmoWidget> AmmoWidgetClass;

	UPROPERTY()
	UAmmoWidget* AmmoWidget;

	void UpdateAmmo(int32 AmmoInClip, int32 SpareAmmo);
};
