// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EchoAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/ArcherCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UEchoAnimInstance::UEchoAnimInstance()
{
}

void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (EchoMovementComponent)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(EchoMovementComponent->Velocity);
		IsFalling = EchoMovementComponent->IsFalling();
		CharacterState = EchoCharacter->GetCharacterState();
	}
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
