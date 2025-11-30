# Smart-Home-Control-Center
A browser-based dashboard to control home devices using ESP32 + HTTP API. Supports voice commands, click/tap control, real-time status updates, and custom commands.

---

## ⚡ Features
- Connect to ESP32 over WiFi using its LAN IP  
- Control lights and fan with click/tap toggles  
- Voice command support using SpeechRecognition API  
- Simulated voice commands when unsupported  
- Real-time status sync using `/status`  
- All ON / All OFF global controls  
- Custom command input  
- Debug log with timestamps  
- Fully responsive mobile-friendly UI  

---

## 🚀 How to Run

### 1. Flash the ESP32
1. Open Arduino IDE  
2. Load `esp.ino`  
3. Select:
   - Board: **ESP32 Dev Module**
   - Correct COM Port  
4. Upload firmware  
5. Open **Serial Monitor**  
6. Note the printed IP address:


### Option B — GitHub Pages Hosting
1. Create a GitHub repository  
2. Upload:
3. Go to **Settings → Pages**  
4. Select:
- Source: **Main branch**
5. Open the generated public URL  

---

### 3. Connect to ESP32
1. Open the webpage  
2. Enter ESP32 IP from Serial Monitor  
3. Click **Connect**  
4. If successful:
- Status indicator turns green  
- Devices load from `/status`  

If it fails:
- Wrong IP  
- ESP32 not on same WiFi  
- Router blocking device-to-device communication  

---

## 🕹️ How to Use

### Device Toggles
Click any card to toggle:
- Kitchen Light  
- Hall Light  
- Hall Fan  
- Reading Light  

### Voice Commands
Click microphone → speak:
