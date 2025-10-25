#include "Components/MovementModsManagerComponent.h"
#include "Components/MovementModComponentBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UMovementModsManagerComponent::UMovementModsManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UMovementModsManagerComponent::Initialize(UCharacterMovementComponent* InMoveComp)
{
	MoveComp = InMoveComp;
	CurrentSpeed = BaseMaxWalkSpeed;
	if (MoveComp) MoveComp->MaxWalkSpeed = BaseMaxWalkSpeed;
}

void UMovementModsManagerComponent::RegisterMod(UMovementModComponentBase* Mod)
{
	if (!Mod) return;
	Mods.AddUnique(Mod);
	// подписка на изменения конкретного мода
	Mod->OnModChanged.AddDynamic(this, &UMovementModsManagerComponent::RecomputeAndApply);
	RecomputeAndApply(true);
}

void UMovementModsManagerComponent::UnregisterMod(UMovementModComponentBase* Mod)
{
	if (!Mod) return;
	Mods.Remove(Mod);
	RecomputeAndApply(true);
}

void UMovementModsManagerComponent::RecomputeAndApply(bool bForce)
{
	FMovementModifiers Combined;
	Mods.Sort([](const TWeakObjectPtr<UMovementModComponentBase>& A, const TWeakObjectPtr<UMovementModComponentBase>& B){
		const auto* LA = A.Get(); const auto* LB = B.Get();
		return (LA ? LA->Priority : 0) < (LB ? LB->Priority : 0);
	});

	for (const auto& W : Mods)
	{
		if (const auto* M = W.Get())
		{
			if (M->IsModActive())
				Combined.Combine(M->GetModifiers());
		}
	}

	const float NewSpeed = FMath::Max(150.f, BaseMaxWalkSpeed * Combined.SpeedMul);
	const float NewAccel = BaseMaxAcceleration * Combined.AccelMul;
	const float NewFriction = BaseGroundFriction * Combined.FrictionMul;

	if (MoveComp && (bForce || !FMath::IsNearlyEqual(NewSpeed, CurrentSpeed)))
	{
		CurrentSpeed = NewSpeed;

		if (GetOwnerRole() == ROLE_Authority)
		{
			MoveComp->MaxWalkSpeed       = NewSpeed;
			MoveComp->MaxAcceleration    = NewAccel;
			MoveComp->GroundFriction     = NewFriction;
			// репликацию CurrentSpeed возьмут клиенты → OnRep_CurrentSpeed
		}
	}
}

void UMovementModsManagerComponent::OnRep_CurrentSpeed()
{
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = CurrentSpeed;
	}
}

void UMovementModsManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMovementModsManagerComponent, CurrentSpeed);
}
