// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseObject.h"

// Sets default values
ABaseObject::ABaseObject() : ID(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	materialDefault = GetMaterialDefault().Object;
	materialHighlight = GetMaterialHighlighted().Object;
	materialSelected = GetMaterialSelected().Object;

}

// Called when the game starts or when spawned
void ABaseObject::BeginPlay()
{
	Super::BeginPlay();
	
}

ConstructorHelpers::FObjectFinder<UStaticMesh>& ABaseObject::GetMesh() const
{
	// TODO: insert return statement here
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));
	return MeshObj;
}

ConstructorHelpers::FObjectFinder<UMaterial>& ABaseObject::GetMaterialDefault() const
{
	static ConstructorHelpers::FObjectFinder<UMaterial> textureDefault(TEXT("Material'/Game/M_BasicMaterial.M_BasicMaterial'"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> textureDefault(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Brushed_Nickel.M_Metal_Brushed_Nickel'"));
	return textureDefault;
}

ConstructorHelpers::FObjectFinder<UMaterial>& ABaseObject::GetMaterialHighlighted() const
{
	// TODO: make highlight material
	static ConstructorHelpers::FObjectFinder<UMaterial> textureDefault(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Copper.M_Metal_Copper'"));
	return textureDefault;
}

ConstructorHelpers::FObjectFinder<UMaterial>& ABaseObject::GetMaterialSelected() const
{
	// TODO: make selected material
	static ConstructorHelpers::FObjectFinder<UMaterial> textureDefault(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold'"));
	return textureDefault;
}

// Called every frame
void ABaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABaseObject::CheckRelationships()
{
	return false;
}

void ABaseObject::AddChild(int child)
{
}

void ABaseObject::Highlight(bool state)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Highlight"));
	if (!mesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("WARNING: mesh is null"));
		return;
	}
	if (state)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Highlight: ON"));
		if (mesh && materialHighlight) mesh->SetMaterial(0, materialHighlight);
		else GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("WARNING: missing highlight material"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Highlight: OFF"));
		if (mesh && materialDefault) mesh->SetMaterial(0, materialDefault);
		else GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("WARNING: missing default material"));
	}

	HighlightEvent(state);
}

void ABaseObject::HighlightEvent_Implementation(bool state)
{
	// intentionally empty
}

