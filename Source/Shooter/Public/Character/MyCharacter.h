#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/HealthComponent.h"
#include "Components/InventoryComponent.h"
#include "Weapons/BaseWeapon.h"
#include "Widgets/PickUpWidget.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

class UStaminaComponent;
class UMovementModsManagerComponent;
class USprintMovementModComponent;
class AShooterHUD;

UCLASS()
class SHOOTER_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	ABaseWeapon* GetCurrentWeapon() {return CurrentWeapon;}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	UInventoryComponent* Inventory;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UMovementModsManagerComponent* MovementMods;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaminaComponent* Stamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USprintMovementModComponent* SprintMod;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UHealthComponent* HealthComponent;
	
	/** === Input (Enhanced Input) === */
	// Data Assets — проставишь в BP/в деталях класса
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputMappingContext* DefaultIMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_Move; // Axis2D (X=Right, Y=Forward)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_Look; // Axis2D (X=Yaw,   Y=Pitch)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_Jump; // Digital

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_Fire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_Reload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_EquipSlot1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_Aim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* IA_PickUp;

	// Состояние спринта
	UPROPERTY(ReplicatedUsing=OnRep_Sprinting)
	bool bIsSprinting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	bool CanSprint = false;

	UPROPERTY(ReplicatedUsing=OnRep_Aiming, BlueprintReadOnly)
	bool bIsAiming = false;

	// Скорость ходьбы (удобно править из деталей)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float WalkSpeed = 300.f;

	UFUNCTION()
	void OnHealthChanged_Client(float NewHealth, float Delta);

	UFUNCTION()
	void OnStaminaChanged_Client(float NewStamina);
	
	UFUNCTION()
	void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();
	
	UFUNCTION()
	void OnRep_Sprinting();

	UFUNCTION()
	void OnRep_Aiming();

	// RPC на сервер — менять скорость только там
	UFUNCTION(Server, Reliable)
	void Server_SetSprinting(bool bNewSprinting);

	// Handlers
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintStop(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void TryPickup_Server();

	UFUNCTION()
	void TryPickup();

	UFUNCTION()
	void PerformInteractionTrace();
	
	// RPC, если спринтом управляет сервер
	UFUNCTION(Server, Reliable)
	void Server_SprintStart();

	UFUNCTION(Server, Reliable)
	void Server_SprintStop();

	UFUNCTION(Server, Reliable)
	void Server_Aiming();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Weapon !!!!

	/** === Weapon === */
	// Класс оружия по умолчанию (задашь в BP)
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<ABaseWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName HandSocket = TEXT("Weapon_R");

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName BackSocket = TEXT("Weapon_Back");
	
	// Сокет на скелет-меше персонажа (куда крепим)
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName WeaponAttachSocket = TEXT("Weapon_R"); // назови как в твоём скелете

	// Текущее оружие (реплицируется)
	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon)
	ABaseWeapon* CurrentWeapon = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_Equipped, BlueprintReadOnly)
	bool bEquip = false;

	UFUNCTION()
	void OnRep_CurrentWeapon();
	// Спавним и экипируем оружие (сервер)
	UFUNCTION(Server, Reliable)
	void Server_SpawnDefaultWeapon();

	UFUNCTION(Server, Reliable) void Server_TogglePrimary();

	UFUNCTION()
	void StartFire();
	
	UFUNCTION()
	void StopFire();

	UFUNCTION()
	void Reload();

	UFUNCTION()
	void OnRep_Equipped();
	
	void SpawnAndHolster(TSubclassOf<ABaseWeapon> WeaponClass);

	// Input (Enhanced Input): обработчик "1"
	void EquipSlot1_Pressed(const struct FInputActionValue&);

	// Вспомогательная: реально спавнит/крепит (только сервер её вызывает)
	void SpawnAndEquip(TSubclassOf<ABaseWeapon> WeaponClass);

	void StartAiming();
	// !!! Animations !!!

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animations");
	UAnimMontage* EquipHolsterMontage ;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animations")
	FName EquipSectionName = TEXT("Equip");

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animations")
	FName HolsterSectionName = TEXT("Holster");

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEquipHolsterMontage();

	UPROPERTY(ReplicatedUsing=OnRep_AimOffset)
	FVector2D AimOffset;
	
	UFUNCTION()
	void OnRep_AimOffset();

	// RPC для обновления сервером
	UFUNCTION(Server, Unreliable)
	void Server_UpdateAimOffset(FVector2D NewAimOffset);
	
public:
	// Доступ к текущему оружию
	UFUNCTION(BlueprintCallable, Category="Weapon")
	ABaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category="Aim Offset")
	FVector2D GetAimOffset() const { return AimOffset; }

	UFUNCTION()
	UHealthComponent* GetHealsComponent() const {return HealthComponent;};

	UFUNCTION()
	void AddItemToInventory(FName ItemID, int32 Quantity = 1);
	
private:
	AShooterHUD* HUD;
	APlayerController* PC;
	AActor* FocusedPickup;
	
	void HUD_Connect();
	void PC_Connect();
};
