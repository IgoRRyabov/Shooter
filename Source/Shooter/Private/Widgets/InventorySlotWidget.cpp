#include "Widgets/InventorySlotWidget.h"
#include"Structs/InventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameplaySystems/ItemDatabase.h"
#include "Structs/ItemData.h"

void UInventorySlotWidget::SetupSlot(const FInventorySlot& SlotData)
{
	if (!ItemIcon || !QuantityText)
		return;

	if (SlotData.IsEmpty())
	{
		// Пустой слот: очищаем и показываем placeholder
		ItemIcon->SetBrushFromTexture(nullptr);
		QuantityText->SetText(FText::GetEmpty());
		return;
	}

	const FItemData ItemData = UItemDatabase::Get()->GetItemByID(SlotData.ItemID);
	if (ItemData.Icon)
	{
		ItemIcon->SetBrushFromTexture(ItemData.Icon);
	}
	QuantityText->SetText(FText::AsNumber(SlotData.Quantity));
}
