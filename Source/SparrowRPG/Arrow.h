// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SparrowRPG.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Arrow.generated.h"

UCLASS()
class SPARROWRPG_API AArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrow();

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, Category = "Effect")
	UParticleSystemComponent* HitEffect;

	void Fire(const FVector& ShootDirection);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult);

};
