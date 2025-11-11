#include "InteractiveObjects/HealthPickup.h"

#include "Character/MyCharacter.h"
#include "Components/SphereComponent.h"

AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(100.f);
	RootComponent = SphereComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnOverlapBegin);
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	textPickUp = FText::FromString("Press E");
}

void AHealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	AMyCharacter* Character = Cast<AMyCharacter>(OtherActor);
	if (Character)
	{
		IPickupable::Execute_OnPickedUp(this, Character);
	}
}

void AHealthPickup::OnPickedUp_Implementation(class AMyCharacter* Picker)
{
	if (!Picker) return;

	Picker->GetHealsComponent()->Heal(HealAmount);
	Destroy();
}

FText AHealthPickup::GetPickupText_Implementation() const
{
	return textPickUp;
}

FVector AHealthPickup::GetPickupLocation_Implementation() const
{
	return GetActorLocation() + FVector(0, 0, 50);
}



