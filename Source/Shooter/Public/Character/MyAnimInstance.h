#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

class AMyCharacter;

USTRUCT()
struct FAimSample
{
	GENERATED_BODY()
	float Time = 0.f;          // локальное время на клиенте
	FVector2D Offset = FVector2D::ZeroVector; // X=Yaw, Y=Pitch
};

UCLASS()
class SHOOTER_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

	// Итоговые значения, которые подаём в AimOffset/BlendSpace
	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimYaw = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimPitch = 0.f;

	// Настройки (можно править из BP-наследника этого AnimInstance)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aim|Smoothing")
	float BufferLatencySeconds = 0.10f;   // визуальная задержка рендера (100 мс)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aim|Smoothing")
	float ExpoSmoothingSpeed = 10.f;      // экспон. сглаживание (чем больше — тем быстрее догоняем)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aim|Smoothing")
	float HistoryKeepSeconds = 1.0f;      // храним секунду истории

private:
	// история сэмплов (для не-владельца)
	TArray<FAimSample> AimHistory;

	// внутренние цели для сглаживания
	FVector2D SmoothedAim = FVector2D::ZeroVector; // то, что подаём в граф


	ACharacter* Char = nullptr;
	AMyCharacter* MyChar = nullptr;
};
