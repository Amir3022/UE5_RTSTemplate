// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSMapWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "RTSTemplateProj/Interfaces/RTSMiniMapObjectInterface.h"
#include "RTSTemplateProj/Core/WorldSettings/RTSWorldSettings.h"
#include "RTSTemplateProj/Camera/RTSCameraPawn.h"

URTSMapWidget::URTSMapWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void URTSMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Set the MiniMap Image Texture
	InitializeMiniMapRenderTarget();
}

void URTSMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateMiniMapRenderTarget(InDeltaTime);
}

FReply URTSMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//Make sure the Left Mouse Button is Pressed
	if (MapImage && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//Get Mouse Screen Position
		FVector2D MouseScreenPosition = InMouseEvent.GetScreenSpacePosition();
		//Convert the Mouse screen position to the Map Image local position
		FGeometry MapImageGeo = MapImage->GetCachedGeometry();
		FVector2D MouseLocalPos = MapImageGeo.AbsoluteToLocal(MouseScreenPosition);
		//Normalize the Mouse position to the bounds of the Map Image
		FVector2D MapImageSize = MapImageGeo.GetLocalSize();
		MouseLocalPos.X = FMath::Clamp(MapImageSize.X > 0.0f ? MouseLocalPos.X / MapImageSize.X : 0.0f, 0.0f, 1.0f);
		MouseLocalPos.Y = FMath::Clamp(MapImageSize.Y > 0.0f ? MouseLocalPos.Y / MapImageSize.Y : 0.0f, 0.0f, 1.0f);
		//Set the Camera Pawn location according to the pressed Map Location
		ARTSWorldSettings* RTSWorldSettings = Cast<ARTSWorldSettings>(GetWorld()->GetWorldSettings());
		if (GetCachedRTSCameraPawn() && RTSWorldSettings)
		{
			FVector TopLeftLoc = RTSWorldSettings->GetMapOrigin() -
				RTSWorldSettings->GetMapBounds().X * RTSWorldSettings->GetMapXDirectionVec() / 2.0f +
				RTSWorldSettings->GetMapBounds().Y * RTSWorldSettings->GetMapYDirectionVec() / 2.0f;

			FVector WorldLocation = TopLeftLoc +
				MouseLocalPos.X * RTSWorldSettings->GetMapBounds().X * RTSWorldSettings->GetMapXDirectionVec() +
				MouseLocalPos.Y * -RTSWorldSettings->GetMapBounds().Y * RTSWorldSettings->GetMapYDirectionVec();

			GetCachedRTSCameraPawn()->SetRTSCameraPawnLocation(WorldLocation);
		}
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URTSMapWidget::InitializeMiniMapRenderTarget()
{
	MiniMapAppliedRT = MiniMapRT.LoadSynchronous();
	UTexture2D* LoadedMiniMapBaseTexture = GetMiniMapTexture(GetWorld());
	UMaterialInterface* LoadedMiniMapBaseMaterial = MiniMapBaseMaterial.LoadSynchronous();
	UMaterialInterface* LoadedMiniMapMaterial = MiniMapMaterial.LoadSynchronous();

	if (MapImage && MiniMapAppliedRT.IsValid() && LoadedMiniMapBaseTexture && LoadedMiniMapBaseMaterial && LoadedMiniMapMaterial)
	{
		//Copy the Texture from the Minimap Texture to the Render Target which will be later used to Draw On
		UMaterialInstanceDynamic* DynamicMiniMapBaseMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), LoadedMiniMapBaseMaterial);
		MiniMapAppliedMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), LoadedMiniMapMaterial);
		if (MiniMapAppliedMaterial.IsValid() && DynamicMiniMapBaseMaterial)
		{
			DynamicMiniMapBaseMaterial->SetTextureParameterValue(MiniMapBaseTextureParamName, LoadedMiniMapBaseTexture);
			MapImage->SetBrushResourceObject(MiniMapAppliedMaterial.Get());
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), MiniMapAppliedRT.Get(), DynamicMiniMapBaseMaterial);
			MiniMapAppliedMaterial->SetTextureParameterValue(MiniMapMatTextureParamName, MiniMapAppliedRT.Get());
		}

		//Resize the Inner Size Box to reflect the size of the Map Texture
		if (OuterSizeBox && InnerSizeBox)
		{
			FVector2D MiniMapTextureSize = FVector2D(LoadedMiniMapBaseTexture->GetSizeX(), LoadedMiniMapBaseTexture->GetSizeY());
			OuterSizeBox->SetHeightOverride(MiniMapSize);
			OuterSizeBox->SetWidthOverride(MiniMapSize);
			float MiniMapAspectRatio = MiniMapTextureSize.X / MiniMapTextureSize.Y;
			InnerSizeBox->SetWidthOverride(MiniMapSize * (MiniMapAspectRatio > 1.0f ? 1.0f : MiniMapAspectRatio));
			InnerSizeBox->SetHeightOverride(MiniMapSize / (MiniMapAspectRatio > 1.0f ? MiniMapAspectRatio : 1.0f));
		}
	}
}

