#pragma once

#include "CoreMinimal.h"
#include "StaminaComponent.h"
#include "Components/MovementModComponentBase.h"
#include "SprintMovementModComponent.generated.h"

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class SHOOTER_API USprintMovementModComponent : public UMovementModComponentBase
{
	GENERATED_BODY()

public:
	void Initialize(UStaminaComponent* InStamina, float InSprintMul = 1.6f);

	//Ввод
	void Input_SprintStart();
	void Input_SprintStop();

	// UMovementModComponentBase
	virtual FMovementModifiers GetModifiers() const override;

protected:
	UPROPERTY()
	UStaminaComponent* Stamina = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Sprint")
	float SprintMultipliers = 1.f;

	//автосброс, когда стамина закочилась
	UFUNCTION()
	void HandleStaminaDepleted();
};
