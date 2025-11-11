#include "Components/StaminaComponent.h"

#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetComponentTickEnabled(false);
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	Stamina = MaxStamina;
}

void UStaminaComponent::StartDrain()
{
	if (!GetOwner() || GetOwnerRole() != ROLE_Authority) return;
	bDraining = true;
	SetComponentTickEnabled(true);
}

void UStaminaComponent::StopDrain()
{
	if (!GetOwner() || GetOwnerRole() != ROLE_Authority) return;
	bDraining = false;
	SetComponentTickEnabled(Stamina < MaxStamina);
}

void UStaminaComponent::TickComponent(float dt, ELevelTick, FActorComponentTickFunction*)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	const float Rate = bDraining ? -DrainRate: +RegenRate;
	const float Old = Stamina;
	Stamina = FMath::Clamp(Stamina + Rate * dt, 0.f, MaxStamina);

	if (!FMath::IsNearlyEqual(Old, Stamina, 0.01f))
	{
		OnStaminaChanged.Broadcast(Stamina);
		OnRep_Stamina();
	}

	if (Stamina <= 0.f && bDraining)
	{
		OnStaminaDepleted.Broadcast();
		bDraining = false;
	}
	if (Stamina >= MaxStamina && !bDraining)
	{
		SetComponentTickEnabled(false);
	}
}

void UStaminaComponent::OnRep_Stamina()
{
	OnStaminaChanged.Broadcast(Stamina);
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStaminaComponent, Stamina);
}




