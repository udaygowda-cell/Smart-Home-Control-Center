let esp32BaseUrl = '';
let isConnected = false;

function debugLog(message) {
    const debugElement = document.getElementById('debugLog');
    const timestamp = new Date().toLocaleTimeString();
    debugElement.innerHTML = `[${timestamp}] ${message}<br>${debugElement.innerHTML}`;
}

function updateConnectionStatus(message, type) {
    const statusElement = document.getElementById('connectionStatus');
    const icons = {
        connected: '✅',
        disconnected: '🔴',
        error: '⚠️'
    };
    const icon = icons[type] || icons.error;
    statusElement.innerHTML = `<span style="font-size: 1.1em;">${icon}</span> <span>${message}</span>`;
    statusElement.className = `connection-status status-${type}`;
}

function connectToESP32() {
    const ip = document.getElementById('esp32Ip').value.trim();
    if (!ip) {
        updateConnectionStatus('❌ Please enter ESP32 IP address', 'error');
        return;
    }

    esp32BaseUrl = `http://${ip}`;
    debugLog(`Trying to connect to: ${esp32BaseUrl}`);
    updateConnectionStatus('🟡 Connecting...', 'error');

    // Test connection with timeout
    testConnection();
}

function testConnection() {
    if (!esp32BaseUrl) {
        updateConnectionStatus('❌ Please enter IP address first', 'error');
        return;
    }

    debugLog('Testing connection to /test endpoint...');
    
    fetch(`${esp32BaseUrl}/test`, { 
        method: 'GET',
        mode: 'cors',
        headers: {
            'Content-Type': 'application/json'
        }
    })
    .then(response => {
        debugLog(`Response status: ${response.status}`);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
    })
    .then(data => {
        isConnected = true;
        updateConnectionStatus('Connected to ESP32!', 'connected');
        debugLog(`Connection successful: ${data.message}`);
        getStatus(); // Get initial status
    })
    .catch(error => {
        isConnected = false;
        updateConnectionStatus('❌ Connection failed - Check IP/Network', 'error');
        debugLog(`Connection failed: ${error.message}`);
        
        // Try without CORS as fallback
        debugLog('Trying fallback connection...');
        fetch(`${esp32BaseUrl}/test`)
            .then(response => response.text())
            .then(data => {
                debugLog(`Fallback successful: ${data}`);
                isConnected = true;
                updateConnectionStatus('Connected (fallback mode)', 'connected');
            })
            .catch(fallbackError => {
                debugLog(`Fallback also failed: ${fallbackError.message}`);
            });
    });
}

function startVoiceRecognition() {
    if (!isConnected) {
        alert('Please connect to ESP32 first!');
        return;
    }

    const btn = document.querySelector('.voice-btn');
    const status = document.getElementById('voiceStatus');
    
    btn.classList.add('listening');
    status.innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">🎤</span><span>Listening... Speak your command now!</span>';
    status.className = 'status-section status-connected';
    
    // Check if browser supports speech recognition
    if (!('webkitSpeechRecognition' in window) && !('SpeechRecognition' in window)) {
        setTimeout(() => {
            btn.classList.remove('listening');
            status.innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">ℹ️</span><span>Speech recognition not supported. Using simulated mode.</span>';
            simulateVoiceRecognition();
        }, 1000);
        return;
    }
    
    const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
    const recognition = new SpeechRecognition();
    
    recognition.continuous = false;
    recognition.interimResults = false;
    recognition.lang = 'en-US';
    
    recognition.onstart = function() {
        status.innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">🎤</span><span>Speech recognition activated. Speak now...</span>';
    };
    
    recognition.onresult = function(event) {
        const transcript = event.results[0][0].transcript.toLowerCase();
        btn.classList.remove('listening');
        status.innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">👂</span><span>Heard: "' + transcript + '"</span>';
        debugLog(`Voice command: ${transcript}`);
        processVoiceCommand(transcript);
    };
    
    recognition.onerror = function(event) {
        btn.classList.remove('listening');
        status.innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">⚠️</span><span>Error: ' + event.error + '</span>';
        debugLog(`Speech recognition error: ${event.error}`);
        simulateVoiceRecognition();
    };
    
    recognition.onend = function() {
        btn.classList.remove('listening');
    };
    
    recognition.start();
}

function simulateVoiceRecognition() {
    const commands = [
        'turn on kitchen light', 'turn off kitchen light',
        'turn on hall light', 'turn off hall light',
        'turn on hall fan', 'turn off hall fan', 
        'turn on reading light', 'turn off reading light',
        'all on', 'all off', 'status'
    ];
    const randomCommand = commands[Math.floor(Math.random() * commands.length)];
    
    const status = document.getElementById('voiceStatus');
    status.innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">🤖</span><span>Simulated command: "' + randomCommand + '"</span>';
    debugLog(`Simulated command: ${randomCommand}`);
    processVoiceCommand(randomCommand);
}

