from vehicle.devices.RoboclawPython3 import Roboclaw

class Driver:
  def __init__ (self, close_loop=False):
    self.rc = Roboclaw('/dev/ROBOCLAW', 115200, timeout=0.01, retries=1)
    self.cl = close_loop
    self.address1 = 0x80
    self.address2 = 0x81
    self.speed = 40
    self.accel = 30000
    if self.rc.Open() == 1:
      print('Connection Established  : ROBOCLAW')
      self.connected = True
    else:
      print('ERROR: Unable to connect: ROBOCLAW')
      self.connected = False

  def __del__ (self):
    print('Closing Port: ROBOCLAW')
    try:
      self.rc._port.close()
    except:
      pass

  def _set_cl_motor_speed(self,spdR,spdL):
#    print('yay ' + str(spdR) + ' : ' + str(spdL))
#    self.rc.SpeedAccelM1(self.address1,self.accel,spd)
    self.rc.SpeedAccelM1M2(self.address1, self.accel, spdR, spdR)
#    self.rc.SpeedAccelM1M2(self.address2, self.accel, -spdL, -spdL)

  def avanzar(self):
    if self.connected:
      if self.cl:
        #Close loop
        spd = self.speed*4000
        self._set_cl_motor_speed(spd,spd)
      else:
        #Open Loop
        pow = int(self.speed/10*12.7)
        self.rc.ForwardM1(self.address1,pow)
        self.rc.ForwardM2(self.address1,pow)
#        self.rc.ForwardM1(self.address2,pow)
#        self.rc.ForwardM2(self.address2,pow)

  def avanzar_gira_d(self):
    if self.connected:
      if self.cl:
        spdR = self.speed*4000
        spdL = (self.speed+4)*4000
        self._set_cl_motor_speed(spdR,spdL)
      else:
        pow = int(self.speed/10*12.7)
        pow2 = int((self.speed + 30 )/10*12.7)
        self.rc.ForwardM1(self.address1,pow)
        self.rc.ForwardM2(self.address1,pow2)
#        self.rc.ForwardM1(self.address2,pow2)
#        self.rc.ForwardM2(self.address2,pow2)

  def avanzar_gira_i(self):
    if self.connected:
      if self.cl:
        spdL = self.speed*4000
        spdR = (self.speed+4)*4000
        self._set_cl_motor_speed(spdR,spdL)
      else:
        pow = int(self.speed/10*12.7)
        pow2 = int((self.speed + 30 )/10*12.7)
        self.rc.ForwardM1(self.address1,pow2)
        self.rc.ForwardM2(self.address1,pow)
#        self.rc.ForwardM1(self.address2,pow)
#        self.rc.ForwardM2(self.address2,pow)

  def retroceder(self):
    if self.connected:
      if self.cl:
        #Close loop
        spd = self.speed*-4000
        self._set_cl_motor_speed(spd,spd)
      else:
        pow = int(self.speed/10*12.7)
        self.rc.BackwardM1(self.address1,pow)
        self.rc.BackwardM2(self.address1,pow)
#        self.rc.BackwardM1(self.address2,pow)
#        self.rc.BackwardM2(self.address2,pow)

  def parar(self):
    if self.connected:
      self.rc.DutyM1M2(self.address1,0,0)
#      self.rc.DutyM1M2(self.address2,0,0)

  def girar_izq(self):
    if self.connected:
      if self.cl:
        spd = self.speed*4000
        self._set_cl_motor_speed(spd,-spd)
      else:
        pow = int(self.speed/10*12.7)
        self.rc.ForwardM1(self.address1,pow)
        self.rc.BackwardM2(self.address1,pow)
#        self.rc.BackwardM1(self.address2,pow)
#        self.rc.BackwardM2(self.address2,pow)

  def girar_der(self):
    if self.connected:
      if self.cl:
        spd = self.speed*4000
        self._set_cl_motor_speed(-spd,spd)
      else:
        pow = int(self.speed/10*12.7)
        self.rc.BackwardM1(self.address1,pow)
        self.rc.ForwardM2(self.address1,pow)
#        self.rc.ForwardM1(self.address2,pow)
#        self.rc.ForwardM2(self.address2,pow)

  def aumentar_vel(self):
    self.speed = min(self.speed+2, 100)
  def reducir_vel(self):
    self.speed= max(self.speed-2,0)

  def get_speed(self):
    return self.speed

  def get_batt(self):
    bat = 1.5
    error = 0
    count = 0
    if self.connected:
      while (not error and count <=3):
        data = self.rc.ReadMainBatteryVoltage(self.address1)
        error = data[0]
        count = count + 1
      if data[0] == 1:
        bat = data[1]/10
    return bat

  def get_current_m1(self):
    current = 0
    return current

  def get_current_m2(self):
    current = 0
    return current

  def get_currents(self):
    cD1 = cD2 = cI1 = cI2 = 0
    error = 0
    count = 0
    #if self.connected:
#      while (not error and count <=1):
        #data1, cD1, cD2 = self.rc.ReadCurrents(self.address1)
        #data2, cI1, cI2 = self.rc.ReadCurrents(self.address2)
        #error = data1 and data2
        #count = count + 1
    #if data1 == 0:
    #    print('Error Reading Currents')
    return(cD1/100,cD2/100,cI1/100,cI2/100)
