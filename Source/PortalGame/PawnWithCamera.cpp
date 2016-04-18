// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PawnWithCamera.h"
#include "PortalUtils.h"
#include "HexEditorActor.h"

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
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	Camera->AttachTo(OurCameraSpringArm, USpringArmComponent::SocketName);

	Light = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Headlight"));
	Light->AttachTo(Camera);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

//========================================================================
void APawnWithCamera::BeginPlay()
{
	Light->CastShadows = false;
	Light->Intensity = PI;
}

//========================================================================
void APawnWithCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Rotate our actor's yaw, which will turn our camera because we're attached to it
	{
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += m_CameraInput.X * MoveSpeed;
		SetActorRotation(NewRotation);
	}

	//Rotate our camera's pitch, but limit it so we're always looking downward
	{
		FRotator NewRotation = OurCameraSpringArm->GetComponentRotation();
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + m_CameraInput.Y * MoveSpeed, -80.0f, -15.0f);
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

// 	Handle zoom
 		{
 			auto& armLength = OurCameraSpringArm->TargetArmLength;
 			armLength = FMath::Clamp(armLength + m_CameraZoom, 150.0f, 2000.0f);
			m_CameraZoom = 0;
 		}
}

//========================================================================
void APawnWithCamera::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("CameraRotate", IE_Pressed, this, &APawnWithCamera::CameraRotatePressed);
	InputComponent->BindAction("CameraRotate", IE_Released, this, &APawnWithCamera::CameraRotateReleased);

	InputComponent->BindAxis("MoveForward", this, &APawnWithCamera::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APawnWithCamera::MoveRight);
	InputComponent->BindAxis("CamYaw", this, &APawnWithCamera::CameraYaw);
	InputComponent->BindAxis("CamPitch", this, &APawnWithCamera::CameraPitch);
	InputComponent->BindAxis("CamZoom", this, &APawnWithCamera::CameraZoom);
}

//========================================================================
void APawnWithCamera::MoveForward(float AxisValue)
{
	m_MovementInput.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

//========================================================================
void APawnWithCamera::MoveRight(float AxisValue)
{
	m_MovementInput.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

//========================================================================
void APawnWithCamera::CameraRotatePressed()
{
	m_CameraRotate = true;
	gHexEditor->m_SuppressCursor = true;
}

//========================================================================
void APawnWithCamera::CameraRotateReleased()
{
	m_CameraRotate = false;
	gHexEditor->m_SuppressCursor = false;
}

//========================================================================
void APawnWithCamera::CameraYaw(float AxisValue)
{
	m_CameraInput.X = 0;
	if (m_CameraRotate)
	{
		m_CameraInput.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	}
}

//========================================================================
void APawnWithCamera::CameraPitch(float AxisValue)
{
	m_CameraInput.Y = 0;
	if (m_CameraRotate)
	{
		m_CameraInput.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	}
}

//========================================================================
void APawnWithCamera::CameraZoom(float AxisValue)
{
	m_CameraZoom = -AxisValue * 20;
}