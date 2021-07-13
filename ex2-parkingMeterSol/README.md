## Parking Meter solution

### Assumption

* The threshold were changed and scaled with values that allowed to do a controlled test. (1 meter to 19 cm and 2.5 meter to 40 cm)
* ToF sensor, STM32 ARM and WiFi module were Implemented. (VL53L0X, STM32L475VGT6 and Inventek Systems (ISM43362-M3G-L44))
* The Hardware send a POST request to IP:PORT server. Only when detect change state event (Avaible_state -> Posible_car_state -> Car_state and vice versa)
* JSON body:

```
{
   "sensor":"sensorTOF_1",
   "data":[
      {
         "sensorValue": 200,
         "carState": 1
      }
   ],
   "deviceID":"50"
}
```
#### Img reference

![grab](https://github.com/jgambox/dsd-firmware/blob/master/ex2-parkingMeterSol/asset/gif_board.gif)

### References

* https://www.st.com/en/imaging-and-photonics-solutions/vl53l0x.html

### Code

* There are two main files that contains the logic of the firmware

#### Pleas check utilStruct.h file. dir: \ex2-parkingMeterSol\Core\Inc\utilStructs.h

```

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

```

#### the main loop. Please check main.c -> dir: \ex2-parkingMeterSol\Core\Src\main.c

```

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	/* START FSM PARKING METER */

		switch (general_states) {

		case INIT_STATE: {
			general_states = PROCEED_STATE;
			parking_states = Init_state;
			break;
		}

		case PROCEED_STATE: {

			general_states = PROCEED_STATE;

			/***
			 * REPRESENTA LA FUNCION   p0.distance = getDistance(uRa0, PIN_PWR0, p0);
			 * LIGERAMENTE MODIFICADA PARA EFECTO DE PRUEBA.
			 * CON ESTE METODO SE OBTIENE EL VALOR DE LA VARIABLE DISTANCIA DEL SENSOR TOF
			 */
			getDistance(&distance);

			/**
			 * REPRESENTA LA FUNCION int current_parking_state(float distance);
			 * HA SIDO LIGERAMENTE MODIFICADA PARA EFECTOS DE PRUEBA.
			 * INCLUYE LA FSM PARA EL CALCULO DEL ESTADO DE LA ZONA DE PARQUEO.
			 *
			 * RECIBE COMO PARAMETRO LA VARIABLE "distance"
			 */
			current_state = current_parking_state(distance);
			HAL_GPIO_TogglePin(GPIOC, LED_BLUE_Pin);

			/**
			 * No es buena practica. Se debe contruir un delay no bloqueante.
			 * es solo con propositos de prueba.
			 */
			HAL_Delay(100);

			startToF();

			if (previus_state != current_state) {
				previus_state = current_state;
				general_states = REPORT_STATE;
			}
			break;
		}

		case ERROR_STATE: {
			general_states = PROCEED_STATE;
			break;
		}

		case ALARM_STATE: {
			general_states = PROCEED_STATE;
			break;
		}

		case REPORT_STATE: {

			/**
			 * cada vez que existe un cambio de estado de la zona de parqueo se realiza el envio
			 * via WIFI a una IP:PUERTO del nuevo estado de la zona de parque.
			 * metodo POST data enviada en formato JSON.
			 *
			 */
			setSensor("ToF_value");
			setDataInt("parking_State", current_state);
			sendData();
			general_states = PROCEED_STATE;
			break;
		}

		case TIMING_STATE: {

			/**
			 * se deberia agregar un temporizador para controlar el timing
			 * de ejecucion de estados.
			 */
			break;
		}

		}
	/* END FSM PARKING METER */
  }
  /* USER CODE END 3 */
}

```

### Results

* 

![]()

* Ouput

![]()


