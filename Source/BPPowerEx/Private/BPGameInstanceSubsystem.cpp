// MIT License, Copyright (c) 2024 CTOinsideYOU (Oleksandr Berezovskyi)

#include "BPGameInstanceSubsystem.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "UObject/WeakObjectPtrTemplates.h"

#if WITH_EDITOR
#include "BlueprintActionDatabase.h"
#endif // WITH_EDITOR

#include UE_INLINE_GENERATED_CPP_BY_NAME(BPGameInstanceSubsystem)


UWorld* UBPGameInstanceSubsystem::GetWorld() const
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

#if WITH_EDITOR
void UBPGameInstanceSubsystem::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	Super::PostCDOCompiled(Context);

	if (this->GetClass() == UBPGameInstanceSubsystem::StaticClass()) return;

	// We want to make sure that the BlueprintActionDatabase is up to date
	// And we have valid actions for our blueprints subsystems
	if (auto* DB = FBlueprintActionDatabase::TryGet())
	{
		// Regenerate all, because our subsystem can be used anywhere
		DB->RefreshAll();
	}
}
#endif // WITH_EDITOR

bool UBPGameInstanceSubsystem::BPShouldCreateSubsystem_Implementation() const
{
	return true;
}

void UBPGameInstanceSubsystem::BPInitialize_Implementation()
{
}

void UBPGameInstanceSubsystem::BPDeinitialize_Implementation()
{
}

void UBPGameInstanceSubsystem::BPOnViewportReady_Implementation()
{
}

void UBPGameInstanceSubsystem::BPHandleRemoveWorld_Implementation(UWorld* InWorld)
{
}

bool UBPGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return BPShouldCreateSubsystem();
}

void UBPGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameViewportClient::OnViewportCreated().AddUObject(this, &UBPGameInstanceSubsystem::BPOnViewportReady);
	
	//FWorldDelegates::OnWorldCleanup.AddUObject(this, &ThisClass::HandleWorldCleanup);
	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &ThisClass::HandleRemoveWorld);
	//FWorldDelegates::OnPreWorldFinishDestroy.AddUObject(this, &ThisClass::HandleRemoveWorld);

	BPInitialize();
}

void UBPGameInstanceSubsystem::Deinitialize()
{
	BPDeinitialize();
}

void UBPGameInstanceSubsystem::HandleWorldCleanup(UWorld* InWorld, bool bSessionEnded, bool bCleanupResoures)
{
	HandleRemoveWorld(InWorld);
}

void UBPGameInstanceSubsystem::HandleRemoveWorld(UWorld* InWorld)
{
	BPHandleRemoveWorld(InWorld);
}


