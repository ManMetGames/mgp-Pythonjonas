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

    // store the noise (please)
    LastHeardLocation = Stimulus.StimulusLocation;

    // distance to noise
    const float Distance = FVector::Dist(
        ControlledPawn->GetActorLocation(),
        LastHeardLocation
    );

    // Update Blackboards
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (BB)
    {
        BB->SetValueAsVector(FName("LastHeardLocation"), LastHeardLocation);
        BB->SetValueAsFloat(FName("DetectionDistance"), Distance);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BLACKBOARD IS NULL in OnPerceptionUpdated"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Noise heard at: %s"), *LastHeardLocation.ToString());
    UE_LOG(LogTemp, Warning, TEXT("Distance to noise: %f"), Distance);

    // distance based again
    if (Distance < 150.f)
    {
        SetDetectionState(EDetectionState::Alert);
    }
    else if (Distance < 300.f)
    {
        SetDetectionState(EDetectionState::Searching);
    }
    else
    {
        SetDetectionState(EDetectionState::Suspicious);
    }

   GetWorldTimerManager().ClearTimer(ResetTimerHandle);
    GetWorldTimerManager().SetTimer(
        ResetTimerHandle,
        this,
        &ADetectionAIController::ResetToIdle,
        10.f,
        false
    );
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

    BB->SetValueAsBool(FName("IsSuspicious"), NewState == EDetectionState::Suspicious);
    BB->SetValueAsBool(FName("IsSearching"), NewState == EDetectionState::Searching);
    BB->SetValueAsBool(FName("IsAlert"), NewState == EDetectionState::Alert);

    UE_LOG(LogTemp, Warning, TEXT("Blackboard Updated"));
}

void ADetectionAIController::ResetToIdle()
{
    SetDetectionState(EDetectionState::Idle);
    StopMovement();

    UE_LOG(LogTemp, Warning, TEXT("AI gave up and is now Idle."));
}