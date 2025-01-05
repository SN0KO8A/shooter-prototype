// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCameraComponent.h"

UShooterCameraComponent::UShooterCameraComponent()
{
	DefaultFOV = FieldOfView;
}

void UShooterCameraComponent::TurnOnAimMode()
{
	StartFOVTransition(AimFOV, AIM);
}

void UShooterCameraComponent::TurnOnDefaultMode()
{
	StartFOVTransition(DefaultFOV, DEFAULT);
}

void UShooterCameraComponent::TurnOnSprintMode()
{
	StartFOVTransition(SprintFOV, SPRINT);
}

bool UShooterCameraComponent::IsCameraInTransition() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(CurrentTimer);
}

UShooterCameraComponent::CameraMode UShooterCameraComponent::GetCameraMode() const
{
	return CurrentCameraMode;
}

void UShooterCameraComponent::StartFOVTransition(float ToFOV, CameraMode ToCameraMode)
{
	StartFOV = FieldOfView;
	EndFOV = ToFOV;

	CurrentCameraMode = ToCameraMode;
	TimeElapsed = 0;
	
	GetWorld()->GetTimerManager().SetTimer(
		CurrentTimer,
		this,
		&UShooterCameraComponent::FOVTransitionProcess,
		TimerRate,
		true
		);
}

void UShooterCameraComponent::FOVTransitionProcess()
{
	TimeElapsed += TimerRate;

	if(TimeElapsed >= TransitionTime)
	{
		OnFOVTransitionStopped();
		return;
	}

	FieldOfView = FMath::InterpEaseOut(StartFOV, EndFOV, TimeElapsed / TransitionTime, 5.0f);
}

void UShooterCameraComponent::OnFOVTransitionStopped()
{
	FieldOfView = EndFOV;

	GetWorld()->GetTimerManager().ClearTimer(CurrentTimer);
}
