var repGP;
var hasGP = false;

function reportGamepad() {
  //-------------------------------------------------------------------------------------------------------
  // * Method used to obtain Gamepad State and generate button chain to send to Flask Backend
  //-------------------------------------------------------------------------------------------------------
  var gp = navigator.getGamepads()[0];
  //Creates array and stores button value (1 or 0) and axis values (-1 to 1)
  var gp_chain = new Array(gp.buttons.length + gp.axes.length);
  for (i = 0 ; i < gp.buttons.length ; i++) {
    gp_chain[i]=gp.buttons[i].value;
  }
  for (j = 0 ; j < gp.axes.length ; j++) {
    gp[gp.buttons.length + j] = gp.axes[j];
  }
  //Buttons 6 and 7 (left/right shoulders) are analog. Checks whether they are pressed to generalize buttons
  gp_chain[6] = gp.buttons[6].pressed ? 1 : 0;
  gp_chain[7] = gp.buttons[7].pressed ? 1 : 0;

  //Data transfer to backend flask server via AJAX
  $.ajax({
    type: "BUTTON",
    url:  "/readGP",
    data: JSON.stringify(gp_chain),
    contentType: "application/json; charset=utf-8",
    success:callbackFunc
  });
}

function callbackFunc(response) {
  values=JSON.parse(response);
  document.getElementById("direccion").innerHTML = values.direction;
  document.getElementById("giro").innerHTML = values.giro;
  document.getElementById("drive").innerHTML = values.speed;
  document.getElementById("Mder_vel").value = values.speed;
  document.getElementById("Mizq_vel").value = values.speed;
  document.getElementById("LedF").value = values.frontlight;
  document.getElementById("LedB").value = values.backlight;
  document.getElementById("camTilt").value = values.cameratilt;
  document.getElementById("BatteryVoltage").innerHTML = values.battery;
  document.getElementById("MeterBat").value = values.battery;
  if (values.battery<14) {
    document.getElementById("lowBattery").innerHTML = "LOW BATTERY";
  }
  else {
    document.getElementById("lowBattery").innerHTML = "";
  }
}

function gpAvailable() {
  //-------------------------------------------------------------------------------------------------------
  // * Method used to verify the availability of Gamepad API within browser
  //     Returns: true when API is present | false when API is missing
  //-------------------------------------------------------------------------------------------------------
  return "getGamepads" in navigator;
}

$(document).ready(function() {
  console.log("ready");
  if(gpAvailable()) {
    console.log("available");

    $(window).on("gamepaddisconnected", function() {
      console.log("Gamepad disconnected");
      window.clearInterval(repGP);
    });

    var checkGP = window.setInterval(function() {
      console.log("checkGP");
      if(navigator.getGamepads()[0]) {
        console.log("gp Detected");
        if(!hasGP) {
          hasGP = true;
          console.log("gp Triggered");
          repGP = window.setInterval(reportGamepad,200);
          window.clearInterval(checkGP);
        }
      }
    },500);
  }
});

function debug(text) {
  document.getElementById("demo").innerHTML = text;
  console.log(text);
}
