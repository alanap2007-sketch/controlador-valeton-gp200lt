#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <MIDI.h>

// ============================================================================
// CONFIGURAÇÃO DO DISPLAY ILI9341 (320x240)
// ============================================================================
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Pinos SPI para ESP32-S3
#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_MOSI 11
#define TFT_CLK 12
#define TFT_MISO 13

Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// ============================================================================
// CONFIGURAÇÃO MIDI / COMUNICAÇÃO COM PEDALEIRA
// ============================================================================
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// ============================================================================
// ESTRUTURA DE DADOS
// ============================================================================
struct PedalState {
  uint8_t currentPreset;
  uint8_t volume;
  bool bypass;
  uint8_t activeEffects[10];
  uint8_t parameterValues[20];
};

PedalState pedalState = {0, 100, false, {0}, {0}};

// ============================================================================
// CORES (TEMA PROFISSIONAL - RGB565)
// ============================================================================
#define COLOR_BG ILI9341_BLACK        // Preto
#define COLOR_ACCENT ILI9341_CYAN     // Ciano
#define COLOR_ACTIVE ILI9341_RED      // Vermelho
#define COLOR_INACTIVE 0x4208         // Cinza escuro
#define COLOR_TEXT ILI9341_WHITE      // Branco
#define COLOR_SUCCESS ILI9341_GREEN   // Verde
#define COLOR_WARN ILI9341_YELLOW     // Amarelo

// ============================================================================
// PROTÓTIPOS DE FUNÇÃO
// ============================================================================
void initDisplay();
void initMIDI();
void updateDisplay();
void drawHeader();
void drawPresetSelector();
void drawEffectsList();
void drawParameterControls();
void drawStatusBar();
void sendMIDICommand(uint8_t command, uint8_t data1, uint8_t data2);
void handleButtonInput();
void drawWaveform();

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== Valeton GP200LT Controller ===");
  Serial.println("Display: ILI9341 (320x240)");
  Serial.println("Initializing...");
  
  // Inicializa display
  initDisplay();
  
  // Inicializa comunicação MIDI
  initMIDI();
  
  // Configurar entradas (botões, sensores)
  pinMode(A0, INPUT);  // Knob rotativo
  pinMode(6, INPUT_PULLUP);  // Botão esquerda
  pinMode(7, INPUT_PULLUP);  // Botão direita
  pinMode(5, INPUT_PULLUP);  // Botão enter
  
  Serial.println("System Ready!");
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================
void loop() {
  handleButtonInput();
  
  // Processar mensagens MIDI da pedaleira
  if (MIDI.read()) {
    uint8_t channel = MIDI.getChannel();
    uint8_t command = MIDI.getType();
    
    Serial.printf("MIDI Received: Type=%d, Data1=%d, Data2=%d\n", 
                  command, MIDI.getData1(), MIDI.getData2());
  }
  
  updateDisplay();
  delay(50);
}

// ============================================================================
// INICIALIZAÇÃO DO DISPLAY
// ============================================================================
void initDisplay() {
  display.begin();
  
  // Tela de inicialização
  display.fillScreen(COLOR_BG);
  display.setTextColor(COLOR_TEXT);
  display.setTextSize(3);
  display.setCursor(40, 100);
  display.println("GP200LT");
  display.setTextSize(1);
  display.setCursor(80, 150);
  display.println("Controller v1.0");
  display.setCursor(60, 170);
  display.println("Initializing Display...");
  
  delay(2000);
  
  display.fillScreen(COLOR_BG);
  Serial.println("Display initialized successfully!");
}

// ============================================================================
// INICIALIZAÇÃO MIDI
// ============================================================================
void initMIDI() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.println("MIDI Initialized on Serial1 (RX2/TX2)");
}

// ============================================================================
// ATUALIZA DISPLAY
// ============================================================================
void updateDisplay() {
  display.fillScreen(COLOR_BG);
  
  drawHeader();
  drawPresetSelector();
  drawEffectsList();
  drawParameterControls();
  drawStatusBar();
}

// ============================================================================
// DESENHA CABEÇALHO
// ============================================================================
void drawHeader() {
  // Linha superior
  display.drawFastHLine(0, 30, SCREEN_WIDTH, COLOR_ACCENT);
  display.setTextColor(COLOR_ACCENT);
  display.setTextSize(2);
  display.setCursor(10, 8);
  display.println("VALETON GP200LT");
}

