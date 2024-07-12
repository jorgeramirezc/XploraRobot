from vehicle.devices.DriverRoboclaw import Driver
from vehicle.devices.LightController import LightController

class Vehicle:

  def __init__(self):
    self.battery = 0
    self.direction = 0
    self.giro = 0
    self.power = 20
    self.frontlight = 0
    self.backlight = 0
    self.cameratilt = 50

    self._gamepad = {}
    self.driver = Driver()
    self.lightcontroller = LightController()
    self.update()
    self._updateLights(self.frontlight,'FRONT', 'DOWN', inc=0)
    self._updateLights(self.backlight,'BACK', 'DOWN', inc=0)
    self._updateLights(self.cameratilt,'TILT', 'DOWN', inc=0)

  @property
  def gamepad(self):
    return self._gamepad

  @gamepad.setter
  def gamepad(self, values):
    try:
      self._gamepad['A'] = values[0]
      self._gamepad['B'] = values[1]
      self._gamepad['X'] = values[2]
      self._gamepad['Y'] = values[3]
      self._gamepad['LB'] = values[4]
      self._gamepad['RB'] = values[5]
      self._gamepad['LT'] = values[6]
      self._gamepad['RT'] = values[7]
      self._gamepad['BACK'] = values[8]
      self._gamepad['START'] = values[9]
      self._gamepad['LSTICK'] = values[10]
      self._gamepad['RSTICK'] = values[11]
      self._gamepad['UP'] = values[12]
      self._gamepad['DOWN'] = values[13]
      self._gamepad['LEFT'] = values[14]
      self._gamepad['RIGHT'] = values[15]
      self._gamepad['LSTICK_HOR'] = values[16]
      self._gamepad['LSTICK_VER'] = values[17]
      self._gamepad['RSTICK_HOR'] = values[18]
      self._gamepad['RSTICK_VER'] = values[19]
    except IndexError:
      print("Index Error: Gamepad Array doesn't have enough button data")

  def update(self):
    try:
      if self.gamepad['X']:
        if self.gamepad['UP']:
          self.driver.aumentar_vel()
        elif self.gamepad['DOWN']:
          self.driver.reducir_vel()

      # Light Control
      if self.gamepad['RB']:
        if self.gamepad['UP']:
          self.frontlight = self._updateLights(self.frontlight,'FRONT','UP')
        if self.gamepad['DOWN']:
          self.frontlight = self._updateLights(self.frontlight,'FRONT', 'DOWN')
      if self.gamepad['LB']:
        if self.gamepad['UP']:
          self.backlight = self._updateLights(self.backlight,'BACK', 'UP')
        if self.gamepad['DOWN']:
          self.backlight = self._updateLights(self.backlight,'BACK', 'DOWN')

      # Camera Control
      if self.gamepad['Y']:
        if self.gamepad['UP']:
          self.cameratilt = self._updateLights(self.cameratilt, 'TILT', 'UP', inc=5)
        if self.gamepad['DOWN']:
          self.cameratilt = self._updateLights(self.cameratilt, 'TILT', 'DOWN', inc=5)
    except KeyError:
      pass
    self.power = self.driver.get_speed()
    self.battery = int(self.driver.get_batt()*10)

  def _updateLights(self, value, light, dir, inc=10):
    if (dir=='UP'):
      value = min(max(0, value + inc), 100)
    elif (dir=='DOWN'):
      value = min(max(0, value - inc), 100)

    self.lightcontroller.lights(light, value)
    return value

  def move(self):
    self.giro = 'Straight'
    self.direction = 'Stop'
    if self.gamepad['A']:
      self.direction = 'Forward'
      if self.gamepad['LT']:
        self.giro = 'Turn Left'
        self.driver.avanzar_gira_i()
      elif self.gamepad['RT']:
        self.giro = 'Turn Right'
        self.driver.avanzar_gira_d()
      else:
        self.driver.avanzar()
    elif self.gamepad['B']:
      self.direction = 'Backward'
      self.driver.retroceder()
    elif self.gamepad['LT']:
      self.giro = 'Turn Left'
      self.driver.girar_izq()
    elif self.gamepad['RT']:
      self.giro = 'Turn Right'
      self.driver.girar_der()
    else:
      self.driver.parar()