void URTSMapWidget::UpdateMiniMapRenderTarget(float InDeltaTime)
{
	//Amir, For Testing Try to reset Render Target and draw carat every frame
	InitializeMiniMapRenderTarget();
	DrawMiniMapObjectCarats();
	DrawMiniMapCameraContour();
}

void URTSMapWidget::DrawCaratOnMap(FVector2D ScreenPercentLoc, FVector2D CaratSize, UMaterialInterface* CaratMaterial)
{
	if (MiniMapAppliedRT.IsValid())
	{
		//Drawing the Carat Material on the Render Target with certain Size and at certain position
		if (CaratMaterial)
		{
			UCanvas* MiniMapCanvas;
			FVector2D MiniMapCanvasSize;
			FDrawToRenderTargetContext MiniMapCanvasContext;
			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), MiniMapAppliedRT.Get(), MiniMapCanvas, MiniMapCanvasSize, MiniMapCanvasContext);	//Initialize canvas
			MiniMapCanvas->K2_DrawMaterial(CaratMaterial, MiniMapCanvasSize * ScreenPercentLoc - CaratSize / 2.0f, CaratSize, FVector2D(0.0f, 0.0f));	//Draw the carat material on canvas with size and position parameters
			UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), MiniMapCanvasContext);	//End moving informations from the canvas to render target
		}
	}
}

void URTSMapWidget::DrawMiniMapObjectCarats()
{
	if (MiniMapAppliedRT.IsValid())
	{
		//Initialize the Canvas from Render Target to draw the carats on it
		UCanvas* MiniMapCanvas;
		FVector2D MiniMapCanvasSize;
		FDrawToRenderTargetContext MiniMapCanvasContext;
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), MiniMapAppliedRT.Get(), MiniMapCanvas, MiniMapCanvasSize, MiniMapCanvasContext);	//Initialize canvas

		TArray<AActor*> MiniMapObjects;
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), URTSMiniMapObjectInterface::StaticClass(), MiniMapObjects);
		for (AActor* MiniMapObject : MiniMapObjects)
		{
			if (MiniMapObject->GetClass()->ImplementsInterface(URTSMiniMapObjectInterface::StaticClass()))
			{
				if (UMaterialInterface* MiniMapCaratMaterial = IRTSMiniMapObjectInterface::Execute_GetMiniMapCaratMaterial(MiniMapObject))
				{
					FVector ObjectBounds;
					FVector ObjectOrigin;
					float ObjectRotation;
					IRTSMiniMapObjectInterface::Execute_GetMiniMapObjectBoundsAndOrigin(MiniMapObject, ObjectBounds, ObjectOrigin, ObjectRotation);
					FVector MapBounds, MapOrigin;
					float MapRotation;
					GetMapDimensions(GetWorld(), MapBounds, MapOrigin, MapRotation);

					//Calculate Carat related variables to draw it on the MiniMap canvas
					FVector2D CaratSize = FVector2D(ObjectBounds.X / MapBounds.X, ObjectBounds.Y / MapBounds.Y) * MiniMapCanvasSize;
					FVector MapCorner = FVector(-MapBounds.X, -MapBounds.Y, 0.0f) / 2.0f;
					ObjectOrigin = ObjectOrigin - MapCorner;
					FVector2D Map2DBounds = FVector2D(MapBounds.X, MapBounds.Y);
					FVector2D ObjectMiniMapLocation = (FVector2D(1.0f, 1.0f) - FVector2D(ObjectOrigin.X, ObjectOrigin.Y) / Map2DBounds) * MiniMapCanvasSize - CaratSize / 2.0f;
					float ObjectRelativeRotation = ObjectRotation - MapRotation;

					//Draw the carat material on canvas with size and position parameters
					MiniMapCanvas->K2_DrawMaterial(MiniMapCaratMaterial, ObjectMiniMapLocation, CaratSize, FVector2D(0.0f, 0.0f), FVector2D::UnitVector, ObjectRelativeRotation);
				}

			}
		}
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), MiniMapCanvasContext);	//End moving informations from the canvas to render target
	}
}

