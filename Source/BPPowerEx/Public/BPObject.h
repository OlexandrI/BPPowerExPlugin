// MIT License, Copyright (c) 2024 CTOinsideYOU (Oleksandr Berezovskyi)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplatesFwd.h"
#include "BPObject.generated.h"

// Forward declaration
class UWorld;

/**
 * Basic minimal UObject class that allow in BP use operation with WorldContext pin
 */
UCLASS(Blueprintable, BlueprintType, Abstract, ClassGroup="Common", Config="Game")
class BPPOWEREX_API UBPObject : public UObject
{
	GENERATED_BODY()
	
public:

	UBPObject();

	// ~UObject interface begin

	virtual UWorld* GetWorld() const override final;
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override final { return true; }
#endif

	/** Overridden to check global network context */
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;

	// ~UObject interface end

protected:

	UPROPERTY(EditDefaultsOnly, Category = "BPObject")
	bool bAutoInit = true;

	UFUNCTION(BlueprintPure, Category = "BPObject", Meta = (DisplayName = "GetWorld"))
	UWorld* BPGetWorld() const;

	/**
	 * Add an object to the root set. This prevents the object and all
	 * its descendants from being deleted during garbage collection.
	 */
	UFUNCTION(BlueprintCallable, Category = "BPObject", Meta = (DisplayName = "AddToRoot"))
	void BPAddToRoot();

	/** Remove an object from the root set. */
	UFUNCTION(BlueprintCallable, Category = "BPObject", Meta = (DisplayName = "RemoveFromRoot"))
	void BPRemoveFromRoot();

	/**
	 * Called after the C++ constructor and after the properties have been initialized, including those loaded from config.
	 * This is called before any serialization or other setup has happened.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "BPObject", Meta = (DisplayName = "PostInitProperties"))
	void BPPostInitProperties();

	/** 
	 * Do any object-specific cleanup required immediately after loading an object.
	 * This is not called for newly-created objects, and by default will always execute on the game thread.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "BPObject", Meta = (DisplayName = "PostLoad"))
	void BPPostLoad();

	/**
	 * This will return detail info about this specific object. (e.g. AudioComponent will return the name of the cue,
	 * ParticleSystemComponent will return the name of the ParticleSystem)  The idea here is that in many places
	 * you have a component of interest but what you really want is some characteristic that you can use to track
	 * down where it came from.  
	 *
	 * @note	safe to call on NULL object pointers!
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "BPObject", Meta = (DisplayName = "GetDetailedInfo"))
	FString BPGetDetailedInfo() const;

	UFUNCTION(BlueprintNativeEvent, Category = "BPObject", Meta = (DisplayName = "Init"))
	void BPInit();

private:

	// ~UObject interface begin
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	virtual FString GetDetailedInfoInternal() const override;

	// ~UObject interface end

	bool IsRealObject() const;

	bool bIsInitialized = false;
	void Init();

	/** GetWorld calls can be expensive, we speed them up by caching the last found world until it goes away. */
	mutable TWeakObjectPtr<UWorld> CachedWorldWeakPtr;

};
