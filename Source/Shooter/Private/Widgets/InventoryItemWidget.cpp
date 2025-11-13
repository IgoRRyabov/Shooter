#include "Widgets/InventoryItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameplaySystems/ItemDatabase.h"
#include "Structs/ItemData.h"

void UInventoryItemWidget::SetupItem(FName ItemID, int32 Quantity)
{
	CurrentItemID = ItemID;
	CurrentQuantity = Quantity;

	const FItemData ItemData = UItemDatabase::Get()->GetItemByID(ItemID);

	if (ItemIcon && ItemData.Icon)
		ItemIcon->SetBrushFromTexture(ItemData.Icon);

	if (ItemQuantity)
		ItemQuantity->SetText(FText::AsNumber(Quantity));
}
