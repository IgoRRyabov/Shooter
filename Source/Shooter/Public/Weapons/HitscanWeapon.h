#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "HitscanWeapon.generated.h"

UCLASS()
class SHOOTER_API AHitscanWeapon : public ABaseWeapon
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Fire")
	float Range = 15000.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Fire")
	float Damage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Debug")
	float DebugLifeTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Debug")
	float DebugThickness = 1.5f;
	
	virtual void PerformShot(const FVector& TraceStart, const FVector& ShotDir) override;
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DrawTrace(const FVector& Start, const FVector& End, bool bHit, const FVector& ImpactPoint);
};
