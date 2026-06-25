// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace Debug {

	static void PrintDebugMessage(const FString& Msg,uint64 key = -1,FColor Color = FColor::MakeRandomColor(),float DisplayTime = 5.f);

	static void PrintDebugData(const FString& Msg,float data ,uint64 key = -1,FColor Color = FColor::MakeRandomColor(),float DisplayTime = 5.f);
}
