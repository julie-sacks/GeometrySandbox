// Copyright (C) Julie Sacks, 2022


#include "Point.h"

#include "Components/ShapeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h" 

#define MAYBECRASH

//ConstructorHelpers::FObjectFinder<UStaticMesh> TestMeshObj(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));

APoint::APoint() //: MeshObj(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"))
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("point constructor"));

	//MeshObj = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PointMesh"));
	if (!mesh) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Purple, TEXT("WARNING: null mesh immediately after it is set"));
	//RootComponent = mesh;

	// load assets
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> textureDefault(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	mesh->SetStaticMesh(GetMesh().Object);
	mesh->SetMaterial(0, GetMaterialDefault().Object);
	//mesh.
	if (!mesh) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Purple, TEXT("WARNING: null mesh at end of constructor"));

}

void APoint::BeginPlay()
{

}

ConstructorHelpers::FObjectFinder<UStaticMesh>& APoint::GetMesh() const
{
	// TODO: insert return statement here
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshobj(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));
	return meshobj;
}

void APoint::Tick(float DeltaTime)
{

}

//void APoint::Highlight(bool active)
//{
//}

void APoint::Select(bool active)
{
}
