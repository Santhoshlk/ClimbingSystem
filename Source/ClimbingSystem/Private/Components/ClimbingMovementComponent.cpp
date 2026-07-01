// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ClimbingMovementComponent.h"

#include "ClimbingSystemDebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FHitResult> UClimbingMovementComponent::CapsuleTraceMultiForObjects(const FVector& Start, const FVector& End,bool bDrawDebugTypes)
{
   TArray<FHitResult> OutCapsuleTraceHitResult;
	
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		CapsuleTraceRadius,
		CapsuleTraceHalfHeight,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		bDrawDebugTypes ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		OutCapsuleTraceHitResult,
		true
		);
	return OutCapsuleTraceHitResult;
}

FHitResult UClimbingMovementComponent::EyeLengthLineTraceSingle(const FVector& Start, const FVector& End,
	bool bDrawDebugTypes)
{
     FHitResult OutHitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		bDrawDebugTypes ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		OutHitResult,
		true
		);
	return OutHitResult;
}



bool UClimbingMovementComponent::AmIClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
}

bool UClimbingMovementComponent::IsClimbingPossible() 
{
	if (IsFalling()) return false;
	
	if (!ClimbableSurfaceDetection()) return false;
	if (!EyeLevelSurfaceDetection(150.f,20.f)) return false;
	

	return true;
}

void UClimbingMovementComponent::ToggleClimbingState(bool bCanClimb)
{
	if (bCanClimb)
	{
		if (IsClimbingPossible())
		{
			
		   SetMovementMode(MOVE_Custom,ECustomMovementMode::MOVE_Climb);
		}
		else
		{
			
		}
	}
	else
	{
		SetMovementMode(MOVE_Walking);
	}
}

void UClimbingMovementComponent::PhysicsClimb(float deltaTime, int32 Iterations)
{
   // this function runs just as tick
	
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Trace for climbable surfaces
	 ClimbableSurfaceDetection();
	 // process climbable surfaces
	ProcessingClimbableSurfaces();
	
	 // if none return early

	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
;
		CalcVelocity(deltaTime, 0.f, true, MaxBreakingDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);


	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		
	}

	if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	Debug::PrintDebugMessage(TEXT("Climbable Surface Location :") + ClimbableSurfaceLocation.ToString());
	Debug::PrintDebugMessage(TEXT("Climbable Surface Normal :") + ClimbableSurfaceNormal.ToString());
}

bool UClimbingMovementComponent::ClimbableSurfaceDetection()
{
   const FVector Start = UpdatedComponent->GetComponentLocation()+UpdatedComponent->GetForwardVector()*25.f;
	const FVector End =Start+UpdatedComponent->GetForwardVector();
	CapsuleTraceHitResult = CapsuleTraceMultiForObjects(Start,End,false);
	return !CapsuleTraceHitResult.IsEmpty();
	
}

bool UClimbingMovementComponent::EyeLevelSurfaceDetection(float TraceDistance, float TraceOffset)
{
	const FVector Start = UpdatedComponent->GetComponentLocation()+UpdatedComponent->GetForwardVector()+UpdatedComponent->GetUpVector()*(CharacterOwner->BaseEyeHeight +TraceOffset );
	const FVector End  = Start+UpdatedComponent->GetForwardVector()*TraceDistance;
	 LineTraceHitResult =  EyeLengthLineTraceSingle(Start,End,false);
	return LineTraceHitResult.bBlockingHit;
}

void UClimbingMovementComponent::ProcessingClimbableSurfaces()
{
	 ClimbableSurfaceLocation = FVector::ZeroVector;
	 ClimbableSurfaceNormal = FVector::ZeroVector;
	if (CapsuleTraceHitResult.IsEmpty())
	{
		return;
	}
	for (const auto& TraceHitResult : CapsuleTraceHitResult)
	{
		ClimbableSurfaceLocation += TraceHitResult.ImpactPoint;
		ClimbableSurfaceNormal += TraceHitResult.ImpactNormal;
	}
	 ClimbableSurfaceLocation/=CapsuleTraceHitResult.Num();
	 ClimbableSurfaceNormal = ClimbableSurfaceNormal.GetSafeNormal();
}

void UClimbingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UClimbingMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	// this gets called every time a movement mode changes by taking previous ones 
    if (AmIClimbing())
    {
	    bOrientRotationToMovement = false;
    	
    	CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
    }
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
		// this function stops u form doing any form of movement
		StopMovementImmediately();	
	}
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UClimbingMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (AmIClimbing())
	{
		PhysicsClimb(deltaTime,Iterations);
	}
	Super::PhysCustom(deltaTime, Iterations);
}
