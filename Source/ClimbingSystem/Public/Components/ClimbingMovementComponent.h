// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimbingMovementComponent.generated.h"

/**
 * 
 */
// old definition of enum

class UAnimInstance;
class UAnimMontage;
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
	UPROPERTY()
	TObjectPtr<UAnimInstance> CharacterAnimInstance;
	
#pragma region Climb Core BP Variables 
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

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
    TObjectPtr<UAnimMontage> IdleToClimbMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ClimbToTopMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	float ClimbDownMinHeight = 150.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	float ClimbDownTraceOffset = 50.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	float WalkStartTraceOffset = 30.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ClimbDownMontage;
#pragma endregion

#pragma region Climb Functions
	bool ClimbableSurfaceDetection();
	bool EyeLevelSurfaceDetection(float TraceDistance,float TraceOffset = 0.f,bool DebugLines = false);

	void ProcessingClimbableSurfaces();

	FQuat SetClimbRotation(float deltaTime) const;

	// this is a function that needs tick
	void SnapToSurfaces(float DeltaTime);
#pragma endregion
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual float GetMaxSpeed() const override;
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;
	virtual float GetMaxAcceleration() const;
	bool ShouldIStopClimbing();

	void PlayClimbMontage( UAnimMontage* Montage) const;
	bool DetectFloorReached();
  
	bool DetectLedgeReached();

	UFUNCTION()
	virtual void OnClimbMontageEnded( UAnimMontage* Montage, bool bInterrupted);
public:
	bool AmIClimbing() const;
	bool IsClimbingPossible();
	bool CanIClimbDown();
	void ToggleClimbingState(bool bCanClimb);

	void PhysicsClimb(float deltaTime, int32 Iterations);

	FVector GetUnRotatedClimbVelocity() const;

	FORCEINLINE FVector GetClimbableSurfaceNormal() const { return ClimbableSurfaceNormal;};
};
