#include "Controller/MyPlayerController.h"

#include "Character/MyCharacter.h"
#include "Widgets/InventoryWidget.h"

AMyPlayerController::AMyPlayerController()
{
	
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
}


