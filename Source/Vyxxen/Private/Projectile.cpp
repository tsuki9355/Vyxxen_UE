#include "Projectile.h"
#include "ArmorComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"

AProjectile::AProjectile()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(3.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_GameTraceChannel1);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Collision->OnComponentHit.AddDynamic(this, &AProjectile::HandleHit);
	Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
	Visual->SetupAttachment(Collision);
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Visual->SetRelativeScale3D(FVector(.16f, .045f, .045f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (Sphere.Succeeded())
		Visual->SetStaticMesh(Sphere.Object);
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->SetUpdatedComponent(Collision);
	Movement->ProjectileGravityScale = 0.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;
	Movement->bSweepCollision = true;
	InitialLifeSpan = 3.f;
}

void AProjectile::InitializeShot(const FVector& Direction, float Speed, float Lifetime, float Damage, int32 InTeam)
{
	Team = InTeam;
	HitDamage = Damage;
	SetLifeSpan(Lifetime);
	Movement->Velocity = Direction.GetSafeNormal() * Speed;
	Movement->InitialSpeed = 0.f; // Preserve the world-space velocity through InitializeComponent.
	Movement->MaxSpeed = Speed;
	Movement->bInitialVelocityInLocalSpace = false;
	Collision->IgnoreActorWhenMoving(GetOwner(), true);
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		const UArmorComponent* Armor = It->FindComponentByClass<UArmorComponent>();
		if (Armor && Armor->Team == Team)
			Collision->IgnoreActorWhenMoving(*It, true);
	}
}

void AProjectile::HandleHit(UPrimitiveComponent* Component, AActor* Other, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult& Hit)
{
	if (bConsumed || !Other || Other == GetOwner())
		return;
	bConsumed = true;
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UGameplayStatics::ApplyPointDamage(Other, HitDamage, GetActorForwardVector(), Hit, GetInstigatorController(), this, nullptr);
	UE_LOG(LogTemp, Verbose, TEXT("Vyxxen: projectile impact on %s."), *Other->GetName());
	Destroy();
}
