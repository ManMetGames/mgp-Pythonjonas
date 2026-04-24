#include "EnemyCharacter.h"
#include "DetectionAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
    // Tell this character to use our custom AI controller (yay)
    AIControllerClass = ADetectionAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	PrimaryActorTick.bCanEverTick = true; //for speeds
}

void AEnemyCharacter::BeginPlay()
{
	CurrentState = EEnemyState::Idle;
	Super::BeginPlay();
	bIsAlert = false;
	bIsSearching = false;
	bIsSuspicious = false;
}
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Speed = GetVelocity().Size();
}
	
void AEnemyCharacter::SetAlert(bool bAlert)
{
    bIsAlert = bAlert;

    if (bAlert)
    {
        bIsSuspicious = false; // Alert overrides
    }
}

void AEnemyCharacter::SetSuspicious(bool bSuspicious)
{
    if (!bIsAlert) // don't override alert
    {
        bIsSuspicious = bSuspicious;
    }
}
