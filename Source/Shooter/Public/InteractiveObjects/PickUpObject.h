#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Pickupable.h"
#include "Structs/ItemData.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Item")
	FName ItemID;

public:
	virtual void OnPickedUp_Implementation(class AMyCharacter* Picker) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
