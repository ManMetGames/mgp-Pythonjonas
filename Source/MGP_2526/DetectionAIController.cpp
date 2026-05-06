#include "DetectionAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Pawn.h"

ADetectionAIController::ADetectionAIController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SetPerceptionComponent(*PerceptionComp);

    // i am hearing
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 3000.f;
    HearingConfig->SetMaxAge(5.f);

    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComp->ConfigureSense(*HearingConfig);

    // i am seeing
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 5000.f;
    SightConfig->LoseSightRadius = 5500.f;
    SightConfig->PeripheralVisionAngleDegrees = 180.f;
    SightConfig->SetMaxAge(1.f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComp->ConfigureSense(*SightConfig);

    
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

    
    PerceptionComp->RequestStimuliListenerUpdate();

    UE_LOG(LogTemp, Warning, TEXT("Sight and Hearing senses configured."));
}

void ADetectionAIController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AI CONTROLLER BEGIN PLAY RUNNING"));
}

void ADetectionAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    UE_LOG(LogTemp, Warning, TEXT("AI CONTROLLER POSSESSED PAWN"));

    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
        UE_LOG(LogTemp, Warning, TEXT("Behavior Tree started successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No Behavior Tree assigned!"));
    }

    if (PerceptionComp)
    {
        PerceptionComp->OnTargetPerceptionUpdated.RemoveDynamic(
            this,
            &ADetectionAIController::OnPerceptionUpdated
        );

        PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
            this,
            &ADetectionAIController::OnPerceptionUpdated
        );

        PerceptionComp->RequestStimuliListenerUpdate();

        UE_LOG(LogTemp, Warning, TEXT("Perception delegate bound."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PerceptionComp is null."));
    }

    SetDetectionState(EDetectionState::Idle);
}

void ADetectionAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor)
    {
        return;
    }

    if (!Stimulus.WasSuccessfullySensed())
    {
        return;
    }

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Error, TEXT("BLACKBOARD IS NULL in OnPerceptionUpdated"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Perception fired. Type: %d"), Stimulus.Type.Index);
    UE_LOG(LogTemp, Warning, TEXT("Sight ID: %d"), UAISense::GetSenseID<UAISense_Sight>().Index);
    UE_LOG(LogTemp, Warning, TEXT("Hearing ID: %d"), UAISense::GetSenseID<UAISense_Hearing>().Index);

 
    // I SEE

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        UE_LOG(LogTemp, Warning, TEXT("SIGHT fired"));

        BB->SetValueAsObject(FName("PlayerActor"), Actor);

        SetDetectionState(EDetectionState::Alert);

        GetWorldTimerManager().ClearTimer(ResetTimerHandle);
        GetWorldTimerManager().SetTimer(
            ResetTimerHandle,
            this,
            &ADetectionAIController::ResetToIdle,
            5.f,
            false
        );

        UE_LOG(LogTemp, Warning, TEXT("Player seen! Alert."));
        return;
    }

    // I HEAR

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        UE_LOG(LogTemp, Warning, TEXT("HEARING fired"));

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

        const float NoiseStrength = Stimulus.Strength;

        BB->SetValueAsVector(FName("LastHeardLocation"), LastHeardLocation);
        BB->SetValueAsFloat(FName("DetectionDistance"), Distance);
        BB->SetValueAsObject(FName("PlayerActor"), Actor);

        UE_LOG(LogTemp, Warning, TEXT("Noise heard at: %s"), *LastHeardLocation.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Distance to noise: %f"), Distance);
        UE_LOG(LogTemp, Warning, TEXT("Noise Strength: %f"), NoiseStrength);

        const float AlertDistance = 700.f;
        const float SearchingDistance = 1600.f;
        const float SuspiciousDistance = 3000.f;

        const float AlertNoiseRequired = 0.8f;
        const float SearchingNoiseRequired = 0.4f;
        const float SuspiciousNoiseRequired = 0.1f;

        if (Distance <= AlertDistance && NoiseStrength >= AlertNoiseRequired)
        {
            SetDetectionState(EDetectionState::Alert);
        }
        else if (Distance <= SearchingDistance && NoiseStrength >= SearchingNoiseRequired)
        {
            SetDetectionState(EDetectionState::Searching);
        }
        else if (Distance <= SuspiciousDistance && NoiseStrength >= SuspiciousNoiseRequired)
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

        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Unknown perception sense fired."));
}

void ADetectionAIController::SetDetectionState(EDetectionState NewState)
{
    const bool bSameState = CurrentState == NewState;

    CurrentState = NewState;

    if (!bSameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetDetectionState called: %d"), (int32)NewState);
    }

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Error, TEXT("BLACKBOARD IS NULL"));
        return;
    }

    BB->SetValueAsBool(FName("IsSuspicious"), NewState == EDetectionState::Suspicious);
    BB->SetValueAsBool(FName("IsSearching"), NewState == EDetectionState::Searching);
    BB->SetValueAsBool(FName("IsAlert"), NewState == EDetectionState::Alert);

    if (NewState == EDetectionState::Suspicious)
    {
        StopMovement();
    }

    if (!bSameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("Blackboard Updated"));
    }
}

void ADetectionAIController::ResetToIdle()
{
    SetDetectionState(EDetectionState::Idle);
    StopMovement();

    UE_LOG(LogTemp, Warning, TEXT("AI gave up and is now on Patrol."));
}