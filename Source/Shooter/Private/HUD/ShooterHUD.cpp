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

	if (HealthWidgetClass)
	{
		HealthWidget = CreateWidget<UHealthWidget>(GetWorld(), HealthWidgetClass);
		if (HealthWidget)
		{
			HealthWidget->AddToViewport();
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

void AShooterHUD::UpdateHealth(float NewHealth, float MaxHealth)
{
	if (HealthWidget)
	{
		HealthWidget->UpdateHealthDisplay(NewHealth, MaxHealth);
	}
}

void AShooterHUD::UpdateStamina(float NewStamina)
{
	if (HealthWidget)
	{
		HealthWidget->UpdateStaminaDipslay(NewStamina);
	}
}
