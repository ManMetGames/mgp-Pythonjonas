#include "DetectionAIController.h"
#include "EnemyCharacter.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.h"

ADetectionAIController::ADetectionAIController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SetPerceptionComponent(*PerceptionComp);

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1000.f;
    HearingConfig->SetMaxAge(5.f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComp->ConfigureSense(*HearingConfig);
    PerceptionComp->SetDominantSense(HearingConfig->GetSenseImplementation());
}

void ADetectionAIController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("DetectionAIController BeginPlay called!"));
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &ADetectionAIController::OnPerceptionUpdated
    );
}

void ADetectionAIController::SetDetectionState(EDetectionState NewState)
{
    CurrentState = NewState;

    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!EnemyChar) return;

    UEnemyAnimInstance* AnimInstance = Cast<UEnemyAnimInstance>(EnemyChar->GetMesh()->GetAnimInstance());
    if (!AnimInstance) return;

    AnimInstance->bIsSuspicious = (CurrentState == EDetectionState::Suspicious);
    AnimInstance->bIsSearching = (CurrentState == EDetectionState::Searching);
    AnimInstance->bIsAlerted = (CurrentState == EDetectionState::Alert);
}

void ADetectionAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UE_LOG(LogTemp, Warning, TEXT("Perception updated!"));
    if (Stimulus.WasSuccessfullySensed())
    {
        LastHeardLocation = Stimulus.StimulusLocation;
        bIsInvestigating = true;
        MoveToLocation(LastHeardLocation, 50.f);

        // Escalate state based on current state
        if (CurrentState == EDetectionState::Idle)
        {
            SetDetectionState(EDetectionState::Suspicious);
        }
        else if (CurrentState == EDetectionState::Suspicious)
        {
            SetDetectionState(EDetectionState::Searching);
        }
        else if (CurrentState == EDetectionState::Searching)
        {
            SetDetectionState(EDetectionState::Alert);
        }

        UE_LOG(LogTemp, Warning, TEXT("Sound heard! Investigating."));
    }
    else
    {
        // Sound lost, calm back down
        SetDetectionState(EDetectionState::Idle);
        bIsInvestigating = false;
        UE_LOG(LogTemp, Warning, TEXT("Sound lost."));
    }
}