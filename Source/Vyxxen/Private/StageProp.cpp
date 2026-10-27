#include "StageProp.h"
#include "ArmorComponent.h"
#include "Components/StaticMeshComponent.h"
AStageProp::AStageProp()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	Armor = CreateDefaultSubobject<UArmorComponent>(TEXT("Armor"));
}
