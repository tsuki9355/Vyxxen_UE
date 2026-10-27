#include "GameManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	DefaultPawnClass = nullptr; // The authored ship Pawn is placed and auto-possessed.
}

void AGameManager::StartPlay()
{
	if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
		Controller->bAutoManageActiveCameraTarget = false;
	Super::StartPlay();
}

void AGameManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bRouteActive || !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.f)
		return;
	SlowElapsed += DeltaSeconds;
	const float Fraction = SlowRampSeconds > 0.f ? FMath::Clamp(SlowElapsed / SlowRampSeconds, 0.f, 1.f) : 1.f;
	SlowAlpha = FMath::Lerp(SlowStart, bSlowHeld ? 1.f : 0.f, Fraction);
	CurrentForwardSpeed = FMath::IsFinite(ForwardSpeed) ? FMath::Max(ForwardSpeed, 0.f) * GetSpeedScale() : 0.f;
	Progress += CurrentForwardSpeed * DeltaSeconds;
}

void AGameManager::SetSlowHeld(bool bHeld)
{
	if (bSlowHeld == bHeld)
		return;
	bSlowHeld = bHeld;
	SlowStart = SlowAlpha;
	SlowElapsed = 0.f;
}

void AGameManager::StopRoute()
{
	bRouteActive = false;
	CurrentForwardSpeed = 0.f;
}
