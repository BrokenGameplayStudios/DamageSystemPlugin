// DamageSystemComponent.cpp (Updated with HealthScalar in ReviveInternal, and all multicasts)

#include "DamageSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"  // For AActor and HasAuthority()

UDamageSystemComponent::UDamageSystemComponent()
{
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
        CurrentHealth = MaxHealth;
    }
}

void UDamageSystemComponent::InitializeHealth(float InitialMaxHealth)
{
    if (GetOwner()->HasAuthority())
    {
        MaxHealth = InitialMaxHealth;
        CurrentHealth = MaxHealth;
    }
    // Clients get values via replication
}

bool UDamageSystemComponent::HandleIncomingDamage(const FDamageInfo& DamageInfo)
{
    if (GetOwner()->HasAuthority())
    {
        return ApplyDamageInternal(DamageInfo);
    }
    else
    {
        Server_HandleIncomingDamage(DamageInfo);
        return true;
    }
}

bool UDamageSystemComponent::ApplyDamageInternal(const FDamageInfo& DamageInfo)
{
    if (IsDead() || DamageInfo.DamageAmount <= 0.0f)
    {
        Multicast_DamageAvoided(DamageInfo);
        return false;
    }

    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageInfo.DamageAmount);
    float Delta = CurrentHealth - OldHealth;

    Multicast_DamageTaken(DamageInfo);

    Multicast_HealthChanged(CurrentHealth, Delta);

    if (IsDead())
    {
        Multicast_OnDeath();
    }

    return true;
}

void UDamageSystemComponent::Server_HandleIncomingDamage_Implementation(const FDamageInfo& DamageInfo)
{
    ApplyDamageInternal(DamageInfo);
}

bool UDamageSystemComponent::Server_HandleIncomingDamage_Validate(const FDamageInfo& DamageInfo)
{
    return true;
}

void UDamageSystemComponent::HandleIncomingHealing(float HealAmount, AActor* Healer)
{
    if (GetOwner()->HasAuthority())
    {
        ApplyHealingInternal(HealAmount, Healer);
    }
    else
    {
        Server_HandleIncomingHealing(HealAmount, Healer);
    }
}

void UDamageSystemComponent::ApplyHealingInternal(float HealAmount, AActor* Healer)
{
    if (HealAmount <= 0.0f || IsDead()) return;

    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
    float Delta = CurrentHealth - OldHealth;

    Multicast_HealReceived(HealAmount, Healer);

    Multicast_HealthChanged(CurrentHealth, Delta);
}

void UDamageSystemComponent::Server_HandleIncomingHealing_Implementation(float HealAmount, AActor* Healer)
{
    ApplyHealingInternal(HealAmount, Healer);
}

bool UDamageSystemComponent::Server_HandleIncomingHealing_Validate(float HealAmount, AActor* Healer)
{
    return HealAmount >= 0.0f;
}

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

void UDamageSystemComponent::SetMaxHealthInternal(float NewMaxHealth)
{
    if (NewMaxHealth <= 0.0f) return;

    MaxHealth = NewMaxHealth;
    CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
    Multicast_MaxHealthChanged(MaxHealth);

    Multicast_HealthChanged(CurrentHealth, 0.0f);
}

void UDamageSystemComponent::Server_SetMaxHealth_Implementation(float NewMaxHealth)
{
    SetMaxHealthInternal(NewMaxHealth);
}

bool UDamageSystemComponent::Server_SetMaxHealth_Validate(float NewMaxHealth)
{
    return NewMaxHealth > 0.0f;
}

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

void UDamageSystemComponent::SetCurrentHealthInternal(float NewHealth)
{
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    float Delta = CurrentHealth - OldHealth;

    Multicast_HealthChanged(CurrentHealth, Delta);

    if (IsDead())
    {
        Multicast_OnDeath();
    }
}

void UDamageSystemComponent::Server_SetCurrentHealth_Implementation(float NewHealth)
{
    SetCurrentHealthInternal(NewHealth);
}

bool UDamageSystemComponent::Server_SetCurrentHealth_Validate(float NewHealth)
{
    return true;
}

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

void UDamageSystemComponent::ResetHealthInternal()
{
    SetCurrentHealthInternal(MaxHealth);
}

void UDamageSystemComponent::Server_ResetHealth_Implementation()
{
    ResetHealthInternal();
}

bool UDamageSystemComponent::Server_ResetHealth_Validate()
{
    return true;
}

void UDamageSystemComponent::Revive(AActor* Reviver, float HealthScalar)
{
    ReviveInternal(Reviver, FTransform::Identity, HealthScalar);
}

void UDamageSystemComponent::ReviveWithTransform(AActor* Reviver, const FTransform& ReviveTransform, float HealthScalar)
{
    ReviveInternal(Reviver, ReviveTransform, HealthScalar);
}

void UDamageSystemComponent::ReviveInternal(AActor* Reviver, const FTransform& ReviveTransform, float HealthScalar)
{
    if (GetOwner()->HasAuthority())
    {
        if (!IsDead()) return;

        if (!ReviveTransform.Equals(FTransform::Identity))
        {
            GetOwner()->SetActorTransform(ReviveTransform);
        }

        float ReviveHealth = FMath::Clamp(HealthScalar, 0.0f, 1.0f) * MaxHealth;
        SetCurrentHealthInternal(ReviveHealth);
        Multicast_OnRevive(Reviver, GetOwner()->GetActorTransform());
    }
    else
    {
        Server_Revive(Reviver, ReviveTransform, HealthScalar);
    }
}

void UDamageSystemComponent::Server_Revive_Implementation(AActor* Reviver, const FTransform& ReviveTransform, float HealthScalar)
{
    ReviveInternal(Reviver, ReviveTransform, HealthScalar);
}

bool UDamageSystemComponent::Server_Revive_Validate(AActor* Reviver, const FTransform& ReviveTransform, float HealthScalar)
{
    return true;
}

void UDamageSystemComponent::OnRep_CurrentHealth()
{
    OnHealthChanged.Broadcast(CurrentHealth, 0.0f);
}

void UDamageSystemComponent::OnRep_MaxHealth()
{
    OnMaxHealthChanged.Broadcast(MaxHealth);
}

void UDamageSystemComponent::Multicast_OnDeath_Implementation()
{
    OnDeath.Broadcast();
}

void UDamageSystemComponent::Multicast_OnRevive_Implementation(AActor* Reviver, const FTransform& ReviveTransform)
{
    OnRevive.Broadcast(Reviver, ReviveTransform);
}

void UDamageSystemComponent::Multicast_DamageTaken_Implementation(const FDamageInfo& DamageInfo)
{
    OnDamageTaken.Broadcast(DamageInfo);
}

void UDamageSystemComponent::Multicast_DamageAvoided_Implementation(const FDamageInfo& DamageInfo)
{
    OnDamageAvoided.Broadcast(DamageInfo);
}

void UDamageSystemComponent::Multicast_HealReceived_Implementation(float HealAmount, AActor* Healer)
{
    OnHealReceived.Broadcast(HealAmount, Healer);
}

void UDamageSystemComponent::Multicast_HealthChanged_Implementation(float NewHealth, float Delta)
{
    OnHealthChanged.Broadcast(NewHealth, Delta);
}

void UDamageSystemComponent::Multicast_MaxHealthChanged_Implementation(float NewMaxHealth)
{
    OnMaxHealthChanged.Broadcast(NewMaxHealth);
}

void UDamageSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UDamageSystemComponent, MaxHealth);
    DOREPLIFETIME(UDamageSystemComponent, CurrentHealth);
}