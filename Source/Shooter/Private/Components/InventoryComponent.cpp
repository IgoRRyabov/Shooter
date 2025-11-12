#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

bool UInventoryComponent::HasItem(FName ItemID) const
{
	return true;
}

void UInventoryComponent::OnRep_Items()
{
}

void UInventoryComponent::AddItem_Internal(const FItemData& NewItem)
{
	
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, Items);
}

void UInventoryComponent::AddItem_Server_Implementation(const FItemData& NewItem)
{
	AddItem_Internal(NewItem);
}
