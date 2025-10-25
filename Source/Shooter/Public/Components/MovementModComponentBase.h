#pragma once

#include "CoreMinimal.h"
#include "MovementModsManagerComponent.h"
#include "Components/ActorComponent.h"
#include "Structs/MovementModifiers.h"
#include "MovementModComponentBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementModChanged, bool, bForce);

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UMovementModComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMovementModComponentBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MovementMod")
	int32 Priority = 0;

	// Активен ли мод
	UFUNCTION(BlueprintCallable, Category="MovementMod")
	bool IsModActive() const {return bActive;}

	// Включить/выключить
	UFUNCTION(BlueprintCallable, Category = "MovementMod")
	virtual void ActivateMod();
	UFUNCTION(BlueprintCallable, Category = "MovementMod")
	virtual void DeactivateMod();

	// что именно мод меняет
	virtual FMovementModifiers GetModifiers() const { return FMovementModifiers(); }

	// менеджео прокинем из персонажа
	void SetManager(UMovementModsManagerComponent* InManager) {Manager = InManager;}

	UPROPERTY(BlueprintAssignable)
	FOnMovementModChanged OnModChanged;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_Active)
	bool bActive = false;
	
	UFUNCTION()
	void OnRep_Active();

	UPROPERTY()
	UMovementModsManagerComponent* Manager = nullptr;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
