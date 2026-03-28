// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "InputMappingContext.h"
#include "Input/PlayerInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "RTSTemplateProj/Camera/RTSCameraPawn.h"
#include "RTSTemplateProj/UI/MapUI/RTSMapWidget.h"

ARTSPlayerController::ARTSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	//Initializing Variables
	bIsHoveringOverUI = false;
	bIsHoveringOverMap = false;
}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		//Set the Mouse cursor to visible
		SetShowMouseCursor(true);
		if (GEngine && GEngine->GameViewport)
		{
			//Force the mouse to be locked within the borders of the game viewport
			GEngine->GameViewport->SetMouseLockMode(EMouseLockMode::LockAlways);
		}

		//Create the Map Widget UI
		CreateMapWidget();
	}
}

//Setup Input Component Using Enhanced Input Component
void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (EnhancedInputSubsystem && InputComponent)
	{
		EnhancedInputSubsystem->ClearAllMappings();
		EnhancedInputSubsystem->AddMappingContext(InputMapping, 0);
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		if (EIC)
		{
			//Setup Of Various Input Configs from Player Input Configs to Call certain methods
			EIC->BindAction(PlayerInputConfig->CameraMoveInput, ETriggerEvent::Triggered, this, &ARTSPlayerController::OnMoveCameraInput);
			EIC->BindAction(PlayerInputConfig->ZoomInput, ETriggerEvent::Started, this, &ARTSPlayerController::OnCameraZoomInput);
			EIC->BindAction(PlayerInputConfig->RotateInput, ETriggerEvent::Triggered, this, &ARTSPlayerController::OnChangeCameraElevationInput);
			EIC->BindAction(PlayerInputConfig->AxisRotateInput, ETriggerEvent::Triggered, this, &ARTSPlayerController::OnRotateCameraInput);
			EIC->BindAction(PlayerInputConfig->CameraRotationSwitchMode, ETriggerEvent::Started, this, &ARTSPlayerController::OnStartRotationMode);
			EIC->BindAction(PlayerInputConfig->CameraRotationSwitchMode, ETriggerEvent::Completed, this, &ARTSPlayerController::OnEndRotationMode);
			EIC->BindAction(PlayerInputConfig->ResetCameraSettings, ETriggerEvent::Started, this, &ARTSPlayerController::OnResetCameraSettingsInput);
			EIC->BindAction(PlayerInputConfig->PrimaryPressInput, ETriggerEvent::Started, this, &ARTSPlayerController::OnPrimaryActionStart);
			EIC->BindAction(PlayerInputConfig->PrimaryPressInput, ETriggerEvent::Triggered, this, &ARTSPlayerController::OnPrimaryActionTriggered);
			EIC->BindAction(PlayerInputConfig->PrimaryPressInput, ETriggerEvent::Completed, this, &ARTSPlayerController::OnPrimaryActionEnd);
			EIC->BindAction(PlayerInputConfig->SecondaryPressInput, ETriggerEvent::Started, this, &ARTSPlayerController::OnSecondaryActionStart);
			EIC->BindAction(PlayerInputConfig->SecondaryPressInput, ETriggerEvent::Triggered, this, &ARTSPlayerController::OnSecondaryActionTriggered);
			EIC->BindAction(PlayerInputConfig->SecondaryPressInput, ETriggerEvent::Completed, this, &ARTSPlayerController::OnSecondaryActionEnd);
		}
	}
}

//Tick Function
void ARTSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle Moving Camera With moving Mouse Cursor
	if (GetRTSCameraPawn())
	{
		//Get the Viewport Size
		FVector2D CurrentViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(CurrentViewportSize);
		}
		//Get Current Mouse Position On Screen
		FVector2D CurrentMousePos;
		if (GetMousePosition(CurrentMousePos.X, CurrentMousePos.Y))
		{
			GetRTSCameraPawn()->MoveCameraWithMouseCursor(DeltaTime, CurrentMousePos, CurrentViewportSize);
		}

		//Update Camera Pawn rotation if the Rotation Mode Enabled
		GetRTSCameraPawn()->UpdateCameraRotationMode(DeltaTime);

		//Check If Currently Hovering Over Any UI
		UUserWidget* LastHoveredWidget = nullptr;	//Pointer to the current hovered over UI Element, used to set focus to it when hovering
		if (LocalMapWidget.IsValid())
		{
			bIsHoveringOverMap = LocalMapWidget->IsHovered();
			if (bIsHoveringOverMap)
			{
				LastHoveredWidget = LocalMapWidget.Get();
			}
		}

		bool bNewHoveringState = bIsHoveringOverMap;	//Checking if any of the UI Elements is currently hovered over
		if (bIsHoveringOverUI != bNewHoveringState)
		{
			bIsHoveringOverUI = bNewHoveringState;
			if (LastHoveredWidget && bIsHoveringOverUI)
			{
				//Lock mouse input to Hovered UI only
				FInputModeUIOnly InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);	//Lock Mouse Input to the screen
				//Set Focus to UI element to mouse inputs is only consumed by that
				if (LastHoveredWidget)
					InputMode.SetWidgetToFocus(LastHoveredWidget->TakeWidget());
				SetInputMode(InputMode);
			}
			else
			{
				//Set Mouse input to game and UI, with the UI set to null as no UI element is currently hovered over
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);	//Lock Mouse Input to the screen
				InputMode.SetHideCursorDuringCapture(false);	//Prevent mouse from hiding when interacting with UI elements
				SetInputMode(InputMode);
			}
		}
		//Update the Hovered Widget Cursor
		if (LastHoveredWidget && LastHoveredWidget->GetCursor() != CurrentMouseCursor)
		{
			LastHoveredWidget->SetCursor(CurrentMouseCursor);
		}
	}
}

