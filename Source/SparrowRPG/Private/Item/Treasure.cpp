// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Treasure.h"
#include "Characters/ArcherCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AArcherCharacter* ArcherCharacter = Cast<AArcherCharacter>(OtherActor);
	if (ArcherCharacter)
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,
				PickupSound, GetActorLocation());
		}
		Destroy();
	}
}
