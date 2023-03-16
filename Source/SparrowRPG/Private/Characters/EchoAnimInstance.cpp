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


void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (EchoMovementComponent)
	{
		//GroundSpeed = UKismetMathLibrary::VSizeXY(EchoMovementComponent->Velocity);
		GroundSpeed = EchoCharacter->GetVelocity().Size();
		IsFalling = EchoMovementComponent->IsFalling();
		CharacterState = EchoCharacter->GetCharacterState();
		ActionState = EchoCharacter->GetActionState();
		DeathPose = EchoCharacter->GetDeathPose();
		IsDead = ActionState == EActionState::EAS_Dead;

	}
}

