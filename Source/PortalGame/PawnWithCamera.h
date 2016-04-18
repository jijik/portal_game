// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PawnWithCamera.generated.h"

UCLASS()
class PORTALGAME_API APawnWithCamera : public APawn
{
	GENERATED_BODY()

public:
	APawnWithCamera();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

protected:
	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;

	UPROPERTY(EditAnywhere)
	float	MoveSpeed = 5;

	UCameraComponent* m_OurCamera;

	FVector2D		m_MovementInput;
	FVector2D		m_CameraInput;
	float				m_CameraZoom;
	bool				m_CameraRotate;
	bool				m_CameraZoomToggle;

	void				MoveForward(float AxisValue);
	void				MoveRight(float AxisValue);
	void				CameraRotatePressed();
	void				CameraRotateReleased();
	void				CameraZoomPressed();
	void				CameraZoomReleased();
	void				CameraYaw(float AxisValue);
	void				CameraPitch(float AxisValue);
};