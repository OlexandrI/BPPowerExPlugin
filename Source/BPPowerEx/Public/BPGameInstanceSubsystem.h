// MIT License, Copyright (c) 2024 CTOinsideYOU (Oleksandr Berezovskyi)

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/WeakObjectPtrTemplatesFwd.h"
#include "BPGameInstanceSubsystem.generated.h"

// Forward declaration
class UWorld;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class BPPOWEREX_API UBPGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	// ~ UObject interface begin

	virtual UWorld* GetWorld() const override final;
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override final { return true; }
	virtual void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
#endif

	// ~ UObject interface end

	UFUNCTION(BlueprintPure, Category = "BPGameInstanceSubsystem")
	FORCEINLINE UGameInstance* BPGetGameInstance() const { return GetGameInstance(); }

protected:

	// ~ USUbsystem interface begin
	
	/** Override to control if the Subsystem should be created at all. */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override final;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override final;

	// ~ USUbsystem interface end

	UFUNCTION(BlueprintNativeEvent, Category = "BPGameInstanceSubsystem")
	bool BPShouldCreateSubsystem() const;

	UFUNCTION(BlueprintNativeEvent, Category = "BPGameInstanceSubsystem")
	void BPInitialize();

	UFUNCTION(BlueprintNativeEvent, Category = "BPGameInstanceSubsystem")
	void BPDeinitialize();

	UFUNCTION(BlueprintNativeEvent, Category = "BPGameInstanceSubsystem")
	void BPOnViewportReady();

	UFUNCTION(BlueprintNativeEvent, Category = "BPGameInstanceSubsystem")
	void BPHandleRemoveWorld(UWorld* InWorld);

private:

	void HandleWorldCleanup(UWorld* InWorld, bool bSessionEnded, bool bCleanupResoures);
	void HandleRemoveWorld(UWorld* InWorld);
	void HandleLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

	/** GetWorld calls can be expensive, we speed them up by caching the last found world until it goes away. */
	mutable TWeakObjectPtr<UWorld> CachedWorldWeakPtr;

};
