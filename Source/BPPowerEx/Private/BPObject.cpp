// MIT License, Copyright (c) 2024 CTOinsideYOU (Oleksandr Berezovskyi)

#include "BPObject.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BPObject)


UBPObject::UBPObject()
{
}

UWorld* UBPObject::GetWorld() const
{
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.WorldType == EWorldType::PIE)
		{
			return Context.World();
		}
	}

	if (IsValid(GWorld))
	{
		return GWorld;
	}

	if (UWorld* CachedWorld = CachedWorldWeakPtr.Get(); IsValid(CachedWorld))
	{
		return CachedWorld;
	}

	// Our Owner (Outer) could be a GameInstance, could be World, could also be a WidgetTree, so we're just going to follow the outer chain to find the world we're in.
	UObject* Outer = GetOuter();
	while (IsValid(Outer))
	{
		if (UWorld* World = Cast<UWorld>(Outer); IsValid(World))
		{
			CachedWorldWeakPtr = World;
			return World;
		}

		if (UWorld* World = Outer->GetWorld(); IsValid(World))
		{
			CachedWorldWeakPtr = World;
			return World;
		}

		Outer = Outer->GetOuter();
	}

	return nullptr;
}

int32 UBPObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if ((Function->FunctionFlags & FUNC_Static))
	{
		// Try to use the same logic as function libraries for static functions, will try to use the global context to check authority only/cosmetic
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}

	return FunctionCallspace::Local;
}

bool UBPObject::IsRealObject() const
{
	return !HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) && !IsDefaultSubobject();
}

void UBPObject::Init()
{
	if (bIsInitialized)
	{
		return;
	}

	bIsInitialized = true;

	// Call BP method
	BPInit();
}

void UBPObject::BPInit_Implementation()
{

}

void UBPObject::PostInitProperties()
{
	Super::PostInitProperties();

	if (IsRealObject())
	{
		// Call BP method
		BPPostInitProperties();
		if (bAutoInit && GetWorld())
		{
			// Register auto initialization
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UBPObject::Init);
		}
	}
}

void UBPObject::BPPostInitProperties_Implementation()
{

}

void UBPObject::PostLoad()
{
	Super::PostLoad();

	if (IsRealObject())
	{
		// Call BP method
		BPPostLoad();
		if (bAutoInit) Init();
	}
}

void UBPObject::BPPostLoad_Implementation()
{

}

FString UBPObject::GetDetailedInfoInternal() const
{
	const FString Result = BPGetDetailedInfo();
	
	if (Result.IsEmpty())
	{
		return Super::GetDetailedInfoInternal();
	}

	return Result;
}

FString UBPObject::BPGetDetailedInfo_Implementation() const
{
	return TEXT("BPObject");
}

UWorld* UBPObject::BPGetWorld() const
{
	return GetWorld();
}

void UBPObject::BPAddToRoot()
{
	this->AddToRoot();
}

void UBPObject::BPRemoveFromRoot()
{
	this->RemoveFromRoot();
}
