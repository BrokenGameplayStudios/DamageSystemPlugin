// DamageSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "DamageSystemTypes.h"
#include "Components/ActorComponent.h"
#include "DamageSystemComponent.generated.h"

// Delegates for Blueprint events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, const FDamageInfo&, DamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageAvoided, const FDamageInfo&, DamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealReceived, float, HealAmount, AActor*, Healer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChanged, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DAMAGESYSTEMPLUGIN_API UDamageSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDamageSystemComponent();

    // Replicated properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxHealth, Category = "Damage System")
    float MaxHealth;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Damage System")
    float CurrentHealth;

    // Delegates for Blueprint events (e.g., VFX, sounds)
    UPROPERTY(BlueprintAssignable, Category = "Damage System")
    FOnDamageTaken OnDamageTaken;

    UPROPERTY(BlueprintAssignable, Category = "Damage System")
    FOnDamageAvoided OnDamageAvoided;

    UPROPERTY(BlueprintAssignable, Category = "Damage System")
    FOnHealReceived OnHealReceived;

    UPROPERTY(BlueprintAssignable, Category = "Damage System")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Damage System")
    FOnMaxHealthChanged OnMaxHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Damage System")
    FOnDeath OnDeath;

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "Damage System")
    void InitializeHealth(float InitialMaxHealth = 100.0f);

    // Getters (local, no replication needed)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage System")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage System")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage System")
    bool IsAlive() const { return CurrentHealth > 0.0f; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage System")
    bool IsDead() const { return CurrentHealth <= 0.0f; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage System")
    bool GetIsDead() const { return IsDead(); }  // For interface compatibility

    // Mutators (client requests, server applies) - Returns true if damage applied
    UFUNCTION(BlueprintCallable, Category = "Damage System")
    bool HandleIncomingDamage(const FDamageInfo& DamageInfo);

    UFUNCTION(BlueprintCallable, Category = "Damage System")
    void HandleIncomingHealing(float HealAmount, AActor* Healer);

    UFUNCTION(BlueprintCallable, Category = "Damage System")
    void SetMaxHealth(float NewMaxHealth);

    UFUNCTION(BlueprintCallable, Category = "Damage System")
    void SetCurrentHealth(float NewHealth);

    UFUNCTION(BlueprintCallable, Category = "Damage System")
    void ResetHealth();

protected:
    virtual void BeginPlay() override;

    // Internal helpers for server logic
    bool ApplyDamageInternal(const FDamageInfo& DamageInfo);
    void ApplyHealingInternal(float HealAmount, AActor* Healer);
    void SetMaxHealthInternal(float NewMaxHealth);
    void SetCurrentHealthInternal(float NewHealth);
    void ResetHealthInternal();

    // RepNotify functions
    UFUNCTION()
    void OnRep_CurrentHealth();

    UFUNCTION()
    void OnRep_MaxHealth();

    // Server RPCs
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_HandleIncomingDamage(const FDamageInfo& DamageInfo);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_HandleIncomingHealing(float HealAmount, AActor* Healer);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetMaxHealth(float NewMaxHealth);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetCurrentHealth(float NewHealth);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ResetHealth();

    // Multicast for synced events
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnDeath();
};