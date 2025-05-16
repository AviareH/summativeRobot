#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char* ssid = "RobotController";
const char* password = "gandalf";

WiFiServer server(80);
Servo motor;

#define MOTOR_PIN D5

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  delay(100);

  Serial.println("AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();

  motor.attach(MOTOR_PIN);
  motor.writeMicroseconds(1500); //rest 
}

void loop() {
  WiFiClient client = server.accept();
  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.flush();

    // Handle joystick Y input
    if (request.indexOf("/joystick?y=") != -1) {
      int yIndex = request.indexOf("y=") + 2;
      int endIndex = request.indexOf(' ', yIndex);
      String yValStr = request.substring(yIndex, endIndex);
      int yVal = yValStr.toInt();

      Serial.print("Joystick Y: ");
      Serial.println(yVal);

      yVal = constrain(yVal, 1000, 2000);  
      motor.writeMicroseconds(yVal);
    }

    // Serve HTML with embedded JS
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP8266 Joystick</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <style>
    html, body {
      margin: 0;
      padding: 0;
      overflow: hidden;
      background-color: #f8f8f8;
      font-family: sans-serif;
    }
    #landscape {
      display: none;
      height: 100vh;
      width: 100vw;
      justify-content: center;
      align-items: center;
      flex-direction: column;
    }
    #rotateMessage {
      display: none;
      height: 100vh;
      width: 100vw;
      justify-content: center;
      align-items: center;
      text-align: center;
      padding: 20px;
    }
    #zone {
      width: 200px;
      height: 200px;
      background: #e0e0e0;
      border-radius: 50%;
    }
  </style>
</head>
<body>
  <div id="landscape">
    <h2>Joystick Y Control</h2>
    <div id="zone"></div>
  </div>
  <div id="rotateMessage">
    <h2>Please rotate your device to landscape mode</h2>
  </div>

  <script>
    function updateOrientation() {
      const landscape = window.innerWidth > window.innerHeight;
      document.getElementById("landscape").style.display = landscape ? "flex" : "none";
      document.getElementById("rotateMessage").style.display = landscape ? "none" : "flex";
    }

    window.addEventListener("resize", updateOrientation);
    window.addEventListener("orientationchange", updateOrientation);
    updateOrientation();

    var zone = document.getElementById("zone");
    var centerX, centerY, dragging = false;
    var lastSent = 1500;

    zone.addEventListener("touchstart", function(e) {
      dragging = true;
      var rect = zone.getBoundingClientRect();
      centerX = rect.left + rect.width / 2;
      centerY = rect.top + rect.height / 2;
    });

    zone.addEventListener("touchmove", function(e) {
      if (!dragging) return;
      var touch = e.touches[0];
      var dy = touch.clientY - centerY;
      dy = Math.max(-100, Math.min(100, dy));
      var output = Math.round(1500 - dy * 5);
      output = Math.max(1000, Math.min(2000, output));
      if (output !== lastSent) {
        fetch("/joystick?y=" + output);
        lastSent = output;
      }
    });

    zone.addEventListener("touchend", function() {
      dragging = false;
      fetch("/joystick?y=1500");
      lastSent = 1500;
    });
  </script>
</body>
</html>
)rawliteral");

    delay(1);
    client.stop();
  }
}
