#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AProjectile;
class UInputComponent;
class UInputAction;
class UDecalComponent;
class USceneComponent;
struct FInputActionValue;

/** Reusable gun: cadence and aim live here, independent of input callback frequency. */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class VYXXEN_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UWeaponComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void BindInput(UInputComponent* InputComponent);
	void SetMuzzle(USceneComponent* InMuzzle);
	void SetAimCursor(UDecalComponent* Cursor);
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void SetFiring(bool bHeld);
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void DisableWeapon();
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TSubclassOf<AProjectile> ProjectileClass;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="0.01", Units="Seconds"))
	float FireInterval = .15f;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="1", Units="CentimetersPerSecond"))
	float ProjectileSpeed = 1800.f;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="0.01", Units="Seconds"))
	float ProjectileLifetime = 3.f;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="0"))
	float Damage = 1.f;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="1", Units="Centimeters"))
	float AimRange = 5000.f;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	int32 Team = 0;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FName MuzzleName = TEXT("Muzzle");
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Input")
	TObjectPtr<UInputAction> FireAction;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	int32 ShotsFired = 0;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	void HandleFire(const FInputActionValue& Value);
	void ReleaseFire(const FInputActionValue& Value);
	void UpdateCursor(const FTransform& MuzzleTransform);
	TWeakObjectPtr<USceneComponent> Muzzle;
	TWeakObjectPtr<UDecalComponent> AimCursor;
	float Cooldown = 0.f;
	bool bFiring = false;
	bool bDisabled = false;
};
