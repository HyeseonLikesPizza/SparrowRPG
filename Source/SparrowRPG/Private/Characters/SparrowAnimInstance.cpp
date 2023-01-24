// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SparrowAnimInstance.h"
#include "Characters/SparrowRPGCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/ArcherCharacter.h"
#include "Kismet/KismetMathLibrary.h"




USparrowAnimInstance::USparrowAnimInstance()
{
	/*
	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		ATTACK_MONTAGE(TEXT("/Game/ParagonSparrow/Characters/Heroes/Sparrow/Animations/Primary_Fire_Med_Montage.Primary_Fire_Med_Montage"));

	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
	*/

	//CanFire = true;
}

void USparrowAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ASparrowRPGCharacter>(TryGetPawnOwner());
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}

}


void USparrowAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	if (Character)
	{
		CalculateGroundSpeed();
		CalculateShouldMove();
		CalculateIsFalling();
		CalculateYawOffset();
		TurnInPlace(DeltaSeconds);
	}
	
}

void USparrowAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MovementComponent)
	{
		CalculateGroundSpeed();
		CalculateIsFalling();
	}
}


void USparrowAnimInstance::PlayMontage(FString name)
{



}

void USparrowAnimInstance::CalculateGroundSpeed()
{
	Velocity = MovementComponent->GetLastUpdateVelocity();
	GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	//UE_LOG(LogTemp, Warning, TEXT("Ground Speed : %f"), GroundSpeed);
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

