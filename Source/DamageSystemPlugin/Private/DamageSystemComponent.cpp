// DamageSystemComponent.cpp 

#include "DamageSystemComponent.h"
#include "Net/UnrealNetwork.h"

UDamageSystemComponent::UDamageSystemComponent()
{
	// Setup replication and defaults
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
}

void UDamageSystemComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwner()->HasAuthority())
    {
		// Ensure health is initialized on server
        CurrentHealth = MaxHealth;
    }
}

void UDamageSystemComponent::InitializeHealth(float InitialMaxHealth)
{
    if (GetOwner()->HasAuthority())
    {
		// Directly set values on server
        MaxHealth = InitialMaxHealth;
        CurrentHealth = MaxHealth;
    }
    else
    {
		// Request server to initialize health (handles replication and events)
        Server_SetMaxHealth(InitialMaxHealth);
        Server_ResetHealth();
    }
}

// Client calls this to request damage application; server validates and applies
bool UDamageSystemComponent::HandleIncomingDamage(const FDamageInfo& DamageInfo)
{
    if (GetOwner()->HasAuthority())
    {
		// Directly apply damage on server
        return ApplyDamageInternal(DamageInfo);
    }
    else
    {
        Server_HandleIncomingDamage(DamageInfo);
        return true;  // Assume applied; actual result on server (client can't wait for sync)
    }
}

bool UDamageSystemComponent::ApplyDamageInternal(const FDamageInfo& DamageInfo)
{
	// Basic validation: ignore if already dead or damage is non-positive
    if (IsDead() || DamageInfo.DamageAmount <= 0.0f)
    {
		// Broadcast avoided damage event for feedback (e.g., "miss" effects)
        OnDamageAvoided.Broadcast(DamageInfo);
        return false;
    }

	// Apply damage and calculate delta for event broadcasting
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageInfo.DamageAmount);
    float Delta = CurrentHealth - OldHealth;

	// Broadcast events for damage taken and health change
    OnDamageTaken.Broadcast(DamageInfo);
    OnHealthChanged.Broadcast(CurrentHealth, Delta);

    if (IsDead())
    {
		// Broadcast death event (e.g., "death" effects)
        Multicast_OnDeath();
    }

    return true;
}

// Server RPC implementation for handling incoming damage
void UDamageSystemComponent::Server_HandleIncomingDamage_Implementation(const FDamageInfo& DamageInfo)
{    
    ApplyDamageInternal(DamageInfo);
}

// Server RPC validation for anti-cheat
bool UDamageSystemComponent::Server_HandleIncomingDamage_Validate(const FDamageInfo& DamageInfo)
{
    return true;  // Add anti-cheat validation
}

void UDamageSystemComponent::HandleIncomingHealing(float HealAmount, AActor* Healer)
{
    if (GetOwner()->HasAuthority())
    {
		// Directly apply healing on server
        ApplyHealingInternal(HealAmount, Healer);
    }
    else
    {		
        Server_HandleIncomingHealing(HealAmount, Healer);
    }
}

// Internal healing logic, called on server
void UDamageSystemComponent::ApplyHealingInternal(float HealAmount, AActor* Healer)
{
    if (HealAmount <= 0.0f || IsDead()) return;

    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
    float Delta = CurrentHealth - OldHealth;

    OnHealReceived.Broadcast(HealAmount, Healer);
    OnHealthChanged.Broadcast(CurrentHealth, Delta);
}

// Server RPC implementation for handling incoming healing
void UDamageSystemComponent::Server_HandleIncomingHealing_Implementation(float HealAmount, AActor* Healer)
{
    ApplyHealingInternal(HealAmount, Healer);
}

// Server RPC validation for anti-cheat
bool UDamageSystemComponent::Server_HandleIncomingHealing_Validate(float HealAmount, AActor* Healer)
{
    return HealAmount >= 0.0f;
}

// Client calls this to request max health change; server validates and applies
void UDamageSystemComponent::SetMaxHealth(float NewMaxHealth)
{
    if (GetOwner()->HasAuthority())
    {
        SetMaxHealthInternal(NewMaxHealth);
    }
    else
    {
        Server_SetMaxHealth(NewMaxHealth);
    }
}

// Internal logic for setting max health, called on server
void UDamageSystemComponent::SetMaxHealthInternal(float NewMaxHealth)
{
    if (NewMaxHealth <= 0.0f) return;

    MaxHealth = NewMaxHealth;
    CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
    OnMaxHealthChanged.Broadcast(MaxHealth);
    OnHealthChanged.Broadcast(CurrentHealth, 0.0f);
}

// Server RPC implementation for setting max health
void UDamageSystemComponent::Server_SetMaxHealth_Implementation(float NewMaxHealth)
{
    SetMaxHealthInternal(NewMaxHealth);
}

// Server RPC validation for anti-cheat
bool UDamageSystemComponent::Server_SetMaxHealth_Validate(float NewMaxHealth)
{
    return NewMaxHealth > 0.0f;
}

// Client calls this to request current health change; server validates and applies
void UDamageSystemComponent::SetCurrentHealth(float NewHealth)
{
    if (GetOwner()->HasAuthority())
    {
        SetCurrentHealthInternal(NewHealth);
    }
    else
    {
        Server_SetCurrentHealth(NewHealth);
    }
}

// Internal logic for setting current health, called on server
void UDamageSystemComponent::SetCurrentHealthInternal(float NewHealth)
{
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    float Delta = CurrentHealth - OldHealth;

    OnHealthChanged.Broadcast(CurrentHealth, Delta);

    if (IsDead())
    {
        Multicast_OnDeath();
    }
}

// Server RPC implementation for setting current health
void UDamageSystemComponent::Server_SetCurrentHealth_Implementation(float NewHealth)
{
    SetCurrentHealthInternal(NewHealth);
}

// Server RPC validation for anti-cheat
bool UDamageSystemComponent::Server_SetCurrentHealth_Validate(float NewHealth)
{
    return true;
}

// Client calls this to request health reset; server validates and applies
void UDamageSystemComponent::ResetHealth()
{
    if (GetOwner()->HasAuthority())
    {
        ResetHealthInternal();
    }
    else
    {
        Server_ResetHealth();
    }
}

// Internal logic for resetting health, called on server
void UDamageSystemComponent::ResetHealthInternal()
{
    SetCurrentHealthInternal(MaxHealth);
}

// Server RPC implementation for resetting health
void UDamageSystemComponent::Server_ResetHealth_Implementation()
{
    ResetHealthInternal();
}

// Server RPC validation for anti-cheat
bool UDamageSystemComponent::Server_ResetHealth_Validate()
{
    return true;
}

// Replication notification functions to update clients when health values change
void UDamageSystemComponent::OnRep_CurrentHealth()
{
    OnHealthChanged.Broadcast(CurrentHealth, 0.0f);  // Delta not tracked here
}

// Replication notification for max health changes
void UDamageSystemComponent::OnRep_MaxHealth()
{
    OnMaxHealthChanged.Broadcast(MaxHealth);
}

// Multicast function to broadcast death event to all clients
void UDamageSystemComponent::Multicast_OnDeath_Implementation()
{
    OnDeath.Broadcast();
}

// Replication setup for health properties
void UDamageSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UDamageSystemComponent, MaxHealth);
    DOREPLIFETIME(UDamageSystemComponent, CurrentHealth);
}