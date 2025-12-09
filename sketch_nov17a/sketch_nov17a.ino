/*
  Step 2 + Step 3
  - Synchronisation avec START_CHAR ('#')
  - Reset logiciel avec RESET_CHAR ('!')
  - Transmission du temps écoulé en secondes (via micros())
  - Utilisation de sprintf() et Serial.println()
  - Pas de delay() bloquant
*/

const char START_CHAR = '#';
const char RESET_CHAR = '!';
const unsigned long RESET_DELAY_MS = 10000UL; // ~10 secondes

enum RunState { WAIT_START, RUNNING, RESET_PENDING };
RunState state = WAIT_START;

unsigned long reset_deadline = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(1000000, SERIAL_8N1);
  state = WAIT_START;
}

void loop() {
  // Lecture des commandes entrantes
  readSerialCommands();

  // Machine d'état
  switch (state) {
    case WAIT_START:
      // Attente du START_CHAR
      break;

    case RUNNING: {
      static unsigned long last_send = 0;
      if (millis() - last_send >= 200) { // envoi toutes les 200 ms
        last_send = millis();

        unsigned long temps_us = micros();
        float temps_s = temps_us / 1000000.0;

        char buffer[64];
        sprintf(buffer, "Temps écoulé (s): %.6f", temps_s);
        Serial.println(buffer);
        Serial.flush(); // vider le buffer série
      }
      break;
    }

    case RESET_PENDING:
      if (millis() >= reset_deadline) {
        softReset();
      }
      break;
  }
}

void readSerialCommands() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (state == WAIT_START && c == START_CHAR) {
      state = RUNNING;
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Arduino : Synchronisation OK !");
    }
    else if (state == RUNNING && c == RESET_CHAR) {
      reset_deadline = millis() + RESET_DELAY_MS;
      state = RESET_PENDING;
      Serial.println("Arduino : RESET reçu, déclenchement dans ~10 s...");
    }
  }
}

void softReset() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Arduino : Reset logiciel - retour à l'attente du START_CHAR");

  state = WAIT_START;
  Serial.flush();
  while (Serial.available() > 0) (void)Serial.read();
  reset_deadline = 0;
}
