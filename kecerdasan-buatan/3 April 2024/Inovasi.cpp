#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define Input_Lenght 16
#define Password_Lenght 6
#define pin_servo 8
#define pin_piezo 9

#define NOTE_C  523
#define NOTE_D  587
#define NOTE_E  659
#define NOTE_F  698
#define NOTE_G  784
#define NOTE_A  880
#define NOTE_B  988

int melody_open[] = { NOTE_C, 0, NOTE_D, NOTE_E, 0, NOTE_F, 0, NOTE_G, NOTE_A, NOTE_B };
int note_open_durations[] = { 4, 5, 4, 4, 5, 4, 4, 4, 5, 5 };

int melody_wrong[] = { NOTE_C, NOTE_D };
int note_wrong_durations[] = { 3, 4 };

int melody_correct[] = { NOTE_A, NOTE_A };
int note_correct_durations[] = { 5, 5 };

int melody_alert[] = { NOTE_E, NOTE_B };
int note_alert_durations[] = { 4, 4 };

Servo servo;
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

int position = 0;
int password_counter = 0;
int a = 0, b = 0, c = 0, d = 0;
String question;
char Data[Input_Lenght];
char Master[Password_Lenght] = "741"; // GANTI PASSWORD
bool password_status;
bool door = true;

char customKey;
const byte rows = 4;
const byte columns = 3;
char keys[rows][columns] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte row_pins[rows] = {1, 2, 3, 4};
byte column_pins[columns] = {5, 6, 7};
byte data_count = 0;

Keypad customKeypad(makeKeymap(keys), row_pins, column_pins, rows, columns);

void tone_melody(int melody[], int noteDurations[], int item) {
  for (int thisNote = 0; thisNote < item; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(9, melody[thisNote], noteDuration);
    int pause = noteDuration * 1.25;
    delay(pause);
    noTone(9);
  }
}

void lcd_clear(int time_delay = 1000) {
  delay(time_delay);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.clear();
}

void lcd_message(String top_message = "                ", String bottom_message = "                ", bool clear = true) {
  lcd.setCursor(0, 0);
  lcd.print(top_message);
  lcd.setCursor(0, 1);
  lcd.print(bottom_message);
  if (clear) { lcd_clear(); }
}

void servo_open() {
  lcd_message("  OPENING DOOR  ", "----------------", false);
  for (position = 93; position >= 0; position--) { 
    servo.write(position);
    delay(15);
  }  
  tone_melody(melody_open, note_open_durations, 10);
}

void servo_close() {
  lcd_message("  CLOSING DOOR  ", "----------------", false);
  for (position = 0; position <= 93; position++) { 
    servo.write(position);
    delay(15);
  }
  tone_melody(melody_wrong, note_wrong_durations, 2);
}

void servo_setup(String top_message, String bottom_message) {
  servo.attach(pin_servo);
  lcd.begin(16, 2);
  lcd_message(top_message, bottom_message, false);
}

void clear_data() {
  while (data_count > 0) { Data[data_count--] = 0; }
  return;
}

void uno_captcha() {
  a = abs(random(0, 10000));
  b = abs(random(0, 10000 - a));
  c = a + b;
}

void setup() {
  servo_setup("  ARDUINO DOOR  ", " INITIALIZATION ");
  lcd_clear();
}

void loop() {
  if (door == 0) {
    customKey = customKeypad.getKey();
    if (customKey == '#') {
      lcd_clear();
      servo_close();
      lcd_clear();
      door = 1;
    }
  } else {
    if (password_counter >= 3) {
      tone_melody(melody_alert, note_alert_durations, 2);
      captcha();
    }
    else {
      open();
    }
  }
}

void captcha() {
  question = String(a) + " + " + String(b) + " :";
  lcd.setCursor(0, 0);
  lcd.print(question);
  customKey = customKeypad.getKey();
  if (customKey == '*') {
    int deleted_data = data_count - 1;
    Data[deleted_data] = ' ';
    lcd.setCursor(deleted_data, 1);
    lcd.print(Data[deleted_data]);
    if (data_count > 0) { data_count--; }
  } else if (customKey == '#') {
    int answer = atoi(Data);
    bool check_password = (answer == c);
    if (check_password) {
      lcd_clear();
      tone_melody(melody_correct, note_correct_durations, 2);
      password_counter = 0;
      uno_captcha();
    } else {
      lcd_clear();
      lcd_message(" WRONG PASSWORD ", "----------------", false);
      tone_melody(melody_wrong, note_wrong_durations, 2);
      lcd_clear();
      password_counter++;
      door = 1;
      uno_captcha();
    }
    clear_data();
  } else if (customKey) {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }
}

void open() {
  lcd.setCursor(0, 0);
  lcd.print(" ENTER PASSWORD ");
  customKey = customKeypad.getKey();
  if (customKey == '*') {
    int deleted_data = data_count - 1;
    Data[deleted_data] = ' ';
    lcd.setCursor(deleted_data, 1);
    lcd.print(Data[deleted_data]);
    if (data_count > 0) { data_count--; }
  } else if (customKey == '#') {
    bool check_password = !strcmp(Data, Master);
    if (check_password) {
      lcd_clear();
      servo_open();
      door = 0;
      lcd_message(" SELAMAT DATANG ", "SOFA MACHABBA H.", false);
      password_counter = 0;
      uno_captcha();
    } else {
      lcd_clear();
      lcd_message(" WRONG PASSWORD ", "----------------", false);
      tone_melody(melody_wrong, note_wrong_durations, 2);
      lcd_clear();
      password_counter++;
      door = 1;
      uno_captcha();
    }
    clear_data();
  } else if (customKey) {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }
}
