// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	CurrentAmmo = MaxAmmo;
}

void AGun::Reload()
{
	if (IsReloading || CurrentAmmo >= MaxAmmo)
		return;
	
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &AGun::OnReloadProcessDone, ReloadTime);
	IsReloading = true;

	OnReload.Broadcast();
}

FVector2D AGun::PullTrigger() 
{
	if (IsReloading)
	{
		return FVector2D(0,0);
	}

	if (CurrentAmmo <= 0)
	{
		Reload();
		return FVector2D(0,0);
	}
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));

	FHitResult Hit;
	FVector ShotDirection;
	bool bSuccess = GunTrace(Hit, ShotDirection);
	if (bSuccess)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
		
		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			float NewRandomDamage = FMath::RandRange(Damage * 0.5f, Damage * 1.5f);
			
			FPointDamageEvent DamageEvent(NewRandomDamage, Hit, ShotDirection, nullptr);
			AController *OwnerController = GetOwnerController();
			HitActor->TakeDamage(NewRandomDamage, DamageEvent, OwnerController, this);
		}
	}

	FVector2D NewRecoil = FVector2D::ZeroVector;
	NewRecoil.Y = -FMath::RandRange(VerticalRecoil  * 0.75f, VerticalRecoil);
	NewRecoil.X = FMath::RandRange(-HorizontalRecoil,HorizontalRecoil);


	AController* Controller = GetOwnerController();

	if (Controller != nullptr && Cast<APlayerController>(Controller))
		CurrentAmmo--;

	OnShot.Broadcast();
	
	return NewRecoil;
}

int AGun::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int AGun::GetMaxAmmo() const
{
	return MaxAmmo;
}

float AGun::GetReloadTime() const
{
	return ReloadTime;
}

bool AGun::GunTrace(FHitResult &Hit, FVector& ShotDirection) 
{
	AController *OwnerController = GetOwnerController();
	if (OwnerController == nullptr)
		return false;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();

	FVector End = Location + Rotation.Vector() * MaxRange;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

void AGun::OnReloadProcessDone()
{
	CurrentAmmo = MaxAmmo;
	IsReloading = false;

	OnReloadDone.Broadcast();
}

AController* AGun::GetOwnerController() const
{
	APawn *OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
		return nullptr;
	return OwnerPawn->GetController();
}

