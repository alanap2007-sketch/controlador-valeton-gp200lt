#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <MIDI.h>
#include "config.h"

// ============================================================================
// INSTÂNCIAS
// ============================================================================
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// ============================================================================
// ESTRUTURA DE DADOS
// ============================================================================
struct EffectSlot {
  const char* name;
  uint16_t color;
  bool active;
  int x, y, width, height;
};

struct UIState {
  uint8_t currentPreset;
  uint8_t currentTab;  // 0=AMP, 1=CAB, 2=EFFECTS, 3=EQ, 4=SYSTEM
  uint8_t volume;
  uint8_t gain;
  uint8_t bass;
  uint8_t middle;
  uint8_t treble;
  uint8_t level;
  uint8_t bpm;
  bool connected;
  unsigned long lastUpdate;
};

UIState uiState = {0, 0, 100, 65, 50, 60, 70, 80, 120, true, 0};

// Chain de efeitos
EffectSlot effectChain[] = {
  {"GATE", COLOR_EFFECT_GATE, true, 10, 80, 40, 40},
  {"COMP", COLOR_EFFECT_COMP, false, 55, 80, 40, 40},
  {"DRV", COLOR_EFFECT_DRV, true, 100, 80, 40, 40},
  {"AMP", COLOR_EFFECT_AMP, true, 145, 80, 40, 40},
  {"CAB", COLOR_EFFECT_CAB, true, 190, 80, 40, 40},
  {"DELAY", COLOR_EFFECT_DELAY, true, 235, 80, 40, 40},
  {"REVERB", COLOR_EFFECT_REVERB, false, 280, 80, 40, 40}
};

const int numEffects = sizeof(effectChain) / sizeof(EffectSlot);

// ============================================================================
// PROTÓTIPOS
// ============================================================================
void initDisplay();
void initMIDI();
void initButtons();
void updateDisplay();
void drawHeader();
void drawTabs();
void drawAmpTab();
void drawEffectsChain();
void drawParameterPanel();
void drawFooter();
void handleButtons();
void sendProgramChange(uint8_t preset);
void sendControlChange(uint8_t controller, uint8_t value);
void drawBox(int x, int y, int w, int h, uint16_t color, const char* text);
void drawKnob(int x, int y, int size, uint8_t value, const char* label);

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n╔════════════════════════════════════════╗");
  Serial.println("║   VALETON GP200LT CONTROLLER v1.0    ║");
  Serial.println("║      Display: ILI9341 (320x240)      ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  initButtons();
  initDisplay();
  initMIDI();
  
  Serial.println("✓ System Ready!");
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================
void loop() {
  handleButtons();
  
  if (MIDI.read()) {
    midi::MidiType cmd = MIDI.getType();
    uint8_t data1 = MIDI.getData1();
    uint8_t data2 = MIDI.getData2();
    
    Serial.printf("MIDI IN: Type=%d, Data1=%d, Data2=%d\n", cmd, data1, data2);
  }
  
  if (millis() - uiState.lastUpdate >= DISPLAY_UPDATE_RATE) {
    updateDisplay();
    uiState.lastUpdate = millis();
  }
  
  delay(10);
}

// ============================================================================
// INICIALIZAÇÃO DISPLAY
// ============================================================================
void initDisplay() {
  Serial.println("Initializing display...");
  
  // Resetar pino RST
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(200);
  
  Serial.println("RST pulso completo");
  
  // Inicializar SPI
  SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI);
  Serial.println("SPI iniciado");
  
  // Inicializar display
  if (!display.begin()) {
    Serial.println("ERROR: Display initialization failed!");
    while(1) {
      Serial.println("✗ Display failed");
      delay(1000);
    }
  }
  
  Serial.println("Display initialized successfully");
  
  display.setRotation(0);
  display.fillScreen(COLOR_BG);
  
  // Teste de cor - desenhar quadrado ciano
  display.fillRect(0, 0, 50, 50, COLOR_ACCENT);
  Serial.println("Drew test rectangle");
  
  delay(1000);
  
  // Tela de splash
  display.fillScreen(COLOR_BG);
  
  display.setTextColor(COLOR_ACCENT);
  display.setTextSize(3);
  display.setCursor(50, 100);
  display.println("GP200LT");
  
  display.setTextSize(1);
  display.setTextColor(COLOR_TEXT_DIM);
  display.setCursor(80, 150);
  display.println("COMPANION");
  
  display.setCursor(60, 170);
  display.println("Ready!");
  
  delay(2000);
  
  Serial.println("✓ Display initialized");
}

// ============================================================================
// INICIALIZAÇÃO MIDI
// ============================================================================
void initMIDI() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.println("✓ MIDI initialized on Serial1");
}

// ============================================================================
// INICIALIZAÇÃO BOTÕES
// ============================================================================
void initButtons() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_ENTER, INPUT_PULLUP);
  pinMode(BTN_MENU, INPUT_PULLUP);
  
  Serial.println("✓ Buttons initialized");
}

