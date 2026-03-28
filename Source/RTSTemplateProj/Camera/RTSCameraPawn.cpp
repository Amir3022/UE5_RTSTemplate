// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCameraPawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "RTSTemplateProj/Core/WorldSettings/RTSWorldSettings.h"

// Sets default values
ARTSCameraPawn::ARTSCameraPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//initializing components
	RootScene = CreateDefaultSubobject<USceneComponent>(FName("Root Scene"));
	RootComponent = RootScene;
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(FName("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	RTSCamera = CreateDefaultSubobject<UCameraComponent>(FName("RTSCamera"));
	RTSCamera->SetupAttachment(CameraArm);

	//Initializing variables
	InitialCameraHeight = 5000.0f;
	MaxCameraHeight = 10000.0f;
	MinCameraHeight = 2000.0f;
	InitialCameraAngle = 45.0f;
	MaxCameraAngle = 90.0f;
	MinCameraAngle = 20.0f;
	CameraMovementSpeed = 1000.0f;
	bUsePercentZoom = false;
	ZoomAmount = 100.0f;
	ZoomPercentage = 0.25f;
	ElevationRotationRate = 45.0f;
	AxisRotationRate = 45.0f;
	MouseDistanceFraction = 100.0f;
	InitialMouseLocation = FVector2D(0.0f);
	bCameraRotationControlMode = false;
	ScreenEdgeTolerance = 40.0f;
}

// Called when the game starts or when spawned
void ARTSCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	//Initialize the Camera setting at the start of the game to the default values
	ResetCameraSettings();
	//Disable spring arm collision test, cache initial spring arm rotation
	if (CameraArm)
	{
		CameraArm->bDoCollisionTest = false;
		CurrentCameraRotation = CameraArm->GetComponentRotation();
	}
}

void ARTSCameraPawn::OnConstruction(const FTransform& InTransform)
{
	Super::OnConstruction(InTransform);
	ResetCameraSettings();
	//Disable spring arm collision test, cache initial spring arm rotation
	if (CameraArm)
	{
		CameraArm->bDoCollisionTest = false;
		CurrentCameraRotation = CameraArm->GetComponentRotation();
	}
}

// Called every frame
void ARTSCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

APlayerController* ARTSCameraPawn::GetPlayerController()
{
	if (!GetController())
		return nullptr;

	if (!CachedPlayerController)
		CachedPlayerController = Cast<APlayerController>(GetController());

	return CachedPlayerController;
}

///Camera Control Related Methods
//Reset Camera Settings to Default Values
void ARTSCameraPawn::ResetCameraSettings()
{
	if (!CameraArm || !RTSCamera)
		return;

	SetCameraHeight(InitialCameraHeight);
	SetCameraElevation(InitialCameraAngle);
	SetCameraAxisRotation(90.0f);
}

//Changing Camera Zoom with delta Amount
void ARTSCameraPawn::ChangeCameraZoom(float DeltaHeight)
{
	if (!CameraArm || !RTSCamera)
		return;

	if (bUsePercentZoom)
	{
		SetCameraHeight(GetCameraHeight() * (DeltaHeight * ZoomPercentage + 1));
	}
	else
	{
		SetCameraHeight(GetCameraHeight() + DeltaHeight * ZoomAmount);
	}
}

//Changing spring arm length
void ARTSCameraPawn::SetCameraHeight(float InHeight)
{
	if (!CameraArm || !RTSCamera)
		return;

	float CurrentCameraHeight = FMath::Clamp(InHeight, MinCameraHeight, MaxCameraHeight);
	CameraArm->TargetArmLength = CurrentCameraHeight;
}

//Get Camera Camera Zoom value
float ARTSCameraPawn::GetCameraHeight()
{
	if (!CameraArm || !RTSCamera)
		return -1.0f;

	return CameraArm->TargetArmLength;
}

//Change the Camera Elevation Based on Input
void ARTSCameraPawn::ChangeCameraElevation(float InputVal)
{
	if (!CameraArm || !RTSCamera)
		return;

	float NewElevationValue = -CurrentCameraRotation.Pitch + InputVal * GetWorld()->GetDeltaSeconds() * ElevationRotationRate;
	SetCameraElevation(NewElevationValue);
}

//Set the angle between the spring arm and the surface ground
void ARTSCameraPawn::SetCameraElevation(float InElevation)
{
	if (!CameraArm || !RTSCamera)
		return;

	CurrentCameraRotation = CameraArm->GetComponentRotation();
	CurrentCameraRotation.Pitch = FMath::ClampAngle(-InElevation, -MaxCameraAngle, -MinCameraAngle);
	CurrentCameraRotation.Roll = 0.0f;
	CameraArm->SetWorldRotation(CurrentCameraRotation);
}

//Change Camera Axis Rotation based on Input
void ARTSCameraPawn::ChangeCameraAxisRotation(float InputVal)
{
	if (!CameraArm || !RTSCamera)
		return;

	float NewAxisRotation = CurrentCameraRotation.Yaw + InputVal * GetWorld()->GetDeltaSeconds() * AxisRotationRate;
	SetCameraAxisRotation(NewAxisRotation);
}

//Set camera rotation around the Z-Axis
void ARTSCameraPawn::SetCameraAxisRotation(float InRotation)
{
	if (!CameraArm || !RTSCamera)
		return;
	CurrentCameraRotation = CameraArm->GetComponentRotation();
	CurrentCameraRotation.Yaw = InRotation;
	CurrentCameraRotation.Roll = 0.0f;
	CameraArm->SetWorldRotation(CurrentCameraRotation);
}

