// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MGP_2526Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMGP_2526Character : public ACharacter
{
    GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

protected:
    /** Jump Input Action */
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* JumpAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookAction;

    /** Mouse Look Input Action */
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MouseLookAction;

public:
    /** Constructor */
    AMGP_2526Character();

protected:
    /** Initialize input action bindings */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void Tick(float DeltaTime) override;

protected:
    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    /** Sprint input handlers */
    void SprintStart();
    void SprintEnd();


    UFUNCTION(BlueprintCallable, Category = "Movement")
    void DoMove(float Right, float Forward);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void DoLook(float Yaw, float Pitch);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void DoJumpStart();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void DoJumpEnd();

private:
    bool bIsSprinting = false;

public:
    /** Sprint Input Action — assign in your Input Mapping Context */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* SprintAction;

    // Movement speeds
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float WalkSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float SprintSpeed = 600.0f;

    // Stamina
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float MaxStamina = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float StaminaDrain = 20.0f;   // per second while sprinting

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float StaminaRegen = 10.0f;   // per second while not sprinting

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};