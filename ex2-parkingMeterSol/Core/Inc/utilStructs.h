/*
 * utilStructs.h
 *
 *  Created on: 11/07/2021
 *      Author: Juan Carlos Gamboa
 */

#ifndef INC_UTILSTRUCTS_H_
#define INC_UTILSTRUCTS_H_

#include <stdbool.h>

#define thresholdMeterMin	 	400
#define thresholdMeterMax	    190
#define maxMeasureSamples       4

int current_state;
int previus_state;

typedef struct {
	bool objectDetected;
	int numberSamples;
	int sensorTOFValues;
} DATA_SENSOR;

typedef enum {
	Init_state = 0, Avaible_state, Possible_Car_state, Car_state
} PARKING_STATES;

typedef enum {
	INIT_STATE = 0,
	PROCEED_STATE,
	ERROR_STATE,
	ALARM_STATE,
	REPORT_STATE,
	TIMING_STATE
} GENERAL_APP_STATE;

typedef struct {
	bool received_data;
} GENERAL_DATA_STATES;

DATA_SENSOR sensorTOF;
PARKING_STATES parking_states, status;
GENERAL_APP_STATE general_states;

PARKING_STATES current_parking_state(float distance_value) {

	switch (parking_states) {

	case Init_state: {
		status = parking_states;
		parking_states = Avaible_state;
		sensorTOF.sensorTOFValues = 0;
		sensorTOF.objectDetected = false;
		sensorTOF.numberSamples = 0;
		break;
	}
	case Avaible_state: {

		parking_states = Avaible_state;
		status = Avaible_state;

		if (distance_value >= thresholdMeterMax
				&& distance_value <= thresholdMeterMin) {
			parking_states = Possible_Car_state;
		}
		break;
	}
	case Possible_Car_state: {

		parking_states = Possible_Car_state;
		status = Possible_Car_state;

		sensorTOF.sensorTOFValues += distance_value;
		sensorTOF.numberSamples++;

		if (sensorTOF.numberSamples >= maxMeasureSamples) {

			sensorTOF.sensorTOFValues = sensorTOF.sensorTOFValues
					/ maxMeasureSamples;

			if (distance_value >= thresholdMeterMax
					&& distance_value <= thresholdMeterMin) {
				sensorTOF.objectDetected = true;
				sensorTOF.numberSamples = 0;
				sensorTOF.sensorTOFValues = 0;
				parking_states = Car_state;
			} else {
				parking_states = Avaible_state;
				sensorTOF.objectDetected = false;
				sensorTOF.numberSamples = 0;
				sensorTOF.sensorTOFValues = 0;
			}
		}

		break;
	}
	case Car_state: {

		parking_states = Car_state;
		status = Car_state;

		if (distance_value > thresholdMeterMin) {
			parking_states = Possible_Car_state;
		}

		break;
	}
	}

	return status;
}

void APP_Initialize(void) {

	general_states = INIT_STATE;
	current_state = Init_state;
	previus_state = Init_state;
}

#endif /* INC_UTILSTRUCTS_H_ */