//Update the MiniMap View of the Camera Location and view To represent where the actual camera Is on map
void URTSMapWidget::DrawMiniMapCameraContour()
{
	//Calculate the Camera Contour Points from Camera Pawn View
	if (GetCachedRTSCameraPawn())
	{
		if(APlayerController* PC = Cast<APlayerController>(GetCachedRTSCameraPawn()->GetController()))
		{
			TArray<FVector> WorldCameraContourLocations;
			//Get the Viewport Size
			FVector2D CurrentViewportSize;
			if (GEngine && GEngine->GameViewport)
			{
				GEngine->GameViewport->GetViewportSize(CurrentViewportSize);
			}

			//Get Camera Contour Points by deprojecting current viewport screen borders to Plane 
			FVector MapBounds, MapOrigin;
			float MapRotation;
			GetMapDimensions(GetWorld(), MapBounds, MapOrigin, MapRotation);
			FPlane HorizontalPlane(FVector(0.0f, 0.0f, MapOrigin.Z), FVector::UpVector);
			for (int32 i = 0; i < 4; i++)
			{
				FVector CameraContourWorldLoc, ContourDir;
				FVector2D ScreenPos = FVector2D((i < 1 || i > 2) == 0 ? 0.0f : CurrentViewportSize.X, i >= 2 ? 0.0f : CurrentViewportSize.Y);
				PC->DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, CameraContourWorldLoc, ContourDir);
				//Get Camera World Location Collision With the PLane
				FVector EndTraceLoc = CameraContourWorldLoc + ContourDir * UE_MAX_FLT;
				FVector LineDir = (EndTraceLoc - CameraContourWorldLoc).GetSafeNormal();
				FVector WorldCameraContourLocation;
				//Check if the Camera Contour Line can Intersect with the test plane, if not return the end trace location
				if (LineDir.Z < 0.0f)
				{
					WorldCameraContourLocation = FMath::LinePlaneIntersection(CameraContourWorldLoc, EndTraceLoc, HorizontalPlane);
				}
				else
				{
					WorldCameraContourLocation = FVector(EndTraceLoc.X, EndTraceLoc.Y, MapOrigin.Z);
				}
				WorldCameraContourLocations.AddUnique(WorldCameraContourLocation);
			}
			if (MiniMapRT.IsValid() && WorldCameraContourLocations.Num() == 4)
			{
				//Initialize the Canvas from Render Target to draw the carats on it
				UCanvas* MiniMapCanvas;
				FVector2D MiniMapCanvasSize;
				FDrawToRenderTargetContext MiniMapCanvasContext;
				UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), MiniMapAppliedRT.Get(), MiniMapCanvas, MiniMapCanvasSize, MiniMapCanvasContext);	//Initialize canvas

				//Convert the Contour Points from World Space to MiniMap Space
				TArray<FVector2D> MiniMapContourCameraPointsLocations;
				GetMapDimensions(GetWorld(), MapBounds, MapOrigin, MapRotation);
				FVector MapCorner = FVector(-MapBounds.X, -MapBounds.Y, 0.0f) / 2.0f;
				for (FVector ContourPoint : WorldCameraContourLocations)
				{
					ContourPoint = ContourPoint - MapCorner;
					FVector2D Map2DBounds = FVector2D(MapBounds.X, MapBounds.Y);
					FVector2D MiniMapContourPoint = (FVector2D(1.0f, 1.0f) - FVector2D(ContourPoint.X, ContourPoint.Y) / Map2DBounds) * MiniMapCanvasSize - FVector2D(CameraContourThickness) / 2.0f;
					MiniMapContourCameraPointsLocations.AddUnique(MiniMapContourPoint);
				}

				//Draw the MiniMapCameraContour on the Minimap Canvas
				if (MiniMapCanvas && MiniMapContourCameraPointsLocations.Num() == 4)
				{
					//Draw Four lines between the camera contour projection points on the Canvas
					for (int32 i = 0; i < MiniMapContourCameraPointsLocations.Num(); i++)
					{
						FVector2D StartPoint = MiniMapContourCameraPointsLocations[i % 4];
						FVector2D EndPoint = MiniMapContourCameraPointsLocations[(i + 1) % 4];
						MiniMapCanvas->K2_DrawLine(StartPoint, EndPoint, CameraContourThickness, CameraContourColor);
					}
				}
				UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), MiniMapCanvasContext);	//End moving informations from the canvas to render target
			}
		}
	}
}

ARTSCameraPawn* URTSMapWidget::GetCachedRTSCameraPawn()
{
	if (!CachedRTSCameraPawn)
	{
		ARTSCameraPawn* LocalRTSCameraPawn = Cast<ARTSCameraPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ARTSCameraPawn::StaticClass()));
		if (LocalRTSCameraPawn && LocalRTSCameraPawn->IsLocallyControlled())
		{
			CachedRTSCameraPawn = LocalRTSCameraPawn;
		}
	}
	return CachedRTSCameraPawn;
}

UTexture2D* URTSMapWidget::GetMiniMapTexture(UWorld* InWorldContext)
{
	if (InWorldContext)
	{
		if (ARTSWorldSettings* RTSWorldSettings = Cast<ARTSWorldSettings>(InWorldContext->GetWorldSettings()))
		{
			return RTSWorldSettings->GetMiniMapTexure();
		}
	}
	return nullptr;
}

void URTSMapWidget::GetMapDimensions(UWorld* WorldContextObject, FVector& MapBounds, FVector& MapOrigin, float& MapYawRotation)
{
	if (WorldContextObject)
	{
		if (ARTSWorldSettings* RTSWorldSettings = Cast<ARTSWorldSettings>(WorldContextObject->GetWorldSettings()))
		{
			MapBounds = RTSWorldSettings->GetMapBounds();
			MapOrigin = RTSWorldSettings->GetMapOrigin();
		}
	}
}


