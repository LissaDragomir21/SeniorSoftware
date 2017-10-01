// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MOVINGWITHMOUSE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMyPlayerController();

protected:
	//true if the controlled character should navigate to the mouse cursor
	uint32 bMoveToMouseCursor : 1;

	//begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	//end PlayerController interface

	//resets orientation in if being used in VR
	void OnResetVR();

	//navigate player to the current mouse cursor location
	void MoveToMouseCursor();

	//navigate player to the current touch location
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);

	//navigate player to the given world location
	void SetNewMoveDestination(const FVector DestLocation);

	//input handlers for SetDestination action
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	
};
