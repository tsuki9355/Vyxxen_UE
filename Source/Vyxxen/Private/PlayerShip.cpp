#include "PlayerShip.h"
#include "ArmorComponent.h"
#include "FlightMovementComponent.h"
#include "GameManager.h"
#include "WeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
APlayerShip::APlayerShip()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FlightRoot"));
	Hull = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Hull"));
	Hull->SetupAttachment(RootComponent);
	Hull->InitCapsuleSize(17.348074f, 50.147388f);
	Hull->SetRelativeLocation(FVector(0.f, 0.f, 14.f));
	Hull->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	Hull->SetCollisionProfileName(TEXT("Pawn"));
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMesh->SetupAttachment(RootComponent);
	ShipMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(ShipMesh);
	Muzzle->SetRelativeLocation(FVector(60.f, 0.f, 14.f));
	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	Armor = CreateDefaultSubobject<UArmorComponent>(TEXT("Armor"));
	Armor->Team = 0;
	AimCursor = CreateDefaultSubobject<UDecalComponent>(TEXT("AimCursor"));
	AimCursor->SetupAttachment(RootComponent);
	AimCursor->SetAbsolute(true, true, true);
	AimCursor->DecalSize = FVector(2.f, 10.f, 10.f);
	AimCursor->SetVisibility(false);
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}
void APlayerShip::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TArray<UCapsuleComponent*> Capsules;
	GetComponents(Capsules);
	for (UCapsuleComponent* Capsule : Capsules)
	{
		if (Capsule->GetFName() == TEXT("CapsuleCollider"))
		{
			// Preserve the Blueprint's authored collider; the native hull is a fallback.
			Hull->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Hull = Capsule;
			Hull->SetCollisionProfileName(TEXT("Pawn"));
			break;
		}
	}
}

void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	Weapon->SetMuzzle(Muzzle);
	Weapon->SetAimCursor(AimCursor);
	if (UFlightMovementComponent* Flight = FindComponentByClass<UFlightMovementComponent>())
		Weapon->AddTickPrerequisiteComponent(Flight);
}
void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UFlightMovementComponent* Flight = FindComponentByClass<UFlightMovementComponent>())
		Flight->BindInput(PlayerInputComponent);
	Weapon->BindInput(PlayerInputComponent);
}
void APlayerShip::Crash()
{
	if (bCrashed)
		return;
	bCrashed = true;
	Weapon->DisableWeapon();
	AimCursor->SetVisibility(false);
	Hull->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (AGameManager* Manager = GetWorld()->GetAuthGameMode<AGameManager>())
		Manager->StopRoute();
	UE_LOG(LogTemp, Display, TEXT("Vyxxen: ship crashed; route and weapon stopped."));
}
