#include "WeaponComponent.h"
#include "ArmorComponent.h"
#include "Projectile.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "GameFramework/Pawn.h"
#include "Components/DecalComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ProjectileClass = AProjectile::StaticClass();
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	TArray<USceneComponent*> Components;
	GetOwner()->GetComponents(Components);
	for (USceneComponent* Component : Components)
	{
		if (Component->GetFName() == MuzzleName)
		{
			Muzzle = Component;
			break;
		}
	}
	if (!Muzzle.IsValid())
		UE_LOG(LogTemp, Warning, TEXT("Weapon on %s needs a scene component named %s."), *GetOwner()->GetName(), *MuzzleName.ToString());
}

void UWeaponComponent::BindInput(UInputComponent* InputComponent)
{
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput || !FireAction)
		return;
	EnhancedInput->BindAction(FireAction, ETriggerEvent::Triggered, this, &UWeaponComponent::HandleFire);
	EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, this, &UWeaponComponent::ReleaseFire);
	EnhancedInput->BindAction(FireAction, ETriggerEvent::Canceled, this, &UWeaponComponent::ReleaseFire);
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bDisabled || !Muzzle.IsValid() || DeltaTime <= 0.f || !FMath::IsFinite(DeltaTime))
		return;
	const UArmorComponent* Armor = GetOwner()->FindComponentByClass<UArmorComponent>();
	if (Armor && Armor->bDestroyed)
	{
		DisableWeapon();
		return;
	}
	// Preserve the fractional frame remainder while held, without banking idle shots.
	Cooldown = bFiring ? Cooldown - DeltaTime : FMath::Max(0.f, Cooldown - DeltaTime);
	const FTransform MuzzleTransform = Muzzle->GetComponentTransform();
	UpdateCursor(MuzzleTransform);
	if (!bFiring || Cooldown > 0.f || !ProjectileClass || FireInterval <= 0.f || ProjectileSpeed <= 0.f || ProjectileLifetime <= 0.f)
		return;
	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.bDeferConstruction = true;
	// Normalize scale so the authored ship scale cannot change bolt speed or collision size.
	const FTransform ShotTransform(MuzzleTransform.GetRotation(), MuzzleTransform.GetLocation());
	AProjectile* Shot = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ShotTransform, Params);
	if (!Shot)
		return;
	Shot->InitializeShot(MuzzleTransform.GetUnitAxis(EAxis::X), ProjectileSpeed, ProjectileLifetime, Damage, Team);
	Shot->FinishSpawning(ShotTransform);
	++ShotsFired;
	Cooldown = FMath::Max(0.f, Cooldown + FireInterval);
}

void UWeaponComponent::UpdateCursor(const FTransform& MuzzleTransform)
{
	if (!AimCursor.IsValid())
		return;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponAim), false, GetOwner());
	// Center trace and projectile use the same world-static/dynamic/pawn blockers.
	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_WorldStatic);
	Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
	Objects.AddObjectTypesToQuery(ECC_Pawn);
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		const UArmorComponent* Armor = It->FindComponentByClass<UArmorComponent>();
		if (It->IsA<AProjectile>() || (Armor && Armor->Team == Team))
			Params.AddIgnoredActor(*It);
	}
	const FVector Origin = MuzzleTransform.GetLocation();
	FHitResult Hit;
	TArray<FHitResult> Hits;
	GetWorld()->LineTraceMultiByObjectType(Hits, Origin,
		Origin + MuzzleTransform.GetUnitAxis(EAxis::X) * AimRange, Objects, Params);
	bool bHit = false;
	for (const FHitResult& Candidate : Hits)
	{
		// Trigger volumes and other objects that ignore bolts must not mask the cursor.
		if (Candidate.GetComponent() && Candidate.GetComponent()->GetCollisionResponseToChannel(ECC_GameTraceChannel1) == ECR_Block)
		{
			Hit = Candidate;
			bHit = true;
			break;
		}
	}
	const bool bShow = bHit && Hit.GetComponent() && Hit.GetComponent()->bReceivesDecals;
	AimCursor->SetVisibility(bShow);
	if (!bShow)
		return;
	AimCursor->SetWorldLocationAndRotation(Hit.ImpactPoint + Hit.ImpactNormal * .5f, (-Hit.ImpactNormal).Rotation());
}

void UWeaponComponent::SetMuzzle(USceneComponent* InMuzzle)
{
	Muzzle = InMuzzle;
}

void UWeaponComponent::SetAimCursor(UDecalComponent* Cursor)
{
	AimCursor = Cursor;
}

void UWeaponComponent::SetFiring(bool bHeld)
{
	bFiring = bHeld && !bDisabled;
}
void UWeaponComponent::DisableWeapon()
{
	bDisabled = true;
	bFiring = false;
	if (AimCursor.IsValid())
		AimCursor->SetVisibility(false);
}
void UWeaponComponent::HandleFire(const FInputActionValue& Value)
{
	SetFiring(Value.Get<bool>());
}

void UWeaponComponent::ReleaseFire(const FInputActionValue&)
{
	SetFiring(false);
}
void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DisableWeapon();
	Super::EndPlay(EndPlayReason);
}
