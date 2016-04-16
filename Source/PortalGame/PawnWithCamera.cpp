// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PawnWithCamera.h"

//========================================================================
APawnWithCamera::APawnWithCamera()
	:m_CameraRotate(false)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->AttachTo(RootComponent);
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 400.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 7.0f;
	OurCameraSpringArm->bDoCollisionTest = false;
	m_OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	m_OurCamera->AttachTo(OurCameraSpringArm, USpringArmComponent::SocketName);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

//========================================================================
void APawnWithCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Rotate our actor's yaw, which will turn our camera because we're attached to it
	{
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += m_CameraInput.X;
		SetActorRotation(NewRotation);
	}

	//Rotate our camera's pitch, but limit it so we're always looking downward
	{
		FRotator NewRotation = OurCameraSpringArm->GetComponentRotation();
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + m_CameraInput.Y, -80.0f, -15.0f);
		OurCameraSpringArm->SetWorldRotation(NewRotation);
	}

	//Handle movement based on our "MoveX" and "MoveY" axes
	{
		if (!m_MovementInput.IsZero())
		{
			//Scale our movement input axis values by 100 units per second
			m_MovementInput = m_MovementInput.GetSafeNormal() * 100.0f * MoveSpeed;
			FVector NewLocation = GetActorLocation();
			NewLocation += GetActorForwardVector() * m_MovementInput.X * DeltaTime;
			NewLocation += GetActorRightVector() * m_MovementInput.Y * DeltaTime;
			SetActorLocation(NewLocation);
		}
	}

	//Handle zoom
	{
		auto& armLength = OurCameraSpringArm->TargetArmLength;
		armLength = FMath::Clamp(armLength + m_CameraZoom, 150.0f, 2000.0f);
	}
}

//========================================================================
void APawnWithCamera::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("CameraRotate", IE_Pressed, this, &APawnWithCamera::CameraRotatePressed);
	InputComponent->BindAction("CameraRotate", IE_Released, this, &APawnWithCamera::CameraRotateReleased);
	InputComponent->BindAction("CameraZoom", IE_Pressed, this, &APawnWithCamera::CameraZoomPressed);
	InputComponent->BindAction("CameraZoom", IE_Released, this, &APawnWithCamera::CameraZoomReleased);

	InputComponent->BindAxis("MoveForward", this, &APawnWithCamera::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APawnWithCamera::MoveRight);
}

//========================================================================
void APawnWithCamera::MoveForward(float AxisValue)
{
	m_MovementInput.X = 0;
	m_CameraInput.Y = 0;
	m_CameraZoom = 0;

	if (m_CameraRotate)
	{
		m_CameraInput.Y = -AxisValue;
	}
	else if (m_CameraZoomToggle)
	{
		m_CameraZoom = 5 * -AxisValue;
	}
	else
	{
		m_MovementInput.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	}
}

//========================================================================
void APawnWithCamera::MoveRight(float AxisValue)
{
	if (m_CameraRotate)
	{
		m_CameraInput.X = AxisValue;
		m_MovementInput.Y = 0;
	}
	else
	{
		m_CameraInput.X = 0;
		m_MovementInput.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	}
}

//========================================================================
void APawnWithCamera::CameraRotatePressed()
{
	m_CameraRotate = true;
}

//========================================================================
void APawnWithCamera::CameraRotateReleased()
{
	m_CameraRotate = false;
}

//========================================================================
void APawnWithCamera::CameraZoomPressed()
{
	m_CameraZoomToggle = true;
}

//========================================================================
void APawnWithCamera::CameraZoomReleased()
{
	m_CameraZoomToggle = false;
}
