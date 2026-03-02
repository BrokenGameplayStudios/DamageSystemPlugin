// DamageableCharacterBase.cpp

#include "DamageableCharacterBase.h"
#include "DamageSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADamageableCharacterBase::ADamageableCharacterBase()
{
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    DamageSystemComponent = CreateDefaultSubobject<UDamageSystemComponent>(TEXT("DamageSystemComponent"));

    bReplicates = true;
}

// Called when the game starts or when spawned
void ADamageableCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    if (DamageSystemComponent)
    {
        DamageSystemComponent->OnDamageTaken.AddDynamic(this, &ADamageableCharacterBase::RespondToDamageTaken);
        DamageSystemComponent->OnDamageAvoided.AddDynamic(this, &ADamageableCharacterBase::RespondToDamageAvoided);
        DamageSystemComponent->OnHealReceived.AddDynamic(this, &ADamageableCharacterBase::RespondToHealReceived);
        DamageSystemComponent->OnDeath.AddDynamic(this, &ADamageableCharacterBase::RespondToDeath);
    }

}

// Called every frame
void ADamageableCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADamageableCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Implement damage taken response (e.g., play hit reaction animation, spawn particles)
void ADamageableCharacterBase::RespondToDamageTaken_Implementation(const FDamageInfo& DamageInfo)
{
    // Implement visual/audio feedback for damage taken (e.g., "hit" effects)
}

// Implement damage taken response (e.g., play hit reaction animation, spawn particles)
void ADamageableCharacterBase::RespondToDamageAvoided_Implementation(const FDamageInfo& DamageInfo)
{
	// Implement visual/audio feedback for avoided damage (e.g., "miss" effects)
}

// Implement damage avoided response (e.g., play block/parry animation, spawn particles)
void ADamageableCharacterBase::RespondToHealReceived_Implementation(float HealAmount, AActor* Healer)
{
	// Implement visual/audio feedback for healing received (e.g., "heal" effects)
}

// Implement death response (e.g., ragdoll, disable input, play animation)
void ADamageableCharacterBase::RespondToDeath_Implementation()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

// Override UE's TakeDamage to route to the component (handles built-in damage events)
float ADamageableCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (DamageSystemComponent)
    {
        FDamageInfo DamageInfo;
        DamageInfo.DamageAmount = DamageAmount;
        // Populate other fields from DamageEvent/Instigator/DamageCauser as needed (e.g., DamageInfo.Instigator = DamageCauser)
        DamageSystemComponent->HandleIncomingDamage(DamageInfo);
    }

    return DamageAmount;
}

// IDamageableInterface implementation
float ADamageableCharacterBase::GetCurrentHealth_Implementation()
{
    if (!DamageSystemComponent)	return 0.0f;

    return DamageSystemComponent->GetCurrentHealth();
}

// IDamageableInterface implementation
float ADamageableCharacterBase::GetMaxHealth_Implementation()
{
    if (!DamageSystemComponent)	return 0.0f;

    return DamageSystemComponent->GetMaxHealth();
}

// IDamageableInterface implementation
bool ADamageableCharacterBase::GetIsDead_Implementation()
{
    if (!DamageSystemComponent) return true;

    return DamageSystemComponent->GetIsDead();
}

// IDamageableInterface implementation
void ADamageableCharacterBase::Heal_Implementation(float HealAmount, AActor* Healer)
{
    if (DamageSystemComponent)
    {
        DamageSystemComponent->HandleIncomingHealing(HealAmount, Healer);
    }

}

// IDamageableInterface implementation
bool ADamageableCharacterBase::TakeDamage_Implementation(const FDamageInfo& DamageInfo)
{
    if (DamageSystemComponent)
    {
        return DamageSystemComponent->HandleIncomingDamage(DamageInfo);
    }
    return false;
}