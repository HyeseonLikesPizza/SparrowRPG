// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterTypes.h"
#include "EchoAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPARROWRPG_API UEchoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UEchoAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeInitializeAnimation() override;

	UPROPERTY(BlueprintReadOnly)
	class AArcherCharacter* EchoCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* EchoMovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool ShouldMove;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FRotator BaseAimRotation;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsDead;

	UFUNCTION()
	void TurnInPlace(float DeltaTime);	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float RootYawOffset;

	FRotator MovingRotation;
	FRotator LastMovingRotation;
	float DistanceCurve;
	float LastDistanceCurve;
	float DeltaDistanceCurve;
	float AbsRootYawOffset;
	float YawExcess;
};
