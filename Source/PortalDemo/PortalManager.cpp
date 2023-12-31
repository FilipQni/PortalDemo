// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "PlayerCharacter.h"
#include "Portal.h"
#include "Engine/SceneCapture2D.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortalManager::APortalManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalEnter = nullptr;
	PortalExit = nullptr;
}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
	
	SpawnParams.bNoFail = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<AActor*> SceneCaptureActors;
	UGameplayStatics::GetAllActorsOfClass(this, ASceneCapture2D::StaticClass(), SceneCaptureActors);
	if(SceneCaptureActors.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASceneCapture2D number: %d"), SceneCaptureActors.Num());
		PortalEnterSceneCapture = Cast<ASceneCapture2D>(SceneCaptureActors[0]);
		PortalExitSceneCapture = Cast<ASceneCapture2D>(SceneCaptureActors[1]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ASceneCapture2D were not found"));
	}
}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PortalEnter && PortalExit)
	{
		MoveAndRotateSceneCapture(PortalExitSceneCapture, PortalEnterSceneCapture, PortalEnter, PortalExit);
		MoveAndRotateSceneCapture(PortalEnterSceneCapture, PortalExitSceneCapture, PortalExit, PortalEnter);
	}
}

void APortalManager::CreatePortalEnter(const FHitResult& Hit)
{
	if(PortalEnter != nullptr)
		PortalEnter->Destroy();

	FRotator Rotacja = Hit.ImpactNormal.Rotation();
	PortalEnter = GetWorld()->SpawnActor<APortal>(PortalEnterClass, Hit.Location, Rotacja, SpawnParams);
	CreatePortal();
	PortalEnter->SetupRightVector();
	PortalEnter->OnOverlapDelegate.BindUObject(this, &APortalManager::TeleportTargetToExit);
}

void APortalManager::CreatePortalExit(const FHitResult& Hit)
{
	if(PortalExit != nullptr)
		PortalExit->Destroy();
	
	FRotator Rotacja = Hit.ImpactNormal.Rotation();
	PortalExit = GetWorld()->SpawnActor<APortal>(PortalExitClass, Hit.Location, Rotacja, SpawnParams);
	CreatePortal();
	PortalExit->SetupRightVector();
	PortalExit->OnOverlapDelegate.BindUObject(this, &APortalManager::TeleportTargetToEnter);
}

void APortalManager::CreatePortal()
{
	if(PortalEnter && PortalExit)
	{
		PortalEnter->Activate();
		PortalExit->Activate();
		
		FVector PortalLocationEnter = PortalEnter->GetActorLocation();
		FRotator PortalRotationEnter = PortalEnter->GetActorRotation();

		FVector NewPortalEnterSceneCaptureLocation = PortalLocationEnter + PortalRotationEnter.Vector() * 100.0f;
		
		PortalEnterSceneCapture->SetActorLocation(NewPortalEnterSceneCaptureLocation);
		PortalEnterSceneCapture->SetActorRotation(PortalRotationEnter);
		PortalEnter->SetScreenCaptureLocation(NewPortalEnterSceneCaptureLocation);
		
		FVector PortalLocationExit = PortalExit->GetActorLocation();
		FRotator PortalRotationExit = PortalExit->GetActorRotation();

		FVector NewPortalExitSceneCaptureLocation = PortalLocationExit + PortalRotationExit.Vector() * 100.0f;
		
		PortalExitSceneCapture->SetActorLocation(NewPortalExitSceneCaptureLocation);
		PortalExitSceneCapture->SetActorRotation(PortalRotationExit);
		PortalExit->SetScreenCaptureLocation(NewPortalExitSceneCaptureLocation);
	}
}

void APortalManager::TeleportTargetToExit(AActor* ActorToTeleport)
{
	Teleport(ActorToTeleport, PortalEnter, PortalExit);
}

void APortalManager::TeleportTargetToEnter(AActor* ActorToTeleport)
{
	Teleport(ActorToTeleport, PortalExit, PortalEnter);
}

void APortalManager::Teleport(AActor* ActorToTeleport, APortal* EntryPortal, APortal* ExitPortal)
{
	PlayerCharacter = Cast<APlayerCharacter>(ActorToTeleport);
	ActorToTeleport->SetActorLocation(ExitPortal->GetDefaultScreenCaptureLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	RotateCharacterAfterTeleportation(ActorToTeleport, EntryPortal, ExitPortal);
}

void APortalManager::MoveAndRotateSceneCapture(ASceneCapture2D* PortalSceneCaptureToRotate, ASceneCapture2D* SecondPortalSceneCapture, APortal* MainPortal, APortal* SecondPortal)
{
	//Rotation part
	FVector VectorToCharacter = PlayerCharacter->GetActorLocation() - SecondPortalSceneCapture->GetActorLocation();
	VectorToCharacter.Normalize();
	float DotProduct = FVector::DotProduct(MainPortal->GetRightVector(), VectorToCharacter) * 0.8;
	float AngleInRadians = FMath::RadiansToDegrees(DotProduct);
	FRotator PortalEnterSceneCaptureRotation = SecondPortal->GetActorForwardVector().Rotation();
	PortalEnterSceneCaptureRotation.Yaw += AngleInRadians;
	PortalSceneCaptureToRotate->SetActorRotation(PortalEnterSceneCaptureRotation);

	//TODO: Ruch SceneCapture
	/*FVector DistanceToCharacter = PlayerCharacter->GetActorLocation() - MainPortal->GetDefaultScreenCaptureLocation();
	float DotProduct2 = FVector::DotProduct(MainPortal->GetActorForwardVector(), SecondPortal->GetActorForwardVector());
	float AngleInRadians2 = FMath::Acos(DotProduct2);
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians2);
	DistanceToCharacter = DistanceToCharacter.RotateAngleAxisRad(AngleInDegrees, FVector::ZAxisVector);
	PortalSceneCaptureToRotate->SetActorLocation(SecondPortal->GetDefaultScreenCaptureLocation() + DistanceToCharacter);
	UE_LOG(LogTemp, Warning, TEXT("Kąt pomiędzy portalami: %f, forwardWektorEnter: (%f, %f, %f), forwardWektorExit: (%f, %f, %f)"),
		AngleInDegrees, MainPortal->GetActorForwardVector().X, MainPortal->GetActorForwardVector().Y,MainPortal->GetActorForwardVector().Z,
		SecondPortal->GetActorForwardVector().X,SecondPortal->GetActorForwardVector().Y,SecondPortal->GetActorForwardVector().Z);*/
}

void APortalManager::RotateCharacterAfterTeleportation(AActor* Player, APortal* EntryPortal, APortal* ExitPortal)
{
	float DotProductOfPlayerForwardAndPortalForward = FVector::DotProduct(EntryPortal->GetActorForwardVector(), Player->GetActorForwardVector());
	float DotProductOfPlayerForwardAndPortalRight = FVector::DotProduct(EntryPortal->GetRightVector(), Player->GetActorForwardVector());
	float AngleInRadians = FMath::Acos(DotProductOfPlayerForwardAndPortalRight);
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

	float NewAngle = 180 - AngleInDegrees;
	if(DotProductOfPlayerForwardAndPortalForward < 0) NewAngle = -NewAngle;
	PlayerCharacter->Rotate(NewAngle);

}
