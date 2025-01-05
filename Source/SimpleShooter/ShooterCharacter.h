// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UShooterCameraComponent;
class AGun;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShoot, FVector2D, NewRecoilImpact);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleAimMode, bool, IsAimMode);

UCLASS()
class SIMPLESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnShoot OnShoot;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnToggleAimMode OnToggleAimMode;
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure)
	float GetRecoilImpactAmount() const;
	
	UFUNCTION(BlueprintPure)
	float GetRecoilHandleValue() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	
	void Shoot();
	void ToggleAimMode();

private:
	void SetHealth(float NewHealth);
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void HandleRecoilImpact(float DeltaTime);

	UPROPERTY(EditDefaultsOnly)
	float RecoilHandlingValue = 10;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	AGun* Gun;

	UPROPERTY(EditDefaultsOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly)
	UShooterCameraComponent* ShooterCameraComponent;
	
	FVector2D TargetRecoilImpact;
	bool IsAimMode = false;
};
