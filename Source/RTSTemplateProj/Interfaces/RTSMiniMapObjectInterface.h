// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RTSMiniMapObjectInterface.generated.h"

UINTERFACE(Blueprintable)
class RTSTEMPLATEPROJ_API URTSMiniMapObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class RTSTEMPLATEPROJ_API IRTSMiniMapObjectInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	UMaterialInterface* GetMiniMapCaratMaterial();
	virtual UMaterialInterface* GetMiniMapCaratMaterial_Implementation() { return nullptr; }

	UFUNCTION(BlueprintNativeEvent)
	void GetMiniMapObjectBoundsAndOrigin(FVector& ObjectBounds, FVector& ObjectOrigin, float& YawRotation);
	virtual void GetMiniMapObjectBoundsAndOrigin_Implementation(FVector& ObjectBounds, FVector& ObjectOrigin, float& YawRotation) {}
};
