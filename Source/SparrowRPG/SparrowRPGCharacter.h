// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SparrowRPG.h"
#include "GameFramework/Character.h"
#include "Arrow.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "SparrowRPGCharacter.generated.h"

UCLASS(config=Game)
class ASparrowRPGCharacter : public ACharacter
{
	GENERATED_BODY()	

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ASparrowRPGCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;


	// 내가 사용하는 것들
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AArrow> ProjectileClass;

	void Attack(float fTime);
	void Fire();

	UPROPERTY()
	bool CanFire;
	bool Loading;
	bool IsZooming;

	UPROPERTY()
	class USparrowAnimInstance* Anim;

	float fLTime;
	float fLastTime;
	float fADeltaTime;

	class UTimelineComponent* MyTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* fCurve;

	UPROPERTY()
	FVector CameraStartLocation;

	UPROPERTY()
	FVector CameraEndLocation;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	float XOffset;

	FOnTimelineFloat InterpFunction{};

	UFUNCTION()
	void TimelineFloatReturn(float value);

	UPROPERTY()
	APlayerCameraManager* CameraManager;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

