// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "ShooterCameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	ShooterCameraComponent = CreateDefaultSubobject<UShooterCameraComponent>(TEXT("Shooter Camera Component"));
	ShooterCameraComponent->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);

	SetHealth(MaxHealth);

	OnInitialized.Broadcast();
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

AGun* AShooterCharacter::GetCurrentGun() const
{
	UE_LOG(LogTemp, Warning, TEXT("IS GUN A NULL? - %hs"), Gun ? "false" : "true");
	return Gun;
}

float AShooterCharacter::GetRecoilImpactAmount() const
{
	return TargetRecoilImpact.Length();
}

float AShooterCharacter::GetRecoilHandleValue() const
{
	return RecoilHandlingValue;
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
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ToggleAimMode);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ReloadCurrentGun);
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
	if(ShooterCameraComponent->IsCameraInTransition())
		return;
	
	FVector2D RecoilImpact = Gun->PullTrigger();

	TargetRecoilImpact.X = IsAimMode ? RecoilImpact.X * 0.25f : RecoilImpact.X;
	TargetRecoilImpact.Y = IsAimMode ? RecoilImpact.Y * 0.25f : RecoilImpact.Y;
	
	AddControllerPitchInput(TargetRecoilImpact.Y);
	AddControllerYawInput(TargetRecoilImpact.X);

	OnShoot.Broadcast(RecoilImpact);
}

void AShooterCharacter::ReloadCurrentGun()
{
	Gun->Reload();
}

void AShooterCharacter::ToggleAimMode()
{
	IsAimMode = !IsAimMode;

	if(IsAimMode)
		ShooterCameraComponent->TurnOnAimMode();
	else
		ShooterCameraComponent->TurnOnDefaultMode();

	OnToggleAimMode.Broadcast(IsAimMode);
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
	float VerticalRecoilDiff = -TargetRecoilImpact.Y - FMath::Lerp(-TargetRecoilImpact.Y, 0.0F, DeltaTime * RecoilHandlingValue);
	float HorizontalRecoilDiff = TargetRecoilImpact.X - FMath::Lerp(TargetRecoilImpact.X, 0.0F, DeltaTime * RecoilHandlingValue);
	
	AddControllerPitchInput(VerticalRecoilDiff);
	AddControllerYawInput(-HorizontalRecoilDiff);
	
	TargetRecoilImpact.Y += VerticalRecoilDiff;
	TargetRecoilImpact.X -= HorizontalRecoilDiff;
}

