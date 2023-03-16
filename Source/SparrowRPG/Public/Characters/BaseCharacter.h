// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;
class AWeapon;
class UAttributeComponent;

UCLASS()
class SPARROWRPG_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	void ShieldGetHit(const FVector& ImpactPoint, AActor* HitActor);
	
protected:
	virtual void BeginPlay() override;

	/* Combat */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();

	UFUNCTION(BlueprintNativeEvent)
	void Die();

	void DirectionalHitReact(const FVector& ImpactPoint, AActor* Target);
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void PlayHitShieldSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void SpawnHitShieldParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	void DisableMeshCollision();
	virtual bool CanAttack();
	bool IsAlive();
	void ShieldBlock();


	/* Montage */
	void PlayHitReactMontage(const FName& SectionName);
	void PlayHitShieldReactMontage();
	void PlayShieldBlockMontage();
	void PlayShieldBlockIdleMontage();
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	virtual void EnableBlock();

	UFUNCTION(BlueprintCallable)
	virtual void DisableBlock();

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* ShieldMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleInstanceOnly)
	bool IsDefending = false;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	virtual void SetName(FString name);
	virtual void SetLevel(int32 level);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AShield* EquippedShield;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	// 0 <= WarpTargetDistance <= 1
	// 0에 가까울수록 Target에 가까움, 1에 가까울수록 내 원래위치와 가까움
	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(EditInstanceOnly)
	FString Name;

	UPROPERTY(EditInstanceOnly)
	int32 Level;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* ShieldCounterParticles;

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

private:
	
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* ShieldHitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* ShieldHitParticles;

	

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HitShieldReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSection;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSection;



public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
	FORCEINLINE bool GetIsDefending() const { return IsDefending; }
};
