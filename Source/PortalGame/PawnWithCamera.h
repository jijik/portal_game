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

	virtual void	BeginPlay() override;
	virtual void	Tick(float DeltaSeconds) override;
	virtual void	SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

protected:
	UPROPERTY(EditAnywhere)
	USpringArmComponent*				OurCameraSpringArm;
	UPROPERTY(EditAnywhere)
	UCameraComponent*						Camera;
	UPROPERTY(EditAnywhere)
	UDirectionalLightComponent*	Light;
	UPROPERTY(EditAnywhere)
	float												MoveSpeed = 5;


	FVector2D		m_MovementInput;
	FVector2D		m_CameraInput;
	float				m_CameraZoom;
	bool				m_CameraRotate;
	bool				m_CameraZoomToggle;

	float				m_FromAngle = 0;
	float				m_ToAngle = 0;
	float				m_CurrentAngle = 0;

	void				MoveForward(float AxisValue);
	void				MoveRight(float AxisValue);
	void				CameraRotatePressed();
	void				CameraRotateReleased();
	void				CameraYaw(float AxisValue);
	void				CameraPitch(float AxisValue);
	void				CameraZoom(float AxisValue);

	void				RotateCamCCW();
	void				RotateCamCW();
};