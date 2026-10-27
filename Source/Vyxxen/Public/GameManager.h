#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameManager.generated.h"

/** Shared route clock: ship and camera consume Progress instead of integrating speed. */
UCLASS()
class VYXXEN_API AGameManager : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGameManager();
	virtual void Tick(float DeltaSeconds) override;
	virtual void StartPlay() override;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Route", meta=(ClampMin="0", Units="CentimetersPerSecond"))
	float ForwardSpeed = 150.f;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Route", meta=(ClampMin="0", Units="Seconds"))
	float SlowRampSeconds = 1.f;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Route")
	float CurrentForwardSpeed = 150.f;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Route")
	double Progress = 0.0;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Route")
	float SlowAlpha = 0.f;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Route")
	bool bRouteActive = true;
	UFUNCTION(BlueprintCallable, Category="Route")
	void SetSlowHeld(bool bHeld);
	UFUNCTION(BlueprintCallable, Category="Route")
	void StopRoute();
	UFUNCTION(BlueprintPure, Category="Route")
	float GetSpeedScale() const
	{
		return 1.f - SlowAlpha * 0.4f;
	}
private:
	bool bSlowHeld = false;
	float SlowStart = 0.f;
	float SlowElapsed = 0.f;
};
