#include "Widgets/InventoryWidget.h"

#include "Character/MyCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "Widgets/InventoryItemWidget.h"
#include "Widgets/InventorySlotWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::InitializeInventory(class UInventoryComponent* InInventory)
{
	if (bInitialized) return;
	Inventory = InInventory;
	if (Inventory)
	{
		Inventory->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshInventory);
	}
	RefreshInventory();
	bInitialized = true;
}

void UInventoryWidget::RefreshInventory()
{
	if (!Inventory || !GridPanel || !SlotWidgetClass) return;

	GridPanel->ClearChildren();

	const TArray<FInventorySlot>& Slots = Inventory->GetSlots();
	const int32 Columns = Inventory->GetNumColumns();
	const int32 Rows = Inventory->GetNumRows();

	// Если слоты ещё не заполнены (Slots.Num()==0), инициализируем пустыми слотами здесь тоже
	if (Slots.Num() == 0)
	{
		const int32 Total = FMath::Max(1, Rows) * FMath::Max(1, Columns);
		// Примечание: мы не меняем Inventory->Slots тут — это только визуальная заглушка.
	}

	int32 Index = 0;
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Col = 0; Col < Columns; ++Col)
		{
			// На случай, если в Inventory меньше элементов, создаём пустой FInventorySlot
			FInventorySlot SlotData;
			if (Slots.IsValidIndex(Index))
			{
				SlotData = Slots[Index];
			}

			// CreateWidget с GetOwningPlayer()
			if (APlayerController* PC = GetOwningPlayer())
			{
				UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(PC, SlotWidgetClass);
				if (SlotWidget)
				{
					SlotWidget->SetupSlot(SlotData);
					GridPanel->AddChildToUniformGrid(SlotWidget, Row, Col);
				}
			}

			++Index;
		}
	}
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		if (AMyCharacter* MyChar = Cast<AMyCharacter>(GetOwningPlayerPawn()))
		{
			MyChar->ToggleInventory(); // вызовем обратно, чтобы закрыть
		}

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
