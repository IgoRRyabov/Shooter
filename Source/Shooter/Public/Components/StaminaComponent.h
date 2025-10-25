#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepleted);

UCLASS( ClassGroup=(Attributes), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stamina")
	float DrainRate = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stamina")
	float RegenRate = 15.f;

	UFUNCTION(BlueprintCallable)
	float GetStamina() const {return Stamina;};

	UFUNCTION(BlueprintCallable)
	bool HasStamina(float Amount) const{return Stamina >= Amount;}

	void StartDrain();
	void StopDrain();

	UPROPERTY(BlueprintAssignable) FOnStaminaChanged OnStaminaChanged;
	UPROPERTY(BlueprintAssignable) FOnStaminaDepleted OnStaminaDepleted;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_Stamina)
	float Stamina = 0.f;

	UFUNCTION()
	void OnRep_Stamina();

	bool bDraining = false;
};
