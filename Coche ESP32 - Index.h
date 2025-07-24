const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html>
<head>
<title>Control de Coche ESP32</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta charset="UTF-8">
<style>
body {
  font-family: Arial, sans-serif;
  text-align: center;
  background: #f0f0f0;
  padding: 20px;
}

#container {
  width: 300px;
  height: 300px;
  margin: 20px auto;
  position: relative;
}

.control-btn {
  position: absolute;
  width: 80px;
  height: 80px;
  border-radius: 50%;
  background: #3498db;
  color: white;
  border: none;
  font-size: 24px;
  cursor: pointer;
  transition: all 0.2s;
}

.control-btn:active {
  transform: scale(0.95);
}

#forward {
  top: 10px;
  left: 50%;
  transform: translateX(-50%);
}

#backward {
  bottom: 10px;
  left: 50%;
  transform: translateX(-50%);
}

#left {
  top: 50%;
  left: 10px;
  transform: translateY(-50%);
}

#right {
  top: 50%;
  right: 10px;
  transform: translateY(-50%);
}

#stop {
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  background: #e74c3c;
  width: 70px;
  height: 70px;
}

#status {
  margin: 20px;
  padding: 10px;
  border-radius: 5px;
  background: #2ecc71;
  color: white;
  display: inline-block;
}
</style>
</head>
<body>
<h2>Control Remoto ESP32</h2>

<div id="container">
  <button id="forward" class="control-btn">↑</button>
  <button id="backward" class="control-btn">↓</button>
  <button id="left" class="control-btn">←</button>
  <button id="right" class="control-btn">→</button>
  <button id="stop" class="control-btn">■</button>
</div>

<div id="status">Conectando...</div>

<script>
// Mapeo de comandos
const COMMANDS = {
  'forward': '1',
  'backward': '2',
  'left': '4',
  'right': '8',
  'stop': '0'
};

let ws = null;
let activeBtn = null;

// Conexión WebSocket automática
function connectWebSocket() {
  ws = new WebSocket(`ws://${window.location.hostname}/ws`);
  
  ws.onopen = () => {
    document.getElementById('status').textContent = "Conectado";
    document.getElementById('status').style.background = "#2ecc71";
  };
  
  ws.onerror = (error) => {
    document.getElementById('status').textContent = "Error de conexión";
    document.getElementById('status').style.background = "#e74c3c";
  };
  
  ws.onclose = () => {
    document.getElementById('status').textContent = "Desconectado";
    document.getElementById('status').style.background = "#f39c12";
    setTimeout(connectWebSocket, 2000); // Reconectar después de 2 segundos
  };
  
  ws.onmessage = (event) => {
    console.log("Respuesta:", event.data);
  };
}

// Manejo de controles
function setupControls() {
  const buttons = document.querySelectorAll('.control-btn');
  
  buttons.forEach(btn => {
    // Eventos para mouse
    btn.addEventListener('mousedown', () => {
      sendCommand(btn.id);
      btn.style.opacity = '0.8';
    });
    
    btn.addEventListener('mouseup', () => {
      sendCommand('stop');
      btn.style.opacity = '1';
    });
    
    btn.addEventListener('mouseleave', () => {
      if(btn === activeBtn) {
        sendCommand('stop');
        btn.style.opacity = '1';
      }
    });
    
    // Eventos para pantalla táctil
    btn.addEventListener('touchstart', (e) => {
      e.preventDefault();
      sendCommand(btn.id);
      btn.style.opacity = '0.8';
    });
    
    btn.addEventListener('touchend', (e) => {
      e.preventDefault();
      sendCommand('stop');
      btn.style.opacity = '1';
    });
  });
}

function sendCommand(cmd) {
  if(ws && ws.readyState === WebSocket.OPEN) {
    const commandCode = COMMANDS[cmd] || '0';
    ws.send(commandCode);
    activeBtn = cmd !== 'stop' ? document.getElementById(cmd) : null;
  }
}

// Inicialización
window.onload = () => {
  connectWebSocket();
  setupControls();
};
</script>
</body>
</html>
)=====";
