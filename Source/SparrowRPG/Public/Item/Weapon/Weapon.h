// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "../../Characters/CharacterTypes.h"
#include "Weapon.generated.h"

/**
 * 
 */

class USoundBase;
class UBoxComponent;

UCLASS()
class SPARROWRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool PlaySound = true);
	void DeactivateEmbers();
	void DisableCollisionSphere();
	void PlayEquipSound();
	void AttachMeshToComponent(USceneComponent* InParent, const FName& InSocketName);
	void AttachMeshToActor(AActor* ParentActor, const FName& InSocketName);
	void DetachWeapon();
	TArray<AActor*> IgnoreActors;
	void ApplyDamage(FHitResult BoxHit, float Damage);
	void Skill1(FVector Start, FVector End);
	FVector GetWeaponBoxLocation();
	int32 CalculateNormalDamage(float Damage);
	int32 CalculateCriticalDamage(float Damage);
	int32 CalculateDamage(float Damage);
	bool IsCritical(float Damage, EAttackType Attack);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float BasicDamage = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Skill1Damage = 1200.f;

protected:
	class AArcherCharacter* Player;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	void BoxTrace(FHitResult& BoxHit);
	void SphereTrace(const FVector Start, const FVector End, TArray<FHitResult>& SphereHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Skill1Radius = 150.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false; 

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;


public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE FVector GetBoxTraceEndLocation() const { return BoxTraceEnd->GetComponentLocation(); }
};
