#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageProp.generated.h"
class UStaticMeshComponent;
class UArmorComponent;
/** Placed target; additional behaviors can be composed as Blueprint components. */
UCLASS()
class VYXXEN_API AStageProp : public AActor
{
	GENERATED_BODY()
public:
	AStageProp();
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Prop")
	TObjectPtr<UStaticMeshComponent> Mesh;
								UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Prop")
	TObjectPtr<UArmorComponent> Armor;
};
