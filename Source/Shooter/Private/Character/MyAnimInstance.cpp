#include "Character/MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/MyCharacter.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	AimHistory.Reset();
	SmoothedAim = FVector2D::ZeroVector;
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!Char) return;

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	// 1) Получаем текущий AimOffset у персонажа
	FVector2D CurrentAim = FVector2D::ZeroVector;

	// Владелец может считать локально, наблюдатели — читают реплицированное поле через геттер:
	if (MyChar)
	{
		CurrentAim = MyChar->GetAimOffset(); // X=Yaw, Y=Pitch (то, что ты уже обновляешь)
	}
	else
	{
		// fallback: посчитать из BaseAimRotation (тоже ок)
		const FRotator AimRot = Char->GetBaseAimRotation();
		const FRotator ActorRot = Char->GetActorRotation();
		const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRot, ActorRot);
		CurrentAim = FVector2D(
			FMath::Clamp(DeltaRot.Yaw, -90.f, 90.f),
			FMath::Clamp(DeltaRot.Pitch, -90.f, 90.f));
	}

	// 2) Если это владелец — никаких задержек: просто мягкое экспоненциальное сглаживание
	if (Char->IsLocallyControlled())
	{
		SmoothedAim.X = FMath::FInterpTo(SmoothedAim.X, CurrentAim.X, DeltaSeconds, ExpoSmoothingSpeed);
		SmoothedAim.Y = FMath::FInterpTo(SmoothedAim.Y, CurrentAim.Y, DeltaSeconds, ExpoSmoothingSpeed);
	}
	else
	{
		// 3) Для наблюдателей — кладём сэмпл в историю
		AimHistory.Add({ Now, CurrentAim });

		// подчистка старых сэмплов
		const float CutTime = Now - HistoryKeepSeconds;
		while (AimHistory.Num() > 0 && AimHistory[0].Time < CutTime)
		{
			AimHistory.RemoveAt(0);
		}

		// 4) Рендерим с небольшим «отставанием» (BufferLatencySeconds)
		const float RenderTime = Now - BufferLatencySeconds;

		// Находим два ближайших сэмпла вокруг RenderTime
		FAimSample Prev = { Now, CurrentAim };
		FAimSample Next = { Now, CurrentAim };

		// если в истории мало данных — используем текущее
		if (AimHistory.Num() >= 2)
		{
			// ищем интервал [i, i+1], где Time_i <= RenderTime <= Time_{i+1}
			int Index = -1;
			for (int i = AimHistory.Num() - 2; i >= 0; --i)
			{
				if (AimHistory[i].Time <= RenderTime)
				{
					Index = i;
					break;
				}
			}

			if (Index >= 0)
			{
				Prev = AimHistory[Index];
				Next = AimHistory[Index + 1];
			}
			else
			{
				// если RenderTime раньше самого первого — зажмёмся к первому
				Prev = AimHistory[0];
				Next = AimHistory[1];
			}
		}

		// 5) Интерполяция между Prev и Next по времени (линейная)
		const float Den = FMath::Max(0.0001f, Next.Time - Prev.Time);
		const float Alpha = FMath::Clamp((RenderTime - Prev.Time) / Den, 0.f, 1.f);
		const FVector2D Target = FMath::Lerp(Prev.Offset, Next.Offset, Alpha);

		// 6) Экспоненциальное сглаживание к целевому Target (визуально приятнее)
		SmoothedAim.X = FMath::FInterpTo(SmoothedAim.X, Target.X, DeltaSeconds, ExpoSmoothingSpeed);
		SmoothedAim.Y = FMath::FInterpTo(SmoothedAim.Y, Target.Y, DeltaSeconds, ExpoSmoothingSpeed);
	}

	// 7) Отдаём в граф
	AimYaw   = SmoothedAim.X;
	AimPitch = SmoothedAim.Y;
}

void UMyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Char = Cast<ACharacter>(TryGetPawnOwner());
	MyChar = Cast<AMyCharacter>(Char);
}
