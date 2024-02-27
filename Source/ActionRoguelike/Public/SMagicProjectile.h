// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectile.generated.h"

class UParticleSystemComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class ACTIONROGUELIKE_API ASMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:
	// For Collision Test
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	// For Movement Control
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementComponent;

	// For Particle Effect
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* EffectComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};