import logging
import sys
import time

from Adafruit_BNO055 import BNO055

# Raspberry Pi configuration with serial UART and RST connected to GPIO 18:
bno = BNO055.BNO055(serial_port='/dev/ttyAMA0', rst=18)

# Initialize the BNO055 and stop if something went wrong.
if not bno.begin():
    raise RuntimeError('Failed to initialize BNO055! Is the sensor connected?')

# File Write Configuration
file = open(time.strftime('accel_measurement_%d-%h.txt', time.localtime()), 'w')
header = 'line\tacc_x\ty\tz\tgyro_x\ty\tz\tgyro_x\ty\tz\tcsys\tcaccel\tcgyro\n'
file.write(header)

counter = 1
startTime = float(time.time())

print('Reading BNO055 data, press Ctrl-C to quit...')
while 1:
    # Read the Euler angles for heading, roll, pitch (all in degrees).
    # heading, roll, pitch = bno.read_euler()
    # Read the calibration status, 0=uncalibrated and 3=fully calibrated.
    sys, gyro, accel, mag = bno.get_calibration_status()

    # Other values you can optionally read:
    # Orientation as a quaternion:
    #x,y,z,w = bno.read_quaterion()

    # Sensor temperature in degrees Celsius:
    #temp_c = bno.read_temp()

    # Magnetometer data (in micro-Teslas):
    #x,y,z = bno.read_magnetometer()

    # Gyroscope data (in degrees per second):
    gyro_x,gyro_y,gyro_z = bno.read_gyroscope()

    # Accelerometer data (in meters per second squared):
    #x,y,z = bno.read_accelerometer()

    # Linear acceleration data (i.e. acceleration from movement, not gravity--
    # returned in meters per second squared):
    acc_x,acc_y,acc_z = bno.read_linear_acceleration()

    # Gravity acceleration data (i.e. acceleration just from gravity--returned
    # in meters per second squared):
    #x,y,z = bno.read_gravity()

    # Read euler
    euler_x,euler_y,euler_z = bno.read_euler()
    
    timeNow = 0
    # Serial output
    if counter%100 == 0:
        print(time.clock()-timeNow)
        timeNow = time.clock()
        print(str(counter)+'\t'+str(sys)+'\t'+str(accel)+'\t'+str(gyro))

    tmp = str(counter)+'\t'+str(acc_x)+'\t'+str(acc_y)+'\t'+str(acc_z)+'\t'+str(gyro_x)+'\t'+str(gyro_y)+'\t'+str(gyro_z)+'\t'+str(euler_x)+'\t'+str(euler_y)+'\t'+str(euler_z)+'\t'+str(sys)+'\t'+str(accel)+'\t'+str(gyro)+'\n'
    file.write(tmp)
    time.sleep(0.005) # Edit depending on output string
    counter = counter + 1

endTime = float(time.time())
print(endTime-startTime)

# From help(BNO055)
#  get_calibration(self)
#      Return the sensor's calibration data and return it as an array of
#      22 bytes. Can be saved and then reloaded with the set_calibration function
#      to quickly calibrate from a previously calculated set of calibration data.

