// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class APickableActor;
class AInteractiveActor;
class APortalGun;

UCLASS()
class PORTALDEMO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void RotateCharacter(float Angle) const;
	void RotateVelocity(const FVector& Rotation);
	USceneComponent* GetHoldingItemComponent() const;
	
	
private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void LookRight(float AxisValue);
	void CreatePortalEnter();
	void CreatePortalExit();
	void Interact();
	void Drop();
	void Throw();

	UPROPERTY(EditAnywhere, Category="Mesh")
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Mesh")
	TSubclassOf<APortalGun> GunClass;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category="Mesh")
	USceneComponent* HoldingComponent;
	
	UPROPERTY()
	APortalGun* PortalGun;
	
	FCollisionQueryParams Params;
	APickableActor* HeldItem;
	float InteractionDistance = 300.0f;
};
