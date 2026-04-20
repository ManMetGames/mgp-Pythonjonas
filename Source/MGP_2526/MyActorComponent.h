#pragma once

[#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
//#include "DetectionComponent.generated.h"

//UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
//class YOURGAME_API UDetectionComponent : public UActorComponent
//{
  //  GENERATED_BODY()

//public:
    // Sets default values for this component's properties
    //UDetectionComponent();

//protected:
    // Called when the game starts
  //virtual void BeginPlay() override;
  
//public:
    // Called every frame
  //  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Current detection level
  //  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
   // float DetectionLevel;

    // Maximum detection
   // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    //float MaxDetection;

    // How fast detection increases
  //  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
   // float DetectionRate;

    // How fast detection decreases
   // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
  //  float DetectionDecayRate;

    // Increase detection
    //void AddDetection(float Amount);

    // Reduce detection over time
    //void ReduceDetection(float DeltaTime);

    // Returns a 0-1 value for UI
    //UFUNCTION(BlueprintCallable, Category = "Detection")
    //float GetDetectionPercent() const;
//};