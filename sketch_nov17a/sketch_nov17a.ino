/*
  Step 2 - Synchronisation 2 (Reset logiciel côté Arduino déclenché par le PC)
  - Démarrage sur caractère spécial ('#')
  - Pendant l'exécution, réception d'un caractère de reset ('!')
  - Attente ~10 s puis "soft reset" (réinitialiser l'état et repartir en attente)
  - Communication série à 1 000 000 bauds (8N1)
*/

const char START_CHAR = '#';   // caractère de démarrage
const char RESET_CHAR = '!';   // caractère de demande de reset
const unsigned long RESET_DELAY_MS = 10000UL; // ~10 secondes

enum RunState { WAIT_START, RUNNING, RESET_PENDING };
RunState state = WAIT_START;

unsigned long reset_deadline = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(1000000, SERIAL_8N1);
  // On démarre en attente du START_CHAR
  state = WAIT_START;
}

void loop() {
  // 1) Lire les commandes entrantes (non bloquant)
  readSerialCommands();

  // 2) Machine d'état
  switch (state) {
    case WAIT_START:
      // On attend START_CHAR dans readSerialCommands()
      break;

    case RUNNING:
       static unsigned long last_send = 0;
        if (millis() - last_send >= 200) {
          last_send = millis();

          unsigned long temps_us = micros(); // compteur µs
          Serial.print("Temps écoulé (µs) : ");
          Serial.println(temps_us);

          Serial.println("Mesure fictive : 123");
  }
  break;

    case RESET_PENDING:
      // Compte à rebours jusqu'au reset logiciel
      if (millis() >= reset_deadline) {
        softReset();
      }
      break;
  }
}

// Lecture des commandes série (START / RESET)
void readSerialCommands() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (state == WAIT_START && c == START_CHAR) {
      state = RUNNING;
      digitalWrite(LED_BUILTIN, HIGH);  // indique que loop démarre
      Serial.println("Arduino : Synchronisation OK !");
    }
    else if (state == RUNNING && c == RESET_CHAR) {
      // PC demande un reset: on programme l'échéance
      reset_deadline = millis() + RESET_DELAY_MS;
      state = RESET_PENDING;
      Serial.println("Arduino : RESET reçu, déclenchement dans ~10 s...");
    }
  }
}

// "Soft reset": réinitialise l'état et repart comme au démarrage
void softReset() {
  // Indication visuelle et message
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Arduino : Reset logiciel - retour à l'attente du START_CHAR");

  // Réinitialiser l'état
  state = WAIT_START;

  // Optionnel: vider les buffers
  Serial.flush();
  while (Serial.available() > 0) (void)Serial.read();

  // Remettre à zéro les variables si nécessaire (exemple)
  reset_deadline = 0;

  // Remarque: ceci ne redémarre pas physiquement la carte.
  // Pour un "hard reset" sur certaines plateformes, on pourrait utiliser
  // une fonction spécifique du core (ex.: NVIC_SystemReset(); si disponible).
  // Mais ce "soft reset" respecte l'esprit de l'étape 2 du TP.
}
