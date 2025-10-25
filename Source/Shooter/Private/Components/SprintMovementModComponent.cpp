#include "Components/SprintMovementModComponent.h"

void USprintMovementModComponent::Initialize(UStaminaComponent* InStamina, float InSprintMul)
{
	Stamina = InStamina;
	SprintMultipliers = InSprintMul;

	ensureMsgf(Stamina, TEXT("Sprint needs StaminaComponent"));

	if (Stamina)
	{
		Stamina->OnStaminaDepleted.AddDynamic(this, &USprintMovementModComponent::HandleStaminaDepleted);
	}
}

void USprintMovementModComponent::Input_SprintStart()
{
	if (!Stamina || !Stamina->HasStamina(5.f)) return;
	if (GetOwnerRole() != ROLE_Authority) return;

	ActivateMod();
	Stamina->StartDrain();
}

void USprintMovementModComponent::Input_SprintStop()
{
	if (GetOwnerRole() != ROLE_Authority) return;
	DeactivateMod();
	if (Stamina) Stamina->StopDrain();
}

FMovementModifiers USprintMovementModComponent::GetModifiers() const
{
	FMovementModifiers M;
	if (IsModActive()) M.SpeedMul = SprintMultipliers;
	return M;
}

void USprintMovementModComponent::HandleStaminaDepleted()
{
	if (GetOwnerRole() != ROLE_Authority) return;
	DeactivateMod();
	if (Stamina) Stamina->StopDrain();
}
