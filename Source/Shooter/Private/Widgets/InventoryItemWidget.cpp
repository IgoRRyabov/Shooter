#include "Widgets/InventoryItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryItemWidget::SetupItem(const FItemData& ItemData, int32 Quantity)
{
	if (ItemIcon && ItemData.Icon)
		ItemIcon->SetBrushFromTexture(ItemData.Icon);

	if (ItemName)
		ItemName->SetText(ItemData.ItemName);

	if (ItemQuantity)
		ItemQuantity->SetText(FText::FromString(FString::Printf(TEXT("x%d"), Quantity)));

}
