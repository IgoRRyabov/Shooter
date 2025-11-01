#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class ACharacter;
class UCameraComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class EWeaponEquipState : uint8
{
	Holstered,   // на спине
	Equipped     // в руках
};

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	SemiAuto,
	FullAuto
};

UCLASS(Abstract, Blueprintable)
class SHOOTER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(ReplicatedUsing=OnRep_OwningCharacter, BlueprintReadOnly, Category="Weapon")
	ACharacter* OwningCharacter = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipped, BlueprintReadOnly, Category="Weapon")
	bool bIsEquipped = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FName AttachSocket = TEXT("Weapon_R");

	UPROPERTY(ReplicatedUsing=OnRep_EquipState, BlueprintReadOnly, Category="Weapon")
	EWeaponEquipState EquipState = EWeaponEquipState::Holstered;

	// Текущий сокет (для OnRep-перепривязки на клиентах)
	UPROPERTY(Replicated)
	FName CurrentSocket;
	
	UFUNCTION()
	void OnRep_OwningCharacter();

	UFUNCTION()
	void OnRep_IsEquipped();

	UFUNCTION()
	void OnRep_EquipState();

	// внутренняя привязка к сокету
	void AttachToSocket(USkeletalMeshComponent* AttachTo, FName Socket);

public:
	// Экипировать конкретному SkeletalMesh
	UFUNCTION(BlueprintCallable, Category="Weapon")
	virtual void Equip(ACharacter* NewOwner, USkeletalMeshComponent* AttachTo, FName InSocket = NAME_None);

	//Снять/убрать
	UFUNCTION(BlueprintCallable, Category="Weapon")
	virtual void Unequip();

protected:
	virtual void OnEquipped();
	virtual void OnUnequipped();

public:
	// Внешние команды ввода
	UFUNCTION(BlueprintCallable, Category="Weapon|Fire")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category="Weapon|Fire")
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category="Weapon|Fire")
	virtual void StartReload();

	UFUNCTION()
	int32 GetAmmoInClip() const {return AmmoInClip;}
	UFUNCTION()
	int32 GetSpareAmmo() const {return SpareAmmo;}

	UFUNCTION(Server, Reliable)
	void Server_HandleReloadPoint();

	UFUNCTION()
	virtual void HandleReloadPoint_Server();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Fire")
	EWeaponFireMode FireMode = EWeaponFireMode::FullAuto;

	// Выстрелов в секунду
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Fire")
	float FireRate = 10.0f;

	// Разброс в градусах (базовый)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Fire")
	float BaseSpread = 0.5f;

	FTimerHandle FireTimer;

	// Серверная точка входа одного выстрела
	UFUNCTION(Server, Reliable)
	void Server_FireOnce();

	UFUNCTION(Server, Reliable)
	void Server_Fire();

	UFUNCTION(Server, Reliable)
	void Server_StopFire();

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	UFUNCTION(Server, Reliable)
	void Server_CancelRelaod();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayReloadMontage(bool bPlay);
	
	// Косметика всем клиентам
	UFUNCTION(NetMulticast,Unreliable)
	void Multicast_FireFX();

	// Главный шаг выстрела (сервер): рассчитываем луч/направление и зовём PerformShot()
	virtual void FireOnce();

	// Получить старт/направление прицеливания (по умолчанию — из камеры владельца)
	virtual void GetAim(FVector& OutStart, FVector& OutDir) const;

	// Сама «физика» выстрела (попадания/спавн снаряда) — ПЕРЕОПРЕДЕЛЯТЬ в наследниках
	virtual void PerformShot(const FVector& TraceStart, const FVector& ShotDir);

	// Можно ли стрелять сейчас (патроны, перезарядка, стейт)
	virtual bool CanFire() const;

	/** === Ammo === */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Ammo")
	int32 ClipSize = 30;  // Вместимость магазина

	UPROPERTY(ReplicatedUsing=OnRep_Ammo, BlueprintReadOnly, Category="Weapon|Ammo")
	int32 AmmoInClip = 30; // Кол-во патронов в магазине

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Ammo")
	int32 SpareAmmo = 90; // Общее кол-во патронов

	UFUNCTION()
	void UpdateLocalHUD();
	UFUNCTION()
	void OnRep_Ammo();

	virtual bool ConsumeAmmo(int32 Amount = 1);

public:
	UPROPERTY(ReplicatedUsing=OnRep_IsReloading, BlueprintReadOnly, Category="Weapon|Ammo")
	bool bIsReloading = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Ammo")
	float ReloadDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Ammo")
	UAnimMontage* ReloadMontage =  nullptr;

	FTimerHandle ReloadTimerHandle;

	void StartReload_Server();
	void FinishReload_Server();
	void CancelReload_Server();

	virtual void OnReloadStarted();
	virtual void OnReloadFinished();
	
	UFUNCTION()
	void OnRep_IsReloading();
	
	UFUNCTION(BlueprintCallable, Category="Weapon|Ammo")
	bool CanReload() const { return AmmoInClip < ClipSize && SpareAmmo > 0; }

	UFUNCTION(BlueprintCallable, Category="Weapon|Ammo")
	virtual void Reload(); // упрощённо, без анимации

	// сервер: назначает владельца (equip/holster делают Attach)
	virtual void SetOwningCharacter(ACharacter* NewOwner);

	// сервер: в руки
	virtual void EquipToHand(USkeletalMeshComponent* AttachTo, FName HandSocket);

	// сервер: на спину
	virtual void HolsterToBack(USkeletalMeshComponent* AttachTo, FName BackSocket);

	// удобный геттер
	bool IsEquipped() const { return EquipState == EWeaponEquipState::Equipped; }

protected:
	// Удобный доступ к камере владельца, если есть
	UCameraComponent* FindOwnerCamera() const;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

