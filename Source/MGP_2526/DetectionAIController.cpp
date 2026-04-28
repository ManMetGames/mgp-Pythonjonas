#include "DetectionAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"

ADetectionAIController::ADetectionAIController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1000.f;
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComp->ConfigureSense(*HearingConfig);
    PerceptionComp->SetDominantSense(HearingConfig->GetSenseImplementation());
}

void ADetectionAIController::BeginPlay()

{
    UE_LOG(LogTemp, Warning, TEXT("AI CONTROLLER BEGIN PLAY RUNNING"))
    Super::BeginPlay();

    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
		UE_LOG(LogTemp, Warning, TEXT("Behavior Tree started successfully."));
        SetDetectionState(EDetectionState::Suspicious);
    }

    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ADetectionAIController::OnPerceptionUpdated);
}

void ADetectionAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        LastHeardLocation = Stimulus.StimulusLocation;
        float Distance = FVector::Dist(GetPawn()->GetActorLocation(), LastHeardLocation);

        UBlackboardComponent* BB = GetBlackboardComponent();
        if (BB)
        {
            BB->SetValueAsVector(FName("LastHeardLocation"), LastHeardLocation);
            BB->SetValueAsFloat(FName("DetectionDistance"), Distance);
        }

   
        if (Distance < 300.f)
        {
   
            SetDetectionState(EDetectionState::Searching);
        }
        else
        {
            if (CurrentState == EDetectionState::Idle)
            {
                SetDetectionState(EDetectionState::Suspicious);
            }
            else if (CurrentState == EDetectionState::Suspicious)
            {
                SetDetectionState(EDetectionState::Searching);
            }
        }

        
        GetWorldTimerManager().ClearTimer(ResetTimerHandle);
        GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &ADetectionAIController::ResetToIdle, 10.f, false);

        // Move to the noise
        MoveToLocation(LastHeardLocation, 50.f);
    }
}

void ADetectionAIController::SetDetectionState(EDetectionState NewState)
{
    CurrentState = NewState;

    UE_LOG(LogTemp, Warning, TEXT("SetDetectionState called: %d"), (int32)NewState);

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Error, TEXT("BLACKBOARD IS NULL"));
        return;
    }

    BB->SetValueAsBool(FName("IsSuspicious"), CurrentState == EDetectionState::Suspicious);
    BB->SetValueAsBool(FName("IsSearching"), CurrentState == EDetectionState::Searching);
    BB->SetValueAsBool(FName("IsAlert"), CurrentState == EDetectionState::Alert);

    UE_LOG(LogTemp, Warning, TEXT("Blackboard Updated"));
}

void ADetectionAIController::ResetToIdle()
{
    SetDetectionState(EDetectionState::Idle);

   
    StopMovement();
    UE_LOG(LogTemp, Warning, TEXT("AI gave up and is now Idle."));
}