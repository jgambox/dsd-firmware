/*
 * utilStructs.h
 *
 *  Created on: 11/07/2021
 *      Author: Juan Carlos Gamboa
 */

#ifndef INC_UTILSTRUCTS_H_
#define INC_UTILSTRUCTS_H_

#include <stdbool.h>

#define thresholdMeterMin	 	400   // 2500  2.5 metros (se reemplaza umbrales a valores a escala para realizar pruebas)
#define thresholdMeterMax	    190   // 1000    1 metro
#define maxMeasureSamples       4

int current_state;
int previus_state;

/**
 * SE CREA ESTRUCTURA PARA ALMACENAR LAS VARIABLES DE UN SENSOR TOF
 */
typedef struct {
	bool objectDetected;
	int numberSamples;
	int sensorTOFValues;
} DATA_SENSOR;

typedef enum {
	Init_state = 0, Avaible_state, Possible_Car_state, Car_state
} PARKING_STATES;


/*
 * Se define estructura enum que contiene los estados de FSM general.
 */
typedef enum {
	INIT_STATE = 0,   // inicializa variables por defecto
	PROCEED_STATE,   // toma de distancia y ejecucion del metodo current_parking_state(float distance);
	ERROR_STATE,
	ALARM_STATE,
	REPORT_STATE,   // reporte via WIFI
	TIMING_STATE    // lo aconsejable es crear un delay no bloqueante puede ser un contador por interrupcion o getTick()
} GENERAL_APP_STATE;

typedef struct {
	bool received_data;
} GENERAL_DATA_STATES;

/**
 * creacion de objetos
 */

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

		/**
		 * teniendo en cuenta la primera lectura del sensor, valida posible estado del parqueo.
		 */
		if (distance_value >= thresholdMeterMax
				&& distance_value <= thresholdMeterMin) {
			parking_states = Possible_Car_state;
		}
		break;
	}
	case Possible_Car_state: {

		parking_states = Possible_Car_state;
		status = Possible_Car_state;

		/**
		 * se realiza la suma de N = 4 veces de los valores y se almacena dentro de una variable
		 */
		sensorTOF.sensorTOFValues += distance_value;
		sensorTOF.numberSamples++;

		/**
		 * cuando sobrepasa la cantidad de muestrar realizada en el paso anterior
		 * procede con la realizacion del promedio de las muestras (#muestras/N=4).
		 * dependiendo del valor se determina el estado de la zona de parqueo.
		 */
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

		/**
		 * si la distancia medida excede el umbral maximo inferior pasa al estado
		 * Possible_Car_state, donde se realizará nuevamente la captura de datos y el promedio
		 * y validar el esado de la zona de parqueo.
		 */
		if (distance_value > thresholdMeterMin) {
			parking_states = Possible_Car_state;
		}

		break;
	}
	}

	return status;
}

/**
 *
 * Se inicializa los estados de cada FSM
 */
void APP_Initialize(void) {

	general_states = INIT_STATE;
	current_state = Init_state;
	previus_state = Init_state;
}

#endif /* INC_UTILSTRUCTS_H_ */
