#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// CONFIGURAÇÃO DE PINOS - ESP32-S3
// ============================================================================

// Display ILI9341 (SPI)
#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_MOSI 11
#define TFT_CLK 12
#define TFT_MISO 13

// Botões de entrada
#define BTN_LEFT 6      // Botão Esquerda (PREV)
#define BTN_RIGHT 7     // Botão Direita (NEXT)
#define BTN_ENTER 5     // Botão Enter (SELECT)
#define BTN_MENU 4      // Botão Menu

// Encoder rotativo (opcional)
#define ENC_A 3
#define ENC_B 2
#define ENC_BTN 1

// Potenciômetro analógico
#define POT_VOLUME A0
#define POT_PARAM1 A1
#define POT_PARAM2 A2

// MIDI Serial
#define MIDI_SERIAL Serial1  // RX2(16), TX2(17)
#define MIDI_BAUDRATE 115200

// ============================================================================
// CORES TEMA PROFISSIONAL (RGB565)
// ============================================================================

#define COLOR_BG 0x0000              // Preto puro
#define COLOR_ACCENT 0x00FF          // Ciano brilhante
#define COLOR_ACCENT_DARK 0x0077     // Ciano escuro
#define COLOR_ACTIVE 0xF800          // Vermelho
#define COLOR_INACTIVE 0x4208        // Cinza escuro
#define COLOR_TEXT 0xFFFF            // Branco
#define COLOR_TEXT_DIM 0x8410        // Branco fraco
#define COLOR_SUCCESS 0x07E0         // Verde
#define COLOR_WARN 0xFBE0            // Amarelo
#define COLOR_DANGER 0xF800          // Vermelho

// Cores para efeitos
#define COLOR_EFFECT_GATE 0x07E0     // Verde
#define COLOR_EFFECT_COMP 0x00FF     // Ciano
#define COLOR_EFFECT_DRV 0xF800      // Vermelho
#define COLOR_EFFECT_AMP 0xFBE0      // Amarelo
#define COLOR_EFFECT_CAB 0xA145      // Marrom
#define COLOR_EFFECT_DELAY 0x00FF    // Ciano
#define COLOR_EFFECT_REVERB 0xA81F   // Magenta

// ============================================================================
// CONSTANTES DE INTERFACE
// ============================================================================

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define HEADER_HEIGHT 35
#define FOOTER_HEIGHT 30
#define CONTENT_HEIGHT (SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT)

// Posições
#define HEADER_Y 0
#define CONTENT_Y HEADER_HEIGHT
#define FOOTER_Y (SCREEN_HEIGHT - FOOTER_HEIGHT)

// ============================================================================
// DEBOUNCE E TIMING
// ============================================================================

#define DEBOUNCE_TIME 50      // ms
#define BUTTON_HOLD_TIME 500  // ms
#define DISPLAY_UPDATE_RATE 50  // ms
#define MIDI_POLL_RATE 10     // ms

// ============================================================================
// LIMITES E RANGES
// ============================================================================

#define PRESET_MIN 0
#define PRESET_MAX 99
#define VOLUME_MIN 0
#define VOLUME_MAX 127
#define PARAM_MIN 0
#define PARAM_MAX 127

#define BPM_MIN 40
#define BPM_MAX 240

#endif // CONFIG_H
