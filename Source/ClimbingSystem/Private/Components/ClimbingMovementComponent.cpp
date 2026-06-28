// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ClimbingMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FHitResult> UClimbingMovementComponent::CapsuleTraceMultiForObjects(const FVector& Start, const FVector& End,bool DrawDebugTypes)
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
		DrawDebugTypes ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		OutCapsuleTraceHitResult,
		true
		);
	return OutCapsuleTraceHitResult;
}

void UClimbingMovementComponent::ClimbableSurfaceDetection()
{
   const FVector Start = UpdatedComponent->GetComponentLocation()+UpdatedComponent->GetForwardVector()*25.f;
	const FVector End =Start+UpdatedComponent->GetForwardVector();
	CapsuleTraceMultiForObjects(Start,End,true);
}

void UClimbingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ClimbableSurfaceDetection();
}
