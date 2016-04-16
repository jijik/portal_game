// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PortalPlayerController.h"

APortalPlayerController::APortalPlayerController()
{
	bShowMouseCursor		= true;
	bEnableClickEvents	= true;
	bEnableTouchEvents	= true;
	DefaultMouseCursor	= EMouseCursor::Crosshairs;
}


