// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplay.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MyCharacter.h"


AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	//keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AMyPlayerController::SetupInputComponent()
{
	//set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AMyPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AMyPlayerController::OnSetDestinationReleased);

	//support for touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMyPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMyPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyPlayerController::OnResetVR);
}

void AMyPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyPlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (AMyCharacter* MyPawn = Cast<AMyCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UNavigationSystem::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		//trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			//we hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void AMyPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	//trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		//we hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AMyPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	AMyCharacter* MyPawn = Cast<AMyCharacter>(GetPawn());

	if (MyPawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		//we need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f) && (MyPawn->getDistanceLeft() > 0))
		{
			MyPawn->modDistanceLeft(-1);
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AMyPlayerController::OnSetDestinationPressed()
{
	//set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AMyPlayerController::OnSetDestinationReleased()
{
	//clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
