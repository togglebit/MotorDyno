# MotorDyno
simple arduino based power meter for motor dyno
Current output is a simple print to screen with torque sensor voltage, torque, speed sensor frequency, rpm and power computation. This is in the serial plotter format so plots can be made from Arduino.

3.27 6.56 492.85 2957.12 2032.27
3.27 6.57 492.85 2957.12 2035.30
3.27 6.56 491.64 2949.85 2027.28
3.27 6.56 492.85 2957.12 2032.27
3.27 6.56 492.85 2957.12 2032.27

3.27V = 6.56NM,  492Hz @ 10pulses per rev = 2957RPM, (2957RPM * 6.56NM)/9.5488 = 2032Watts
