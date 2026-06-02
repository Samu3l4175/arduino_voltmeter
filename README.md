# 🔌 Voltmetro con Arduino e Display LCD

Progetto basato su: [Arduino Voltmeter – SetNFix](https://projecthub.arduino.cc/SetNFix/arduino-voltmeter-3bf7f4)

---

## 📋 Descrizione

Questo progetto realizza un **voltmetro digitale** utilizzando un Arduino Uno e un display LCD 16x2. Il circuito è in grado di misurare tensioni fino a circa **55V** (con le resistenze specificate), sfruttando un partitore di tensione per ricondurre il segnale entro il range accettabile dall'ingresso analogico dell'Arduino (0–5V), e mostrare il risultato sul display.

---

## 🧰 Materiali Necessari

| Componente                 | Quantità | Note                                      |
| -------------------------- | -------- | ----------------------------------------- |
| Arduino Uno Rev3           | 1        | Microcontrollore principale               |
| Display LCD 16x2 (HD44780) | 1        | Compatibile con la libreria LiquidCrystal |
| Resistenza 100kΩ (R1)      | 1        | Parte alta del partitore di tensione      |
| Resistenza 10kΩ (R2)       | 1        | Parte bassa del partitore di tensione     |
| Resistenza 120Ω            | 1        | Per la retroilluminazione del display LCD |
| Potenziometro 10kΩ         | 1        | Per regolare il contrasto del display     |
| Breadboard                 | 1        | Per il montaggio senza saldature          |
| Cavi jumper                | q.b.     | Per i collegamenti                        |

---

## ⚡ Teoria: Il Partitore di Tensione

### Il Problema

L'ingresso analogico di Arduino accetta tensioni **tra 0V e 5V**. Se vogliamo misurare tensioni più alte (es. 12V, 24V…) dobbiamo prima ridurle proporzionalmente, senza alterarne la forma. Lo strumento ideale per farlo è il **partitore di tensione**.

### Il Circuito

```
  VIN ──┬── R1 (100kΩ) ──┬── VOUT (→ A0 di Arduino)
        │                │
       GND             R2 (10kΩ)
                         │
                     3   GND
```

Due resistenze in serie dividono la tensione di ingresso. La tensione misurata sul nodo centrale (tra R1 e R2) è proporzionale a quella di ingresso.

### Derivazione della Formula

Applicando la **Legge di Ohm** e la **Legge di Kirchhoff** al circuito:

La corrente che scorre nel ramo è unica (serie):

```
I = VIN / (R1 + R2)
```

La caduta di tensione su R2 vale:

```
VOUT = I × R2 = VIN × R2 / (R1 + R2)
```

Quindi, invertendo per ricavare VIN:

```
VIN = VOUT × (R1 + R2) / R2
```

Oppure equivalentemente:

```
VIN = VOUT / (R2 / (R1 + R2))
```

> 💡 Questa è esattamente la formula usata nel codice Arduino.

Con R1 = 100kΩ e R2 = 10kΩ:

```
VIN = VOUT × (100000 + 10000) / 10000 = VOUT × 11
```

Il circuito quindi **divide la tensione per 11**, permettendo di misurare tensioni fino a 5V × 11 = **55V** (range massimo teorico).

### Conversione ADC → Tensione

Arduino ha un ADC (Convertitore Analogico-Digitale) a **10 bit**, che restituisce un valore intero da 0 a 1023, proporzionale alla tensione sull'ingresso (0–5V):

```
VOUT = valore_ADC × (5.0 / 1024)
```

Combinando le due formule si ottiene la tensione reale:

```
VIN = (valore_ADC × 5.0 / 1024) / (R2 / (R1 + R2))
```

---

## 💻 Codice Commentato

```cpp
// Inclusione della libreria per gestire il display LCD
#include <LiquidCrystal.h>

// --- Definizione dei pin LCD ---
// RS = pin 12, EN = pin 11, D4..D7 = pin 5,4,3,2
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

// Inizializzazione dell'oggetto LCD con i pin definiti sopra
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// --- Variabili globali ---
int analogInput = 0;       // Pin analogico A0 dove è collegato il partitore
float vout = 0.0;          // Tensione letta dall'ADC (0–5V), già convertita in volt
float vin  = 0.0;          // Tensione reale in ingresso (calcolata tramite partitore)
float R1 = 100000.0;       // Resistenza R1 = 100kΩ (parte alta del partitore)
float R2 =  10000.0;       // Resistenza R2 = 10kΩ (parte bassa del partitore)
int value = 0;             // Valore grezzo restituito dall'ADC (0–1023)

// -------------------------------------------------------
void setup() {
  lcd.begin(16, 2);            // Inizializza il display: 16 colonne, 2 righe
  lcd.print("Samuel Iuele");   // Messaggio di benvenuto alla prima accensione
  pinMode(analogInput, INPUT); // Imposta A0 come ingresso analogico
  Serial.begin(9600);          // Avvia la comunicazione seriale (utile per debug)
  delay(5000);                 // Pausa di 5 secondi per mostrare il messaggio iniziale
}

// -------------------------------------------------------
void loop() {

  // 1. Lettura del valore grezzo dall'ADC (intero da 0 a 1023)
  value = analogRead(analogInput);

  // 2. Conversione ADC → tensione al pin A0 (VOUT del partitore)
  //    Formula: VOUT = valore × (Vref / risoluzione) = valore × 5.0 / 1024
  vout = (value * 5.0) / 1024.0;

  // 3. Calcolo della tensione reale in ingresso (VIN) invertendo la formula del partitore
  //    VIN = VOUT / (R2 / (R1 + R2))
  //    Con R1=100k, R2=10k → fattore di scala = 11 → VIN = VOUT × 11
  vin = vout / (R2 / (R1 + R2));

  // 4. Filtro per eliminare letture spurie vicino allo zero
  //    (rumore elettrico può generare valori piccoli anche senza segnale)
  if (vin < 0.09) {
    vin = 0.0;
  }

  // 5. Visualizzazione sul display LCD
  lcd.setCursor(0, 0);        // Posiziona il cursore: colonna 0, riga 0 (prima riga)
  lcd.print("Volt Meter");    // Titolo fisso sulla prima riga

  lcd.setCursor(0, 1);        // Posiziona il cursore: colonna 0, riga 1 (seconda riga)

  lcd.print(vin);             // Stampa il valore della tensione (es: "12.35")
  lcd.print("  ");            // Spazi per pulire eventuali cifre residue
  lcd.print("Volts");         // Unità di misura
  lcd.print("  ");            // Spazi finali per pulizia display
}
```

---

## 🔌 Schema di Collegamento

### Display LCD → Arduino

| Pin LCD         | Collegamento                   |
| --------------- | ------------------------------ |
| VSS (GND)       | GND                            |
| VCC             | 5V                             |
| VO (contrasto)  | Cursore del potenziometro 10kΩ |
| RS              | Pin digitale 12                |
| R/W             | GND (solo scrittura)           |
| E (Enable)      | Pin digitale 11                |
| D4              | Pin digitale 5                 |
| D5              | Pin digitale 4                 |
| D6              | Pin digitale 3                 |
| D7              | Pin digitale 2                 |
| A (Anode LED)   | 5V tramite resistenza 120Ω     |
| K (Cathode LED) | GND                            |

### Partitore di Tensione → Arduino

| Connessione                      | Dove              |
| -------------------------------- | ----------------- |
| Tensione da misurare (+)         | Un capo di R1     |
| Altro capo di R1 + un capo di R2 | Pin A0 di Arduino |
| Altro capo di R2                 | GND di Arduino    |
| GND della sorgente               | GND di Arduino    |

> ⚠️ **Attenzione:** collegare sempre il GND della sorgente al GND di Arduino prima di applicare la tensione da misurare.

---

## 📏 Limiti e Precisione

| Parametro                   | Valore                                 |
| --------------------------- | -------------------------------------- |
| Tensione minima misurabile  | ~0.09 V (soglia software)              |
| Tensione massima misurabile | ~55 V (con R1=100k, R2=10k)            |
| Risoluzione ADC             | 10 bit (1024 livelli)                  |
| Tensione di riferimento     | 5.0 V                                  |
| Passo minimo teorico        | ≈ 0.054 V per passo ADC (5V/1024 × 11) |

---

## 🔗 Sitografia

- https://www.edutecnica.it/elettrotecnica/wheatstone/wheatstone.htm;
