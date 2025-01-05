// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "ShooterCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UShooterCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	enum CameraMode
	{
		DEFAULT,
		AIM,
		SPRINT,
	};

	UShooterCameraComponent();
	
protected:
	UPROPERTY(EditAnywhere)
	float TransitionTime = 0.3;

	UPROPERTY(EditAnywhere)
	float AimFOV = 45;

	UPROPERTY(EditAnywhere)
	float SprintFOV = 100;

	CameraMode CurrentCameraMode = DEFAULT;
	
private:
	float DefaultFOV = 0;

//--- TRANSITION VALUES ---//
	FTimerHandle CurrentTimer;
	float TimeElapsed = 0;
	float TimerRate = 0.01;
	
	float StartFOV = 0;
	float EndFOV = 0;

public:
	void TurnOnAimMode();
	void TurnOnDefaultMode();
	void TurnOnSprintMode();

	bool IsCameraInTransition() const;
	CameraMode GetCameraMode() const;

private:
	void StartFOVTransition(float ToFOV, CameraMode ToCameraMode);
	void FOVTransitionProcess();
	void OnFOVTransitionStopped();
};
