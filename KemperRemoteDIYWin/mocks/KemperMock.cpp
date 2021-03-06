/*!
 *  @file       Kemper.cpp
 *  Project     Arduino Kemper MIDI Library
 *  @brief      Kemper MIDI Library for the Arduino
 *  @version    1.0
 *  @author     Ersin Basaran
 *  @date       16/07/15
 *  @license    GPL v3.0
*/

#include "KemperMock.h"

USING_NAMESPACE_KEMPER

KemperMock::KemperMock() {
	state.mode = MODE_BROWSE;
	state.currentRig = 0;
	state.tempoEnabled = false;

	state.performance = 0;
	state.slot = 0;

	state.senseReceived = true;

	//srand (time(NULL));
	srand (0);

	for (int i = 0;i<KEMPER_STOMP_COUNT;i++) {
		state.stomps[i].info.type = 0;
	}

	for (int t = 0;t<2;t++) {
		for (int i = 0;i<RIG_COUNT*5;i++) {
			int rIdx = 0;
			for (int j = 0;j<6;j++) {
				if (rand()%10<8) {
					rIdx = rand() % (AllStompsCount - 8);
					defaultStompStates[t][i][j].active = rand()%10 < 7;
					//defaultStompStates[t][i][j].info = (AllStomps[rIdx]);
					loadStompInfo(&defaultStompStates[t][i][j].info, j, AllStomps[rIdx].type);
				}
				else {
					defaultStompStates[t][i][j].active = false;
					defaultStompStates[t][i][j].info.type = 0;
				}
			}
			rIdx = (AllStompsCount - 9) + (rand() % 3);
			defaultStompStates[t][i][6].active = rand()%10<8;
			//defaultStompStates[t][i][6].info = (AllStomps[rIdx]);
			loadStompInfo(&defaultStompStates[t][i][6].info , 0, AllStomps[rIdx].type);

			rIdx = (AllStompsCount - 6) + (rand() % 5);
			defaultStompStates[t][i][7].active = rand()%10<8;
			//defaultStompStates[t][i][7].info = (AllStomps[rIdx]);
			loadStompInfo(&defaultStompStates[t][i][7].info, 0, AllStomps[rIdx].type);
		}
	}

	memset(defaultStompParameters, 0, sizeof(defaultStompParameters));
	for (int i = 0; i < sizeof(AllStomps) / sizeof(AllStomps[0]); i++)
	{
		if (AllStomps[i].params)
		{
			for (int j = 0; j < sizeof(AllStomps[i].params) / sizeof(AllStomps[i].params[0]); j++)
			{
				for (int k = 0; k < KEMPER_STOMP_COUNT;k++)
				{
					if (AllStomps[i].params[j]->optionCount == 0)
						defaultStompParameters[k][AllStomps[i].params[j]->number] = rand() % (1 << 14);
					else {
						defaultStompParameters[k][AllStomps[i].params[j]->number] = rand() % (AllStomps[i].params[j]->optionCount);
					}
				}
			}
		}
	}

	state.tune = 0x2000;
}

void KemperMock::read() {
	state.tempo = rand()%0x3FFF;
	state.tempoLed = millis()%500<250;
	state.key = rand()%60;
	//tune = rand()%0x3FFF;
	state.tune += (rand()%401) - 200;
}

void KemperMock::tunerOn() {
	lastMode = state.mode;
	state.mode = MODE_TUNER;
}

void KemperMock::tunerOff() {
	state.mode = lastMode;
}

void KemperMock::tapOn() {
	state.tempoEnabled = true;
}

void KemperMock::tapOff() {
}

void KemperMock::setRig(byte rig) {
	state.currentRig = rig;
	sprintf(state.rigName, "RIG #%d", (int)(state.currentRig+1));
	for (int i=0;i<KEMPER_STOMP_COUNT;i++) {
		state.stomps[i].active = defaultStompStates[0][state.currentRig][i].active;
		state.stomps[i].info = defaultStompStates[0][state.currentRig][i].info;
	}
}

void KemperMock::toggleStomp(byte idx) {
	state.stomps[idx].active = !state.stomps[idx].active;
}

void KemperMock::setPerformance(int idx, int slot) {
	state.performance = idx;
	state.slot = slot;

	sprintf(state.performanceNames[0], "(%d) PERF. #%d", (state.performance+1), (state.performance+1));
	sprintf(state.performanceNames[1], "SLOT %d", 1);
	sprintf(state.performanceNames[2], "SLOT %d", 2);
	sprintf(state.performanceNames[3], "SLOT %d", 3);
	sprintf(state.performanceNames[4], "SLOT %d", 4);
	sprintf(state.performanceNames[5], "SLOT %d", 5);

	for (int i=0;i<KEMPER_STOMP_COUNT;i++) {
		state.stomps[i].active = defaultStompStates[1][state.performance*5+slot][i].active;
		state.stomps[i].info = defaultStompStates[1][state.performance*5+slot][i].info;
	}

}

void KemperMock::sendControlChange(byte data1, byte data2) {
}

void KemperMock::getStompParameter(int stompIdx, int paramNumber) {
	if (paramNumber >= 100 && paramNumber < 120)
		paramNumber -= 100;
	if (paramNumber >= 120 && paramNumber < 140)
		paramNumber -= 120;
	if (stompIdx >= 0 && paramNumber >= 0 && stompIdx < 8 && paramNumber < 128) {
		lastStompParam[2] = defaultStompParameters[stompIdx][paramNumber];
		if (stompIdx == parameter.stompIdx) {
			if (updateStompParameterValue(&parameter, defaultStompParameters[stompIdx][paramNumber])) {
				loadStompParameters(&parameter);
				state.parameterState++;
			}
		}
	}
}

void KemperMock::setStompParam(int stompIdx, byte number, int val) {
	if (number >= 100 && number < 120)
		number -= 100;
	if (number >= 120 && number < 140)
		number -= 120;
	if (stompIdx >= 0 && number >= 0 && stompIdx < 8 && number < 128) {
		defaultStompParameters[stompIdx][number] = val;
		state.parameterState++;
	}
}
