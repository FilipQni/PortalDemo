// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickableActor.generated.h"

class APlayerCharacter;

UCLASS()
class PORTALDEMO_API APickableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(const APlayerCharacter* HoldingPlayer);
	void Drop();
	void Throw(FVector CameraForwardVector);

protected:
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	UStaticMeshComponent* MeshComponent;
	
	bool bGravity;
	bool bHeld;
};