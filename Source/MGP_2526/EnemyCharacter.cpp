#include "EnemyCharacter.h"
#include "DetectionAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
    // Tell this character to use our custom AI controller (yay)
    AIControllerClass = ADetectionAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
}