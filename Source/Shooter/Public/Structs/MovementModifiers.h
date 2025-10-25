#pragma once

#include "CoreMinimal.h"
#include "MovementModifiers.generated.h"

USTRUCT(BlueprintType)
struct FMovementModifiers
{
	GENERATED_BODY()

	// Мультипликаторы, которые компоненты могут менять (по умолчанию = 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccelMul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FrictionMul = 1.f;

	// Складываем несколько модов: умножаем поля
	void Combine(const FMovementModifiers& Other)
	{
		SpeedMul    *= Other.SpeedMul;
		AccelMul    *= Other.AccelMul;
		FrictionMul *= Other.FrictionMul;
	}
};
