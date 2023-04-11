// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectComponent.h"
#include "EffectInfluenced.h"
#include "UObject/UObjectGlobals.h"

// Sets default values for this component's properties
UEffectComponent::UEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
	bAutoRegister = true;
	this->SetComponentTickEnabled(true);
	this->SetComponentTickInterval(0);
	// ...
}


// Called when the game starts
void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}


// Called every frame
void UEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	Effect->CallTick(DeltaTime, GetOwner(), effectOwner);
	//Effect->time -= DeltaTime;

	if ((timer -= DeltaTime) < 0)
		Activate(false);
	// ...
}

void UEffectComponent::Activate(bool bReset){
	if (bReset) { // Add activity
		Effect->CallStart(GetOwner(), effectOwner);
	}
	else { // Remove component
		Effect->CallEnd(GetOwner(), effectOwner);
		DestroyComponent();
	}
	Super::Activate(bReset);
}

void UEffectComponent::Init(UEffect* effect, AActor* other, float radius){
	Effect = NewObject<UEffect>();
	Effect->Copy(effect);
	Effect->power = Effect->GetPowerByLength(radius);
	effectOwner = other;
	this->timer = effect->time;
}