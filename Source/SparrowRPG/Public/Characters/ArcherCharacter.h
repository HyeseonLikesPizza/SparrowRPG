// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "ArcherCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USparrowOverlay;
class ASoul;
class ATreasure;
class ULoadWidget;

UCLASS()
class SPARROWRPG_API AArcherCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AArcherCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void ShieldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;


	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;
	void SetCameraTransform(FTransform Transform);
	FTransform GetCameraTransform() const;
	void SetSpringArmTransform(FTransform Transform);
	FTransform GetSpringArmTransform() const;
	void SetAttributeComponent(UAttributeComponent* Component);
	void LoadGold(int32 gold);
	void LoadSouls(int32 NumberOfSouls);
	void LoadStamina(float Stamina, float MaxStamina);
	void LoadHealth(float Health, float MaxHealth);
	void LoadDodgeCost(float NewCost);
	void LoadStaminaRegenRate(float NewRate);
	void LoadActionState(EActionState state);
	void LoadCharacterState(ECharacterState state);
	void LoadEquippedWeapon(AWeapon* weapon);
	void LoadPlayerName(FString name);
	void LoadPlayerLevel(int32 level);
	void LoadPlayerPlace(FString place);
	void LoadPlayTime(float playtime);
	void EquipWeapon(AWeapon* Weapon, bool PlaySound = true);
	void RemoveWeapon();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	bool bRightMouseClicked = false;
	

protected:


	/** 입력에 필요한 콜백함수 */
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Dodge();
	void UseStaminaAndHUDUpdate(float Cost);
	void EKeyPressed();
	void RightMouseClicked();
	void RightMouseReleased();
	virtual void Attack() override;
	void CounterAttack();
	void Skill_1();

	UFUNCTION(BlueprintNativeEvent)
	void SaveGame();

	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void HideLoadWidget(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void ShowLoadWidget(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void SpawnCastingParticles();

	/* Combat */

	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();
	bool IsOccupied();
	void SetPlayerPlace(FString place);
	void SetPlayTime(float playtime);
	void Skill_SwordShield_CastingAndStrike();

	UFUNCTION(BlueprintCallable)
	void SpawnCastingStrikeParticles();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	int32 PlayerLevel;

	UPROPERTY()
	FString PlayerPlace;

	UPROPERTY()
	float PlayTime;




	/* Advanced Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* ArcherContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SaveGameAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LoadGameAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RightMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Skill_1_Action;

private:
	bool IsUnoccupied();
	void InitializeSparrowOverlay();
	void SetHUDHealth();
	void SpawnDefaultShield();
	void SpawnDefaultWeapon();
	void SpawnSwordShield();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/* 캐릭터 컴포넌트 */

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* CastingStrikeMontage;

	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY()
	USparrowOverlay* SparrowOverlay;

	UPROPERTY()
	ULoadWidget* LoadWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> ShieldClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShakeBase> CameraShake;

	float startSeconds;

	bool bCounterAttack = false;

	void SetCounterAttackEnable();
	void SetCounterAttackDisable();

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* CounterAttackMontage;

	FTimerHandle CounterAttackTimer;

	float CounterAttackInputTime = 3.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* CastingAndStrikeParticles;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* CastingParticles;
	

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE UAttributeComponent* GetAttributeComponent() const { return Attributes; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE float GetPlayTime() const { return PlayTime; }
	FORCEINLINE int32 GetPlayerLevel() const { return PlayerLevel; }
	FORCEINLINE FString GetPlayerName() const { return PlayerName; }
	FORCEINLINE FString GetPlayerPlace() const { return PlayerPlace; }
};
