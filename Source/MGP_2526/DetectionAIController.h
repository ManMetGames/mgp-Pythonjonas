#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "DetectionAIController.generated.h"

UCLASS()
class MGP_2526_API ADetectionAIController : public AAIController
{
    GENERATED_BODY()

public:
    ADetectionAIController();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
    UPROPERTY(VisibleAnywhere)
    UAIPerceptionComponent* PerceptionComp;

    UPROPERTY(VisibleAnywhere)
    UAISenseConfig_Hearing* HearingConfig;

    FVector LastHeardLocation;
    bool bIsInvestigating = false;
};