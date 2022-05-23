// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
		GenerateRoute();
		CurrentTargetLocation = Route.IsValidIndex(RouteIndex) ? Route[RouteIndex] : FVector::ZeroVector;
		StartLocation = GetActorLocation();
		if (!bUseTriggers)
		{
			ActiveTriggers = 1.f;
		}
	}
}

void AMovingPlatform::AddActiveTrigger()
{
	ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0)
	{
		ActiveTriggers--;
	}
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveTriggers > 0)
	{
		if (HasAuthority())
		{
			FVector Location = GetActorLocation();
			FVector Path = CurrentTargetLocation - StartLocation;
			const float PathLength = Path.Size();
			const float PathTraveled = (Location - StartLocation).Size();
			if (PathTraveled >= PathLength)
			{
				SetNextTarget();
			}
			const FVector Direction = (CurrentTargetLocation - StartLocation).GetSafeNormal();
			Location += Speed * DeltaTime * Direction;
			SetActorLocation(Location);
		}
	}

}

void AMovingPlatform::GenerateRoute()
{
	for (FVector CurrentPos : TargetLocations)
	{
		Route.Add(GetTransform().TransformPosition(CurrentPos));
	}

	Route.Add(GetActorLocation());
}

void AMovingPlatform::SetNextTarget()
{
	RouteIndex++;
	if (Route.IsValidIndex(RouteIndex))
	{
		StartLocation = GetActorLocation();
		CurrentTargetLocation = Route[RouteIndex];
	}
	else
	{
		RouteIndex = 0;
		if (Route.IsValidIndex(RouteIndex))
		{
			StartLocation = GetActorLocation();
			CurrentTargetLocation = Route[RouteIndex];
		}
		else
		{
			CurrentTargetLocation = StartLocation;
		}
	}
}
