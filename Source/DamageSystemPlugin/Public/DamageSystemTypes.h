// DamageSystemTypes.h
#pragma once

#include "CoreMinimal.h"
#include "DamageSystemTypes.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None UMETA(DisplayName = "None"),
	Physical UMETA(DisplayName = "Physical"),
	Magical UMETA(DisplayName = "Magical"),
	Environment UMETA(DisplayName ="Environment"),
};

UENUM(BlueprintType)
enum class EDamageResponse : uint8
{
	None UMETA(DisplayName ="None"),
	HitReaction UMETA(DisplayName = "Hit Reaction"),
	Knockback UMETA(DisplayName = "Knockback"),
	Stagger UMETA(DisplayName = "Stagger"),
	Stun UMETA(DisplayName = "Stun"),
};


USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	AActor* DamageCauser;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	bool CanBeBlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	bool CanBeParried = false;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	bool ShouldDamageInvincible = false;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	bool ShouldForceInterrupt = false;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	EDamageType DamageType = EDamageType::None;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Damage")
	EDamageResponse DamageResponse = EDamageResponse::None;
};