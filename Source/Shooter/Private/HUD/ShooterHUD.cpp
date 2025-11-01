#include "HUD/ShooterHUD.h"

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();

	if (AmmoWidgetClass)
	{
		AmmoWidget = CreateWidget<UAmmoWidget>(GetWorld(), AmmoWidgetClass);
		if (AmmoWidget)
		{
			AmmoWidget->AddToViewport();
		}
	}
}

void AShooterHUD::UpdateAmmo(int32 AmmoInClip, int32 SpareAmmo)
{
	if (AmmoWidget)
	{
		AmmoWidget->UpdateAmmoDisplay(AmmoInClip, SpareAmmo);
	}
}
