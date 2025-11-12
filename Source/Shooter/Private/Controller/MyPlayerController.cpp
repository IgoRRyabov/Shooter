#include "Controller/MyPlayerController.h"

#include "Character/MyCharacter.h"

AMyPlayerController::AMyPlayerController()
{
	
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	if (IsLocalController() && InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport();

			if (APawn* P = GetPawn())
			{
				if (AMyCharacter* Char = Cast<AMyCharacter>(P))
				{
					InventoryWidget->InitializeInventory(Char->Inventory);
				}
			}
		}
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &AMyPlayerController::ToggleInventory);
}

void AMyPlayerController::ToggleInventory()
{
	if (!InventoryWidget) return;

	bool bShouldShow = !InventoryWidget->IsInventoryVisible();
	InventoryWidget->SetInventoryVisible(bShouldShow);

	if (bShouldShow)
	{
		// Открываем UI
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		bShowMouseCursor = true;
	}
	else
	{
		// Возвращаем управление игроку
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);

		bShowMouseCursor = false;
	}
}


