// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimbingMovementComponent.generated.h"

/**
 * 
 */
// old definition of enum
UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName = "Climb Mode")
	};
}


UCLASS()
class CLIMBINGSYSTEM_API UClimbingMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

#pragma region Climb Traces
   TArray<FHitResult> CapsuleTraceMultiForObjects(const FVector& Start , const FVector& End , bool bDrawDebugTypes);
	FHitResult EyeLengthLineTraceSingle(const FVector& Start,const FVector& End , bool bDrawDebugTypes );
#pragma endregion

#pragma region Climb Core

#pragma endregion
  FHitResult LineTraceHitResult;
  TArray<FHitResult> CapsuleTraceHitResult;
	
#pragma region Climb Trace BP Variables 
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float CapsuleTraceRadius = 50.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float CapsuleTraceHalfHeight = 80.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
#pragma endregion

#pragma region Climb Trace Detection Logic
	bool ClimbableSurfaceDetection();
	bool EyeLevelSurfaceDetection(float TraceDistance,float TraceOffset = 0.f);
#pragma endregion

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	bool AmIClimbing() const;
	bool IsClimbingPossible();

	void ToggleClimbingState(bool bCanClimb);
};
