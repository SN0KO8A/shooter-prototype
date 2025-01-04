// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetHealth(MaxHealth);

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float AShooterCharacter::GetRecoilImpactAmount() const
{
	return TargetRecoilImpact.Length();
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float MouseDeltaX, MouseDeltaY;
	GetWorld()->GetFirstPlayerController()->GetInputMouseDelta(MouseDeltaX, MouseDeltaY);
	
	if(MouseDeltaX > 0.1 || MouseDeltaY > 0.1 || GetRecoilImpactAmount() <= 0.1f)
	{
		TargetRecoilImpact = FVector2D::ZeroVector;
	}

	if(GetRecoilImpactAmount() > 0.1f)
	{
		HandleRecoilImpact(DeltaTime);
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) 
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);

	SetHealth(Health - DamageToApply);

	if (IsDead())
	{
		ASimpleShooterGameModeBase *GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}

void AShooterCharacter::Shoot() 
{
	FVector2D RecoilImpact = Gun->PullTrigger();

	TargetRecoilImpact.X = RecoilImpact.X;
	TargetRecoilImpact.Y = RecoilImpact.Y;
	
	AddControllerPitchInput(TargetRecoilImpact.Y);
	AddControllerYawInput(TargetRecoilImpact.X);
}

void AShooterCharacter::SetHealth(float NewHealth)
{
	Health = NewHealth;
	OnHealthChanged.Broadcast(GetHealthPercent());
}

void AShooterCharacter::MoveForward(float AxisValue) 
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue) 
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::HandleRecoilImpact(float DeltaTime)
{
	float VerticalRecoilDiff = -TargetRecoilImpact.Y - (-TargetRecoilImpact.Y - DeltaTime * RecoilHandlingValue);

	float HorizontalRecoilDirection = FMath::Sign(TargetRecoilImpact.X);
	float HorizontalRecoilDiff = FMath::Abs(TargetRecoilImpact.X) - (FMath::Abs(TargetRecoilImpact.X) - DeltaTime * RecoilHandlingValue);
	
	AddControllerPitchInput(VerticalRecoilDiff);
	AddControllerYawInput(HorizontalRecoilDiff * -HorizontalRecoilDirection);
	
	TargetRecoilImpact.Y += VerticalRecoilDiff;
	TargetRecoilImpact.X += HorizontalRecoilDiff * -HorizontalRecoilDirection;
}

