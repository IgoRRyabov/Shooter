#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

UCLASS()
class SHOOTER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	static ABasePlayerController* GetInstance();

	virtual void BeginPlay() override;

private:

	static ABasePlayerController* Instance;
	
};
