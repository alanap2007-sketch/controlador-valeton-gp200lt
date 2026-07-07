
# 🎸 GUIA COMPLETO - Valeton GP200LT Controller

## 📋 Índice
1. [Requisitos](#requisitos)
2. [Instalação](#instalação)
3. [Conexão de Hardware](#conexão-de-hardware)
4. [Configuração do Código](#configuração-do-código)
5. [Upload e Testes](#upload-e-testes)
6. [Troubleshooting](#troubleshooting)

---

## 📦 Requisitos

### Hardware
- **Microcontrolador**: ESP32-S3 DevKit C-1
- **Display**: ILI9341 TFT LCD (320x240)
- **Comunicação**: MIDI Serial (via RX2/TX2)
- **Entrada**: 4x Botões + Potenciômetros analógicos

### Software
- **PlatformIO** (extensão VSCode)
- **Python 3.x** (para PlatformIO)
- **Visual Studio Code**

---

## 🚀 Instalação

### 1. Instalar dependências

```bash
# Instalar PlatformIO CLI
pip install platformio

# Ou usar extensão VSCode:
# Extensions → Procurar por "PlatformIO IDE" → Instalar
```

### 2. Clonar repositório

```bash
git clone https://github.com/alanap2007-sketch/controlador-valeton-gp200lt.git
cd controlador-valeton-gp200lt
```

### 3. Abrir no VSCode

```bash
code .
```

### 4. Instalar bibliotecas

PlatformIO instala automaticamente ao fazer upload, mas você pode instalar manualmente:

```bash
pio lib install "adafruit/Adafruit GFX Library"
pio lib install "adafruit/Adafruit ILI9341"
pio lib install "FortySevenEffects/MIDI Library"
```

---

## 🔌 Conexão de Hardware

### Display ILI9341 - Interface SPI

| Pino ILI9341 | ESP32-S3 | Função |
|---|---|---|
| VCC | 3V3 | Alimentação 3.3V |
| GND | GND | Terra |
| CS | GPIO 10 | Chip Select |
| DC | GPIO 8 | Data/Command |
| RST | GPIO 9 | Reset |
| CLK | GPIO 12 | Clock SPI |
| MOSI | GPIO 11 | Data Out (Master Out) |
| MISO | GPIO 13 | Data In (Master In) |

**Esquema visual:**

```
ESP32-S3                    ILI9341
┌─────────────┐            ┌──────────┐
│   3V3   ────┼────────────┤ VCC      │
│   GND   ────┼────────────┤ GND      │
│ GPIO 10 ────┼────────────┤ CS       │
│  GPIO 8 ────┼────────────┤ DC       │
│  GPIO 9 ────┼────────────┤ RST      │
│ GPIO 12 ────┼────────────┤ CLK      │
│ GPIO 11 ────┼────────────┤ MOSI     │
│ GPIO 13 ────┼────────────┤ MISO     │
└─────────────┘            └──────────┘
```

### Botões de Entrada

| Botão | ESP32-S3 | Função |
|---|---|---|
| BTN LEFT | GPIO 6 | Preset Anterior |
| BTN RIGHT | GPIO 7 | Próximo Preset |
| BTN ENTER | GPIO 5 | Select/Confirmar |
| BTN MENU | GPIO 4 | Alternar Abas |

**Conexão dos botões:**
```
Botão ───[10kΩ]─── 3V3
  │
ESP32 (GPIO X)
  │
GND
```

### Potenciômetro Analógico (Volume)

```
         +3V3
          │
       [10kΩ]
          │
Pot ──────┼──── GPIO A0
          │
         GND
```

### MIDI Serial (RX2/TX2)

| ESP32-S3 | Função | Pedaleira |
|---|---|---|
| GPIO 16 (RX2) | Recebe MIDI | TX Pedaleira |
| GPIO 17 (TX2) | Envia MIDI | RX Pedaleira |
| GND | Terra | GND Pedaleira |

⚠️ **IMPORTANTE**: Adicione resistor de 220Ω em série com TX se necessário (proteção).

---

## ⚙️ Configuração do Código

### 1. Editar `src/config.h`

Se você precisar alterar os pinos padrão, edite este arquivo:

```cpp
// Display SPI
#define TFT_CS 10      // Pino Chip Select
#define TFT_DC 8       // Pino Data/Command
#define TFT_RST 9      // Pino Reset
#define TFT_MOSI 11    // Pino MOSI
#define TFT_CLK 12     // Pino Clock
#define TFT_MISO 13    // Pino MISO

// Botões
#define BTN_LEFT 6
#define BTN_RIGHT 7
#define BTN_ENTER 5
#define BTN_MENU 4

// Potenciômetros
#define POT_VOLUME A0
```

### 2. Personalizar Cores

Em `src/config.h`, você pode ajustar as cores (RGB565):

```cpp
#define COLOR_BG 0x0000           // Preto
#define COLOR_ACCENT 0x00FF       // Ciano
#define COLOR_ACTIVE 0xF800       // Vermelho
#define COLOR_TEXT 0xFFFF         // Branco

// Cores de efeitos
#define COLOR_EFFECT_GATE 0x07E0  // Verde
#define COLOR_EFFECT_DRV 0xF800   // Vermelho
#define COLOR_EFFECT_DELAY 0x00FF // Ciano
```

### 3. Estrutura do Projeto

```
controlador-valeton-gp200lt/
├── src/
│   ├── main.cpp           # Código principal
│   ├── config.h           # Configurações de pinos e cores
│   ├── ui.cpp             # Funções de interface (futuro)
│   └── midi.cpp           # Funções MIDI (futuro)
├── include/               # Headers adicionais
├── platformio.ini         # Configuração PlatformIO
├── README.md              # Este arquivo
├── docs/
│   ├── MIDI_COMMANDS.md   # Referência MIDI
│   ├── PINOUT.md          # Diagrama de pinos
│   └── SETUP.md           # Este guia de instalação
└── .gitignore
```

---

## 📤 Upload e Testes

### 1. Conectar ESP32-S3

Conecte via cabo USB-C ao computador.

### 2. Verificar porta

```bash
# Linux/Mac
ls /dev/tty.* | grep -i usb

# Windows (usar Device Manager ou)
pio device list
```

### 3. Build e Upload

```bash
# Build do projeto
pio run

# Upload para o ESP32
pio run -t upload

# Monitor serial em tempo real
pio device monitor --baud 115200
```

### 4. Saída esperada no Serial

```
╔════════════════════════════════════════╗
║   VALETON GP200LT CONTROLLER v1.0    ║
║      Display: ILI9341 (320x240)      ║
╚════════════════════════════════════════╝

✓ Display initialized
✓ MIDI initialized on Serial1
✓ Buttons initialized
✓ System Ready!
```

### 5. Testes de Funcionalidade

| Teste | Como | Resultado Esperado |
|---|---|---|
| Display | Ligar ESP32 | Splash screen → Layout principal |
| Botão LEFT | Pressionar | Preset anterior, mensagem no serial |
| Botão RIGHT | Pressionar | Próximo preset |
| Botão ENTER | Pressionar | "Select pressed" no serial |
| Botão MENU | Pressionar | Alternar entre abas (AMP → CAB → EFFECTS...) |
| MIDI | Conectar pedaleira | Mensagens MIDI no serial monitor |

---

## 🔧 Troubleshooting

### Display não inicializa

**Problema**: Tela preta ou sem resposta

**Solução**:
1. Verificar conexão SPI (CLK, MOSI, CS, DC, RST)
2. Confirmar voltagem 3.3V no VCC
3. Testar pino RST com multímetro
4. Ajustar tempo de inicialização em `setup()`:
   ```cpp
   void initDisplay() {
     display.begin();
     delay(5000);  // Aumentar tempo
   }
   ```

### Botões não funcionam

**Problema**: Botões não respondem

**Solução**:
1. Verificar conexão de terra (GND)
2. Testar com multímetro a continuidade
3. Adicionar capacitor 100nF entre botão e GND
4. Aumentar tempo de debounce em `config.h`:
   ```cpp
   #define DEBOUNCE_TIME 100  // de 50 para 100ms
   ```

### MIDI não funciona

**Problema**: Nenhuma mensagem MIDI recebida

**Solução**:
1. Verificar conexão RX/TX
2. Confirmar baudrate (115200)
3. Testar com software MIDI monitor:
   - **Windows**: loopMIDI
   - **Mac**: MIDI Monitor
   - **Linux**: QMidiNet

4. Verificar com Serial Monitor:
   ```bash
   pio device monitor --baud 115200
   ```

5. Se ainda não funcionar, adicionar debug no MIDI:
   ```cpp
   void loop() {
     if (MIDI.read()) {
       Serial.println("MIDI recebido!");
       // ...
     }
   }
   ```

### Caracteres estranhos no Serial

**Problema**: Lixo de dados no monitor serial

**Solução**:
1. Verificar baudrate: deve ser **115200**
2. Reset o ESP32 (botão RST ou conectar/desconectar USB)

### Erro "Board not found"

**Problema**: PlatformIO não encontra ESP32

**Solução**:
1. Instalar driver USB: [CP210x](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
2. Verificar com `pio device list`
3. Especificar porta em `platformio.ini`:
   ```ini
   upload_port = /dev/ttyUSB0  # Linux
   upload_port = COM3           # Windows
   upload_port = /dev/cu.SLAB_USBtoUART  # Mac
   ```

---

## 📚 Referências

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [ILI9341 Controller](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)
- [MIDI Specification](https://www.midi.org/specifications-old)
- [Adafruit ILI9341 Library](https://github.com/adafruit/Adafruit_ILI9341)
- [MIDI Library for Arduino](https://github.com/FortySevenEffects/arduino_midi_library)

---

## 🎓 Próximos Passos

1. ✅ **Configurar hardware** - Conexões prontas
2. ✅ **Upload do código** - Compilar e fazer upload
3. ⏳ **Testar MIDI** - Comunicação com pedaleira
4. ⏳ **Customizar interface** - Adicionar mais abas
5. ⏳ **Melhorar display** - Gráficos e animações

---

## 💡 Dicas Úteis

- Sempre manter backup do código
- Usar Serial Monitor para debug
- Testar conexões com multímetro
- Adicionar capacitores de desacoplamento (100nF) perto de VCC
- Manter fios SPI curtos (< 30cm)

---

## 🤝 Suporte

Se encontrar problemas:

1. Verificar [MIDI_COMMANDS.md](../docs/MIDI_COMMANDS.md)
2. Consultar [PINOUT.md](../docs/PINOUT.md)
3. Abrir issue no GitHub
4. Verificar logs do Serial Monitor

---

**Status do Projeto**: Em Desenvolvimento 🚧

**Última atualização**: Julho 2026

**Autor**: [@alanap2007-sketch](https://github.com/alanap2007-sketch)

---

## 📸 Visualização Esperada

```
┌──────────────────────────────────────┐
│ WiFi      GP200LT        BPM 120 VOL 100% │
├──────────────────────────────────────┤
│ AMP │ CAB │ EFFECTS │ EQ │ SYSTEM    │
├──────────────────────────────────────┤
│                                      │
│ MATCHLESS DC30                       │
│ VINTAGE                              │
│                                      │
│ ●GATE ●COMP ●DRV ●AMP ●CAB ●DELAY  │
│                                      │
│ GAIN: [███████░░] MIDDLE: [████░░░░] │
│ BASS: [█████░░░░] TREBLE: [██████░░] │
│                                      │
│ LEVEL: [████████████░░░░░░]          │
├──────────────────────────────────────┤
│ Preset: 1/100           ONLINE       │
└──────────────────────────────────────┘
```

