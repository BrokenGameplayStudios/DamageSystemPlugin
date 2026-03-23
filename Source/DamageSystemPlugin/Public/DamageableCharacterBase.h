// DamageableCharacterBase.h

#pragma once

#include "CoreMinimal.h"
#include "DamageableInterface.h"
#include "GameFramework/Character.h"
#include "DamageableCharacterBase.generated.h"

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

    UFUNCTION(BlueprintNativeEvent)
    void RespondToDamageTaken(const FDamageInfo& DamageInfo);

    UFUNCTION(BlueprintNativeEvent)
    void RespondToDamageAvoided(const FDamageInfo& DamageInfo);

    UFUNCTION(BlueprintNativeEvent)
    void RespondToHealReceived(float HealAmount, AActor* Healer);

    UFUNCTION(BlueprintNativeEvent)
    void RespondToDeath();

    UFUNCTION(BlueprintNativeEvent)
    void RespondToRevive(AActor* Reviver, const FTransform& ReviveTransform);

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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage System")
    TObjectPtr<class UDamageSystemComponent> DamageSystemComponent;
};