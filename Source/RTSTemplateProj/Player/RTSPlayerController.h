// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "RTSPlayerController.generated.h"

UCLASS()
class RTSTEMPLATEPROJ_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

	ARTSPlayerController();

protected:
	// Called at the Start of The Game
	virtual void BeginPlay() override;
	// Called to bind functionality to input
	virtual void SetupInputComponent() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//Get Cached RTS Camera Pawn
	class ARTSCameraPawn* GetRTSCameraPawn();

	//Input Related Methods
	void OnMoveCameraInput(const FInputActionValue& InVal);
	void OnResetCameraSettingsInput(const FInputActionValue& InVal);
	void OnPrimaryActionStart(const FInputActionValue& InVal);
	void OnPrimaryActionTriggered(const FInputActionValue& InVal);
	void OnPrimaryActionEnd(const FInputActionValue& InVal);
	void OnSecondaryActionStart(const FInputActionValue& InVal);
	void OnSecondaryActionTriggered(const FInputActionValue& InVal);
	void OnSecondaryActionEnd(const FInputActionValue& InVal);
	void OnCameraZoomInput(const FInputActionValue& InVal);
	void OnRotateCameraInput(const FInputActionValue& InVal);
	void OnChangeCameraElevationInput(const FInputActionValue& InVal);
	void OnStartRotationMode(const FInputActionValue& InVal);
	void OnEndRotationMode(const FInputActionValue& InVal);

	//UI Methods
	void CreateMapWidget();

protected:
	//Input Config
	UPROPERTY(EditDefaultsOnly, Category = "Player Input")
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, Category = "Player Input")
	class UPlayerInputConfig* PlayerInputConfig;

	//References
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<class URTSMapWidget> MapWidgetTemplate;

private:
	//Cached Referenced to created Objects
	TObjectPtr<class ARTSCameraPawn> CachedCameraPawn;
	TWeakObjectPtr<class URTSMapWidget> LocalMapWidget;

	//UI Related Variables
	bool bIsHoveringOverUI;
	bool bIsHoveringOverMap;
};
