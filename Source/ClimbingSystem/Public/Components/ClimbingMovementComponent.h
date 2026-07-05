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
	FVector ClimbableSurfaceLocation = FVector::ZeroVector;
	FVector ClimbableSurfaceNormal = FVector::ZeroVector;
	
#pragma region Climb Trace BP Variables 
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float CapsuleTraceRadius = 50.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float CapsuleTraceHalfHeight = 80.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float MaxBreakingDeceleration = 500.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float MaxClimbSpeed = 100.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float MaxClimbAcceleration = 400.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float ClimbSlopeMinAngle = 45.f;
#pragma endregion

#pragma region Climb Functions
	bool ClimbableSurfaceDetection();
	bool EyeLevelSurfaceDetection(float TraceDistance,float TraceOffset = 0.f);

	void ProcessingClimbableSurfaces();

	FQuat SetClimbRotation(float deltaTime) const;

	// this is a function that needs tick
	void SnapToSurfaces(float DeltaTime);
#pragma endregion
protected:
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const;
	bool ShouldIStopClimbing();
public:
	bool AmIClimbing() const;
	bool IsClimbingPossible();

	void ToggleClimbingState(bool bCanClimb);

	void PhysicsClimb(float deltaTime, int32 Iterations);

	FORCEINLINE FVector GetClimbableSurfaceNormal() const { return ClimbableSurfaceNormal;};
};
