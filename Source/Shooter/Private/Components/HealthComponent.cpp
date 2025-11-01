#include "Components/HealthComponent.h"

#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	Health = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	//Инициализация начального здоровья
	Health = FMath::Clamp(Health, 0, MaxHealth);

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}

void UHealthComponent::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health, 0.f);
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead) return;

	if (GetOwnerRole() == ROLE_Authority)
	{
		ApplyDamage(Damage, InstigatedBy,  DamageCauser);
	}
}

void UHealthComponent::ApplyDamage(float Damage, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead) return;

	const float OldHealth = Health;
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	const float Delta = Health - OldHealth;

	OnHealthChanged.Broadcast(OldHealth, Delta);

	if (Health <= 0.f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast();

		AActor* OwnerActor = GetOwner();
		if (OwnerActor)
		{
			if (OwnerActor->GetClass()->ImplementsInterface(UInterface::StaticClass()))
			{
				
			}
		}
	}
}

void UHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.f || bIsDead) return;

	if (GetOwnerRole() == ROLE_Authority)
	{
		const float Old = Health;
		Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
		OnHealthChanged.Broadcast(Old, Health);
	}else
	{
		
	}
}








