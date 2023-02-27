
#include "Characters/ArcherCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Item/Item.h"
#include "Item/Weapon/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/SparrowHUD.h"
#include "HUD/SparrowOverlay.h"
#include "Item/Soul.h"
#include "Item/Treasure.h"
#include "Gameplay/EchoPlayerController.h"
#include "HUD/LoadWidget.h"


AArcherCharacter::AArcherCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	PlayerName = FString("Echo");
	PlayerLevel = 1;
	PlayTime = 0.f;
	PlayerPlace = FString("Test Map");
}

void AArcherCharacter::Tick(float DeltaTime)
{
	if (Attributes && SparrowOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SparrowOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	PlayTime = FPlatformTime::Seconds() - startSeconds;
}

void AArcherCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AArcherCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AArcherCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AArcherCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AArcherCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AArcherCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AArcherCharacter::Dodge);
		EnhancedInputComponent->BindAction(SaveGameAction, ETriggerEvent::Triggered, this, &AArcherCharacter::SaveGame_Implementation);
		EnhancedInputComponent->BindAction(LoadGameAction, ETriggerEvent::Triggered, this, &AArcherCharacter::LoadGame);
	}
}

void AArcherCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

float AArcherCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	SetHUDHealth();
	return DamageAmount;
}

void AArcherCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}

}

void AArcherCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AArcherCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SparrowOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SparrowOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AArcherCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SparrowOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SparrowOverlay->SetGold(Attributes->GetGold());
	}
}

void AArcherCharacter::SetCameraTransform(FTransform Transform)
{
	ViewCamera->SetRelativeTransform(Transform);
}

FTransform AArcherCharacter::GetCameraTransform() const
{
	return ViewCamera->GetRelativeTransform();
}

void AArcherCharacter::SetSpringArmTransform(FTransform Transform)
{
	CameraBoom->SetRelativeTransform(Transform);
}

FTransform AArcherCharacter::GetSpringArmTransform() const
{
	return CameraBoom->GetRelativeTransform();
}

void AArcherCharacter::SetAttributeComponent(UAttributeComponent* Component)
{
	Attributes = Component;
}

void AArcherCharacter::LoadGold(int32 gold)
{
	if (SparrowOverlay && Attributes)
	{
		Attributes->SetGold(gold);
		SparrowOverlay->SetGold(Attributes->GetGold());
	}
}

