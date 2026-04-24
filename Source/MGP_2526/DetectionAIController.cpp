#include "DetectionAIController.h"
#include "EnemyAnimInstance.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

ADetectionAIController::ADetectionAIController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SetPerceptionComponent(*PerceptionComp);

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1000.f;
    HearingConfig->SetMaxAge(10.f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComp->ConfigureSense(*HearingConfig);
    PerceptionComp->SetDominantSense(HearingConfig->GetSenseImplementation());
}

void ADetectionAIController::BeginPlay()
{
    Super::BeginPlay();
  

    // Initialize blackboard
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (BB)
    {
        BB->SetValueAsBool(FName("IsSuspicious"), false);
        BB->SetValueAsBool(FName("IsSearching"), false);
        BB->SetValueAsBool(FName("IsAlerted"), false);
    }
    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
        UE_LOG(LogTemp, Warning, TEXT("Behavior Tree started!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Behavior Tree assigned!"));
    }


    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &ADetectionAIController::OnPerceptionUpdated
    );
}
void ADetectionAIController::SetDetectionState(EDetectionState NewState)
{
    CurrentState = NewState;

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Blackboard!"));
        return;
    }
    

    BB->SetValueAsBool(FName("IsSuspicious"), NewState == EDetectionState::Suspicious);
    BB->SetValueAsBool(FName("IsSearching"), NewState == EDetectionState::Searching);
    BB->SetValueAsBool(FName("IsAlerted"), NewState == EDetectionState::Alert);

    UE_LOG(LogTemp, Warning, TEXT("State set via Blackboard!"));
}
void ADetectionAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UE_LOG(LogTemp, Warning, TEXT("Perception updated!"));

    if (Stimulus.WasSuccessfullySensed())
    {
        LastHeardLocation = Stimulus.StimulusLocation;
        bIsInvestigating = true;
        MoveToLocation(LastHeardLocation, 50.f);

        if (CurrentState == EDetectionState::Idle)
            SetDetectionState(EDetectionState::Suspicious);
        else if (CurrentState == EDetectionState::Suspicious)
            SetDetectionState(EDetectionState::Searching);
        else if (CurrentState == EDetectionState::Searching)
            SetDetectionState(EDetectionState::Alert);

        GetWorldTimerManager().SetTimer(
            ResetTimerHandle,
            this,
            &ADetectionAIController::ResetToIdle,
            10.f,
            false
        );
    }
}

void ADetectionAIController::ResetToIdle()
{
    bIsInvestigating = false;
    SetDetectionState(EDetectionState::Idle);
    UE_LOG(LogTemp, Warning, TEXT("Resetting to Idle."));
}