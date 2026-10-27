#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "FlightMovementComponent.generated.h"
class AGameManager;
class UInputComponent;
class UInputAction;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;

UCLASS(ClassGroup=(Flight), meta=(BlueprintSpawnableComponent))
class VYXXEN_API UFlightMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	UFlightMovementComponent();
	void BindInput(UInputComponent* InputComponent);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Movement", meta=(ClampMin="0", Units="CentimetersPerSecond"))
	float LateralSpeed = 200.f;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Movement", meta=(ClampMin="0", Units="CentimetersPerSecond"))
	float ClimbSpeed = 200.f;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Pose", meta=(ClampMin="0"))
	float MaxPitchDegrees = 15.f;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Pose", meta=(ClampMin="0"))
	float MaxBankDegrees = 20.f;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Pose", meta=(ClampMin="0"))
	float PoseEaseSeconds = .2f;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Bounds")
	float MinFlightHeight = 100.f;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Bounds")
	float MaxFlightHeight = 400.f;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Input")
	TObjectPtr<UInputMappingContext> FlightMappingContext;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Input")
	TObjectPtr<UInputAction> SteerAction;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Input")
	TObjectPtr<UInputAction> ClimbAction;
								UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flight|Input")
	TObjectPtr<UInputAction> SlowAction;
								UFUNCTION(BlueprintCallable, Category="Flight|Input")
	void SetSteerInput(float NewInput);
								UFUNCTION(BlueprintCallable, Category="Flight|Input")
	void SetClimbInput(float NewInput);
								UFUNCTION(BlueprintCallable, Category="Flight|Input")
	void ResetControlRequests();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	void HandleSteer(const FInputActionValue& Value);
	void ClearSteer(const FInputActionValue& Value);
	void HandleClimb(const FInputActionValue& Value);
	void ClearClimb(const FInputActionValue& Value);
	void HandleSlow(const FInputActionValue& Value);
	void ClearSlow(const FInputActionValue& Value);
	TWeakObjectPtr<AGameManager> Manager;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;
	double InitialProgress = 0.0;
	double InitialX = 0.0;
	float SteerInput = 0.f;
	float ClimbInput = 0.f;
};
