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
		Server_Fire();
	}
}

void ABaseWeapon::StopFire()
{
	Server_StopFire();
}

void ABaseWeapon::StartReload()
{
	
}

void ABaseWeapon::Server_FireOnce_Implementation()
{
	FireOnce();
}

void ABaseWeapon::Server_Fire_Implementation()
{
	if (HasAuthority())
	{
		const float Period = (FireRate > 0.f) ? (1.f / FireRate) : 0.1f;
		GetWorldTimerManager().SetTimer(FireTimer, this, &ABaseWeapon::Server_FireOnce, Period, true, Period);
	}
}

void ABaseWeapon::Server_StopFire_Implementation()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
	}
}

void ABaseWeapon::Server_HandleReloadPoint_Implementation()
{
	
}

void ABaseWeapon::Server_Reload_Implementation()
{
	StartReload_Server();
}

void ABaseWeapon::StartReload_Server()
{
	if (!HasAuthority()) return;
	if (!CanFire()) return;

	bIsReloading = true;
	OnRep_IsReloading();

	Multicast_PlayReloadMontage(true);

	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ABaseWeapon::FinishReload_Server, ReloadDuration, false);

	OnReloadStarted();
}

void ABaseWeapon::FinishReload_Server()
{
	if (!HasAuthority()) return;

	const int32 Need = ClipSize - AmmoInClip;
	const int32 ToTake = FMath::Min(Need, SpareAmmo);

	AmmoInClip += ToTake;
	SpareAmmo  -= ToTake;

	bIsReloading = false;
	OnRep_IsReloading();

	Multicast_PlayReloadMontage(false);

	OnRep_Ammo();
	OnReloadFinished();
}

void ABaseWeapon::CancelReload_Server()
{
	if (!HasAuthority()) return;

	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	bIsReloading = false;
	OnRep_IsReloading();

	Multicast_PlayReloadMontage(false);
}

void ABaseWeapon::OnRep_IsReloading()
{
}

void ABaseWeapon::Server_CancelRelaod_Implementation()
{
	CancelReload_Server();
}

void ABaseWeapon::Multicast_PlayReloadMontage_Implementation(bool bPlay)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;
	USkeletalMeshComponent* Mesh = OwnerChar->GetMesh();
	if (!Mesh) return;
	UAnimInstance* Anim = Mesh->GetAnimInstance();
	if (!Anim) return;

	if (bPlay)
	{
		if (ReloadMontage)
		{
			// Если уже играет, перепрыгнуть в начало
			if (!Anim->Montage_IsPlaying(ReloadMontage))
			{
				Anim->Montage_Play(ReloadMontage, 1.f);
			}
			else
			{
				Anim->Montage_Stop(0.0f, ReloadMontage);
				Anim->Montage_Play(ReloadMontage, 1.f);
			}
		}
	}
	else
	{
		if (ReloadMontage && Anim->Montage_IsPlaying(ReloadMontage))
		{
			Anim->Montage_Stop(0.15f, ReloadMontage);
		}
	}
	
}

void ABaseWeapon::HandleReloadPoint_Server()
{
	if (!HasAuthority() || !bIsReloading) return;

	const int32  Need = ClipSize - AmmoInClip;
	const int32 ToTake = FMath::Min(Need, SpareAmmo);

	AmmoInClip += ToTake;
	SpareAmmo -= ToTake;

	OnRep_Ammo();

	UE_LOG(LogTemp, Warning, TEXT("Reload point reached: Ammo = %d/%d"), AmmoInClip, SpareAmmo);
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
	if (!CanReload()) return;
	
	if (HasAuthority())
	{
		StartReload_Server();
	}else
	{
		Server_Reload();
	}
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








