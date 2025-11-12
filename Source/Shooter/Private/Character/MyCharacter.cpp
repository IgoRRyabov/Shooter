#include "Character/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SprintMovementModComponent.h"
#include "HUD/ShooterHUD.h"
#include "Interfaces/Pickupable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/BaseWeapon.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Сетевой персонаж (перемещение уже реплицируется базовым Character)
	bReplicates = true;

	// Повороты: вращаемся в сторону движения, камера — от контроллера
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll  = false;

	// Камера: бум + смещённая камера “за плечом”
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 60.f, 60.f); // вправо/вверх чуть-чуть

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Тюнинг движения
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.25f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true; // хотим поворот к контроллеру
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	
	MovementMods	= CreateDefaultSubobject<UMovementModsManagerComponent>(TEXT("MovementModsManager"));
	Stamina			= CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina"));
	SprintMod		= CreateDefaultSubobject<USprintMovementModComponent>(TEXT("SprintMod"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	Inventory		= CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MovementMods->Initialize(GetCharacterMovement());

	SprintMod->Initialize(Stamina, 2.f);
	SprintMod->SetManager(MovementMods);
	MovementMods->RegisterMod(SprintMod);

	if (IsLocallyControlled())
	{
		PC_Connect();
		if (PC) HUD_Connect();
	}
	// Прокинем Mapping Context локальному игроку

	if (APlayerController* PC_Local = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC_Local->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				if (DefaultIMC)
				{
					Subsys->AddMappingContext(DefaultIMC, /*Priority*/0);
				}
			}
		}
	}
	
	
	if (HasAuthority() && DefaultWeaponClass)
	{
		SpawnAndHolster(DefaultWeaponClass);
	}

	if (HasAuthority())
	{
		bEquip = false;
		CanSprint = true;
	}

	if (CurrentWeapon)
	{
		if (IsLocallyControlled())
		{
			if (HUD) HUD_Connect();
			HUD->UpdateAmmo(CurrentWeapon->GetAmmoInClip(), CurrentWeapon->GetSpareAmmo());
		}
	}

	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &AMyCharacter::OnHealthChanged_Client);
		HealthComponent->OnDeath.AddDynamic(this, &AMyCharacter::OnDeath);

		if (Stamina)
		{
			Stamina->OnStaminaChanged.AddDynamic(this, &AMyCharacter::OnStaminaChanged_Client);
			OnStaminaChanged_Client(Stamina->GetStamina());
		}
		if (IsLocallyControlled())
		{
			if (HUD) HUD_Connect();
			HUD->UpdateHealth(HealthComponent->Health, HealthComponent->MaxHealth);
		}
	}
}

void AMyCharacter::AddItemToInventory(FName ItemID, int32 Quantity)
{
	if (HasAuthority())
	{
		Inventory->ServerAddItem(ItemID, Quantity);
	}
}

void AMyCharacter::HUD_Connect()
{
	if (!IsLocallyControlled()) return;
	
	if (!PC) PC_Connect();
	HUD = Cast<AShooterHUD>(PC->GetHUD());
}

void AMyCharacter::PC_Connect()
{
	if (!IsLocallyControlled()) return;
	
	PC = Cast<APlayerController>(GetController());
}

void AMyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!IsLocallyControlled()) return;
	
	PerformInteractionTrace();
	
	const FRotator AimRot = GetBaseAimRotation();
	const FRotator ActorRot = GetActorRotation();

	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRot, ActorRot);

	FVector2D NewOffset(DeltaRot.Yaw, DeltaRot.Pitch);
	
	NewOffset.X = FMath::Clamp(NewOffset.X, -90.f, 90.f);
	NewOffset.Y = FMath::Clamp(NewOffset.Y, -90.f, 90.f);
	
	AimOffset = NewOffset;

	if (HasAuthority() == false)
	{
		Server_UpdateAimOffset(NewOffset);
	}
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
		
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Move) EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		if (IA_Look) EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

		if (IA_Jump)
		{
			EIC->BindAction(IA_Jump, ETriggerEvent::Started,   this, &ACharacter::Jump);
			EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (IA_Sprint)
		{
			EIC->BindAction(IA_Sprint, ETriggerEvent::Started,   this, &AMyCharacter::SprintStart);
			EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AMyCharacter::SprintStop);
		}
		if (IA_Fire)
		{
			if (CurrentWeapon)
			{
				EIC->BindAction(IA_Fire, ETriggerEvent::Started, this, &AMyCharacter::StartFire);
				EIC->BindAction(IA_Fire, ETriggerEvent::Completed, this, &AMyCharacter::StopFire);
			}
		}
		if (IA_Reload)
		{
			EIC->BindAction(IA_Reload, ETriggerEvent::Completed, this, &AMyCharacter::Reload);
		}
		if (IA_EquipSlot1)
		{
			EIC->BindAction(IA_EquipSlot1, ETriggerEvent::Started, this, &AMyCharacter::EquipSlot1_Pressed);
		}
		if (IA_Aim)
		{
			EIC->BindAction(IA_Aim, ETriggerEvent::Started, this, &AMyCharacter::StartAiming);
		}
		if (IA_PickUp)
		{
			EIC->BindAction(IA_PickUp, ETriggerEvent::Completed, this, &AMyCharacter::TryPickup);	
		}
	}
}

void AMyCharacter::TryPickup()
{
	if (!HasAuthority())
	{
		TryPickup_Server();
	}
}

void AMyCharacter::PerformInteractionTrace()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetControlRotation().Vector() * 300.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	AActor* HitActor = (bHit ? Hit.GetActor() : nullptr);

	if (HitActor && HitActor->Implements<UPickupable>())
	{
		if (HitActor != FocusedPickup)
		{
			FocusedPickup = HitActor;
			HUD->UpdatePickupPrompt(true);
		}
	}
	else
	{
		FocusedPickup = nullptr;
		HUD->UpdatePickupPrompt(false);
	}	
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!Controller || Axis.IsNearlyZero()) return;

	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right,   Axis.X);
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AMyCharacter::SprintStart(const FInputActionValue& Value)
{
	Server_SprintStart();
}

void AMyCharacter::SprintStop(const FInputActionValue& Value)
{
	Server_SprintStop();
}

void AMyCharacter::TryPickup_Server_Implementation()
{
	if (!HasAuthority()) return;

	FVector Start = GetActorLocation();
	FVector End = Start + GetControlRotation().Vector() * 300.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	AActor* HitActor = (bHit ? Hit.GetActor() : nullptr);


	if (HitActor && HitActor->Implements<UPickupable>())
	{
		IPickupable::Execute_OnPickedUp(HitActor, this);
	}
}

void AMyCharacter::Server_SetSprinting_Implementation(bool bNewSprinting)
{
	bIsSprinting = bNewSprinting;
	OnRep_Sprinting();
}

void AMyCharacter::OnHealthChanged_Client(float NewHealth, float Delta)
{
	UE_LOG(LogTemp, Log, TEXT("%s: Health changed -> %.1f (Δ %.1f)"), *GetName(), NewHealth, Delta);

	if (IsLocallyControlled())
	{
		HUD->UpdateHealth(NewHealth, HealthComponent->MaxHealth);
	}
}

void AMyCharacter::OnStaminaChanged_Client(float NewStamina)
{
	if (IsLocallyControlled())
	{
		HUD->UpdateStamina(NewStamina);
	}
}

void AMyCharacter::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: died"), *GetName());

	if (HasAuthority())
	{
		Multicast_OnDeath();
	}
}

