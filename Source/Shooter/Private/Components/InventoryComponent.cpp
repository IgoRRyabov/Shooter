#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::ServerAddItem_Implementation(FName ItemID, int32 Quantity)
{
	AddItem_Internal(ItemID, Quantity);
}

void UInventoryComponent::ServerRemoveItem_Implementation(FName ItemID, int32 Quantity)
{
	RemoveItem_Internal(ItemID, Quantity);
}

bool UInventoryComponent::HasItem(FName ItemID) const
{
	for (const FInventoryItem& MyItem : Items)
	{
		if (MyItem.ItemID == ItemID)
			return true;
	}
	return false;
}

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::AddItem_Internal(FName ItemID, int32 Quantity)
{
	for (FInventoryItem& Item : Items)
	{
		if (Item.ItemID == ItemID)
		{
			Item.Quantity += Quantity;
			OnRep_Items();
			return;
		}
	}

	Items.Add(FInventoryItem(ItemID, Quantity));
	OnRep_Items();
}

void UInventoryComponent::RemoveItem_Internal(FName ItemID, int32 Quantity)
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].ItemID == ItemID)
		{
			Items[i].Quantity -= Quantity;
			if (Items[i].Quantity <= 0)
			{
				Items.RemoveAt(i);
			}
			OnRep_Items();
			return;
		}
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, Items);
}

