#pragma once

namespace F4CW {
	void PouyaFunction(std::monostate);
}

bool InitializeSharedGameVariables();

void F4SEMessageCallback(F4SE::MessagingInterface::Message* myMessage);

void GameDataLoaded();
void GameDataReady();

void InitializeCustomConsoleCommands();

bool RegisterScaleforms(const F4SE::ScaleformInterface* scaleformInterface);

void InitializeGameEvents();

void GetAllINISettings();