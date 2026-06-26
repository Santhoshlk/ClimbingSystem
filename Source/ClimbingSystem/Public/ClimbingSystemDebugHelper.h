// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

namespace Debug {

	static void PrintDebugMessage(const FString& Msg,uint64 key = -1,FColor Color = FColor::MakeRandomColor(),float DisplayTime = 5.f)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(key,DisplayTime,Color,Msg);
			UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);
		}
	}

	static void PrintDebugData(const FString& Msg,float data ,uint64 key = -1,FColor Color = FColor::MakeRandomColor(),float DisplayTime = 5.f)
	{
		if (GEngine)
		{
			const FString Message = FString::Printf(TEXT("%s : %s "),*Msg,*FString::SanitizeFloat(data));
		
			GEngine->AddOnScreenDebugMessage(key,DisplayTime,Color,Message);
			UE_LOG(LogTemp,Warning,TEXT("%s"),*Message);
		}
	}
}
