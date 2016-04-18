// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PortalPlayerController.h"

APortalPlayerController::APortalPlayerController()
{
	bShowMouseCursor		= false;
	bEnableClickEvents	= true;
	bEnableTouchEvents	= true;
	DefaultMouseCursor	= EMouseCursor::None;
}


