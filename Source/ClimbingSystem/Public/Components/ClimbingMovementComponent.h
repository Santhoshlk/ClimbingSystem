// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimbingMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CLIMBINGSYSTEM_API UClimbingMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

#pragma region Traces
   TArray<FHitResult> CapsuleTraceMultiForObjects(const FVector& Start , const FVector& End , bool DrawDebugTypes);
	
#pragma endregion

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float CapsuleTraceRadius = 50.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	float CapsuleTraceHalfHeight = 80.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true") )
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	 
};
