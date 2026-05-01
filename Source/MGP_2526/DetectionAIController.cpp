#pragma once
#include "DetectionAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

ADetectionAIController::ADetectionAIController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SetPerceptionComponent(*PerceptionComp);

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 3000.f;
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

    UE_LOG(LogTemp, Warning, TEXT("AI CONTROLLER BEGIN PLAY RUNNING"));

    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
        UE_LOG(LogTemp, Warning, TEXT("Behavior Tree started successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No Behavior Tree assigned!"));
    }

    SetDetectionState(EDetectionState::Idle);

    if (PerceptionComp)
    {
        PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
            this,
            &ADetectionAIController::OnPerceptionUpdated
        );
    }
}

void ADetectionAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Stimulus.WasSuccessfullySensed())
    {
        return;
    }

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("No controlled pawn!"));
        return;
    }

    LastHeardLocation = Stimulus.StimulusLocation;

    const float Distance = FVector::Dist(
        ControlledPawn->GetActorLocation(),
        LastHeardLocation

    );

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (BB)
    {
        BB->SetValueAsVector(FName("LastHeardLocation"), LastHeardLocation);
        BB->SetValueAsFloat(FName("DetectionDistance"), Distance);
        BB->SetValueAsObject(FName("PlayerActor"), Actor);
    }

    UE_LOG(LogTemp, Warning, TEXT("Distance to noise: %f"), Distance);

    // Detection distances (update if i need to)
    const float AlertDistance = 600.f;
    const float SearchingDistance = 1400.f;
    const float SuspiciousDistance = 2500.f;

    if (Distance <= AlertDistance)
    {
        SetDetectionState(EDetectionState::Alert);
    }
    else if (Distance <= SearchingDistance)
    {
        SetDetectionState(EDetectionState::Searching);
    }
    else if (Distance <= SuspiciousDistance)
    {
        SetDetectionState(EDetectionState::Suspicious);
    }

    GetWorldTimerManager().ClearTimer(ResetTimerHandle);
    GetWorldTimerManager().SetTimer(
        ResetTimerHandle,
        this,
        &ADetectionAIController::ResetToIdle,
        8.f,
        false
    );
}

void ADetectionAIController::SetDetectionState(EDetectionState NewState)
{
    CurrentState = NewState;

    if (NewState == EDetectionState::Suspicious)
    {
        StopMovement();
    }

    UE_LOG(LogTemp, Warning, TEXT("SetDetectionState called: %d"), (int32)NewState);

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Error, TEXT("BLACKBOARD IS NULL"));
        return;
    }

    BB->SetValueAsBool(FName("IsSuspicious"), NewState == EDetectionState::Suspicious);
    BB->SetValueAsBool(FName("IsSearching"), NewState == EDetectionState::Searching);
    BB->SetValueAsBool(FName("IsAlert"), NewState == EDetectionState::Alert);

    UE_LOG(LogTemp, Warning, TEXT("Blackboard Updated"));
}

void ADetectionAIController::ResetToIdle()
{
	SetDetectionState(EDetectionState::Idle);
    StopMovement();

    UE_LOG(LogTemp, Warning, TEXT("AI gave up and is now on Patrol."));
}