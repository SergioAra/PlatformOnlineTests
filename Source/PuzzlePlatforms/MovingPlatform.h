// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */

UCLASS()
class PUZZLEPLATFORMS_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	
	AMovingPlatform();

	virtual void BeginPlay() override;

	void AddActiveTrigger();
	void RemoveActiveTrigger();

protected:

	virtual void Tick(float DeltaTime) override;

	void GenerateRoute();

	void SetNextTarget();

	UPROPERTY(EditAnywhere)
	float Speed = 20.f;
	//Creates an editor widget to show the location
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true))
	TArray<FVector> TargetLocations;

	TArray<FVector> Route;

	FVector CurrentTargetLocation;

	FVector StartLocation;

	float RouteIndex = 0;

	int ActiveTriggers = 0;

	UPROPERTY(EditAnywhere)
	bool bUseTriggers = false;

};
