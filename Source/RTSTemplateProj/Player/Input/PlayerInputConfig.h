// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "PlayerInputConfig.generated.h"


UCLASS()
class RTSTEMPLATEPROJ_API UPlayerInputConfig : public UDataAsset
{
	GENERATED_BODY()

//Setting up the Inputs to be used by the RTSPlayerController
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* CameraMoveInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* ZoomInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* RotateInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* AxisRotateInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* CameraRotationSwitchMode;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* PrimaryPressInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* SecondaryPressInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputAction* ResetCameraSettings;
};