// ============================================================================
// SELETOR DE PRESETS
// ============================================================================
void drawPresetSelector() {
  int y = 45;
  int boxWidth = 70;
  int boxHeight = 50;
  int spacing = 80;
  
  display.setTextColor(COLOR_TEXT);
  display.setTextSize(1);
  display.setCursor(10, y - 15);
  display.println("PRESETS");
  
  // Mostrar 4 presets na primeira linha
  for (int i = 0; i < 4; i++) {
    int x = 10 + (i * spacing);
    uint16_t color = (pedalState.currentPreset == i) ? COLOR_ACTIVE : COLOR_INACTIVE;
    
    display.drawRect(x, y, boxWidth, boxHeight, color);
    if (pedalState.currentPreset == i) {
      display.fillRect(x + 2, y + 2, boxWidth - 4, boxHeight - 4, COLOR_ACTIVE);
      display.setTextColor(COLOR_BG);
    } else {
      display.setTextColor(color);
    }
    display.setTextSize(2);
    display.setCursor(x + 25, y + 15);
    display.printf("%d", i + 1);
  }
}

// ============================================================================
// LISTA DE EFEITOS
// ============================================================================
void drawEffectsList() {
  int y = 110;
  display.setTextColor(COLOR_ACCENT);
  display.setTextSize(1);
  display.setCursor(10, y);
  display.println("ACTIVE EFFECTS:");
  
  y += 15;
  
  const char* effects[] = {"DRIVE", "MODULATION", "DELAY", "REVERB"};
  bool status[] = {true, false, true, false};
  
  for (int i = 0; i < 4; i++) {
    uint16_t color = status[i] ? COLOR_SUCCESS : COLOR_INACTIVE;
    display.setTextColor(color);
    display.setTextSize(1);
    display.setCursor(20, y + (i * 13));
    display.printf("● %s", effects[i]);
  }
}

// ============================================================================
// CONTROLES DE PARÂMETROS
// ============================================================================
void drawParameterControls() {
  int y = 180;
  display.setTextColor(COLOR_ACCENT);
  display.setTextSize(1);
  display.setCursor(10, y);
  display.println("VOLUME");
  
  // Barra de volume
  int barWidth = 150;
  int barHeight = 15;
  int volumePos = (pedalState.volume * barWidth) / 100;
  
  display.drawRect(50, y + 5, barWidth, barHeight, COLOR_INACTIVE);
  display.fillRect(50, y + 5, volumePos, barHeight, COLOR_ACCENT);
  
  display.setTextColor(COLOR_TEXT);
  display.setTextSize(1);
  display.setCursor(210, y + 10);
  display.printf("%d%%", pedalState.volume);
}

// ============================================================================
// BARRA DE STATUS
// ============================================================================
void drawStatusBar() {
  int y = SCREEN_HEIGHT - 20;
  
  display.drawFastHLine(0, y, SCREEN_WIDTH, COLOR_ACCENT);
  display.setTextColor(COLOR_ACCENT);
  display.setTextSize(1);
  display.setCursor(10, y + 5);
  display.println("< PREV | SELECT | NEXT >");
}

// ============================================================================
// ENVIAR COMANDO MIDI
// ============================================================================
void sendMIDICommand(uint8_t command, uint8_t data1, uint8_t data2) {
  MIDI.send(command, data1, data2, 1);
  Serial.printf("MIDI Sent: Cmd=0x%02X, D1=%d, D2=%d\n", command, data1, data2);
}

// ============================================================================
// TRATAMENTO DE ENTRADA
// ============================================================================
void handleButtonInput() {
  // Botão esquerda (PREV)
  if (digitalRead(6) == LOW) {
    delay(20);
    if (digitalRead(6) == LOW) {
      if (pedalState.currentPreset > 0) {
        pedalState.currentPreset--;
        sendMIDICommand(0xC0, pedalState.currentPreset, 0);
        Serial.printf("Preset changed to: %d\n", pedalState.currentPreset);
        delay(200);
      }
    }
  }
  
  // Botão direita (NEXT)
  if (digitalRead(7) == LOW) {
    delay(20);
    if (digitalRead(7) == LOW) {
      if (pedalState.currentPreset < 127) {
        pedalState.currentPreset++;
        sendMIDICommand(0xC0, pedalState.currentPreset, 0);
        Serial.printf("Preset changed to: %d\n", pedalState.currentPreset);
        delay(200);
      }
    }
  }
  
  // Botão enter (SELECT)
  if (digitalRead(5) == LOW) {
    delay(20);
    if (digitalRead(5) == LOW) {
      // Ação para botão select
      Serial.println("SELECT button pressed");
      delay(200);
    }
  }
}