// ============================================================================
// ATUALIZA DISPLAY COMPLETO
// ============================================================================
void updateDisplay() {
  display.fillScreen(COLOR_BG);
  
  drawHeader();
  drawTabs();
  
  switch(uiState.currentTab) {
    case 0: drawAmpTab(); break;
    case 1: break; // CAB Tab
    case 2: drawEffectsChain(); break;
    case 3: break; // EQ Tab
    case 4: break; // SYSTEM Tab
  }
  
  drawParameterPanel();
  drawFooter();
}

// ============================================================================
// DESENHA CABEÇALHO
// ============================================================================
void drawHeader() {
  // Fundo do header
  display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, COLOR_ACCENT_DARK);
  display.drawFastHLine(0, HEADER_HEIGHT - 1, SCREEN_WIDTH, COLOR_ACCENT);
  
  // WiFi Icon
  display.setTextColor(COLOR_ACCENT);
  display.setTextSize(1);
  display.setCursor(5, 5);
  display.print("WiFi");
  
  // Título central
  display.setTextColor(COLOR_TEXT);
  display.setTextSize(2);
  display.setCursor(110, 8);
  display.print("GP200");
  
  // BPM
  display.setTextColor(COLOR_WARN);
  display.setTextSize(1);
  display.setCursor(220, 5);
  display.printf("BPM %d", uiState.bpm);
  
  // Volume
  display.setTextColor(COLOR_TEXT);
  display.setCursor(220, 20);
  display.printf("VOL %d%%", uiState.volume);
}

// ============================================================================
// DESENHA ABAS (TABS)
// ============================================================================
void drawTabs() {
  const char* tabs[] = {"AMP", "CAB", "EFFECTS", "EQ", "SYSTEM"};
  int tabWidth = SCREEN_WIDTH / 5;
  int tabY = HEADER_HEIGHT + 2;
  
  for (int i = 0; i < 5; i++) {
    uint16_t bgColor = (i == uiState.currentTab) ? COLOR_ACCENT : COLOR_INACTIVE;
    uint16_t textColor = (i == uiState.currentTab) ? COLOR_BG : COLOR_TEXT;
    
    display.fillRect(i * tabWidth, tabY, tabWidth, 20, bgColor);
    display.setTextColor(textColor);
    display.setTextSize(1);
    
    int textLen = strlen(tabs[i]) * 6;
    int x = (i * tabWidth) + (tabWidth - textLen) / 2;
    display.setCursor(x, tabY + 6);
    display.print(tabs[i]);
  }
  
  // Linha divisória
  display.drawFastHLine(0, tabY + 20, SCREEN_WIDTH, COLOR_ACCENT);
}

// ============================================================================
// ABA AMP
// ============================================================================
void drawAmpTab() {
  // Seção título
  display.setTextColor(COLOR_ACCENT);
  display.setTextSize(1);
  display.setCursor(10, 70);
  display.println("MATCHLESS DC30");
  
  display.setTextColor(COLOR_WARN);
  display.setCursor(10, 80);
  display.println("VINTAGE");
  
  // Área de efeitos/chain (simplificado)
  display.drawRect(10, 70, 300, 60, COLOR_ACCENT);
}

// ============================================================================
// DESENHA CADEIA DE EFEITOS
// ============================================================================
void drawEffectsChain() {
  display.setTextColor(COLOR_TEXT);
  display.setTextSize(1);
  display.setCursor(10, 70);
  display.println("EFFECTS CHAIN");
  
  // Desenha cada efeito
  for (int i = 0; i < numEffects; i++) {
    EffectSlot* effect = &effectChain[i];
    
    uint16_t boxColor = effect->active ? effect->color : COLOR_INACTIVE;
    
    // Box do efeito
    display.drawRect(effect->x, effect->y, effect->width, effect->height, boxColor);
    
    if (effect->active) {
      display.fillRect(effect->x + 2, effect->y + 2, 
                       effect->width - 4, effect->height - 4, boxColor);
      display.setTextColor(COLOR_BG);
    } else {
      display.setTextColor(boxColor);
    }
    
    // Nome do efeito
    display.setTextSize(1);
    int nameLen = strlen(effect->name) * 6;
    int textX = effect->x + (effect->width - nameLen) / 2;
    display.setCursor(textX, effect->y + 15);
    display.print(effect->name);
  }
}

// ============================================================================
// PAINEL DE PARÂMETROS
// ============================================================================
void drawParameterPanel() {
  int panelY = 130;
  int col1X = 10;
  int col2X = 165;
  int knobSize = 30;
  int spacing = 50;
  
  // Coluna esquerda
  drawKnob(col1X, panelY, knobSize, uiState.gain, "GAIN");
  drawKnob(col1X, panelY + spacing, knobSize, uiState.bass, "BASS");
  
  // Coluna direita
  drawKnob(col2X, panelY, knobSize, uiState.middle, "MIDDLE");
  drawKnob(col2X, panelY + spacing, knobSize, uiState.treble, "TREBLE");
  
  // Parâmetros inferiores
  int bottomY = panelY + spacing + spacing;
  display.setTextColor(COLOR_TEXT);
  display.setTextSize(1);
  
  // Level
  display.setCursor(10, bottomY);
  display.printf("LEVEL: %d", uiState.level);
  display.drawRect(10, bottomY + 12, 150, 10, COLOR_INACTIVE);
  display.fillRect(10, bottomY + 12, (uiState.level * 150) / 127, 10, COLOR_ACCENT);
}

