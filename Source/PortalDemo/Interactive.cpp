// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive.h"

// Sets default values
AInteractive::AInteractive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractive::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AInteractive::GetIsActive() const
{
	return this->IsActive;
}

int AInteractive::GetInteractionId() const
{
	return this->InteractionId;
}

void AInteractive::ReactToInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("ReactToInteraction works from the AInteractive"));
}
