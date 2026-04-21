#include "DetectionAIController.h"

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

    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &ADetectionAIController::OnPerceptionUpdated
    );
}

void ADetectionAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        LastHeardLocation = Stimulus.StimulusLocation;
        bIsInvestigating = true;
        MoveToLocation(LastHeardLocation, 50.f);

        UE_LOG(LogTemp, Warning, TEXT("Sound heard?! Im investigating."));
    }
    else
    {
        bIsInvestigating = false;
        UE_LOG(LogTemp, Warning, TEXT("Sound lost... "));
    }
}