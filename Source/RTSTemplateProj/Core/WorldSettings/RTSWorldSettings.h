// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "RTSWorldSettings.generated.h"

UCLASS()
class RTSTEMPLATEPROJ_API ARTSWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

	ARTSWorldSettings();

public:
	UTexture2D* GetMiniMapTexure();
	const FVector& GetMapBounds();
	const FVector& GetMapOrigin();
	const FVector& GetMapXDirectionVec() { return XDirectionVec; }
	const FVector& GetMapYDirectionVec() { return YDirectionVec; }

protected:
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (bAllowPrivateAccess = true))
	TSoftObjectPtr<UTexture2D> MiniMapTexture;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (bAllowPrivateAccess = true))
	FVector MapBounds;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (bAllowPrivateAccess = true))
	FVector MapOrigin;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (bAllowPrivateAccess = true))
	FVector XDirectionVec;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (bAllowPrivateAccess = true))
	FVector YDirectionVec;
};
