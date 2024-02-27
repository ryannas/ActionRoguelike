// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRoguelike/Public/SInteractComponent.h"

#include "DrawDebugHelpers.h"
#include "ActionRoguelike/Public/SGameplayInterface.h"

// Sets default values for this component's properties
USInteractComponent::USInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	AActor* ComponentOwner = GetOwner();
	
	FVector EyePosition;
	FRotator EyeRotator;
	ComponentOwner->GetActorEyesViewPoint(EyePosition, EyeRotator);

	const FVector Start = EyePosition;
	const FVector End = Start + EyeRotator.Vector() * 1000;
	
	/*
	// Use Line Tracing to Find Out Which Object Can Be Interacting With
	FHitResult HitResult;
	bool bHitSomething = GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ObjectQueryParams);

	AActor* HitActor = HitResult.GetActor();
	if (HitActor != nullptr)
	{
		if (HitActor->Implements<USGameplayInterface>())
		{
			APawn* Owner = Cast<APawn>(ComponentOwner);
			ISGameplayInterface::Execute_Interact(HitActor, Owner);
		}
	}

	FColor LineColor = bHitSomething ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
	*/

	// Use Sweep Trace
	constexpr float Radius = 30.f;
	
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(Radius);
	
	TArray<FHitResult> HitResults;
	const bool bHitSomething = GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, ObjectQueryParams, CollisionShape);

	const FColor LineColor = bHitSomething ? FColor::Red : FColor::Green;

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor != nullptr)
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				APawn* Owner = Cast<APawn>(ComponentOwner);
				ISGameplayInterface::Execute_Interact(HitActor, Owner);
				break;
			}
		}
	}

	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
}