//Toggle the use of the Camera rotation mode, which allows to rotate the camera in Z-Axis, and set elevation using mouse cursor position
void ARTSCameraPawn::ToggleCameraRotationMode(bool InState)
{
	if (InState)
	{
		if (GetPlayerController())
		{
			GetPlayerController()->GetMousePosition(InitialMouseLocation.X, InitialMouseLocation.Y);
			bCameraRotationControlMode = true;
			GetPlayerController()->SetShowMouseCursor(false);
		}
	}
	else
	{
		bCameraRotationControlMode = false;
		GetPlayerController()->SetShowMouseCursor(true);
	}
}

//Setting camera rotation based on current mouse cursor position compared to the initial cursor position when the toggle camera rotation input was pressed
void ARTSCameraPawn::UpdateCameraRotationMode(float DeltaSeconds)
{
	if (GetPlayerController() && bCameraRotationControlMode)
	{
		FVector2D CurrentMousePos;
		GetPlayerController()->GetMousePosition(CurrentMousePos.X, CurrentMousePos.Y);
		FVector2D MouseDirection = CurrentMousePos - InitialMouseLocation;
		float xVal = MouseDirection.X;
		float yVal = MouseDirection.Y;
		ChangeCameraAxisRotation(xVal);
		ChangeCameraElevation(yVal);
		//Reset the Mouse Location To the Initial Location After Applying Rotation
		GetPlayerController()->SetMouseLocation(InitialMouseLocation.X, InitialMouseLocation.Y);
	}
}

//Move the camera as the mouse cursor goes to the borders of the screen
void ARTSCameraPawn::MoveCameraWithMouseCursor(float DeltaTime, FVector2D CurrentMousePos, FVector2D CurrentViewportSize)
{
	if (!CameraArm || !RTSCamera)
		return;

	//Return from this function if Rotation Control Mode is used
	if (bCameraRotationControlMode)
		return;

	//Get Current mouse position in relation with the viewport dimensions to determine where the camera should move
	FVector2D CameraMoveDir = FVector2D(0.0f);
	if (CurrentMousePos.X < ScreenEdgeTolerance)
		CameraMoveDir.X = -1;
	else if (CurrentMousePos.X > (CurrentViewportSize.X - ScreenEdgeTolerance))
		CameraMoveDir.X = 1;
	if (CurrentMousePos.Y < ScreenEdgeTolerance)
		CameraMoveDir.Y = 1;
	else if (CurrentMousePos.Y > (CurrentViewportSize.Y - ScreenEdgeTolerance))
		CameraMoveDir.Y = -1;

	CameraMoveDir.Normalize();
	MoveCameraInDirection(DeltaTime, CameraMoveDir);
}

//Move the Camera based on the Input Direction
void ARTSCameraPawn::MoveCameraInDirection(float DeltaTime, const FVector2D& MoveDir)
{
	//Get current camera forward direction, and get right vector from in (Both with Z set to zero to get directions in the XY Plane)
	FVector YDirection = FVector(CameraArm->GetForwardVector().X, CameraArm->GetForwardVector().Y, 0.0f);
	FVector XDirection = -1 * YDirection.Cross(FVector::UpVector);
	//Multiply the Forward and Right Vectors of the camera with X and Y direction base on mouse pos to get move direction
	FVector MoveDirection = (YDirection * MoveDir.Y) + (XDirection * MoveDir.X);
	MoveDirection.Normalize();
	FVector MoveTarget = MoveDirection * CameraMovementSpeed * DeltaTime;
	MoveTarget += GetActorLocation();
	//Limit Camera Movement beyond the Edges of the Map if setup in RTS World Settings
	if (GetWorld())
	{
		if (ARTSWorldSettings* RTSWorldSettings = Cast<ARTSWorldSettings>(GetWorld()->GetWorldSettings()))
		{
			FVector MinMapLimit = RTSWorldSettings->GetMapOrigin() - RTSWorldSettings->GetMapBounds() / 2.0f;
			FVector MaxMapLimit = RTSWorldSettings->GetMapOrigin() + RTSWorldSettings->GetMapBounds() / 2.0f;
			MoveTarget.X = FMath::Clamp(MoveTarget.X, MinMapLimit.X, MaxMapLimit.X);
			MoveTarget.Y = FMath::Clamp(MoveTarget.Y, MinMapLimit.Y, MaxMapLimit.Y);
		}
	}

	SetActorLocation(MoveTarget);
}

//Set The Camera Location based on the location calculated from the Minimap Widget
void ARTSCameraPawn::SetRTSCameraPawnLocation(FVector InLocation)
{
	InLocation.Z = GetActorLocation().Z;
	//Limit Camera Movement beyond the Edges of the Map if setup in RTS World Settings
	if (GetWorld())
	{
		if (ARTSWorldSettings* RTSWorldSettings = Cast<ARTSWorldSettings>(GetWorld()->GetWorldSettings()))
		{
			FVector MinMapLimit = RTSWorldSettings->GetMapOrigin() - RTSWorldSettings->GetMapBounds() / 2.0f;
			FVector MaxMapLimit = RTSWorldSettings->GetMapOrigin() + RTSWorldSettings->GetMapBounds() / 2.0f;
			InLocation.X = FMath::Clamp(InLocation.X, MinMapLimit.X, MaxMapLimit.X);
			InLocation.Y = FMath::Clamp(InLocation.Y, MinMapLimit.Y, MaxMapLimit.Y);
		}
	}

	SetActorLocation(InLocation);
}

