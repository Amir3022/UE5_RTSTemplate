// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RTSMapWidget.generated.h"

UCLASS()
class RTSTEMPLATEPROJ_API URTSMapWidget : public UUserWidget
{
	GENERATED_BODY()

	URTSMapWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void InitializeMiniMapRenderTarget();
	void UpdateMiniMapRenderTarget(float InDeltaSeconds);
	void DrawCaratOnMap(FVector2D ScreenLoc, FVector2D CaratSize, UMaterialInterface* CaratMaterial);
	void DrawMiniMapObjectCarats();
	void DrawMiniMapCameraContour();
	class ARTSCameraPawn* GetCachedRTSCameraPawn();

public:
	//Minimap assets Functions
	UTexture2D* GetMiniMapTexture(class UWorld* InWorldContext);
	void GetMapDimensions(UWorld* WorldContextObject, FVector& MapBounds, FVector& MapOrigin, float& MapYawRotation);

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class USizeBox* OuterSizeBox;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class USizeBox* InnerSizeBox;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UImage* MapImage;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap Texture")
	TSoftObjectPtr<UMaterialInterface> MiniMapBaseMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Texture")
	FName MiniMapBaseTextureParamName;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Texture")
	TSoftObjectPtr<UMaterialInterface> MiniMapMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Texture")
	FName MiniMapMatTextureParamName;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Texture")
	TSoftObjectPtr<UTextureRenderTarget2D> MiniMapRT;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Texture")
	float MiniMapSize;
	UPROPERTY(EditDefaultsOnly, Category = "MiniMap Texture")
	FColor CameraContourColor;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Texture")
	float CameraContourThickness;

private:
	TObjectPtr<class ARTSCameraPawn> CachedRTSCameraPawn;
	TWeakObjectPtr<UMaterialInstanceDynamic> MiniMapAppliedMaterial;
	TWeakObjectPtr<UTextureRenderTarget2D> MiniMapAppliedRT;
	TArray<FVector> LastCachedCameraContours;
};
