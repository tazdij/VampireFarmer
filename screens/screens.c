#include "screens.h"
#include "../utils/hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static map_t screens = 0;
static Screen* currentScreen = 0;


void Screens_Init(void) {
	screens = hashmap_new();
}

void Screens_Update(float delta) {
	if (currentScreen == 0) {
		return;
	}

    currentScreen->Update(delta);
}

void Screens_Draw(void) {
	if (currentScreen == 0) {
		return;
	}

	currentScreen->Draw();
}

void Screens_AddScreen(const char* screenId, Screen* screen) {

	int index = 0;
	int error = 0;
	Screen* mapScreen = 0;
	
	// Check if the screen already is initialized
	error = hashmap_get(screens, screenId, (void**)(&mapScreen));
	
	if (error == MAP_OK) {
		return;
	}
	
	
	// add screen to map
	Screen* hpScreen = (Screen*)malloc(sizeof(Screen));
	
	strcpy(hpScreen->screenId, screenId);
	hpScreen->Update = screen->Update;
	hpScreen->Draw = screen->Draw;
	hpScreen->Init = screen->Init;
	hpScreen->Load = screen->Load;
	hpScreen->Unload = screen->Unload;
	hpScreen->Finish = screen->Finish;
	
	// Try and add the screen to screens
	error = hashmap_put(screens, hpScreen->screenId, hpScreen);
	if (error != MAP_OK) {
		// TODO: Print Error to console
		printf("Failed to insert into screens map");
	}
	
	
}

void Screens_Free(void) {
	int error = 0;
	Screen* mapScreen = 0;


	do {
		error = hashmap_get_one(screens, (void**)&mapScreen, 1);		
		if (error == MAP_OK) {
			
		}
	} while (error == MAP_OK);
}

void Screens_SetScreen(const char* screenId) {
	// TODO: Actaully set the current screen pointer to the screen

	int index = 0;
	int error = 0;
	Screen* mapScreen = 0;

	// Check if the screen already is initialized
	error = hashmap_get(screens, screenId, (void**)(&mapScreen));
	
	if (error != MAP_OK) {
		printf("Unable to find screen %s", screenId);
		return;
	}

	// Unload the current screen
	if (currentScreen != 0) {
		currentScreen->Unload();
	}

	// Load the new screen
	mapScreen->Load();

	// Set CurrentScreen = mapScreen
	currentScreen = mapScreen;
}
