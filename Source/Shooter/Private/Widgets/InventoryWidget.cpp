#include "Widgets/InventoryWidget.h"
#include "Components/ScrollBox.h"
#include "GameplaySystems/ItemDatabase.h"
#include "Widgets/InventoryItemWidget.h"

void UInventoryWidget::InitializeInventory(UInventoryComponent* InInventory)
{
	InventoryComponent = InInventory;

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshInventory);
		RefreshInventory();
	}

	SetVisibility(ESlateVisibility::Hidden);
	bIsVisible = false;
}

void UInventoryWidget::SetInventoryVisible(bool bVisible)
{
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	bIsVisible = bVisible;
}

void UInventoryWidget::RefreshInventory()
{
	if (!InventoryComponent || !ScrollBox_Items || !ItemWidgetClass) return;

	ScrollBox_Items->ClearChildren();

	const TArray<FInventoryItem>& Items = InventoryComponent->GetItems();

	for (const FInventoryItem& Item : Items)
	{
		const FItemData Data = UItemDatabase::Get()->GetItemByID(Item.ItemID);

		UInventoryItemWidget* ItemWidget = CreateWidget<UInventoryItemWidget>(GetWorld(), ItemWidgetClass);
		if (ItemWidget)
		{
			ItemWidget->SetupItem(Data, Item.Quantity);
			ScrollBox_Items->AddChild(ItemWidget);
		}
	}
}
