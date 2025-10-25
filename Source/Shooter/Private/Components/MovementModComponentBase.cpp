#include "Components/MovementModComponentBase.h"
#include "Net/UnrealNetwork.h"

UMovementModComponentBase::UMovementModComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UMovementModComponentBase::ActivateMod()
{
	if (GetOwnerRole() == ROLE_Authority && !bActive)
	{
		bActive = true;
		OnRep_Active();
	}
}

void UMovementModComponentBase::DeactivateMod()
{
	if (GetOwnerRole() == ROLE_Authority && bActive)
	{
		bActive = false;
		OnRep_Active();
	}
}

void UMovementModComponentBase::OnRep_Active()
{
	OnModChanged.Broadcast(false);
}

void UMovementModComponentBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMovementModComponentBase,  bActive);
}
