#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Pickupable.h"
#include "PickUpObject.generated.h"

UCLASS()
class SHOOTER_API APickUpObject : public AActor, public IPickupable
{
	GENERATED_BODY()
	
public:	
	APickUpObject();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* MeshComponent;

public:
	virtual void OnPickedUp_Implementation(class AMyCharacter* Picker) override;
};
