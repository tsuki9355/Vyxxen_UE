#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

/** A world-space bolt; launch velocity is sampled once and never follows the shooter. */
UCLASS()
class VYXXEN_API AProjectile : public AActor
{
	GENERATED_BODY()
public:
	AProjectile();
	/** Configure before FinishSpawning, while collision and movement are inactive. */
	void InitializeShot(const FVector& Direction, float Speed, float Lifetime, float Damage, int32 InTeam);
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	TObjectPtr<USphereComponent> Collision;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	TObjectPtr<UStaticMeshComponent> Visual;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	int32 Team = 0;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	float HitDamage = 1.f;
private:
	UFUNCTION()
	void HandleHit(UPrimitiveComponent* Component, AActor* Other, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult& Hit);
	bool bConsumed = false;
};
