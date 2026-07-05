// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MorrowBoneAnimInstance.generated.h"

class UClimbingMovementComponent;
class AClimbingSystemCharacter;
/**
 * 
 */
UCLASS()
class CLIMBINGSYSTEM_API UMorrowBoneAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation();

	virtual void NativeUpdateAnimation(float DeltaSeconds);

protected:
	UPROPERTY()
	TObjectPtr<AClimbingSystemCharacter> ClimbingSystemCharacter;
	UPROPERTY()
	TObjectPtr<UClimbingMovementComponent> ClimbingMovementComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Locomotion")
	float GroundSpeed;
	void UpdateGroundSpeed();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Locomotion")
	float AirSpeed;
	void UpdateAirSpeed();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Locomotion")
	bool bShouldIMove;
	void UpdatebShouldIMove();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Locomotion")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Locomotion")
	float LocomotionDirection;
	void UpdateLocomotionDirection();

	void UpdateIsFalling();
};



