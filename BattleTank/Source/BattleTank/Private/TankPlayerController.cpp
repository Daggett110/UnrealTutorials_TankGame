// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTank.h"
#include "TankPlayerController.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();
	if (!ControlledTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not possesing a tank."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Controller possesing %s"), *(ControlledTank->GetName()));
	}
}

void ATankPlayerController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	
	AimTowardsCrosshair();
}

ATank* ATankPlayerController::GetControlledTank() const
{
	// Grab the pawn this controller is using and turn it into an ATank object
	return Cast<ATank>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair()
{
	// Ensure that we, the player are currently controlling a tank
	if (!GetControlledTank()) { return; }

	FVector HitLocation;	// Out Parameter
	if (GetSightRayHitLocation(HitLocation))
	{
		GetControlledTank()->AimAt(HitLocation);
	}
}

// GetWorld lovation of linetrace through crosshair, true if hits landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector& OutHitLocation) const
{
	// Find the crosshair position
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);
	// "De-project" the screen position of the crosshair to a world direction
	FVector LookDirection;
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		// Line-trace along that look direction, and see what we hit (up to max range)
		GetLookVectorHitLocation(LookDirection, OutHitLocation);
	}
	return true;
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& OutHitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility))
	{
		// Set Hit Location
		OutHitLocation = HitResult.Location;
		return true;
	}
	OutHitLocation = FVector(0);
	return false;
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // For discarding
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y,
		CameraWorldLocation,
		LookDirection);
}
