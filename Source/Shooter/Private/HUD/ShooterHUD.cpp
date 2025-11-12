#include "HUD/ShooterHUD.h"

#include "Interfaces/Pickupable.h"

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

	if (PickUpWidgetClass)
	{
		PickUpWidget = CreateWidget<UPickUpWidget>(GetWorld(), PickUpWidgetClass);
		if (PickUpWidget)
		{
			PickUpWidget->AddToViewport();
			PickUpWidget->SetVisibility(ESlateVisibility::Hidden);
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

void AShooterHUD::UpdatePickupPrompt(bool isVisible)
{
	if (isVisible)
	{
		PickUpWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PickUpWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
