#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RouteCameraComponent.generated.h"
class AGameManager;

/** On the placed CameraActor; retains its authored Y, Z, and rotation. */
UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class VYXXEN_API URouteCameraComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	URouteCameraComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
private:
	TWeakObjectPtr<AGameManager> Manager;
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;
	double InitialProgress = 0.0;
};
