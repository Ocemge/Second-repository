/*
  Step 1 - Synchronisation Arduino <-> PC
  - Attendre un caractère spécial (#)
  - Allumer LED_BUILTIN quand loop() démarre réellement
  - Communication série à 1 000 000 bauds (8N1)
*/

const char START_CHAR = '#';     // caractère spécial
bool ready = false;              // l'Arduino attend l'ordre du PC

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Configuration série : 1 000 000 bps, 8N1
  Serial.begin(1000000, SERIAL_8N1);

  // Attente que le port USB soit prêt
  while (!ready) {
    if(Serial.available()) {
      char c = Serial.read();
      if (c == START_CHAR) {
        ready = true;
        digitalWrite(LED_BUILTIN, HIGH);   // indique que loop démarre
        Serial.println("Arduino : Synchronisation OK !");
      }
    }
  }
}

void loop() {

  // -----------------------
  //  Code principal ici
  // -----------------------

  // Exemple : envoyer un message toutes les 200 ms
  Serial.println("Mesure fictive : 123");
  delay(200);
}
