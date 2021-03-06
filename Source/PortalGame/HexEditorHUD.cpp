// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorHUD.h"
#include "HexEditorActor.h"
#include "PortalUtils.h"

//========================================================================
AHexEditorHUD::AHexEditorHUD()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> Cursor(TEXT("	Material'/Game/Materials/Cursor.Cursor'"));
	m_CursorMat = Cursor.Object;
}

//========================================================================
void AHexEditorHUD::DrawHUD()
{
	Super::DrawHUD();

	auto* playerController = GetOwningPlayerController();
	float x, y;
	bool valid = playerController->GetMousePosition(x,y);

	if (valid && !gHexEditor->m_SuppressCursor)
	{
		DrawMaterialSimple(m_CursorMat, x, y, 32, 32);
	}
}
