// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadDone);

UCLASS()
class SIMPLESHOOTER_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	void Reload();
	FVector2D PullTrigger();

private:
	UFUNCTION(BlueprintPure)
	int GetCurrentAmmo() const;

	UFUNCTION(BlueprintPure)
	int GetMaxAmmo() const;

	UFUNCTION(BlueprintPure)
	float GetReloadTime() const;
	
public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnShot OnShot;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnReload OnReload;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnReloadDone OnReloadDone;
	
private:
	
// -- COMPONENTS -- //
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem *MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem *ImpactEffect;

	UPROPERTY(EditAnywhere)
	USoundBase *ImpactSound;
	
// -- PROPERTIES -- //

	UPROPERTY(VisibleAnywhere)
	int CurrentAmmo = 0;

	UPROPERTY(VisibleAnywhere)
	bool IsReloading = false;

	UPROPERTY(EditDefaultsOnly, Category="Gun Properties")
	float ReloadTime = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="Gun Properties")
	int MaxAmmo = 15;
	
	UPROPERTY(EditDefaultsOnly, Category="Gun Properties")
	float MaxRange = 1000;

	UPROPERTY(EditDefaultsOnly, Category="Gun Properties")
	float Damage = 10;

	UPROPERTY(EditDefaultsOnly, Category="Gun Properties")
	float VerticalRecoil = 1.0;

	UPROPERTY(EditDefaultsOnly, Category="Gun Properties")
	float HorizontalRecoil = 1.0;

	FTimerHandle ReloadTimer;
	
	bool GunTrace(FHitResult &Hit, FVector& ShotDirection);
	void OnReloadProcessDone();
	
	AController* GetOwnerController() const;
};
