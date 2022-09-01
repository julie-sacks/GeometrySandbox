// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseObject.generated.h"

UCLASS()
class GEOMETRYSANDBOX_API ABaseObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// functions for getting the pre-loaded assets
	// overload this for each shape's mesh
	virtual ConstructorHelpers::FObjectFinder<UStaticMesh>& GetMesh() const;
	// materials
	virtual ConstructorHelpers::FObjectFinder<UMaterial>& GetMaterialDefault() const;
	virtual ConstructorHelpers::FObjectFinder<UMaterial>& GetMaterialHighlighted() const;
	virtual ConstructorHelpers::FObjectFinder<UMaterial>& GetMaterialSelected() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	// components
	// static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* mesh;

	// materials
	UPROPERTY(VisibleAnywhere)
	UMaterial* materialDefault = nullptr;
	UPROPERTY(VisibleAnywhere)
	UMaterial* materialHighlight = nullptr;
	UPROPERTY(VisibleAnywhere)
	UMaterial* materialSelected = nullptr;

	const int ID = 0;
	std::vector<int> parents;
	std::vector<int> children;
	bool isDirty;

	bool CheckRelationships();
	void AddChild(int child);

	UFUNCTION(BlueprintCallable)
	virtual void Highlight(bool state = true);
	UFUNCTION(BlueprintNativeEvent)
	void HighlightEvent(bool state);

};
