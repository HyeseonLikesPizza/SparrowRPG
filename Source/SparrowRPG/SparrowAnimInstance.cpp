// Fill out your copyright notice in the Description page of Project Settings.


#include "SparrowAnimInstance.h"
#include "SparrowRPGCharacter.h"




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

	if (Character)
	{
		
		Velocity = MovementComponent->GetLastUpdateVelocity();
		//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), Velocity.X, Velocity.Y, Velocity.Z);
		GroundSpeed = FVector(Velocity.X, Velocity.Y, 0).Length();

		if (GroundSpeed > 3.f && (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector))
		{
			
			ShouldMove = true;
		}
		else
			ShouldMove = false;

		IsFalling = MovementComponent->IsFalling();
		BaseAimRotation = Character->GetBaseAimRotation();
		YawOffset = (FRotationMatrix::MakeFromX(Velocity).Rotator() - BaseAimRotation).GetNormalized().Yaw;
		//UE_LOG(LogTemp, Warning, TEXT("Yaw Offset : %f"), YawOffset);
		//Montage_Play(AttackMontage, 1.0f);
	}

}

void USparrowAnimInstance::PlayMontage(FString name)
{



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