//Lazy Caching of the Controlled Camera Pawn
ARTSCameraPawn* ARTSPlayerController::GetRTSCameraPawn()
{
	if (!CachedCameraPawn)
	{
		CachedCameraPawn = Cast<ARTSCameraPawn>(GetPawn());
	}
	return CachedCameraPawn;
}


//Input Related Methods
void ARTSPlayerController::OnMoveCameraInput(const FInputActionValue& InVal)
{
	FVector2D InputVal = InVal.Get<FVector2D>();
	//Handle Input for Camera Pawn
	if (GetRTSCameraPawn() && GetWorld())
	{
		GetRTSCameraPawn()->MoveCameraInDirection(GetWorld()->GetDeltaSeconds(), InputVal);
	}
}

void ARTSPlayerController::OnResetCameraSettingsInput(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
	//Handle Input for Controlled Pawn
	if (GetRTSCameraPawn())
	{
		if (InputVal)
			GetRTSCameraPawn()->ResetCameraSettings();
	}
}

void ARTSPlayerController::OnPrimaryActionStart(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
}

void ARTSPlayerController::OnPrimaryActionTriggered(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
}

void ARTSPlayerController::OnPrimaryActionEnd(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
}

void ARTSPlayerController::OnSecondaryActionStart(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
	//TODO - Add Code Handling Here
}

void ARTSPlayerController::OnSecondaryActionTriggered(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
	//TODO - Add Code Handling Here
}

void ARTSPlayerController::OnSecondaryActionEnd(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
}

void ARTSPlayerController::OnCameraZoomInput(const FInputActionValue& InVal)
{
	float ScrollValue = InVal.Get<float>();
	//Handle Input For Controlled Pawn
	if (GetRTSCameraPawn())
	{
		GetRTSCameraPawn()->ChangeCameraZoom(ScrollValue);
	}
}

void ARTSPlayerController::OnRotateCameraInput(const FInputActionValue& InVal)
{
	float InputAxisValue = InVal.Get<float>();
	//Handle Input For Controlled Pawn
	if (GetRTSCameraPawn())
	{
		GetRTSCameraPawn()->ChangeCameraAxisRotation(InputAxisValue);
	}
}

void ARTSPlayerController::OnChangeCameraElevationInput(const FInputActionValue& InVal)
{
	float InputAxisValue = InVal.Get<float>();
	//Handle Input For Controlled Pawn
	if (GetRTSCameraPawn())
	{
		GetRTSCameraPawn()->ChangeCameraElevation(InputAxisValue);
	}
}

void ARTSPlayerController::OnStartRotationMode(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
	//Hanlde Input For Controlled Pawn
	if (GetRTSCameraPawn())
	{
		GetRTSCameraPawn()->ToggleCameraRotationMode(InputVal);
	}
}

void ARTSPlayerController::OnEndRotationMode(const FInputActionValue& InVal)
{
	bool InputVal = InVal.Get<bool>();
	//Hanlde Input For Controlled Pawn
	if (GetRTSCameraPawn())
	{
		GetRTSCameraPawn()->ToggleCameraRotationMode(InputVal);
	}
}

//Create The Map UI from Template and add it To Player Screen
void ARTSPlayerController::CreateMapWidget()
{
	if (TSubclassOf<URTSMapWidget> MapWidgetClass = MapWidgetTemplate.LoadSynchronous())
	{
		LocalMapWidget = CreateWidget<URTSMapWidget>(this, MapWidgetClass);
		if (LocalMapWidget.IsValid())
		{
			LocalMapWidget->AddToViewport();
		}
	}
}
