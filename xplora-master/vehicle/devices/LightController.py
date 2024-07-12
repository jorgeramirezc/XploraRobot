import serial

class LightController:
  head = '$OAX'
  commands = {('FRONT','UP'  ): 'I',
              ('FRONT','DOWN'): 'O',
              ('BACK' ,'UP'  ): 'K',
              ('BACK' ,'DOWN'): 'L'
             }

  def __init__ (self):
    self.cameratilt = '1'
    self.frontlight = '2'
    self.backlight  = '3'

    try:
      self.usb = serial.Serial('/dev/NUCLEO', 115200)
      print('Connection Established to: NUCLEO')
      self.connected = True
    except:
      print('Error connecting to device: NUCLEO')
      self.connected = False

  def __del__(self):
     self.close()

  def close(self):
     print('Closing port: Nucleo')
     try:
       self.usb.close()
     except:
       pass

  def sendCmd(self, cmd):
    if self.connected:
      self.usb.write(bytes(LightController.head + cmd, 'latin-1'))

  def cameraTilt(self, value):
    self.sendCmd('P' + self.cameratilt + str(value).zfill(3))
    print(f'Camera Tile: {value}')

  def lights(self, direction, value):
    if (direction=='FRONT'):
      self.sendCmd('P' + self.frontlight + str(value).zfill(3))
#      print(f'Front light: {value}')
    elif (direction=='BACK'):
      self.sendCmd('P' + self.backlight  + str(value).zfill(3))
#      print(f'Back light: {value}')
    elif (direction=='TILT'):
      self.sendCmd('P' + self.cameratilt + str(value).zfill(3))
#      print(f'Camera Tile: {value}')
    else:
      print(f'direction: {direction} not recognized')
