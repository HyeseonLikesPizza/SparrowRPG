// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/SparrowRPGCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Characters/SparrowAnimInstance.h"
#include "Item/Arrow.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"

//////////////////////////////////////////////////////////////////////////
// ASparrowRPGCharacter

ASparrowRPGCharacter::ASparrowRPGCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	

	ProjectileClass = AArrow::StaticClass();

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	/*
	static ConstructorHelpers::FClassFinder<UAnimInstance>
		SPARROW_ANIM(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	if (SPARROW_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SPARROW_ANIM.Class);
	}
	*/

	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));

	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));

	XOffset = 50.f;

	

	CanFire = true;
	fLTime = 0.f;
	

}

//////////////////////////////////////////////////////////////////////////
// Input

void ASparrowRPGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASparrowRPGCharacter::Move);
	}

	/*
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASparrowRPGCharacter::Load);
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASparrowRPGCharacter::Fire);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASparrowRPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASparrowRPGCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Attack", this, &ASparrowRPGCharacter::Attack);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ASparrowRPGCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ASparrowRPGCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASparrowRPGCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASparrowRPGCharacter::TouchStopped);
	*/


}

void ASparrowRPGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* world = GetWorld();
	ABCHECK(world != nullptr);
	CameraManager = UGameplayStatics::GetPlayerCameraManager(world, 0);
	fADeltaTime = world->GetTimeSeconds();

	Anim = Cast<USparrowAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != Anim);
	Anim->CanFireCheck.AddLambda([this]()->void {
		CanFire = true;
	});
	Anim->FireCheck.AddLambda([this]()->void {
		CanFire = false;
		Loading = false;
	});
	Anim->ResetComboCheck.AddLambda([this]()->void {
		//fLTime = 2.f;
	});
	Anim->LoadingCheck.AddLambda([this]()->void {
		Loading = true;
	});



	if (fCurve)
	{
		MyTimeline->AddInterpFloat(fCurve, InterpFunction, FName("Alpha"));

		CameraStartLocation = CameraBoom->GetRelativeLocation();
		CameraEndLocation = FVector(CameraStartLocation.X+XOffset, CameraStartLocation.Y,
			CameraStartLocation.Z);


		MyTimeline->SetLooping(false);
		MyTimeline->SetIgnoreTimeDilation(true);
	}
	
	
	
}

void ASparrowRPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ArcherMappingContext, 0);
		}
	}



}

void ASparrowRPGCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ASparrowRPGCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ASparrowRPGCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ASparrowRPGCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ASparrowRPGCharacter::Attack(float fTime)
{
	UE_LOG(LogTemp, Warning, TEXT("fLTime : %f"), fLTime);

	//발사 후 재장전
	if (Loading == true)
	{
		Anim->PlayAttackMontage(FName(FString::FromInt(1)),1.f);
		Loading = false;
	}

	//장전
	if((fTime==1.f && fLTime == 0.f) )
	{
		if (MyTimeline->GetPlaybackPosition() == 0.0f)
		{
			GLog->Log("Play");
			MyTimeline->SetPlayRate(1.0f);
			MyTimeline->Play();
		}
			
		Anim->PlayAttackMontage(FName(FString::FromInt(1)), 0.5f);
	}

	if(fTime == 1.f && fLTime < 1.f)
		fLTime += fTime / 30.f;
	

	//발사
	if (fLTime > 0.f && fTime == 0.f && CanFire == true)
	{
		Anim->PlayAttackMontage(FName(FString::FromInt(0)), 0.8f);
		Fire();

		if (MyTimeline->GetPlaybackPosition() == MyTimeline->GetTimelineLength())
		{
			GLog->Log("Reverse");
			MyTimeline->SetPlayRate(1.8f);
			MyTimeline->Reverse();
		}
			
	}

}

void ASparrowRPGCharacter::Fire()
{


	
	UWorld* world = GetWorld();
	ABCHECK(nullptr != world)

	FHitResult HitResult;
	const FName TraceTag("MyTraceTag");
	FCollisionQueryParams Params;
	FActorSpawnParameters spawnParams;

	Params.AddIgnoredActor(this);
	world->DebugDrawTraceTag = TraceTag;
	Params.TraceTag = TraceTag;

	FVector CrosshairWorldLocation = CameraManager->GetCameraLocation();
	FVector ImpactPoint = CrosshairWorldLocation + CameraManager->GetActorForwardVector() * 15000.f;
	FVector ArrowSpawnLocation;
	FRotator ArrowSpawnRotation;

	if (world->LineTraceSingleByChannel(HitResult, CrosshairWorldLocation, ImpactPoint, ECollisionChannel::ECC_Visibility, Params))
	{
		//DrawDebugLine(world, CrosshairWorldLocation, ImpactPoint, FColor::Red, false, 3.0f);

		if (HitResult.bBlockingHit)
		{
			ImpactPoint = HitResult.ImpactPoint;

		}

		ArrowSpawnLocation = GetMesh()->GetSocketLocation("arrow_socket");
		ArrowSpawnRotation = (ImpactPoint - ArrowSpawnLocation).Rotation();
	}

	spawnParams.Owner = this;
	spawnParams.Instigator = GetInstigator();

	AArrow* arrow = world->SpawnActor<AArrow>(ProjectileClass, ArrowSpawnLocation, ArrowSpawnRotation, spawnParams);

	if (arrow && Anim)
	{
		
		FVector LaunchDirection = ArrowSpawnRotation.Vector();
		arrow->Fire(LaunchDirection * fLTime);
		fLTime = 0.f;
	}




	
}

void ASparrowRPGCharacter::Move(const FInputActionValue& value)
{
	const bool CurrentValue = value.Get<bool>();
	if (CurrentValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("IA_Move Triggered"));
	}
}



void ASparrowRPGCharacter::TimelineFloatReturn(float value)
{
	CameraBoom->SetRelativeLocation(FMath::Lerp(CameraStartLocation, CameraEndLocation, value));
}

void ASparrowRPGCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASparrowRPGCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
