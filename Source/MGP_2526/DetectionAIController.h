#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "DetectionAIController.generated.h"

UENUM(BlueprintType)
enum class EDetectionState : uint8
{
    Idle,
    Suspicious,
    Searching,
    Alert
};

UCLASS()
class MGP_2526_API ADetectionAIController : public AAIController
{
    GENERATED_BODY()

public:
    ADetectionAIController();

    UPROPERTY(BlueprintReadOnly, Category = "Detection")
    EDetectionState CurrentState = EDetectionState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Detection")
    FVector LastHeardLocation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    UBehaviorTree* BehaviorTree;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    UFUNCTION(BlueprintCallable, Category = "Detection")
    void SetDetectionState(EDetectionState NewState);

    void ResetToIdle();

private:
    UPROPERTY(VisibleAnywhere)
    UAIPerceptionComponent* PerceptionComp;

    UPROPERTY(VisibleAnywhere)
    UAISenseConfig_Hearing* HearingConfig;

    FTimerHandle ResetTimerHandle;
};