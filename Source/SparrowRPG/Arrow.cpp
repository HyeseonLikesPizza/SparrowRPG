// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"

// Sets default values
AArrow::AArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SM_ARROW(TEXT("StaticMesh'/Game/ParagonSparrow/FX/Meshes/Heroes/Sparrow/Abilities/SM_Sparrow_Arrow2.SM_Sparrow_Arrow2'"));
	if (SM_ARROW.Succeeded())
	{
		Mesh->SetStaticMesh(SM_ARROW.Object);
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(Mesh);
	ProjectileMovementComponent->InitialSpeed = 6500.0f;
	ProjectileMovementComponent->MaxSpeed = 6500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Box->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(130, 0, 0), FVector(0.25, 0.25, 0.25)));

	HitEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HitEffect"));
	HitEffect->SetupAttachment(Box);

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		P_Hit(TEXT("/Game/ParagonSparrow/FX/Particles/Sparrow/Abilities/Primary/FX/P_Sparrow_HitHero.P_Sparrow_HitHero"));
	if (P_Hit.Succeeded())
	{
		HitEffect->SetTemplate(P_Hit.Object);
		HitEffect->bAutoActivate = false;
	}

	//InitialLifeSpan = 10.f;

	
}

void AArrow::Fire(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	//Effect->Activate(true);
	//Mesh->ToggleVisibility();
}

// Called when the game starts or when spawned
void AArrow::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArrow::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnOverlapBegin);
}

// Called every frame
void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor != this)
	{
		
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;

		FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, true);
		this->AttachToActor(OtherActor, rules);

		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		
		if (HitEffect->Template!=NULL)
		{
			HitEffect->Activate(true);
		}
		
		
	}
}

