// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SparrowRPG.h"
#include "Animation/AnimInstance.h"
#include "SparrowAnimInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FResetComboCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FLoadingCheck);
DECLARE_MULTICAST_DELEGATE(FFireCheck);
DECLARE_MULTICAST_DELEGATE(FCanFireCheck);

UCLASS()
class SPARROWRPG_API USparrowAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float RootYawOffset;

	UFUNCTION()
	void TurnInPlace(float DeltaTime);


	FRotator MovingRotation;
	FRotator LastMovingRotation;
	float DistanceCurve;
	float LastDistanceCurve;
	float DeltaDistanceCurve;
	float AbsRootYawOffset;
	float YawExcess;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* AttackMontage;

	FResetComboCheckDelegate ResetComboCheck;
	FLoadingCheck LoadingCheck;
	FFireCheck FireCheck;
	FCanFireCheck CanFireCheck;

	UFUNCTION()
	void AnimNotify_ResetCombo();

	UFUNCTION()
	void AnimNotify_Loading();

	UFUNCTION()
	void AnimNotify_Fire();

	UFUNCTION()
	void AnimNotify_CanFire();


	UFUNCTION(BlueprintImplementableEvent)
	void PlayAttackMontage(FName sectionName, float playRate);
	
	void PlayMontage(FString name);

	
public:
	USparrowAnimInstance();
	virtual void NativeUpdateAnimation(float fDeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
};
