#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerShip.generated.h"
class UCapsuleComponent;
class UStaticMeshComponent;
class UWeaponComponent;
class UDecalComponent;
class UArmorComponent;
UCLASS()
class VYXXEN_API APlayerShip : public APawn
{
	GENERATED_BODY()
public:
	APlayerShip();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship")
	TObjectPtr<UCapsuleComponent> Hull;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship")
	TObjectPtr<UStaticMeshComponent> ShipMesh;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship")
	TObjectPtr<USceneComponent> Muzzle;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship")
	TObjectPtr<UWeaponComponent> Weapon;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship")
	TObjectPtr<UDecalComponent> AimCursor;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship")
	TObjectPtr<UArmorComponent> Armor;
	/** Stops route and firing once. Respawn rules are outside this combat cut. */
	UFUNCTION(BlueprintCallable, Category="Ship")
	void Crash();
protected:
	virtual void BeginPlay() override;
private:
	bool bCrashed = false;
};
