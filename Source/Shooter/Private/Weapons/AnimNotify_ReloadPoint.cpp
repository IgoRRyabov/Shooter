#include "Weapons/AnimNotify_ReloadPoint.h"

#include "Character/MyCharacter.h"
#include "Weapons/BaseWeapon.h"

void UAnimNotify_ReloadPoint::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwnerChar) return;

	if (AMyCharacter* MyChar = Cast<AMyCharacter>(OwnerChar))
	{
		if (ABaseWeapon* Weapon = MyChar->GetCurrentWeapon()) // или MyChar->CurrentWeapon
		{
			if (OwnerChar->HasAuthority())
			{
				Weapon->HandleReloadPoint_Server();
			}
			else
			{
				Weapon->Server_HandleReloadPoint();
			}
		}
	}
}
