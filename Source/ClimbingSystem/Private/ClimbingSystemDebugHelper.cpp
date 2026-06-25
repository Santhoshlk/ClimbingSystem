// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingSystemDebugHelper.h"




void Debug::PrintDebugMessage(const FString& Msg, uint64 key, FColor Color, float DisplayTime)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(key,DisplayTime,Color,Msg);
		UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);
	}
}

void Debug::PrintDebugData(const FString& Msg, float data, uint64 key, FColor Color, float DisplayTime)
{
	if (GEngine)
	{
		const FString Message = FString::Printf(TEXT("%s : %s "),*Msg,*FString::SanitizeFloat(data));
		
		GEngine->AddOnScreenDebugMessage(key,DisplayTime,Color,Message);
		UE_LOG(LogTemp,Warning,TEXT("%s"),*Message);
	}
}


