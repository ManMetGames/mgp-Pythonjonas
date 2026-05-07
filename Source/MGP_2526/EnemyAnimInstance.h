#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class MGP_2526_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    bool bIsSuspicious = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    bool bIsSearching = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    bool bIsAlerted = false;
};