# Connection Diagram (ESP32 → Relay → Appliances)

ESP32 5V  → Relay VCC  
ESP32 GND → Relay GND  

GPIO5  → IN1 (Kitchen Light)  
GPIO18 → IN2 (Hall Light)  
GPIO19 → IN3 (Hall Fan)  
GPIO21 → IN4 (Reading Light)  

# AC Wiring (Critical)

 Mains LIVE  → Relay COM  
 Relay NO    → Appliance LIVE  
 Mains NEUTRAL → Appliance NEUTRAL directly

# Notes / Safety

ESP32 and relays must share GND
Use proper insulation for AC wiring
Keep ESP32 powered via USB/5V adapter
If relays click weakly, use a separate 5V supply