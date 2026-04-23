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

    void Tick(float DeltaTime);

public:

    // Movement
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Speed;

    // AI States
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool bIsAlert;

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool bIsSearching;

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool bIsSuspicious;
    UFUNCTION(BlueprintCallable)
    void SetAlert(bool bAlert);

    UFUNCTION(BlueprintCallable)
    void SetSuspicious(bool bSuspicious);
};