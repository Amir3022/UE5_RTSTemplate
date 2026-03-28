// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSWorldSettings.h"
#include "Components/BoxComponent.h"

ARTSWorldSettings::ARTSWorldSettings()
{

}

UTexture2D* ARTSWorldSettings::GetMiniMapTexure()
{
	return MiniMapTexture.LoadSynchronous();
}

const FVector& ARTSWorldSettings::GetMapBounds()
{
	return MapBounds;
}

const FVector& ARTSWorldSettings::GetMapOrigin()
{
	return MapOrigin;
}