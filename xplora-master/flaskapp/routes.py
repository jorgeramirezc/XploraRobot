from flask import send_from_directory, render_template, request
from flaskapp import app
from vehicle import vehicle

import os
import json

@app.route("/")
def index():
  return render_template('index.html', title='Xplora')

@app.route('/favicon.ico')
def favicon():
  return send_from_directory(os.path.join(app.root_path, 'static'),
                             'favicon.ico', mimetype='image/vnd.microsoft.icon')

@app.route('/readGP', methods=['BUTTON'])
def readGP():
  vehicle.gamepad = request.get_json()
  vehicle.move()
  vehicle.update()
  values = vehicle.direction + ',' + vehicle.giro + ',' + str(vehicle.power)
  values = [vehicle.direction, vehicle.giro, vehicle.power]
  values = {
    "direction" : vehicle.direction,
    "giro"      : vehicle.giro,
    "speed"     : vehicle.power,
    "battery"   : vehicle.battery/10,
    "frontlight": vehicle.frontlight,
    "backlight" : vehicle.backlight,
    "cameratilt": vehicle.cameratilt
  }
  return json.dumps(values)
