#include "ArmorComponent.h"
#include "PlayerShip.h"
#include "WeaponComponent.h"
#include "Projectile.h"

void UArmorComponent::BeginPlay()
{
	Super::BeginPlay();
	HitPoints = FMath::Max(MaxHitPoints, .01f);
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UArmorComponent::HandleDamage);
}

void UArmorComponent::HandleDamage(AActor* Actor, float Damage, const UDamageType* Type, AController* Instigator, AActor* Causer)
{
	if (bDestroyed || !FMath::IsFinite(Damage) || Damage <= 0.f)
		return;
	if (const AProjectile* Shot = Cast<AProjectile>(Causer); Shot && Shot->Team == Team)
		return;
	HitPoints = FMath::Max(0.f, HitPoints - Damage);
	if (HitPoints > 0.f)
		return;
	bDestroyed = true;
	if (UWeaponComponent* Weapon = Actor->FindComponentByClass<UWeaponComponent>())
		Weapon->DisableWeapon();
	OnDestroyed.Broadcast();
	UE_LOG(LogTemp, Display, TEXT("Vyxxen: %s destroyed once by %s."), *Actor->GetName(), *GetNameSafe(Causer));
	if (APlayerShip* Ship = Cast<APlayerShip>(Actor))
		Ship->Crash();
	else
		Actor->Destroy();
}
