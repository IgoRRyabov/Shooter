#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementModsManagerComponent.generated.h"

class UCharacterMovementComponent;
class UMovementModComponentBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UMovementModsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMovementModsManagerComponent();

	void Initialize(UCharacterMovementComponent* InMoveComp);

	// Регистрация мода
	void RegisterMod(UMovementModComponentBase* Mod);
	void UnregisterMod(UMovementModComponentBase* Mod);

	// Применить агрегированное состояние
	UFUNCTION()
	void RecomputeAndApply(bool bForce = false);

protected:
	UPROPERTY()
	UCharacterMovementComponent* MoveComp =  nullptr;

	UPROPERTY()
	TArray<TWeakObjectPtr<UMovementModComponentBase>> Mods;

	// Базовые значения движения
	UPROPERTY(EditDefaultsOnly, Category="Defaults")
	float BaseMaxWalkSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, Category="Defaults")
	float BaseMaxAcceleration = 2048.f;
	UPROPERTY(EditDefaultsOnly, Category="Defaults")
	float BaseGroundFriction = 8.f;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentSpeed)
	float CurrentSpeed = 600.f;

	UFUNCTION()
	void OnRep_CurrentSpeed();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
