## dsd-firmware


### Ex. 1

An SPI communication with a thermal camera is being developed. For that it is needed to print in console the hex of the resulting array for debugging. This is the function prototype: Void print_hex(uint16_t * array_list); Implement the code to do the task in C.

#### TODO

- [ ] Ask about dimension of the array or the resolution of the thermal camera.
- [x] Ask about if exist a EOF in the frame to indicated the end of the data.

### Ex. 2

For developing a Parking Meter solution, a Radar sensor is used to detect whther or not there is a car parked.
The function used for getting the distance of the object in front of the sensor is the following:

p0.distance = getDistance(uRa0, PIN_PWR0, p0); // returns a float of the distance in meters

If there is no car, the distance will be iqual to -1. If the distance is between 1 and 2.5 meters there is a car.
if the distance is different, it should be ignored.

A state machine is needed to know the current state of the parking. Those states are:

* Avaible_state: The distance in the sensor is -1
* Possible_Car_state: The distance is between 1 and 2.5 meters. The idea is to measure the sensor at least 3 times
to be sure that there is a car, so it jump to the Car_state
* Car_state: The distance is between 1 and 2.5 meters

The prototype of the funtion needed is the following:

// The function return the current state (Availble_state. Possible_Car_state, Car_state)
int current_parking_state(float distance);

Implement the function in C


### My experiences doing hardware and firmware projects 

[![photos - Dropbox]](https://www.dropbox.com/sh/929f3a131wgjagc/AAAoyH01LUJFGUEyKchIXFwAa?dl=0)

[![Drone]](https://youtu.be/Y9YmeWb3GSw)

[![Data App with Streamlit and Python]](https://youtu.be/lqdVRQTszzc)


