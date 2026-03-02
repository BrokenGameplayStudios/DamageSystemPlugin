// DamageableCharacterBase.h
#pragma once

#include "CoreMinimal.h"
#include "DamageableInterface.h" // For the IDamageableInterface
#include "GameFramework/Character.h"
#include "DamageableCharacterBase.generated.h"

// Forward declaration to avoid circular dependency
class UDamageSystemComponent;

UCLASS()
class DAMAGESYSTEMPLUGIN_API ADamageableCharacterBase : public ACharacter, public IDamageableInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ADamageableCharacterBase();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

	// Blueprint events for responding to damage/healing/death (can be overridden in Blueprints for VFX/sounds)
    UFUNCTION(BlueprintNativeEvent)
    void RespondToDamageTaken(const FDamageInfo& DamageInfo);

	// Event for when damage is avoided (e.g., blocked/parried/invincible)
    UFUNCTION(BlueprintNativeEvent)
    void RespondToDamageAvoided(const FDamageInfo& DamageInfo);

	// Event for when healing is received
    UFUNCTION(BlueprintNativeEvent)
    void RespondToHealReceived(float HealAmount, AActor* Healer);

	// Event for when the character dies
    UFUNCTION(BlueprintNativeEvent)
    void RespondToDeath();

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Override UE's TakeDamage to route to the component (handles built-in damage events)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // IDamageableInterface implementation
    virtual float GetCurrentHealth_Implementation() override;
    virtual float GetMaxHealth_Implementation() override;
    virtual bool GetIsDead_Implementation() override;
    virtual void Heal_Implementation(float HealAmount, AActor* Healer) override;
    virtual bool TakeDamage_Implementation(const FDamageInfo& DamageInfo) override;

    // Damage System Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class UDamageSystemComponent> DamageSystemComponent;
};