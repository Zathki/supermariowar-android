#include "input.h"

#include "GameValues.h"
#include "GlobalConstants.h"

extern CGameValues game_values;

CInputControl* basicControlsMenu;
CInputControl* touchControlsMenu;
CInputControl* touchControlsGame;

CPlayerInput::CPlayerInput() {
//up, down, left, right, select, cancel, random, fast scroll
//{menu_up, menu_down, menu_left, menu_right, menu_select, menu_cancel, menu_random, menu_scrollfast;

	basicControlsMenu = new CInputControl();
	int basicKeys[] = {12, 13, 14, 15, 1, 3, -1, -1};
	for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
		basicControlsMenu->keys[iKey] = basicKeys[iKey] + JOY_BUTTON_START;
	}
	touchControlsMenu = new CInputControl();
	int menuKeys[] = {12, 13, 14, 15, 7, 5, 4, 3};
	for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
		touchControlsMenu->keys[iKey] = menuKeys[iKey] + JOY_BUTTON_START - 1;
	}

	//left, right, jump, down, turbo, powerup, start, cancel
	touchControlsGame = new CInputControl();
    int gameKeys[] = {14, 15, 1, 13, 3, 4, 7, 5};
	for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
		touchControlsGame->keys[iKey] = gameKeys[iKey] + JOY_BUTTON_START - 1;
	}

    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;

	fUsingMouse = false;
}

void CPlayerInput::CheckIfMouseUsed()
{
#ifndef _XBOX

    for (short iGameState = 0; iGameState < 2; iGameState++) {
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
			CInputControl * inputControl = &inputControls[iPlayer]->inputGameControls[iGameState];

            for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
				SDL_KEYTYPE iInputKey = inputControl->keys[iKey];

				//Need to reset analog mouse and joystick because there isn't a no longer moving event
				if ((iInputKey >= MOUSE_UP && iInputKey <= MOUSE_RIGHT) ||
                        iInputKey == MOUSE_BUTTON_START + 4 || iInputKey == MOUSE_BUTTON_START + 5) {
					fUsingMouse = true;
					return;
				}
			}
		}
	}

#endif

	fUsingMouse = false;
}

//Pass in 0 for game and 1 for menu
//Clear old button pushed states
void CPlayerInput::ClearPressedKeys(short iGameState)
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
		CInputControl * inputControl = &inputControls[iPlayer]->inputGameControls[iGameState];
		COutputControl * outputControl = &outputControls[iPlayer];

        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
			outputControl->keys[iKey].fPressed = false;

#ifndef _XBOX
            if (fUsingMouse) {
				SDL_KEYTYPE iInputKey = inputControl->keys[iKey];

				//Need to reset analog mouse and joystick because there isn't a no longer moving event
				if ((iInputKey >= MOUSE_UP && iInputKey <= MOUSE_RIGHT) /*||
					(iInputKey >= JOY_STICK_1_UP && iInputKey <= JOY_STICK_2_RIGHT)*/) {
					outputControl->keys[iKey].fDown = false;
				}

				//Clear mouse scroll inputs (scroll button down/up happen on same frame so it needs special case)
                if (iInputKey == MOUSE_BUTTON_START + 4 || iInputKey == MOUSE_BUTTON_START + 5) {
					outputControl->keys[iKey].fDown = false;
				}
			}
#endif
		}
	}

	iPressedKey = 0;
}

void CPlayerInput::ClearGameActionKeys()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
		COutputControl * outputControl = &outputControls[iPlayer];

		// 0-3: direction keys, 4: turbo
        for (int iKey = 5; iKey < NUM_KEYS; iKey++) {
			outputControl->keys[iKey].fPressed = false;
			outputControl->keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;
}

//Clear all button pushed and down states
//Call this when switching from menu to game
void CPlayerInput::ResetKeys()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;
}

