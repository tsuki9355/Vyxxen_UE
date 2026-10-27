#include "RouteCameraComponent.h"
#include "GameManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

URouteCameraComponent::URouteCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URouteCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	InitialLocation = GetOwner()->GetActorLocation();
	InitialRotation = GetOwner()->GetActorRotation();
	Manager = GetWorld()->GetAuthGameMode<AGameManager>();
	if (!Manager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Route camera needs GameManager as the level GameMode."));
		SetComponentTickEnabled(false);
		return;
	}
	InitialProgress = Manager->Progress;
	AddTickPrerequisiteActor(Manager.Get());
	if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
		Controller->SetViewTarget(GetOwner());
}

void URouteCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Manager.IsValid())
		return;
	GetOwner()->SetActorLocationAndRotation(InitialLocation + FVector(Manager->Progress - InitialProgress, 0.0, 0.0), InitialRotation);
}
