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
    }
  </style>
</head>
<body>
  <div id="container">
    <div id="left" class="zone"></div>
    <div id="right" class="zone"></div>
  </div>

  <script>
    function sendY(side, y) {
      fetch('/joystick?side=' + side + '&y=' + y);
    }

    function setupJoystick(id, side) {
      const zone = document.getElementById(id);
      let centerY = 0;
      let dragging = false;
      let lastSent = 1500;

      zone.addEventListener('touchstart', e => {
        dragging = true;
        const rect = zone.getBoundingClientRect();
        centerY = rect.top + rect.height / 2;
      });

      zone.addEventListener('touchmove', e => {
        if (!dragging) return;
        e.preventDefault();
        const touch = e.touches[0];
        let dy = touch.clientY - centerY;
        dy = Math.max(-100, Math.min(100, dy));
        let output = Math.round(1500 - dy * 5);
        output = Math.max(1000, Math.min(2000, output));
        if (output !== lastSent) {
          sendY(side, output);
          lastSent = output;
        }
      });

      zone.addEventListener('touchend', e => {
        dragging = false;
        sendY(side, 1500);
        lastSent = 1500;
      });
    }

    setupJoystick('left', 'left');
    setupJoystick('right', 'right');
  </script>
</body>
</html>
)rawliteral";

#endif
