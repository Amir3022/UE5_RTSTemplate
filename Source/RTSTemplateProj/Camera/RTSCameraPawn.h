// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RTSCameraPawn.generated.h"

UCLASS()
class RTSTEMPLATEPROJ_API ARTSCameraPawn : public APawn
{
	GENERATED_BODY()

	// Sets default values for this pawn's properties
	ARTSCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	//Camera Control Related Functions
	void ChangeCameraZoom(float DeltaHeight);
	void SetCameraHeight(float InHeight);
	float GetCameraHeight();
	void ChangeCameraElevation(float InputVal);
	void SetCameraElevation(float InElevation);
	void ChangeCameraAxisRotation(float InputVal);
	void SetCameraAxisRotation(float InRotation);
	void ResetCameraSettings();
	void ToggleCameraRotationMode(bool InState);
	void UpdateCameraRotationMode(float DeltaSeconds);
	void MoveCameraWithMouseCursor(float DeltaTime, FVector2D CurrentMousePos, FVector2D CurrentViewportSize);
	void MoveCameraInDirection(float DeltaTime, const FVector2D& MoveDir);
	void SetRTSCameraPawnLocation(FVector InLocation);

private:
	class APlayerController* GetPlayerController();

	//MiniMap Methods
	void DrawCameraViewOnMiniMap();

	//Class Properties and variables
protected:
	//Camera editable variables
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float InitialCameraHeight;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MaxCameraHeight;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MinCameraHeight;
	UPROPERTY(EditAnywhere, Category = "Camera Settings", meta = (ClampMin = "0", ClampMax = "90"))
	float InitialCameraAngle;
	UPROPERTY(EditAnywhere, Category = "Camera Settings", meta = (ClampMin = "0", ClampMax = "90"))
	float MaxCameraAngle;
	UPROPERTY(EditAnywhere, Category = "Camera Settings", meta = (ClampMin = "0", ClampMax = "90"))
	float MinCameraAngle;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraMovementSpeed;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	bool bUsePercentZoom;
	UPROPERTY(EditAnywhere, Category = "Camera Settings", meta = (EditCondition = "!bUsePercentZoom"))
	float ZoomAmount;
	UPROPERTY(EditAnywhere, Category = "Camera Settings", meta = (EditCondition = "bUsePercentZoom"))
	float ZoomPercentage;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float ElevationRotationRate;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float AxisRotationRate;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MouseDistanceFraction;
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float ScreenEdgeTolerance;

protected:
	//Pawn Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* RTSCamera;

private:
	//Cached Player Controller
	TObjectPtr<class APlayerController> CachedPlayerController;
	//Camera Internal Variables
	FRotator CurrentCameraRotation;
	FVector2D InitialMouseLocation;
	bool bCameraRotationControlMode;
};
