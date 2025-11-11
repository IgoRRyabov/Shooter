#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Pickupable.h"
#include "HealthPickup.generated.h"

UCLASS()
class SHOOTER_API AHealthPickup : public AActor, public IPickupable
{
	GENERATED_BODY()
	
public:	
	AHealthPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category="Pickup")
	float HealAmount = 50.0f;

	UPROPERTY(EditAnywhere, Category="Pickup")
	FText textPickUp;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

public:
	virtual void OnPickedUp_Implementation(class AMyCharacter* Picker) override;
	virtual FText GetPickupText_Implementation() const override;
	virtual FVector GetPickupLocation_Implementation() const override;
};
