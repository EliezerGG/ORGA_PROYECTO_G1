#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Inicializar el LCD con dirección y dimensiones adecuadas
LiquidCrystal_I2C lcd(0x27, 20, 4); // Puedes ajustar la dirección y dimensiones según tu LCD

// Pines de Flip-Flops
const int D_pins[9] = {22, 24, 25, 26, 27, 28, 29, 30, 31}; // Pines D1-D9

// Pines CLK para los flip-flops
const int CLK_pin1 = 23; // CLK para el 74LS174 (6 flip-flops)
const int CLK_pin2 = 32; // CLK para el segundo chip (3 flip-flops)

// Configura los pines para el Bluetooth
SoftwareSerial BT(51, 53); // RX, TX

// Variables del juego
bool bombas[9] = {false, false, false, false, false, false, false, false, false};        // Posiciones de las bombas
bool descubiertas[9] = {false, false, false, false, false, false, false, false, false};  // Casillas descubiertas
bool flipFlopStates[9] = {false, false, false, false, false, false, false, false, false}; // Estados de los flip-flops
int estado = 1; // Iniciar en estado de juego

// Variables para controlar la visualización de mensajes una sola vez
bool mensajeMostradoGameOver = false;
bool mensajeMostradoGanaste = false;

void setup() {

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Buscaminas 3x3");
  lcd.setCursor(0, 1);
  lcd.print("Iniciado");
  
  // Configurar pines de Flip-Flops
  for (int i = 0; i < 9; i++) {
    pinMode(D_pins[i], OUTPUT);
    digitalWrite(D_pins[i], LOW); // Inicializar D en LOW
  }
  pinMode(CLK_pin1, OUTPUT);
  digitalWrite(CLK_pin1, LOW); // CLK en LOW inicialmente
  pinMode(CLK_pin2, OUTPUT);
  digitalWrite(CLK_pin2, LOW); // CLK en LOW inicialmente

  // Iniciar comunicación serial y Bluetooth
  Serial.begin(9600);
  BT.begin(9600);

  Serial.println("Buscaminas 3x3 Iniciado");
  BT.println("Buscaminas 3x3 Iniciado");
}

void loop() {

  String input = "";

  // Leer entrada desde Serial
  if (Serial.available() > 0) {
    input = Serial.readStringUntil('\n');
    input.trim(); // Eliminar espacios en blanco
  }

  // Leer entrada desde Bluetooth
  if (BT.available() > 0) {
    input = BT.readStringUntil('\n');
    input.trim(); // Eliminar espacios en blanco
  }

  if (input.length() > 0) {
    // Procesar entrada
    if (estado == 2 || estado == 3) {
      if (input == "A9") {
        estado = 0; // Reiniciar al estado de configuración
        Serial.println("Reiniciando el juego...");
        BT.println("Reiniciando el juego...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reiniciando el");
        lcd.setCursor(0, 1);
        lcd.print("juego...");
        mensajeMostradoGameOver = false;
        mensajeMostradoGanaste = false;
      }
    } else {
      // Detectar si se presiona el botón de cambio de estado
      if (input == "A9") {
        estado = (estado == 1) ? 0 : 1; // Cambiar entre estado de juego y configuración
        Serial.println("Botón A9 presionado.");
        BT.println("Botón A9 presionado.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Boton A9");
        lcd.setCursor(0, 1);
        lcd.print("presionado.");
      }

      if (estado == 0) {
        configurarBombas(input);
      } else if (estado == 1) {
        jugar(input);
      }
    }
  }

  // Llamar a gameOver() o ganaste() si es necesario
  if (estado == 2) {
    gameOver();
  } else if (estado == 3) {
    ganaste();
  }
}

void configurarBombas(String input) {
  static bool configuracionIniciada = false;

  if (!configuracionIniciada) {
    Serial.println("Configura las bombas ingresando A0, A1, ..., A8.");
    Serial.println("Ingresa A9 nuevamente para comenzar el juego.");
    BT.println("Configura las bombas ingresando A0, A1, ..., A8.");
    BT.println("Ingresa A9 nuevamente para comenzar el juego.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Configura bombas");
    lcd.setCursor(0, 1);
    lcd.print("A0-A8");
    lcd.setCursor(0, 2);
    lcd.print("Ingresa A9 para");
    lcd.setCursor(0, 3);
    lcd.print("iniciar juego");
    // Reiniciar el arreglo de bombas
    for (int i = 0; i < 9; i++) {
      bombas[i] = false;
    }
    configuracionIniciada = true;
  }

  if (input == "A9") {
    estado = 1; // Pasar al estado de juego
    configuracionIniciada = false;
    Serial.println("Configuración de bombas terminada. Iniciando el juego...");
    BT.println("Configuración de bombas terminada. Iniciando el juego...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Configuracion de");
    lcd.setCursor(0, 1);
    lcd.print("bombas terminada.");
    lcd.setCursor(0, 2);
    lcd.print("Iniciando juego...");
    return;
  }

  // Detectar si se presionan los botones de las casillas
  for (int i = 0; i < 9; i++) {
    if (input == "A" + String(i)) {
      if (!bombas[i]) {
        bombas[i] = true;
        Serial.print("Bomba configurada en casilla ");
        Serial.println(i + 1);
        BT.print("Bomba configurada en casilla ");
        BT.println(i + 1);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bomba en casilla ");
        lcd.print(i + 1);
      }
    }
  }
}

