// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "ShooterCharacter.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

void AShooterPlayerController::InitHUD()
{
    HUD = CreateWidget(this, HUDClass);
    if (HUD != nullptr)
    {
        HUD->AddToViewport();
    }
}

void AShooterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());

    if (ShooterCharacter->IsDead())
    {
        ShooterCharacter->OnInitialized.AddUObject(this, &AShooterPlayerController::InitHUD);
    }

    else
    {
        InitHUD();
    }
}

void AShooterPlayerController::GameHasEnded(class AActor *EndGameFocus, bool bIsWinner) 
{
    Super::GameHasEnded(EndGameFocus, bIsWinner);

    HUD->RemoveFromViewport();
    if (bIsWinner)
    {
        UUserWidget *WinScreen = CreateWidget(this, WinScreenClass);
        if (WinScreen != nullptr)
        {
            WinScreen->AddToViewport();
        }
    }
    else
    {
        UUserWidget *LoseScreen = CreateWidget(this, LoseScreenClass);
        if (LoseScreen != nullptr)
        {
            LoseScreen->AddToViewport();
        }
    }

    GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}

