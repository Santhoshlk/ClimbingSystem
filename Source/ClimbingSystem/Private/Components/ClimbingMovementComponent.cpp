// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ClimbingMovementComponent.h"
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
		bDrawDebugTypes ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
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
		bDrawDebugTypes ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		OutHitResult,
		true
		);
	return OutHitResult;
}

void UClimbingMovementComponent::ClimbableSurfaceDetection()
{
   const FVector Start = UpdatedComponent->GetComponentLocation()+UpdatedComponent->GetForwardVector()*25.f;
	const FVector End =Start+UpdatedComponent->GetForwardVector();
	CapsuleTraceMultiForObjects(Start,End,true);
}

void UClimbingMovementComponent::EyeLevelSurfaceDetection(float TraceDistance, float TraceOffset)
{
	const FVector Start = UpdatedComponent->GetComponentLocation()+UpdatedComponent->GetUpVector()*(CharacterOwner->BaseEyeHeight +TraceOffset );
	const FVector End  = Start+UpdatedComponent->GetForwardVector()*TraceDistance;
	EyeLengthLineTraceSingle(Start,End,true);
}

void UClimbingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ClimbableSurfaceDetection();
	EyeLevelSurfaceDetection(150.f);
}
