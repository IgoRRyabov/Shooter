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

void UInventoryComponent::ServerRemoveItem_Implementation(int32 ItemID, int32 Quantity)
{
	RemoveItem_Internal(ItemID, Quantity);
}

void UInventoryComponent::OnRep_Slots()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::OnRep_Items()
{
	
}

void UInventoryComponent::AddItem_Internal(FName ItemID, int32 Quantity)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	if (ItemID.IsNone() || Quantity <= 0) return;

	// Если слоты не инициализированы — создаём
	if (Slots.Num() == 0)
	{
		const int32 Total = FMath::Max(1, NumRows) * FMath::Max(1, NumColumns);
		Slots.SetNum(Total);
	}

	// Попытка сложить в существующий слот
	for (FInventorySlot& Slot : Slots)
	{
		if (!Slot.IsEmpty() && Slot.ItemID == ItemID)
		{
			Slot.Quantity += Quantity;
			OnRep_Slots();
			return;
		}
	}

	// Добавляем в первый пустой слот
	for (FInventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty())
		{
			Slot.ItemID = ItemID;
			Slot.Quantity = Quantity;
			OnRep_Slots();
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Inventory full! Could not add %s"), *ItemID.ToString());

}

void UInventoryComponent::RemoveItem_Internal(int32 SlotIndex, int32 Quantity)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!Slots.IsValidIndex(SlotIndex)) return;
	if (Quantity <= 0) return;

	FInventorySlot& Slot = Slots[SlotIndex];
	if (Slot.IsEmpty()) return;

	Slot.Quantity -= Quantity;
	if (Slot.Quantity <= 0)
	{
		Slot = FInventorySlot();
	}
	OnRep_Slots();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, Slots);
}