void jugar(String input) {
  Serial.println("Selecciona una casilla (A0-A8):");
  BT.println("Selecciona una casilla (A0-A8):");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selecciona una");
  lcd.setCursor(0, 1);
  lcd.print("casilla (A0-A8):");

  for (int i = 0; i < 9; i++) {
    if (input == "A" + String(i) && !descubiertas[i]) {
      descubiertas[i] = true;
      if (bombas[i]) {
        estado = 2; // Game Over
        Serial.print("¡Boom! Encontraste una bomba en casilla ");
        Serial.println(i + 1);
        BT.print("¡Boom! Encontraste una bomba en casilla ");
        BT.println(i + 1);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("¡Boom! Bomba en");
        lcd.setCursor(0, 1);
        lcd.print("casilla ");
        lcd.print(i + 1);
      } else {
        Serial.print("Casilla ");
        Serial.print(i + 1);
        Serial.println(" segura.");
        BT.print("Casilla ");
        BT.print(i + 1);
        BT.println(" segura.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Casilla ");
        lcd.print(i + 1);
        lcd.print(" segura.");

        // Actualizar el estado del flip-flop
        flipFlopStates[i] = true;

        // Establecer las entradas D de todos los flip-flops según su estado
        for (int j = 0; j < 9; j++) {
          digitalWrite(D_pins[j], flipFlopStates[j] ? HIGH : LOW);
        }

        // Pulsar el reloj para actualizar los flip-flops
        actualizarFlipFlops();
      }
    }
  }

  // Verificar si todas las casillas sin bomba han sido descubiertas
  int casillasSinBombas = 0;
  int totalCasillasSinBombas = 0;
  for (int i = 0; i < 9; i++) {
    if (!bombas[i]) {
      totalCasillasSinBombas++;
      if (descubiertas[i]) {
        casillasSinBombas++;
      }
    }
  }
  if (casillasSinBombas == totalCasillasSinBombas) {
    estado = 3; // Ganaste
  }
}

void actualizarFlipFlops() {
  // Actualizar flip-flops del 74LS174 (primeros 6 flip-flops)
  digitalWrite(CLK_pin1, HIGH);
  delay(10);
  digitalWrite(CLK_pin1, LOW);

  // Actualizar flip-flops del segundo chip (últimos 3 flip-flops)
  digitalWrite(CLK_pin2, HIGH);
  delay(10);
  digitalWrite(CLK_pin2, LOW);
}

void gameOver() {
  if (!mensajeMostradoGameOver) {
    Serial.println("¡Game Over!");
    Serial.println("Ingresa A9 para reiniciar el juego.");
    BT.println("¡Game Over!");
    BT.println("Ingresa A9 para reiniciar el juego.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("¡Game Over!");
    lcd.setCursor(0, 1);
    lcd.print("Ingresa A9 para");
    lcd.setCursor(0, 2);
    lcd.print("reiniciar juego.");

    // Reiniciar los estados de los flip-flops y variables del juego
    for (int i = 0; i < 9; i++) {
      flipFlopStates[i] = false;
      descubiertas[i] = false;
    }

    // Establecer las entradas D de todos los flip-flops a LOW
    for (int j = 0; j < 9; j++) {
      digitalWrite(D_pins[j], LOW);
    }

    // Pulsar el reloj para actualizar los flip-flops
    actualizarFlipFlops();

    mensajeMostradoGameOver = true;
  }
}

void ganaste() {
  if (!mensajeMostradoGanaste) {
    Serial.println("¡Felicidades, ganaste!");
    Serial.println("Ingresa A9 para reiniciar el juego.");
    BT.println("¡Felicidades, ganaste!");
    BT.println("Ingresa A9 para reiniciar el juego.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("¡Felicidades!");
    lcd.setCursor(0, 1);
    lcd.print("Has ganado.");
    lcd.setCursor(0, 2);
    lcd.print("Ingresa A9 para");
    lcd.setCursor(0, 3);
    lcd.print("reiniciar juego.");

    // Reiniciar los estados de los flip-flops y variables del juego
    for (int i = 0; i < 9; i++) {
      flipFlopStates[i] = false;
      descubiertas[i] = false;
    }

    // Establecer las entradas D de todos los flip-flops a LOW
    for (int j = 0; j < 9; j++) {
      digitalWrite(D_pins[j], LOW);
    }

    // Pulsar el reloj para actualizar los flip-flops
    actualizarFlipFlops();

    mensajeMostradoGanaste = true;
  }
}
