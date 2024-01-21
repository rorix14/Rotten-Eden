// Fill out your copyright notice in the Description page of Project Settings.


#include "REInteractablePlaceBase.h"
#include "REPlayerController.h"
#include "Components/SphereComponent.h"

AREInteractablePlaceBase::AREInteractablePlaceBase()
{
	if((SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"))) != nullptr)
	{
		RootComponent = SphereComp;
		SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		
		if((MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"))) != nullptr)
		{
			MeshComp->SetupAttachment(SphereComp);
		}
	}

	PromptMessage = TEXT("Press 'E' to do something!");
	MenuIndex = 1;
	//Might not need to be replicated;
	bReplicates = true;
}

void AREInteractablePlaceBase::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority() && SphereComp)
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AREInteractablePlaceBase::OnPlayerEnterInteractableRadius);
		SphereComp->OnComponentEndOverlap.AddDynamic(this, &AREInteractablePlaceBase::OnPlayerLeaveInteractableRadius);
	}
}

void AREInteractablePlaceBase::OnPlayerEnterInteractableRadius(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if( auto PC = OtherActor->GetInstigatorController<AREPlayerController>())
	{
		PC->SetClientInteractablePointVisibility(true, PromptMessage, MenuIndex, this);
	}
}

void AREInteractablePlaceBase::OnPlayerLeaveInteractableRadius(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(auto PC = OtherActor->GetInstigatorController<AREPlayerController>())
	{
		PC->SetClientInteractablePointVisibility(false, PromptMessage, 0, this);
	}
}
