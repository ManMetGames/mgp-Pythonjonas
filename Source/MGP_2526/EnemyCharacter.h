#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class MGP_2526_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;
};