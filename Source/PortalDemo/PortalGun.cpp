// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalGun.h"

#include "PortalManager.h"
#include "PortalWall.h"

// Sets default values
APortalGun::APortalGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void APortalGun::BeginPlay()
{
	Super::BeginPlay();

	PortalManager = GetWorld()->SpawnActor<APortalManager>(PortalManagerClass);
}

// Called every frame
void APortalGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FRotator CharacterRotation = GetOwner()->GetActorRotation();
	//UE_LOG(LogTemp, Warning, TEXT("Rotacja: %s"), *CharacterRotation.Vector().ToString());
}

void APortalGun::CreatePortalEnter()
{
	FHitResult Hit;
	if (Shot(Hit) && Hit.GetActor() != nullptr && Hit.GetActor()->IsA<APortalWall>())
	{
		FixPortalPosition(Hit);
		FVector OwnerRotationVector = GetOwner()->GetActorRotation().Vector();
		PortalManager->CreatePortalEnter(Hit, OwnerRotationVector);
	}
}

void APortalGun::CreatePortalExit()
{
	FHitResult Hit;
	if (Shot(Hit) && Hit.GetActor() != nullptr && Hit.GetActor()->IsA<APortalWall>())
	{
		FixPortalPosition(Hit);
		FVector OwnerRotationVector = GetOwner()->GetActorRotation().Vector();
		PortalManager->CreatePortalExit(Hit, OwnerRotationVector);
	}
}

bool APortalGun::Shot(FHitResult& Hit)
{
	FVector ShotDirection;
	bool GunTraceResult = PortalGunTrace(Hit, ShotDirection);


	return GunTraceResult;
	//TODO: Animacja i dźwięk strzału
	if (GunTraceResult)
	{
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
	}
}

bool APortalGun::PortalGunTrace(FHitResult& Hit, FVector& ShotDirection) const
{
	const AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr)
		return false;

	FVector Location;
	FRotator Rotation;

	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();

	const FVector End = Location + Rotation.Vector() * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End,
	                                            ECollisionChannel::ECC_GameTraceChannel1, Params);;
}

void APortalGun::FixPortalPosition(FHitResult& PortalHit)
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	FVector Location = PortalHit.Location + PortalHit.Normal * 100;

	FVector RightOffset = PortalHit.Normal.Rotation().RotateVector(FVector::LeftVector);
	FVector RightPosition = Location + RightOffset * 85.0;

	FVector LeftOffset = PortalHit.Normal.Rotation().RotateVector(FVector::RightVector);
	FVector LeftPosition = Location + LeftOffset * 85.0;

	FVector AboveOffset = PortalHit.Normal.Rotation().RotateVector(FVector::UpVector);
	FVector AbovePosition = Location + AboveOffset * 140.0;

	FVector BelowOffset = PortalHit.Normal.Rotation().RotateVector(FVector::DownVector);
	FVector BelowPosition = Location + BelowOffset * 140.0;

	FHitResult FirstOffsetHit;
	FHitResult SecondOffsetHit;

	DrawDebugLine(GetWorld(), AbovePosition, (AbovePosition - PortalHit.Normal * 200), FColor::Red, false, 8, 0, 1);
	DrawDebugLine(GetWorld(), BelowPosition, (BelowPosition - PortalHit.Normal * 200), FColor::Red, false, 8, 0, 1);
	DrawDebugLine(GetWorld(), RightPosition, (RightPosition - PortalHit.Normal * 200), FColor::Red, false, 8, 0, 1);
	DrawDebugLine(GetWorld(), LeftPosition, (LeftPosition - PortalHit.Normal * 200), FColor::Red, false, 8, 0, 1);

	//Left and right positions
	GetWorld()->LineTraceSingleByChannel(FirstOffsetHit, LeftPosition, (LeftPosition - PortalHit.Normal * 200),
	                                     ECollisionChannel::ECC_GameTraceChannel1,
	                                     Params);
	GetWorld()->LineTraceSingleByChannel(SecondOffsetHit, RightPosition, (RightPosition - PortalHit.Normal * 200),
	                                     ECollisionChannel::ECC_GameTraceChannel1,
	                                     Params);

	ShiftPortalPositionIfNeeded(FirstOffsetHit, SecondOffsetHit, PortalHit, RightPosition, LeftPosition, RightOffset,
	                            LeftOffset);

	//Upper and lower positions
	GetWorld()->LineTraceSingleByChannel(FirstOffsetHit, AbovePosition, (AbovePosition - PortalHit.Normal * 200),
	                                     ECollisionChannel::ECC_GameTraceChannel1,
	                                     Params);
	GetWorld()->LineTraceSingleByChannel(SecondOffsetHit, BelowPosition, (BelowPosition - PortalHit.Normal * 200),
	                                     ECollisionChannel::ECC_GameTraceChannel1,
	                                     Params);

	ShiftPortalPositionIfNeeded(FirstOffsetHit, SecondOffsetHit, PortalHit, BelowPosition, AbovePosition, BelowOffset,
	                            AboveOffset);
}

void APortalGun::ShiftPortalPosition(FHitResult& HitToShift, FVector& Location,
                                     const FVector& DirectionVector) const
{
	bool IsFixed = false;
	int Counter = 0;
	FHitResult Hit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	do
	{
		Counter++;
		Location = Location + DirectionVector * 20.0;
		GetWorld()->LineTraceSingleByChannel(Hit, Location, (Location - HitToShift.Normal * 200),
		                                     ECollisionChannel::ECC_GameTraceChannel1,
		                                     Params);

		if (Hit.GetActor() != nullptr && Hit.GetActor()->IsA<APortalWall>()) IsFixed = true;
		if (Counter > 30) IsFixed = true;
	}
	while (!IsFixed);

	HitToShift.Location = HitToShift.Location + Counter * 20 * DirectionVector;
}

void APortalGun::ShiftPortalPositionIfNeeded(FHitResult& FirstOffsetHit, FHitResult& SecondOffsetHit,
                                             FHitResult& PortalHit, FVector& SecondHitPosition,
                                             FVector& FirstHitPosition,
                                             FVector& SecondOffset, FVector& FirstOffset)
{
	if (FirstOffsetHit.GetActor() != nullptr && FirstOffsetHit.GetActor()->IsA<APortalWall>())
	{
		if (SecondOffsetHit.GetActor() == nullptr || SecondOffsetHit.GetActor() != nullptr && !SecondOffsetHit.
			GetActor()->IsA<APortalWall>())
			ShiftPortalPosition(PortalHit, SecondHitPosition, FirstOffset);
	}
	else ShiftPortalPosition(PortalHit, FirstHitPosition, SecondOffset);
}

AController* APortalGun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
		return nullptr;
	return OwnerPawn->GetController();
}
