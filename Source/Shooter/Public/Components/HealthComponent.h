#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category="Health")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float MaxHealth = 100;

	/* Делегаты*/
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category="Health")
	void Heal(float Amount);
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/* Обработчик реплики */
	UFUNCTION()
	void OnRep_Health();

	// Обрабатывает любое входящее повреждение, которое AActor генерирует
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Внутренняя функция для изменения HP
	void ApplyDamage(float Damage, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated)
	bool bIsDead = false;
};