function processVoiceCommand(command) {
    let device = '';
    let action = '';
    
    if (command.includes('kitchen light')) {
        device = 'kitchen_light';
        action = command.includes('turn on') || command.includes('on') ? 'on' : 'off';
    }
    else if (command.includes('hall light')) {
        device = 'hall_light';
        action = command.includes('turn on') || command.includes('on') ? 'on' : 'off';
    }
    else if (command.includes('hall fan')) {
        device = 'hall_fan';
        action = command.includes('turn on') || command.includes('on') ? 'on' : 'off';
    }
    else if (command.includes('reading light')) {
        device = 'reading_light';
        action = command.includes('turn on') || command.includes('on') ? 'on' : 'off';
    }
    else if (command.includes('all on')) {
        device = 'all';
        action = 'on';
    }
    else if (command.includes('all off')) {
        device = 'all';
        action = 'off';
    }
    else if (command.includes('status')) {
        getStatus();
        return;
    }
    else {
        sendVoiceCommand(command);
        return;
    }
    
    sendControlCommand(device, action);
}

function sendVoiceCommand(command) {
    debugLog(`Sending voice command: ${command}`);
    
    fetch(`${esp32BaseUrl}/voice?command=${encodeURIComponent(command)}`)
        .then(response => response.json())
        .then(data => {
            document.getElementById('voiceStatus').innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">✅</span><span>Response: ' + data.message + '</span>';
            debugLog(`Voice response: ${data.message}`);
            updateAllStatus();
        })
        .catch(error => {
            console.error('Error:', error);
            document.getElementById('voiceStatus').innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">❌</span><span>Error sending command</span>';
            debugLog(`Voice command error: ${error.message}`);
        });
}

function sendControlCommand(device, action) {
    debugLog(`Sending control: device=${device}, action=${action}`);
    
    fetch(`${esp32BaseUrl}/control?device=${encodeURIComponent(device)}&action=${encodeURIComponent(action)}`)
        .then(response => response.json())
        .then(data => {
            document.getElementById('voiceStatus').innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">✅</span><span>Response: ' + data.message + '</span>';
            debugLog(`Control response: ${data.message}`);
            updateAllStatus();
        })
        .catch(error => {
            console.error('Error:', error);
            document.getElementById('voiceStatus').innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">❌</span><span>Error sending command</span>';
            debugLog(`Control command error: ${error.message}`);
        });
}

function toggleDevice(device) {
    if (!isConnected) {
        alert('Not connected to ESP32');
        return;
    }
    
    sendControlCommand(device, 'toggle');
}

function controlAll(action) {
    if (!isConnected) {
        alert('Not connected to ESP32');
        return;
    }
    
    sendControlCommand('all', action);
}

function sendCustomCommand() {
    const command = document.getElementById('customCommand').value;
    if (command.trim()) {
        debugLog(`Sending custom command: ${command}`);
        processVoiceCommand(command);
        document.getElementById('customCommand').value = '';
    }
}

function getStatus() {
    if (!isConnected) {
        alert('Not connected to ESP32');
        return;
    }
    
    debugLog('Fetching device status...');
    
    fetch(`${esp32BaseUrl}/status`)
        .then(response => response.json())
        .then(data => {
            updateDeviceStatus(data);
            document.getElementById('voiceStatus').innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">✅</span><span>Status updated successfully</span>';
            debugLog('Status updated successfully');
        })
        .catch(error => {
            console.error('Error:', error);
            document.getElementById('voiceStatus').innerHTML = '<span style="font-size: 1.1em; margin-right: 8px;">❌</span><span>Error getting status</span>';
            debugLog(`Status error: ${error.message}`);
        });
}

function updateDeviceStatus(status) {
    // Update kitchen light
    updateDeviceUI('kitchen', status.kitchen_light);
    // Update hall light
    updateDeviceUI('hallLight', status.hall_light);
    // Update hall fan
    updateDeviceUI('hallFan', status.hall_fan);
    // Update reading light
    updateDeviceUI('reading', status.reading_light);
}

function updateDeviceUI(device, isOn) {
    const statusElement = document.getElementById(device + 'Status');
    const cardElement = document.getElementById(device + 'Card');
    
    // Add transition effect
    cardElement.style.transition = 'all 0.3s ease';
    
    if (isOn) {
        statusElement.textContent = 'ON';
        statusElement.className = 'device-status status-on';
        cardElement.classList.add('on');
        // Add a subtle glow effect
        cardElement.style.boxShadow = '0 0 20px rgba(16, 185, 129, 0.3)';
    } else {
        statusElement.textContent = 'OFF';
        statusElement.className = 'device-status status-off';
        cardElement.classList.remove('on');
        cardElement.style.boxShadow = '';
    }
}

function updateAllStatus() {
    if (isConnected) {
        getStatus();
    }
}

// Auto-save IP
window.addEventListener('load', function() {
    const savedIp = localStorage.getItem('esp32Ip');
    if (savedIp) {
        document.getElementById('esp32Ip').value = savedIp;
        debugLog('Loaded saved IP: ' + savedIp);
    }
});

// Save IP when connecting
const originalConnect = connectToESP32;
connectToESP32 = function() {
    const ip = document.getElementById('esp32Ip').value.trim();
    localStorage.setItem('esp32Ip', ip);
    originalConnect();
}

