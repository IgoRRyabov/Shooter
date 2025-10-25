#include "Weapons/HitscanWeapon.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Консольная переменная: 0=off, 1=owner only, 2=all (через мультикаст)
static TAutoConsoleVariable<int32> CVarWeaponShowTraces(
	TEXT("wpn.ShowTraces"),
	1,
	TEXT("Show hitscan traces: 0=off, 1=owner only, 2=all (multicast)"),
	ECVF_Cheat);


void AHitscanWeapon::PerformShot(const FVector& TraceStart, const FVector& ShotDir)
{
	const FVector TraceEnd = TraceStart + ShotDir * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(HitscanShot), /*bTraceComplex*/ true, this);
	Params.AddIgnoredActor(this);

	if (OwningCharacter) Params.AddIgnoredActor(Cast<const AActor>(OwningCharacter));
	
	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UGameplayStatics::ApplyPointDamage(
				HitActor,
				Damage,
				ShotDir,                                  // const FVector&
				Hit,                                       // const FHitResult&
				OwningCharacter ? OwningCharacter->GetController() : nullptr, // AController*
				this,                                      // AActor* damage causer (оружие)
				UDamageType::StaticClass()                 // класс урона
			);
		}
	}

	// ===== Debug Visual =====

	const int32 Mode = CVarWeaponShowTraces.GetValueOnAnyThread();
	if (Mode != 0)
	{
		const FVector DebugEnd = bHit ? Hit.ImpactPoint : TraceEnd;
		if (Mode == 2 && HasAuthority())
		{
			Multicast_DrawTrace(TraceStart, TraceEnd, bHit, bHit?Hit.ImpactPoint : FVector::ZeroVector);
		}else
		{
			const FColor LineColor = bHit ? FColor::Green : FColor::Red;
			DrawDebugLine(GetWorld(), TraceStart, DebugEnd, LineColor, false, DebugLifeTime, 0, DebugThickness);

			if (bHit)
			{
				DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.f, FColor::Yellow, false, DebugLifeTime);
			}
		}
	}

}

void AHitscanWeapon::Multicast_DrawTrace_Implementation(const FVector& Start, const FVector& End, bool bHit, const FVector& ImpactPoint)
{
	// На все машины: просто нарисовать локально
	const FColor LineColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, DebugLifeTime, 0, DebugThickness);

	if (bHit)
	{
		DrawDebugPoint(GetWorld(), ImpactPoint, 8.f, FColor::Yellow, false, DebugLifeTime);
	}
}
