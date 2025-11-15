#define BUZZER_PIN LED_BUILTIN

void test_1() {
    Serial.print("Running Test 1... ");
    // TODO
    Serial.println("Pass");
}

void setup() {
    Serial.begin(9600);
    pinMode(BUZZER_PIN, OUTPUT);
    Serial.println("======== Tests ========");
    test_1();
}

void loop() {}
