// MIT License, Copyright (c) 2024 CTOinsideYOU (Oleksandr Berezovskyi)

#pragma once

#include "Modules/ModuleManager.h"

class FBPPowerExModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
