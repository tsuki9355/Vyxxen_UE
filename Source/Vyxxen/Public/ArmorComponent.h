#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArmorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FArmorDestroyed);

/** Receives Unreal damage and guarantees a single Alive-to-Destroyed transition. */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class VYXXEN_API UArmorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Armor", meta=(ClampMin="0.01"))
	float MaxHitPoints = 1.f;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Armor")
	int32 Team = 1;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Armor")
	float HitPoints = 1.f;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Armor")
	bool bDestroyed = false;
								UPROPERTY(BlueprintAssignable, Category="Armor")
	FArmorDestroyed OnDestroyed;
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void HandleDamage(AActor* Actor, float Damage, const UDamageType* Type, AController* Instigator, AActor* Causer);
};
