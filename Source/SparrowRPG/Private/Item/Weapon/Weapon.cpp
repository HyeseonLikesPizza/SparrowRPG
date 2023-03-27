// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Weapon.h"
#include "Characters/ArcherCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool PlaySound)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToComponent(InParent, InSocketName);
	DisableCollisionSphere();
	DeactivateEmbers();
	Player = Cast<AArcherCharacter>(GetOwner());
	if (PlaySound)
		PlayEquipSound();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::DisableCollisionSphere()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, EquipSound, GetActorLocation());
	}
}

void AWeapon::AttachMeshToComponent(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::AttachMeshToActor(AActor* ParentActor, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	this->AttachToActor(ParentActor, TransformRules, InSocketName);
}

void AWeapon::DetachWeapon()
{
	FDetachmentTransformRules TransformRules(EDetachmentRule::KeepWorld, false);
	ItemMesh->DetachFromComponent(TransformRules);
}

void AWeapon::ApplyDamage(FHitResult BoxHit, float Damage)
{
	if (BoxHit.GetActor())
	{
		if (ActorIsSameType(BoxHit.GetActor())) return;
		UE_LOG(LogTemp, Warning, TEXT("BoxHit Actor : %s"), *BoxHit.GetActor()->GetName());
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::Skill1(FVector Start, FVector End)
{
	TArray<FHitResult> SphereHit;
	TArray<FString> Done;

	SphereTrace(Start, End, SphereHit);
	
	Done.Empty();

	for (auto result : SphereHit)
	{
		if (Done.Find(result.GetActor()->GetName()) == INDEX_NONE)
		{
			ApplyDamage(result, Skill1Damage);
			Done.Add(result.GetActor()->GetName());
		}
	} 

	IgnoreActors.Empty();
}

FVector AWeapon::GetWeaponBoxLocation()
{
	return WeaponBox->GetComponentLocation();
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;
	
	FHitResult BoxHit;
	BoxTrace(BoxHit);
	
	ApplyDamage(BoxHit, BasicDamage);
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}

}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetOwner());
	ActorsToIgnore.AddUnique(this);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);

	IgnoreActors.AddUnique(BoxHit.GetActor());
}

void AWeapon::SphereTrace(const FVector Start, const FVector End, TArray<FHitResult>& SphereHit)
{
	//const FVector Start = BoxTraceStart->GetComponentLocation();
	//const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetOwner());
	ActorsToIgnore.AddUnique(this);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::SphereTraceMulti(
		this, Start, End, Skill1Radius,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		SphereHit,
		true
	);

	for (auto result : SphereHit)
	{
		IgnoreActors.AddUnique(result.GetActor());
	}
	
	//IgnoreActors.AddUnique(BoxHit.GetActor());
}
