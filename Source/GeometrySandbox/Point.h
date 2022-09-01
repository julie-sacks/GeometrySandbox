// Copyright (C) Julie Sacks, 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseObject.h"
#include "Point.generated.h"

/**
 * 
 */
UCLASS()
class GEOMETRYSANDBOX_API APoint : public ABaseObject
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool isHighlighted = false;
	bool isSelected = false;

	// assets
	// mesh
	ConstructorHelpers::FObjectFinder<UStaticMesh>& GetMesh() const;
	// highlight texture

	// selected texture


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// declare point light comp
	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* MyBillboardComp;

	//void Highlight(bool active = true);

	UFUNCTION(BlueprintCallable)
	void Select(bool active = true);
};
