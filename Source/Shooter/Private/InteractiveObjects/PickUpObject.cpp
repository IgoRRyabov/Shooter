#include "InteractiveObjects/PickUpObject.h"

#include "Character/MyCharacter.h"
#include "Components/SphereComponent.h"
#include "GameplaySystems/ItemDatabase.h"
#include "Net/UnrealNetwork.h"

APickUpObject::APickUpObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(150.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APickUpObject::BeginPlay()
{
	Super::BeginPlay();
}

void APickUpObject::OnPickedUp_Implementation(class AMyCharacter* Picker)
{
	if (HasAuthority())
	{
		if (AMyCharacter* MyChar = Cast<AMyCharacter>(Picker))
		{
			MyChar->AddItemToInventory(ItemID, 1);
		}

		Destroy();
	}
}

void APickUpObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
