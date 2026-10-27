#include "FlightMovementComponent.h"
#include "GameManager.h"
#include "PlayerShip.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
UFlightMovementComponent::UFlightMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UFlightMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	Manager = GetWorld()->GetAuthGameMode<AGameManager>();
	if (!Manager.IsValid() || !Cast<APlayerShip>(GetOwner()))
	{
		UE_LOG(LogTemp, Error, TEXT("Flight needs a PlayerShip Pawn and GameManager GameMode."));
		SetComponentTickEnabled(false);
		return;
	}
	SetUpdatedComponent(GetOwner()->GetRootComponent());
	InitialX = GetOwner()->GetActorLocation().X;
	InitialProgress = Manager->Progress;
	AddTickPrerequisiteActor(Manager.Get());
}
void UFlightMovementComponent::BindInput(UInputComponent* InputComponent)
{
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput)
		return;
	APlayerController* Controller = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
	if (Controller && Controller->GetLocalPlayer() && FlightMappingContext)
	{
		InputSubsystem = Controller->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem.IsValid())
			InputSubsystem->AddMappingContext(FlightMappingContext, 0);
	}
	if (SteerAction)
	{
		EnhancedInput->BindAction(SteerAction, ETriggerEvent::Triggered, this, &UFlightMovementComponent::HandleSteer);
		EnhancedInput->BindAction(SteerAction, ETriggerEvent::Completed, this, &UFlightMovementComponent::ClearSteer);
		EnhancedInput->BindAction(SteerAction, ETriggerEvent::Canceled, this, &UFlightMovementComponent::ClearSteer);
	}
	if (ClimbAction)
	{
		EnhancedInput->BindAction(ClimbAction, ETriggerEvent::Triggered, this, &UFlightMovementComponent::HandleClimb);
		EnhancedInput->BindAction(ClimbAction, ETriggerEvent::Completed, this, &UFlightMovementComponent::ClearClimb);
		EnhancedInput->BindAction(ClimbAction, ETriggerEvent::Canceled, this, &UFlightMovementComponent::ClearClimb);
	}
	if (SlowAction)
	{
		EnhancedInput->BindAction(SlowAction, ETriggerEvent::Triggered, this, &UFlightMovementComponent::HandleSlow);
		EnhancedInput->BindAction(SlowAction, ETriggerEvent::Completed, this, &UFlightMovementComponent::ClearSlow);
		EnhancedInput->BindAction(SlowAction, ETriggerEvent::Canceled, this, &UFlightMovementComponent::ClearSlow);
	}
}
void UFlightMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetControlRequests();
	if (InputSubsystem.IsValid() && FlightMappingContext)
		InputSubsystem->RemoveMappingContext(FlightMappingContext);
	Super::EndPlay(EndPlayReason);
}
void UFlightMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	APlayerShip* Ship = Cast<APlayerShip>(GetOwner());
	if (!Ship || !Manager.IsValid() || !Manager->bRouteActive || DeltaTime <= 0.f || !FMath::IsFinite(DeltaTime))
		return;
	const FVector Previous = Ship->GetActorLocation();
	FVector Desired = Previous;
	Desired.X = InitialX + Manager->Progress - InitialProgress;
	Desired.Y += SteerInput * FMath::Max(LateralSpeed, 0.f) * Manager->GetSpeedScale() * DeltaTime;
	Desired.Z = FMath::Clamp(Previous.Z + ClimbInput * FMath::Max(ClimbSpeed, 0.f) * DeltaTime, MinFlightHeight, MaxFlightHeight);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ShipFlight), false, Ship);
	FHitResult Hit;
	const FVector HullStart = Ship->Hull->GetComponentLocation();
	if (GetWorld()->SweepSingleByProfile(Hit, HullStart, HullStart + Desired - Previous,
		Ship->Hull->GetComponentQuat(), Ship->Hull->GetCollisionProfileName(), Ship->Hull->GetCollisionShape(), Params))
	{
		Ship->Crash();
		return;
	}
	Ship->SetActorLocation(Desired);
	const float ClimbFraction = ClimbSpeed > 0.f ? (Desired.Z - Previous.Z) / (ClimbSpeed * DeltaTime) : 0.f;
	const FRotator DesiredPose(FMath::Clamp(ClimbFraction, -1.f, 1.f) * MaxPitchDegrees, 0.f, -SteerInput * MaxBankDegrees);
	Ship->SetActorRotation(PoseEaseSeconds > 0.f ? FMath::RInterpTo(Ship->GetActorRotation(), DesiredPose, DeltaTime, 1.f / PoseEaseSeconds) : DesiredPose);
	// Pose can push a wing or nose into geometry after a clear translation sweep.
	if (GetWorld()->OverlapBlockingTestByProfile(Ship->Hull->GetComponentLocation(), Ship->Hull->GetComponentQuat(),
		Ship->Hull->GetCollisionProfileName(), Ship->Hull->GetCollisionShape(), Params))
		Ship->Crash();
	Velocity = (Desired - Previous) / DeltaTime;
}
void UFlightMovementComponent::SetSteerInput(float NewInput)
{
	SteerInput = FMath::IsFinite(NewInput) ? FMath::Clamp(NewInput, -1.f, 1.f) : 0.f;
}
void UFlightMovementComponent::SetClimbInput(float NewInput)
{
	ClimbInput = FMath::IsFinite(NewInput) ? FMath::Clamp(NewInput, -1.f, 1.f) : 0.f;
}
void UFlightMovementComponent::ResetControlRequests()
{
	SetSteerInput(0.f);
	SetClimbInput(0.f);
	if (Manager.IsValid())
		Manager->SetSlowHeld(false);
}
void UFlightMovementComponent::HandleSteer(const FInputActionValue& Value)
{
	SetSteerInput(Value.Get<float>());
}
void UFlightMovementComponent::ClearSteer(const FInputActionValue&)
{
	SetSteerInput(0.f);
}
void UFlightMovementComponent::HandleClimb(const FInputActionValue& Value)
{
	SetClimbInput(Value.Get<float>());
}
void UFlightMovementComponent::ClearClimb(const FInputActionValue&)
{
	SetClimbInput(0.f);
}
void UFlightMovementComponent::HandleSlow(const FInputActionValue& Value)
{
	if (Manager.IsValid())
		Manager->SetSlowHeld(Value.Get<bool>());
}
void UFlightMovementComponent::ClearSlow(const FInputActionValue&)
{
	if (Manager.IsValid())
		Manager->SetSlowHeld(false);
}