// ============================================================================
// DESENHA KNOB (BOTÃO ROTATIVO)
// ============================================================================
void drawKnob(int x, int y, int size, uint8_t value, const char* label) {
  // Círculo externo
  display.drawCircle(x + size/2, y + size/2, size/2, COLOR_ACCENT);
  
  // Preenchimento do valor
  int fillSize = (size * value) / 127;
  display.fillCircle(x + size/2, y + size/2, fillSize/2, COLOR_ACCENT);
  
  // Rótulo
  display.setTextColor(COLOR_TEXT_DIM);
  display.setTextSize(1);
  int labelLen = strlen(label) * 6;
  display.setCursor(x + size/2 - labelLen/2, y + size + 5);
  display.print(label);
  
  // Valor
  display.setTextColor(COLOR_ACCENT);
  display.setCursor(x + size/2 - 10, y + size/2 - 3);
  display.printf("%d", value);
}

// ============================================================================
// RODAPÉ
// ============================================================================
void drawFooter() {
  int footerY = SCREEN_HEIGHT - FOOTER_HEIGHT;
  
  // Fundo
  display.fillRect(0, footerY, SCREEN_WIDTH, FOOTER_HEIGHT, COLOR_ACCENT_DARK);
  display.drawFastHLine(0, footerY, SCREEN_WIDTH, COLOR_ACCENT);
  
  // Informações
  display.setTextColor(COLOR_TEXT);
  display.setTextSize(1);
  display.setCursor(10, footerY + 5);
  display.printf("Preset: %d/%d", uiState.currentPreset + 1, PRESET_MAX + 1);
  
  // Status conexão
  uint16_t statusColor = uiState.connected ? COLOR_SUCCESS : COLOR_DANGER;
  display.setTextColor(statusColor);
  display.setCursor(280, footerY + 5);
  display.print(uiState.connected ? "ONLINE" : "OFFLINE");
}

// ============================================================================
// BOX GENÉRICO
// ============================================================================
void drawBox(int x, int y, int w, int h, uint16_t color, const char* text) {
  display.drawRect(x, y, w, h, color);
  display.setTextColor(color);
  display.setTextSize(1);
  
  int textLen = strlen(text) * 6;
  int textX = x + (w - textLen) / 2;
  int textY = y + (h / 2) - 4;
  display.setCursor(textX, textY);
  display.print(text);
}

// ============================================================================
// ENVIAR PROGRAM CHANGE (Mudança de Preset)
// ============================================================================
void sendProgramChange(uint8_t preset) {
  MIDI.sendProgramChange(preset, 1);  // Canal 1
  Serial.printf("MIDI OUT: Program Change = %d (Channel 1)\n", preset);
}

// ============================================================================
// ENVIAR CONTROL CHANGE (Parâmetros)
// ============================================================================
void sendControlChange(uint8_t controller, uint8_t value) {
  MIDI.sendControlChange(controller, value, 1);  // Canal 1
  Serial.printf("MIDI OUT: CC #%d = %d (Channel 1)\n", controller, value);
}

// ============================================================================
// TRATAMENTO DE BOTÕES
// ============================================================================
void handleButtons() {
  static unsigned long lastPress[4] = {0, 0, 0, 0};
  
  // Botão LEFT - Preset anterior
  if (digitalRead(BTN_LEFT) == LOW && millis() - lastPress[0] > DEBOUNCE_TIME) {
    if (uiState.currentPreset > 0) {
      uiState.currentPreset--;
      sendProgramChange(uiState.currentPreset);
      Serial.printf("↓ Preset: %d\n", uiState.currentPreset);
    }
    lastPress[0] = millis();
  }
  
  // Botão RIGHT - Próximo preset
  if (digitalRead(BTN_RIGHT) == LOW && millis() - lastPress[1] > DEBOUNCE_TIME) {
    if (uiState.currentPreset < PRESET_MAX) {
      uiState.currentPreset++;
      sendProgramChange(uiState.currentPreset);
      Serial.printf("↑ Preset: %d\n", uiState.currentPreset);
    }
    lastPress[1] = millis();
  }
  
  // Botão ENTER - Select
  if (digitalRead(BTN_ENTER) == LOW && millis() - lastPress[2] > DEBOUNCE_TIME) {
    Serial.println("✓ Select pressed");
    sendControlChange(7, uiState.volume);  // CC 7 = Volume
    lastPress[2] = millis();
  }
  
  // Botão MENU - Próxima aba
  if (digitalRead(BTN_MENU) == LOW && millis() - lastPress[3] > DEBOUNCE_TIME) {
    uiState.currentTab = (uiState.currentTab + 1) % 5;
    Serial.printf("→ Tab: %d\n", uiState.currentTab);
    lastPress[3] = millis();
  }
}
