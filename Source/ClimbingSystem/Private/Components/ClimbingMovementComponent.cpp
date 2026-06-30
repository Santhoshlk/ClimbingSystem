// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ClimbingMovementComponent.h"
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
		
		SetMovementMode(MOVE_Falling);
	}
}

bool UClimbingMovementComponent::ClimbableSurfaceDetection()
{
   const FVector Start = UpdatedComponent->GetComponentLocation()+UpdatedComponent->GetForwardVector()*25.f;
	const FVector End =Start+UpdatedComponent->GetForwardVector();
	CapsuleTraceHitResult = CapsuleTraceMultiForObjects(Start,End,true);
	return !CapsuleTraceHitResult.IsEmpty();
}

bool UClimbingMovementComponent::EyeLevelSurfaceDetection(float TraceDistance, float TraceOffset)
{
	const FVector Start = UpdatedComponent->GetComponentLocation()+UpdatedComponent->GetForwardVector()+UpdatedComponent->GetUpVector()*(CharacterOwner->BaseEyeHeight +TraceOffset );
	const FVector End  = Start+UpdatedComponent->GetForwardVector()*TraceDistance;
	 LineTraceHitResult =  EyeLengthLineTraceSingle(Start,End,true);
	return LineTraceHitResult.bBlockingHit;
}

void UClimbingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UClimbingMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	// this gets called every time a movement mode changes by taking previous ones 
    if (IsClimbingPossible())
    {
	    bOrientRotationToMovement = false;
    	
    	CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
    }
	if (PreviousMovementMode == MOVE_Custom && PreviousMovementMode == ECustomMovementMode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
	}
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}
