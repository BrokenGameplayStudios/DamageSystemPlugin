// DamageableCharacterBase.cpp

#include "DamageableCharacterBase.h"
#include "DamageSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ADamageableCharacterBase::ADamageableCharacterBase()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    DamageSystemComponent = CreateDefaultSubobject<UDamageSystemComponent>(TEXT("DamageSystemComponent"));

    bReplicates = true;
    SetReplicateMovement(true);
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
        DamageSystemComponent->OnRevive.AddDynamic(this, &ADamageableCharacterBase::RespondToRevive);
    }

}

void ADamageableCharacterBase::RespondToDamageAvoided_Implementation(const FDamageInfo& DamageInfo)
{
}

void ADamageableCharacterBase::RespondToHealReceived_Implementation(float HealAmount, AActor* Healer)
{
}

void ADamageableCharacterBase::RespondToDeath_Implementation()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ADamageableCharacterBase::RespondToDamageTaken_Implementation(const FDamageInfo& DamageInfo)
{
}

void ADamageableCharacterBase::RespondToRevive_Implementation(AActor* Reviver, const FTransform& ReviveTransform)
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetSimulatePhysics(false);

    // Force mesh render update to fix client invisibility (marks render state dirty)
    GetMesh()->MarkRenderStateDirty();
    GetMesh()->ReregisterComponent();
    GetMesh()->MarkRenderTransformDirty();
    GetMesh()->MarkRenderDynamicDataDirty();

    // Snap mesh back to default relative transform
    GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), FRotator(0.0f, -90.0f, 0.0f)); // Adjust defaults as needed    
        
    // Add montage play here if desired, or override in BP
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

float ADamageableCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (DamageSystemComponent)
    {
        FDamageInfo DamageInfo;
        DamageInfo.DamageAmount = DamageAmount;        
        DamageSystemComponent->HandleIncomingDamage(DamageInfo);
    }

    return DamageAmount;
}

float ADamageableCharacterBase::GetCurrentHealth_Implementation()
{
    if (!DamageSystemComponent)	return 0.0f;

    return DamageSystemComponent->GetCurrentHealth();
}

float ADamageableCharacterBase::GetMaxHealth_Implementation()
{
    if (!DamageSystemComponent)	return 0.0f;

    return DamageSystemComponent->GetMaxHealth();
}

bool ADamageableCharacterBase::GetIsDead_Implementation()
{
    if (!DamageSystemComponent) return true;

    return DamageSystemComponent->GetIsDead();
}

void ADamageableCharacterBase::Heal_Implementation(float HealAmount, AActor* Healer)
{
    if (DamageSystemComponent)
    {
        DamageSystemComponent->HandleIncomingHealing(HealAmount, Healer);
    }

}

bool ADamageableCharacterBase::TakeDamage_Implementation(const FDamageInfo& DamageInfo)
{
    if (DamageSystemComponent)
    {
        return DamageSystemComponent->HandleIncomingDamage(DamageInfo);
    }
    return false;
}