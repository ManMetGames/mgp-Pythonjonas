#include "DetectionComponent.h"

// Sets default values
UDetectionComponent::UDetectionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    DetectionLevel = 0.f;
    MaxDetection = 100.f;
    DetectionRate = 20.f;
    DetectionDecayRate = 5.f;
}

void UDetectionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Automatically decay detection over time
    ReduceDetection(DeltaTime);
}

void UDetectionComponent::AddDetection(float Amount)
{
    DetectionLevel = FMath::Clamp(DetectionLevel + Amount, 0.f, MaxDetection);
}

void UDetectionComponent::ReduceDetection(float DeltaTime)
{
    DetectionLevel = FMath::Clamp(DetectionLevel - DetectionDecayRate * DeltaTime, 0.f, MaxDetection);
}

float UDetectionComponent::GetDetectionPercent() const
{
    return DetectionLevel / MaxDetection;
}