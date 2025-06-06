#ifndef WEBPAGE_H
#define WEBPAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Tank Drive</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <style>
    html, body {
      margin: 0;
      padding: 0;
      overflow: hidden;
      background-color: #f8f8f8;
      font-family: sans-serif;
    }
    #container {
      display: flex;
      justify-content: space-around;
      height: 100vh;
      align-items: center;
      padding: 0 10vw;
    }
    .zone {
      width: 100px;
      height: 100px;
      background: #e0e0e0;
      border-radius: 50%;
      touch-action: none;
      position: relative;
    }
    .label {
      text-align: center;
      margin-top: 10px;
      font-size: 1.2em;
      font-weight: bold;
    }
    .joystick-container {
      display: flex;
      flex-direction: column;
      align-items: center;
    }
  </style>
</head>
<body>
  <div id="container">
    <div class="joystick-container">
      <div id="left" class="zone"></div>
      <div class="label" id="leftVal">1500</div>
    </div>
    <div class="joystick-container">
      <div id="right" class="zone"></div>
      <div class="label" id="rightVal">1500</div>
    </div>
  </div>

  <script>
    function sendY(side, val) {
      fetch('/joystick?side=' + side + '&y=' + val);
    }

    function setupJoystick(id, side, axis) {
      const zone = document.getElementById(id);
      const label = document.getElementById(id + 'Val');
      let center = 0;
      let dragging = false;
      let lastSent = 1500;

      zone.addEventListener('touchstart', e => {
        dragging = true;
        const rect = zone.getBoundingClientRect();
        center = axis === 'y' ? rect.top + rect.height / 2 : rect.left + rect.width / 2;
      });

      zone.addEventListener('touchmove', e => {
        if (!dragging) return;
        e.preventDefault();
        const touch = e.touches[0];
        let delta = axis === 'y' ? touch.clientY - center : touch.clientX - center;
        delta = Math.max(-100, Math.min(100, delta));
        let output = Math.round(1500 - delta * 5);
        output = Math.max(1000, Math.min(2000, output));
        if (output !== lastSent) {
          sendY(side, output);
          label.textContent = output;
          lastSent = output;
        }
      });

      zone.addEventListener('touchend', e => {
        dragging = false;
        sendY(side, 1500);
        label.textContent = 1500;
        lastSent = 1500;
      });
    }

    setupJoystick('left', 'left', 'x');   // Left joystick = turning (X-axis)
    setupJoystick('right', 'right', 'y'); // Right joystick = forward/backward (Y-axis)
  </script>
</body>
</html>
)rawliteral";

#endif
