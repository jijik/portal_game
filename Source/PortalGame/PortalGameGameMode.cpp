// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PortalGameGameMode.h"
#include "PortalPlayerController.h"
#include "HexEditorHUD.h"

APortalGameGameMode::APortalGameGameMode()
{
	AGameMode::DefaultPawnClass = nullptr;
	AGameMode::PlayerControllerClass = APortalPlayerController::StaticClass();
	AGameMode::HUDClass = AHexEditorHUD::StaticClass();
}



