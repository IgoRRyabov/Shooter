#include "Weapons/BaseWeapon.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);

	// Репликация
	SetReplicates(true);
	AActor::SetReplicateMovement(true);
	bNetUseOwnerRelevancy = true;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, OwningCharacter);
	DOREPLIFETIME(ABaseWeapon, bIsEquipped);
	DOREPLIFETIME(ABaseWeapon, AmmoInClip);
	DOREPLIFETIME(ABaseWeapon, ClipSize);
	DOREPLIFETIME(ABaseWeapon, SpareAmmo);
	DOREPLIFETIME(ABaseWeapon, EquipState);
	DOREPLIFETIME(ABaseWeapon, CurrentSocket);
}

void ABaseWeapon::Equip(ACharacter* NewOwner, USkeletalMeshComponent* AttachTo, FName InSocket)
{
	if (!HasAuthority()) return;
	if (!NewOwner || !AttachTo) return;

	OwningCharacter = NewOwner;
	AttachSocket = (InSocket.IsNone() ? AttachSocket  : InSocket);

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(AttachTo, Rules, AttachSocket);

	SetOwner(NewOwner);
	bIsEquipped = true;
	OnEquipped();
}

void ABaseWeapon::Unequip()
{
	if (!HasAuthority()) return;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);
	OwningCharacter = nullptr;
	bIsEquipped = false;
	OnUnequipped();
}

void ABaseWeapon::OnRep_OwningCharacter() {}
void ABaseWeapon::OnRep_IsEquipped() {}

void ABaseWeapon::OnRep_EquipState()
{
	if (!OwningCharacter) return;
	if (USkeletalMeshComponent* OwnerMesh = OwningCharacter->FindComponentByClass<USkeletalMeshComponent>())
	{
		AttachToSocket(OwnerMesh, CurrentSocket);
	}
}

void ABaseWeapon::AttachToSocket(USkeletalMeshComponent* AttachTo, FName Socket)
{
	if (!AttachTo || Socket.IsNone()) return;
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(AttachTo, Rules, Socket);
	CurrentSocket = Socket;
}

void ABaseWeapon::EquipToHand(USkeletalMeshComponent* AttachTo, FName HandSocket)
{
	check(HasAuthority());
	EquipState = EWeaponEquipState::Equipped;
	AttachToSocket(AttachTo, HandSocket);
}

void ABaseWeapon::HolsterToBack(USkeletalMeshComponent* AttachTo, FName BackSocket)
{
	check(HasAuthority());
	EquipState = EWeaponEquipState::Holstered;
	AttachToSocket(AttachTo, BackSocket);
}


void ABaseWeapon::OnEquipped() {}
void ABaseWeapon::OnUnequipped() {}

bool ABaseWeapon::CanFire() const
{
	return EquipState == EWeaponEquipState::Equipped && AmmoInClip > 0 && OwningCharacter != nullptr;
}

void ABaseWeapon::StartFire()
{
	if (FireMode == EWeaponFireMode::SemiAuto)
	{
		Server_FireOnce();
	}
	else
	{
		Server_FireOnce();
		if (HasAuthority())
		{
			const float Period = (FireRate > 0.f) ? (1.f / FireRate) : 0.1f;
			GetWorldTimerManager().SetTimer(FireTimer, this, &ABaseWeapon::Server_FireOnce, Period, true, Period);
		}
	}
}

void ABaseWeapon::StopFire()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
	}
}

void ABaseWeapon::Server_FireOnce_Implementation()
{
	FireOnce();
}

void ABaseWeapon::FireOnce()
{
	if (!HasAuthority() || !CanFire()) return;

	if (!ConsumeAmmo(1)) return;

	FVector Start, Dir;
	GetAim(Start, Dir);

	PerformShot(Start, Dir);

	Multicast_FireFX();
}

void ABaseWeapon::Multicast_FireFX_Implementation()
{
	
}

void ABaseWeapon::GetAim(FVector& OutStart, FVector& OutDir) const
{
	OutStart = GetActorLocation();
	OutDir = GetActorForwardVector();

	if (const UCameraComponent* Cam = FindOwnerCamera())
	{
		OutStart = Cam->GetComponentLocation();
		OutDir = Cam->GetComponentRotation().Vector();
	}

	// базовый разброс
	if (BaseSpread > 0.f)
	{
		const float HalfAngle = FMath::DegreesToRadians(BaseSpread * 0.5f);
		OutDir = FMath::VRandCone(OutDir, HalfAngle);
	}
}

bool ABaseWeapon::ConsumeAmmo(int32 Amount)
{
	if (AmmoInClip < Amount) return false;
	AmmoInClip -= Amount;
	OnRep_Ammo();
	return true;
}

void ABaseWeapon::OnRep_Ammo()
{
	// обновить HUD/виджеты;
}

void ABaseWeapon::Reload()
{
	if (!HasAuthority()) return;
	if (!CanReload()) return;

	const int32 Need = ClipSize - AmmoInClip;
	const int32 Take = FMath::Min(Need, SpareAmmo);

	AmmoInClip += Take;
	SpareAmmo -= Take;
	OnRep_Ammo();
}

void ABaseWeapon::SetOwningCharacter(ACharacter* NewOwner)
{
	check(HasAuthority());
	OwningCharacter =  NewOwner;
	SetOwner(OwningCharacter);
	//SetOwner(nullptr);
}

UCameraComponent* ABaseWeapon::FindOwnerCamera() const
{
	if (!OwningCharacter) return nullptr;

	TArray<UCameraComponent*> Cams;
	OwningCharacter->GetComponents<UCameraComponent>(Cams);
	return Cams.Num() > 0 ? Cams[0] : nullptr;
}

void ABaseWeapon::PerformShot(const FVector& TraceStart, const FVector& ShotDir)
{
	// базовый класс ничего не делает.
	// Наследники (хитскан/снарядное) реализуют конкретную логику.
}








