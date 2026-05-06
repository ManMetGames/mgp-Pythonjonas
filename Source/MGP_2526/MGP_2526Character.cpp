// Copyright Epic Games, Inc. All Rights Reserved.
#include "MGP_2526Character.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MGP_2526.h"


AMGP_2526Character::AMGP_2526Character()
{
    PrimaryActorTick.bCanEverTick = true; //  stamina

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 500.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

// Tick: stamina drain/regen

void AMGP_2526Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSprinting)
    {
        Stamina -= StaminaDrain * DeltaTime;
        if (Stamina <= 0.0f)
        {
            Stamina = 0.0f;
            SprintEnd(); // out of stamina, stop sprinting
        }
    }
    else
    {
        Stamina = FMath::Min(Stamina + StaminaRegen * DeltaTime, MaxStamina);
    }


   // {
     //   Super::Tick(DeltaTime);
    //    FVector CameraLocation = FollowCamera->GetForwardVector();
    //    float NewZ = FMath::FInterpTo(CameraLocation.Z, TargetCameraZ, DeltaTime, 10.0f);
    //    FollowCamera->SetRelativeLocation(FVector(CameraLocation.X, CameraLocation.Y, NewZ));
   // };
    

}

// Input bindings

void AMGP_2526Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Move & Look
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Move);
        EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);
        EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);

        // Sprint (Shift)
        EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &AMGP_2526Character::SprintStart);
        EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMGP_2526Character::SprintEnd);

        //Crouch (LeftCTRL)
       // PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMGP_2526Character::CrouchStart);
       // PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMGP_2526Character::CrouchEnd);
       
        EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMGP_2526Character::CrouchStart);
        EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMGP_2526Character::CrouchEnd);

    }
    else
    {
        UE_LOG(LogMGP_2526, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
    }
}

// Sprint

void AMGP_2526Character::SprintStart()
{
    // Can't sprint with no stamina
    if (Stamina <= 0.0f) return;

    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMGP_2526Character::SprintEnd()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}


//crouching
void AMGP_2526Character::CrouchStart()
{
    bIsCrouching = true;
    Crouch();
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;
    //TargetCameraZ = 40.0f;
}

void AMGP_2526Character::CrouchEnd()
{
    bIsCrouching = false;
    UnCrouch();
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
   // TargetCameraZ = 40.0f;
}

void AMGP_2526Character::DoMove(float Right, float Forward)
{
    if (GetController() != nullptr)
    {
        const FRotator Rotation = GetController()->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector  ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector  RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(ForwardDir, Forward);
        AddMovementInput(RightDir, Right);
    }
}


void AMGP_2526Character::DoLook(float Yaw, float Pitch)
{
    if (GetController() != nullptr)
    {
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}

void AMGP_2526Character::DoJumpStart() { Jump(); }
void AMGP_2526Character::DoJumpEnd() { StopJumping(); }



void AMGP_2526Character::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    DoMove(MovementVector.X, MovementVector.Y);
    UE_LOG(LogTemp, Warning, TEXT("Move called!"));

    //I want to have varying loudness based on the players movement:

    float Loudness = 0.0f;
    
    if (bIsCrouching)
    {
        Loudness = 0.1f; //mouse
    }

    else if (bIsSprinting)
    {
        Loudness = 1.0f; //loud (stop)
    }

    else
    {
        Loudness = 0.5f; // default
    }
    //UE_LOG(LogTemp, Warning, TEXT("Reporting noise with loudness: %f"), Loudness);
    UAISense_Hearing::ReportNoiseEvent(
        GetWorld(),
        GetActorLocation(),
        Loudness,
        this,
        2000.0f,
        FName("Footstep")
    );
}

void AMGP_2526Character::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    DoLook(LookAxisVector.X, LookAxisVector.Y);
}