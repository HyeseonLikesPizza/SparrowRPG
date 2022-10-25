// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SparrowRPG.h"
#include "Animation/AnimInstance.h"
#include "SparrowAnimInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FResetComboCheckDelegate);

UCLASS()
class SPARROWRPG_API USparrowAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	class ASparrowRPGCharacter* Character;

	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GroundSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator BaseAimRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float YawOffset;

	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* AttackMontage;

	FResetComboCheckDelegate ResetComboCheck;

	UFUNCTION()
	void AnimNotify_ResetCombo();


	void PlayAttackMontage();

	
public:
	USparrowAnimInstance();
	virtual void NativeUpdateAnimation(float fDeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
};
