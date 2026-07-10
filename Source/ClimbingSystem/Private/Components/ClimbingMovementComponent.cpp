// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ClimbingMovementComponent.h"
#include "ClimbingSystemDebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
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
			
		  
			// pLay the Montage to start Climb
			PlayClimbMontage(IdleToClimbMontage);
		}
		else
		{
			// temporary
			SetMovementMode(MOVE_Walking);
		}
	}
	else
	{
		SetMovementMode(MOVE_Falling);
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
	if (ShouldIStopClimbing())
	{
		 ToggleClimbingState(false);
	}

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
	SafeMoveUpdatedComponent(Adjusted, SetClimbRotation(deltaTime), true, Hit);

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

	SnapToSurfaces(deltaTime);
}

FVector UClimbingMovementComponent::GetUnRotatedClimbVelocity() const
{
	// unrotated climb velocity is velocity in the direction of the character
	// we rotate the velocity which is given in the world direction axis to our direction to feed the blend spaces
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(),Velocity);
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

FQuat UClimbingMovementComponent::SetClimbRotation(float deltaTime) const 
{
	// check if root motion is running the animation
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return UpdatedComponent->GetComponentQuat();
	}

	// next is u do interpolation
	// u need to set a target quad make it toward the one  u need to rotate
	 const FQuat TargetQuat = FRotationMatrix::MakeFromX(-ClimbableSurfaceNormal).ToQuat();
	return FMath::QInterpTo(UpdatedComponent->GetComponentQuat(),TargetQuat,deltaTime,5.f);
}

void UClimbingMovementComponent::SnapToSurfaces(float DeltaTime)
{
	// u need to move towards the wall
	// so u need distance
	// first vector from character to component
	const FVector ComponentToSurface = (ClimbableSurfaceNormal - UpdatedComponent->GetComponentLocation()).ProjectOnTo(UpdatedComponent->GetForwardVector());
	const FVector SnapVector = (-ClimbableSurfaceNormal)*ComponentToSurface.Length();
	UpdatedComponent->MoveComponent(
    SnapVector*DeltaTime* MaxClimbSpeed,
    UpdatedComponent->GetComponentQuat(),
    true
	);
	
}

void UClimbingMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterAnimInstance =  CharacterOwner->GetMesh()->GetAnimInstance();
	if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnMontageEnded.AddDynamic(this,&UClimbingMovementComponent::OnClimbMontageEnded);
		CharacterAnimInstance->OnMontageBlendingOut.AddDynamic(this,&UClimbingMovementComponent::OnClimbMontageEnded);
	}
	
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
		const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
		const FRotator CleanRotation = FRotator(0.f,DirtyRotation.Yaw,0.f);
		UpdatedComponent->SetRelativeRotation(CleanRotation);
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

float UClimbingMovementComponent::GetMaxSpeed() const
{
	if (AmIClimbing())
	{
		// as you are climbing return ur set speed
		return MaxClimbSpeed;
	}
	return Super::GetMaxSpeed();
}

float UClimbingMovementComponent::GetMaxAcceleration() const
{
	if (AmIClimbing())
	{
		// as you are climbing return ur set acceleration
		return MaxClimbAcceleration;
	}
	return Super::GetMaxAcceleration();
}

bool UClimbingMovementComponent::ShouldIStopClimbing()
{
	if (CapsuleTraceHitResult.IsEmpty()) return true;
	const float DotProductResult = FVector::DotProduct(ClimbableSurfaceNormal,FVector::UpVector);
	const float DegreeAngle = FMath::RadiansToDegrees(FMath::Acos(DotProductResult));

	
	if (DegreeAngle<=ClimbSlopeMinAngle)
	{
		return true;
	}
   
return false;
}

void UClimbingMovementComponent::PlayClimbMontage( UAnimMontage* Montage) const
{
	if (!Montage) return;
	if (!CharacterAnimInstance) return;
	if (CharacterAnimInstance->IsAnyMontagePlaying()) return;
	CharacterAnimInstance->Montage_Play(Montage);
}

void UClimbingMovementComponent::OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == IdleToClimbMontage)
	{
		// once this montage has ende u change the state
		SetMovementMode(MOVE_Custom,ECustomMovementMode::MOVE_Climb);
	}
}