void AArcherCharacter::LoadSouls(int32 NumberOfSouls)
{
	if (SparrowOverlay && Attributes)
	{
		Attributes->SetSouls(NumberOfSouls);
		SparrowOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AArcherCharacter::LoadStamina(float Stamina, float MaxStamina)
{
	Attributes->SetStamina(Stamina);
	Attributes->SetMaxStamina(MaxStamina);

	if (SparrowOverlay && Attributes)
	{
		SparrowOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AArcherCharacter::LoadHealth(float Health, float MaxHealth)
{
	Attributes->SetHealth(Health);
	Attributes->SetMaxHealth(MaxHealth);

	if (SparrowOverlay && Attributes)
	{
		SparrowOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AArcherCharacter::LoadDodgeCost(float NewCost)
{
	Attributes->SetDodgeCost(NewCost);
}

void AArcherCharacter::LoadStaminaRegenRate(float NewRate)
{
	Attributes->SetStaminaRegenRate(NewRate);
}

void AArcherCharacter::LoadActionState(EActionState state)
{
	ActionState = state;
}

void AArcherCharacter::LoadCharacterState(ECharacterState state)
{
	CharacterState = state;
}

void AArcherCharacter::LoadEquippedWeapon(AWeapon* weapon)
{
	EquippedWeapon = weapon;
}

void AArcherCharacter::LoadPlayerName(FString name)
{
	PlayerName = name;
}

void AArcherCharacter::LoadPlayerLevel(int32 level)
{
	PlayerLevel = level;
}

void AArcherCharacter::LoadPlayerPlace(FString place)
{
	PlayerPlace = place;
}

void AArcherCharacter::LoadPlayTime(float playtime)
{
	PlayTime = playtime;
}

void AArcherCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ArcherContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget"));

	InitializeSparrowOverlay();

	startSeconds = FPlatformTime::Seconds();


}

void AArcherCharacter::Move(const FInputActionValue& value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector2D MovementVector = value.Get<FVector2D>();

	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AArcherCharacter::Look(const FInputActionValue& value)
{
	const FVector2D LookAxisVector = value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AArcherCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;

	if (Attributes && SparrowOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SparrowOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AArcherCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void AArcherCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AArcherCharacter::SaveGame_Implementation()
{
		
	AEchoPlayerController* PlayerController = Cast<AEchoPlayerController>(GetController());

	if (PlayerController && ActionState == EActionState::EAS_Unoccupied)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Game Saved")));

		PlayerController->SaveGame();
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Not Saved")));
	}
	
}

void AArcherCharacter::LoadGame()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (!LoadWidget)
		{
			ASparrowHUD* SparrowHUD = Cast<ASparrowHUD>(PlayerController->GetHUD());
			if (SparrowHUD)
			{
				LoadWidget = SparrowHUD->GetLoadWidget();
				if (LoadWidget)
				{
					LoadWidget->AddToViewport();
					ShowLoadWidget(PlayerController);
				}
			}
		}
		else
		{
			if (LoadWidget->IsVisible())
			{
				HideLoadWidget(PlayerController);
			}
			else
			{
				LoadWidget->RefreshListBox();
				ShowLoadWidget(PlayerController);
			}

		}
	}

}

void AArcherCharacter::HideLoadWidget(APlayerController* PlayerController)
{
	LoadWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;
}

void AArcherCharacter::ShowLoadWidget(APlayerController* PlayerController)
{
	LoadWidget->SetVisibility(ESlateVisibility::Visible);
	PlayerController->SetInputMode(FInputModeGameAndUI());
	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;
}

void AArcherCharacter::EquipWeapon(AWeapon* Weapon, bool PlaySound)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this, PlaySound);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void AArcherCharacter::RemoveWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->DetachWeapon();
		EquippedWeapon = nullptr;
		CharacterState = ECharacterState::ECS_Unequipped;
	}
}

void AArcherCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AArcherCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

bool AArcherCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState != ECharacterState::ECS_Unequipped;
}

bool AArcherCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied 
		&& CharacterState != ECharacterState::ECS_Unequipped;
}

bool AArcherCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState == ECharacterState::ECS_Unequipped
		&& EquippedWeapon;
}

void AArcherCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EqquippingWeapon;
}

void AArcherCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EqquippingWeapon;
}

void AArcherCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToComponent(GetMesh(), FName("SpineSocket"));
	}
}

void AArcherCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToComponent(GetMesh(), FName("RightHandSocket"));
	}
}

void AArcherCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AArcherCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
}

bool AArcherCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool AArcherCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void AArcherCharacter::SetPlayerPlace(FString place)
{
	PlayerPlace = place;
}

void AArcherCharacter::SetPlayTime(float playtime)
{
	PlayTime = playtime;
}

void AArcherCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AArcherCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AArcherCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AArcherCharacter::InitializeSparrowOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASparrowHUD* SparrowHUD = Cast<ASparrowHUD>(PlayerController->GetHUD());
		if (SparrowHUD)
		{
			SparrowOverlay = SparrowHUD->GetSparrowOverlay();
			if (SparrowOverlay && Attributes)
			{
				SparrowOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SparrowOverlay->SetStaminaBarPercent(1.f);
				SparrowOverlay->SetGold(0);
				SparrowOverlay->SetSouls(0);
			}
		}
	}
}

void AArcherCharacter::SetHUDHealth()
{
	if (SparrowOverlay && Attributes)
	{
		SparrowOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}