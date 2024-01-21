// Fill out your copyright notice in the Description page of Project Settings.


#include "REWall.h"
#include "Components/SkeletalMeshComponent.h"

AREWall::AREWall()
{
	LowWallComponent1 = CreateDefaultSubobject<UStaticMeshComponent>("Low Wall Component 1");
	LowWallComponent1->SetupAttachment(RootComponent);
	LowWallComponent2 = CreateDefaultSubobject<UStaticMeshComponent>("Low Wall Component 2");
	LowWallComponent2->SetupAttachment(RootComponent);
	WallComponent1 = CreateDefaultSubobject<UStaticMeshComponent>("Wall Component 1");
	WallComponent1->SetupAttachment(RootComponent);
	WallComponent2 = CreateDefaultSubobject<UStaticMeshComponent>("Wall Component 2");
	WallComponent2->SetupAttachment(RootComponent);
	WallComponent3 = CreateDefaultSubobject<UStaticMeshComponent>("Wall Component 3");
	WallComponent3->SetupAttachment(RootComponent);
	WallComponent4 = CreateDefaultSubobject<UStaticMeshComponent>("Wall Component 4");
	WallComponent4->SetupAttachment(RootComponent);
	WallComponent5 = CreateDefaultSubobject<UStaticMeshComponent>("Wall Component 5");
	WallComponent5->SetupAttachment(RootComponent);
}