void AMyCharacter::Multicast_OnDeath_Implementation()
{
	if (USkeletalMeshComponent* MyMesh = GetMesh())
	{
		if (AController* C = GetController())
		{
			DisableInput(Cast<APlayerController>(C));
		}

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
			MoveComp->DisableMovement();
		}

		MyMesh->SetCollisionProfileName(TEXT("Ragdoll"));
		MyMesh->SetAllBodiesSimulatePhysics(true);
		MyMesh->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMyCharacter::OnRep_Sprinting()
{
	//GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void AMyCharacter::OnRep_Aiming()
{
	
}

void AMyCharacter::Server_Aiming_Implementation()
{
	if (!HasAuthority() || !bEquip) return;
	bIsAiming = !bIsAiming;
	CanSprint = !bIsAiming;
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, bIsSprinting);
	DOREPLIFETIME(AMyCharacter, CurrentWeapon);
	DOREPLIFETIME_CONDITION(AMyCharacter, AimOffset, COND_SkipOwner);
}

void AMyCharacter::Server_SprintStart_Implementation()
{
	if (!HasAuthority() || !CanSprint) return;
	if (SprintMod) SprintMod->Input_SprintStart();
}

void AMyCharacter::Server_SprintStop_Implementation()
{
	if (!HasAuthority()) return;
	if (SprintMod) SprintMod->Input_SprintStop();
}

void AMyCharacter::Server_SpawnDefaultWeapon_Implementation()
{
	if (!HasAuthority()) return;
	if (DefaultWeaponClass)
	{
		SpawnAndHolster(DefaultWeaponClass);
	}
}

void AMyCharacter::Server_TogglePrimary_Implementation()
{
	if (!HasAuthority() || !CurrentWeapon) return;

	bEquip = CurrentWeapon->IsEquipped();
	
	if (!bEquip)
	{
		// Взять в руки
		CurrentWeapon->EquipToHand(GetMesh(), HandSocket);
		bEquip = true;
	}else
	{
		// Убрать за спину
		CurrentWeapon->HolsterToBack(GetMesh(), BackSocket);
		bEquip = false;
	}
}

void AMyCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AMyCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AMyCharacter::Reload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void AMyCharacter::OnRep_Equipped()
{
	Multicast_PlayEquipHolsterMontage();
}

void AMyCharacter::SpawnAndHolster(TSubclassOf<ABaseWeapon> WeaponClass)
{
	if (!HasAuthority() || !WeaponClass) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABaseWeapon* W = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, Params);
	if (!W) return;

	W->SetOwningCharacter(this);
	CurrentWeapon = W;

	W->HolsterToBack(GetMesh(), BackSocket);
}

void AMyCharacter::EquipSlot1_Pressed(const struct FInputActionValue&)
{
	Server_TogglePrimary();
}

void AMyCharacter::SpawnAndEquip(TSubclassOf<ABaseWeapon> WeaponClass)
{
	if (!HasAuthority() || !WeaponClass) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Unequip();
		CurrentWeapon->Destroy();
		CurrentWeapon =  nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABaseWeapon* NewWeap = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, Params);
	if (!NewWeap) return;

	CurrentWeapon =  NewWeap;
	NewWeap->Equip(this, GetMesh(), WeaponAttachSocket);
}

void AMyCharacter::StartAiming()
{
	Server_Aiming();
}

void AMyCharacter::OnRep_AimOffset() {}

void AMyCharacter::Server_UpdateAimOffset_Implementation(FVector2D NewAimOffset)
{
	AimOffset = NewAimOffset;
}

void AMyCharacter::Multicast_PlayEquipHolsterMontage_Implementation()
{
	if (!EquipHolsterMontage) return;

	if (USkeletalMeshComponent* MyMesh = GetMesh())
	{
		if (UAnimInstance* Anim = MyMesh->GetAnimInstance())
		{
			if (Anim->Montage_IsPlaying(EquipHolsterMontage) == false)
			{
				Anim->Montage_Play(EquipHolsterMontage, 1.f);
			}
			const FName Section = bEquip ? EquipSectionName :  HolsterSectionName;
			if (Section != NAME_None)
			{
				Anim->Montage_JumpToSection(Section,  EquipHolsterMontage);
			}
		}
	}
}

void AMyCharacter::OnRep_CurrentWeapon(){}
