// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/MorrowBoneAnimInstance.h"

#include "ClimbingSystemCharacter.h"
#include "Components/ClimbingMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMorrowBoneAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// define all the pointers
	ClimbingSystemCharacter = Cast<AClimbingSystemCharacter>(TryGetPawnOwner());
	if (ClimbingSystemCharacter)
	{
		ClimbingMovementComponent = ClimbingSystemCharacter->GetClimbingMovementComponent();
	}
}

void UMorrowBoneAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ClimbingSystemCharacter || !ClimbingMovementComponent) return;
	UpdateGroundSpeed();
	UpdateAirSpeed();
	UpdatebShouldIMove();
	UpdateIsFalling();
}

void UMorrowBoneAnimInstance::UpdateGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(ClimbingMovementComponent->Velocity);
}

void UMorrowBoneAnimInstance::UpdateAirSpeed()
{
	AirSpeed = ClimbingMovementComponent->Velocity.Z;
}

void UMorrowBoneAnimInstance::UpdatebShouldIMove()
{
	if (ClimbingMovementComponent->GetCurrentAcceleration().Length() > 0 &&  GroundSpeed>5.f && !bIsFalling  )
	{
		bShouldIMove = true;
	}
	bShouldIMove = false;
}

void UMorrowBoneAnimInstance::UpdateIsFalling()
{
	bIsFalling = ClimbingMovementComponent->IsFalling();
}
