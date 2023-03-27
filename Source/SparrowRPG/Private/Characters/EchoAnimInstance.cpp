// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EchoAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/ArcherCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UEchoAnimInstance::UEchoAnimInstance()
{
}

void UEchoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	EchoCharacter = Cast<AArcherCharacter>(TryGetPawnOwner());
	if (EchoCharacter)
	{
		EchoMovementComponent = EchoCharacter->GetCharacterMovement();
	}
}

void UEchoAnimInstance::TurnInPlace(float DeltaTime)
{
	if (GroundSpeed > 0.f || IsFalling)
	{
		RootYawOffset = FMath::FInterpTo(RootYawOffset, 0.f, DeltaTime, 20.0f);
		MovingRotation = EchoCharacter->GetActorRotation();
		LastMovingRotation = MovingRotation;
	}
	else
	{
		LastMovingRotation = MovingRotation;
		MovingRotation = EchoCharacter->GetActorRotation();

		FRotator Delta;
		Delta = UKismetMathLibrary::NormalizedDeltaRotator(MovingRotation, LastMovingRotation);
		RootYawOffset = RootYawOffset - Delta.Yaw;


	}

	// IsTurning?
	if (GetCurveValue(FName("Turning")) > 0.f)
	{
		LastDistanceCurve = DistanceCurve;
		DistanceCurve = GetCurveValue(FName("DistanceCurve"));
		DeltaDistanceCurve = DistanceCurve - LastDistanceCurve;

		//Turning Left
		if (RootYawOffset > 0.f)
		{
			RootYawOffset = RootYawOffset - DeltaDistanceCurve;
		}
		else
		{
			RootYawOffset = RootYawOffset + DeltaDistanceCurve;
		}

		AbsRootYawOffset = abs(RootYawOffset);

		if (AbsRootYawOffset > 90.f)
		{
			YawExcess = AbsRootYawOffset - 90.f;

			if (RootYawOffset > 0.f)
			{
				RootYawOffset = RootYawOffset - YawExcess;
			}
			else
			{
				RootYawOffset = RootYawOffset + YawExcess;
			}
		}


	}
}


void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (EchoMovementComponent)
	{
		//GroundSpeed = UKismetMathLibrary::VSizeXY(EchoMovementComponent->Velocity);
		GroundSpeed = EchoCharacter->GetVelocity().Size();
		ShouldMove = (EchoMovementComponent->GetCurrentAcceleration() != FVector(0.f, 0.f, 0.f)) && (GroundSpeed > 3.0f);
		IsFalling = EchoMovementComponent->IsFalling();
		CharacterState = EchoCharacter->GetCharacterState();
		ActionState = EchoCharacter->GetActionState();
		DeathPose = EchoCharacter->GetDeathPose();
		IsDead = ActionState == EActionState::EAS_Dead;
		BaseAimRotation = EchoCharacter->GetBaseAimRotation();
		Velocity = EchoMovementComponent->GetLastUpdateVelocity();
		YawOffset = (FRotationMatrix::MakeFromX(Velocity).Rotator() - BaseAimRotation).GetNormalized().Yaw;

		TurnInPlace(DeltaTime);

	}
}

