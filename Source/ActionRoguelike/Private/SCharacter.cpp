// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRoguelike/Public/SCharacter.h"

#include "ActionRoguelike/Public/SInteractComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Component Via UObject::CreateDefaultSubobject (Can Only Called in Constructor)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	// Attach Component to Root
	SpringArmComponent->SetupAttachment(RootComponent);
	// Turn On Using Pawn Control Rotation to Enable Pitching
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	InteractComponent = CreateDefaultSubobject<USInteractComponent>("InteractComponent");

	// Avoid Unnatural Rotation of Character
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRotator = GetControlRotation();
	// We Only Need Yaw Rotation
	ControlRotator.Pitch = 0.0f;
	ControlRotator.Roll = 0.0f;
	
	AddMovementInput(ControlRotator.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRotator = GetControlRotation();
	ControlRotator.Pitch = 0.0f;
	ControlRotator.Roll = 0.0f;

	// X -> Forward
	// Y -> Right
	// Z -> Up
	const FVector RightVector = FRotationMatrix(ControlRotator).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	// Begin Play Attack Animation
	PlayAnimMontage(AttackAnimation);

	// Call ASCharacter::PrimaryAttack_TimeElapsed After 0.2s
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	// Get Hand Location
	const FVector HandLocation = GetMesh()->GetSocketLocation(*HandSocketName);
	FRotator SpawnRotation;
	
	// Ignore Player Character
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Add More Object Types to Trace
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	// From Camera Location to Trace a Ray
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + GetControlRotation().Vector() * 10000;
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ObjectQueryParams, QueryParams);

	AActor* HitActor = HitResult.GetActor();
	if (HitActor != nullptr)
	{
		SpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, HitResult.ImpactPoint);
	}
	else
	{
		SpawnRotation = GetControlRotation();
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = this;
	
	const FTransform SpawnTM = FTransform(SpawnRotation, HandLocation);

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParameters);
}

void ASCharacter::PrimaryInteract()
{
	InteractComponent->PrimaryInteract();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ASCharacter::MoveForward is Implemented by Ourselves
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	// AddControllerYawInput is Implemented by Pawn
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Jumping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	// Attack
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed,this, &ASCharacter::PrimaryAttack);

	// Interact
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