//Called during game loop to read input events and see if
//configured keys were pressed.  If they were, then turn on
//key flags to be used by game logic
//iGameState == 0 for in game and 1 for menu
void CPlayerInput::Update(SDL_Event event, short iGameState)
{
	bool fFound = false;
    for (short iPlayer = -1; iPlayer < MAX_PLAYERS+1; iPlayer++) {
		CInputControl * inputControl;
        int gameStartButton = -1;
		COutputControl * outputControl;
		short iPlayerID = iPlayer;
		short iDeviceID = DEVICE_KEYBOARD;

		//Allow all devices to do basic menu control (if not configured with custom controls already
        if(iPlayer == MAX_PLAYERS) {
            if(fFound)
                continue;
            if (iGameState == 1) {
                inputControl = touchControlsMenu;
                outputControl = &outputControls[0];
				iDeviceID = -2;
				iPlayerID = 0;
            } else {
				inputControl = touchControlsGame;
				outputControl = &outputControls[0];
				iDeviceID = -2;
				iPlayerID = -1;
            }
        } else if (iPlayer == -1) {
            if (iGameState == 1) {
				inputControl = &game_values.inputConfiguration[0][0].inputGameControls[1];
				outputControl = &outputControls[0];
				iPlayerID = 0;
				iDeviceID = 0;
            } else if(iGameState == 0 && inputControls[0]->iDevice != 0) {
				inputControl = &game_values.inputConfiguration[0][0].inputGameControls[0];
				outputControl = &outputControls[0];
				iPlayerID = 0;
				iDeviceID = 0;
			}
        } else {
			if (!inputControls[iPlayer])
				continue;

			inputControl = &inputControls[iPlayer]->inputGameControls[iGameState];
            gameStartButton = inputControls[iPlayer]->inputGameControls[0].keys[6];
			outputControl = &outputControls[iPlayer];
			iDeviceID = inputControls[iPlayer]->iDevice;
		}

        if (iDeviceID == DEVICE_KEYBOARD) {
            if (SDL_KEYDOWN == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.keysym.sym) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}

				iPressedKey = (SDL_KEYTYPE)event.key.keysym.sym;
            } else if (SDL_KEYUP == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.keysym.sym) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
            } else if (SDL_MOUSEMOTION == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] >= MOUSE_UP) {
						if ((inputControl->keys[iKey] == MOUSE_UP && event.motion.yrel < -MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_DOWN && event.motion.yrel > MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_LEFT && event.motion.xrel < -MOUSE_X_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_RIGHT && event.motion.xrel > MOUSE_X_DEAD_ZONE) ||
                                (inputControl->keys[iKey] >= MOUSE_BUTTON_START && (event.motion.state & SDL_BUTTON(inputControl->keys[iKey] - MOUSE_BUTTON_START)))) {
							fFound = true;

							//Ignore input for cpu controlled players
							if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
								continue;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							//Ignore input for cpu controlled players
							if (iGameState == 0 && (iPlayerID || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
								continue;

							//Mouse scroll wheel up/down events happen on same frame so ignore up event (and clear it in the ClearPressedKeys() method)
							if (inputControl->keys[iKey] == MOUSE_BUTTON_START + 4 || inputControl->keys[iKey] == MOUSE_BUTTON_START + 5)
								continue;

							outputControl->keys[iKey].fDown = false;
						}
					}
				}
            } else if (SDL_MOUSEBUTTONDOWN == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.button.button + MOUSE_BUTTON_START) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}
            } else if (SDL_MOUSEBUTTONUP == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.button.button + MOUSE_BUTTON_START) {
						fFound = true;

						//Mouse scroll wheel up/down events happen on same frame so ignore up event (and clear it in the ClearPressedKeys() method)
						if (inputControl->keys[iKey] == MOUSE_BUTTON_START + 4 || inputControl->keys[iKey] == MOUSE_BUTTON_START + 5)
							continue;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
			}
        } else {
            if (SDL_JOYHATMOTION == event.type) {
				if (iDeviceID != event.jhat.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
                    if (inputControl->keys[iKey] >= JOY_HAT_UP && inputControl->keys[iKey] <= JOY_HAT_RIGHT) {
						if ((inputControl->keys[iKey] == JOY_HAT_UP && (event.jhat.value & SDL_HAT_UP)) ||
							(inputControl->keys[iKey] == JOY_HAT_DOWN && (event.jhat.value & SDL_HAT_DOWN)) ||
							(inputControl->keys[iKey] == JOY_HAT_LEFT && (event.jhat.value & SDL_HAT_LEFT)) ||
                                (inputControl->keys[iKey] == JOY_HAT_RIGHT && (event.jhat.value & SDL_HAT_RIGHT))) {
							fFound = true;

							//Ignore input for cpu controlled players
							if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
								continue;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							//Ignore input for cpu controlled players
							if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
								continue;

							outputControl->keys[iKey].fDown = false;
						}
					}
				}
            } else if (SDL_JOYBUTTONDOWN == event.type) {
                if (iPlayer != MAX_PLAYERS && iDeviceID != event.jhat.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
					bool customMatch = false;
					if(iPlayer == MAX_PLAYERS) {
						if(iKey == 4 && event.jbutton.button == 0)
							customMatch = true;
						if(iKey == 5 && event.jbutton.button == 1)
							customMatch = true;
					}
                    if(iKey == 6 && gameStartButton == event.jbutton.button)
                        customMatch = true;
                    if (inputControl->keys[iKey] == event.jbutton.button + JOY_BUTTON_START || customMatch) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}
            } else if (SDL_JOYBUTTONUP == event.type) {
				if (iPlayer != MAX_PLAYERS && iDeviceID != event.jbutton.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
					bool customMatch = false;
					if(iPlayer == MAX_PLAYERS) {
						if(iKey == 4 && event.jbutton.button == 0)
							customMatch = true;
						if(iKey == 5 && event.jbutton.button == 1)
							customMatch = true;
					}
                    if(iKey == 6 && gameStartButton == event.jbutton.button)
                        customMatch = true;
                    if (inputControl->keys[iKey] == event.jbutton.button + JOY_BUTTON_START || customMatch) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
            } else if (SDL_JOYAXISMOTION == event.type) {
				if (iPlayer != MAX_PLAYERS && iDeviceID != event.jaxis.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
					bool fUseJoystickInput = false;
					bool fJoystickDown = false;

                    if (event.jaxis.axis == 0 && inputControl->keys[iKey] == JOY_STICK_1_LEFT || (event.jaxis.axis == 0 && iPlayer == MAX_PLAYERS && iKey == 2)) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 0 && inputControl->keys[iKey] == JOY_STICK_1_RIGHT || (event.jaxis.axis == 0 && iPlayer == MAX_PLAYERS && iKey == 3)) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 1 && inputControl->keys[iKey] == JOY_STICK_1_UP || (event.jaxis.axis == 1 && iPlayer == MAX_PLAYERS && iKey == 0)) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 1 && inputControl->keys[iKey] == JOY_STICK_1_DOWN || (event.jaxis.axis == 1 && iPlayer == MAX_PLAYERS && iKey == 1)) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 2 && inputControl->keys[iKey] == JOY_STICK_2_LEFT) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 2 && inputControl->keys[iKey] == JOY_STICK_2_RIGHT) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 3 && inputControl->keys[iKey] == JOY_STICK_2_UP) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 3 && inputControl->keys[iKey] == JOY_STICK_2_DOWN) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
					}

                    if (fUseJoystickInput) {
						//Ignore input for cpu controlled players
						if (iGameState == 0 && (iPlayerID == -1 || game_values.playercontrol[iPlayerID] != 1) && iKey < 6)
							continue;

                        if (fJoystickDown) {
							fFound = true;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							outputControl->keys[iKey].fDown = false;
						}
					}
				}
			}
		}

		//This line might be causing input from some players not to be read
		//if (fFound)
			//break;
	}
}
