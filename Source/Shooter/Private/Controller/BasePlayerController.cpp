#include "Controller/BasePlayerController.h"

ABasePlayerController* ABasePlayerController::Instance = nullptr;


ABasePlayerController* ABasePlayerController::GetInstance()
{
	if (!Instance)
	{
		Instance = NewObject<ABasePlayerController>();
		Instance->AddToRoot(); // Предотвращаем garbage collection
	}
	return Instance;
	
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
}
