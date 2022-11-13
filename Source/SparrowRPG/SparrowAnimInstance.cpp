// Fill out your copyright notice in the Description page of Project Settings.


#include "SparrowAnimInstance.h"
#include "SparrowRPGCharacter.h"
#include "Kismet/KismetMathLibrary.h"




USparrowAnimInstance::USparrowAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		ATTACK_MONTAGE(TEXT("/Game/ParagonSparrow/Characters/Heroes/Sparrow/Animations/Primary_Fire_Med_Montage.Primary_Fire_Med_Montage"));

	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}

	//CanFire = true;
}

void USparrowAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ASparrowRPGCharacter>(GetOwningActor());
	if (Character)
	{
		MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
	}


}


void USparrowAnimInstance::NativeUpdateAnimation(float fDeltaSeconds)
{
	Super::NativeUpdateAnimation(fDeltaSeconds);

	

}

void USparrowAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (Character)
	{
		CalculateGroundSpeed();
		CalculateShouldMove();
		CalculateIsFalling();
		CalculateYawOffset();
		
		TurnInPlace(DeltaSeconds);

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
}

void USparrowAnimInstance::PlayMontage(FString name)
{



}

void USparrowAnimInstance::CalculateGroundSpeed()
{
	
	Velocity = MovementComponent->GetLastUpdateVelocity();
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0).Length();
}

void USparrowAnimInstance::CalculateShouldMove()
{
	if (GroundSpeed > 3.f && (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector))
	{

		ShouldMove = true;
	}
	else
		ShouldMove = false;
}

void USparrowAnimInstance::CalculateIsFalling()
{
	IsFalling = MovementComponent->IsFalling();
}

void USparrowAnimInstance::CalculateYawOffset()
{
	BaseAimRotation = Character->GetBaseAimRotation();
	YawOffset = (FRotationMatrix::MakeFromX(Velocity).Rotator() - BaseAimRotation).GetNormalized().Yaw;
}



void USparrowAnimInstance::TurnInPlace(float DeltaTime)
{
	if (GroundSpeed > 0.f || IsFalling)
	{
		RootYawOffset = FMath::FInterpTo(RootYawOffset, 0.f, DeltaTime, 20.0f);
		MovingRotation = Character->GetActorRotation();
		LastMovingRotation = MovingRotation;
	}
	else
	{
		LastMovingRotation = MovingRotation;
		MovingRotation = Character->GetActorRotation();

		FRotator Delta;
		Delta = UKismetMathLibrary::NormalizedDeltaRotator(MovingRotation, LastMovingRotation);
		RootYawOffset = RootYawOffset - Delta.Yaw;

		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Root Yaw Offset : %f"), RootYawOffset));


	}
	
	
}

void USparrowAnimInstance::AnimNotify_ResetCombo()
{
	ResetComboCheck.Broadcast();
}

void USparrowAnimInstance::AnimNotify_Loading()
{
	LoadingCheck.Broadcast();
}


void USparrowAnimInstance::AnimNotify_Fire()
{
	FireCheck.Broadcast();
}

void USparrowAnimInstance::AnimNotify_CanFire()
{
	CanFireCheck.Broadcast();
}

