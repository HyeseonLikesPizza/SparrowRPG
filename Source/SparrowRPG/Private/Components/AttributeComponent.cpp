

#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}

void UAttributeComponent::SetGold(int32 AmountOfGold)
{
	Gold = AmountOfGold;
}

void UAttributeComponent::SetSouls(int32 NumberOfSouls)
{
	Souls = NumberOfSouls;
}

void UAttributeComponent::SetHealth(float NewHealth)
{
	Health = NewHealth;
}

void UAttributeComponent::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}

void UAttributeComponent::SetStamina(float NewStamina)
{
	Stamina = NewStamina;
}

void UAttributeComponent::SetMaxStamina(float NewMaxStamina)
{
	MaxStamina = NewMaxStamina;
}

void UAttributeComponent::SetDodgeCost(float NewCost)
{
	DodgeCost = NewCost;
}

void UAttributeComponent::SetStaminaRegenRate(float NewRate)
{
	StaminaRegenRate = NewRate;
}
