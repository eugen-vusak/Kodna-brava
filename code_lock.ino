#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Servo.h>

/*deklaracija LCD zaslona*/
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/*deklaracija servo varijable*/
Servo servo;

/*deklaracija polja za lozinke*/
char lozinka[4] = {'0', '0', '0', '0'};   //ispravna lozinka
char pokusaj[4] = {'\0', '\0', '\0', '\0'}; //lozinka koju upisuje korisnik(ispunjena "smećem")
char newpass[4] = {'\0', '\0', '\0', '\0'}; //nova lozinka

/*deklaracija funkcija*/
int vrati_stanje(int an0);                //funkcija koja vraća stanje tipke (1, 0)
void dodaj(char j);                       //funkcija koja dodaje pritisnutu tipku u polje pokusaj
void provjera();                          //funkcija koja provjerava točnost upisane lozinke
void blink();                             //funkcija koja trepne svim žutim LED diodama
char vrati_tipku(int a0);                 //funkcija koja vraća pritisnutu tipku

/*deklaracija varijabli*/
char tipka;                               //varijabla koja sadržava vrijednost tipke
int stanje;                               //varijabla koja sadržava stanje tipke
int in = 0;                               //varijabla koja sadržava vrijednost sa analognog ulaza
int i = 0;                                //brojilo
int z = 0;                                //brojilo
int nt = 0;                               //zastavica
unsigned long time, l_time;

/*funkcija se izvršava samo prilikom uključenja ili reseta*/
void setup() {
  /* --> dodati znakove '* /', ali bez razmaka, na kraj
   ukoliko ste zaboravili lozinku te učitati program,
    zatim ponovno maknuti '* /'' i opet učitati program.
   dodati ovdije -->*/
  for (i = 0; i < 4; i++) {
    EEPROM.write(i, lozinka[i]);          //zapisivanje lozinke 0123 u EEPROM memoriju
  }
  /*deklaracija pinova*/
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);

  servo.attach(0,544,2350);              //deklaracija signalnog pina servo motora
  lcd.begin(16, 2);                      //inicijalizacija LCD modula
  servo.write(180);                        //Pstavlja servo u poziciju 0
  delay(100);
  lcd.print("Zavrsni rad:");
  lcd.setCursor(0, 1);
  lcd.print("KODNA BRAVA");
  delay(1000);
  lcd.clear();
  lcd.print("Napravio:");
  lcd.setCursor(0, 1);
  lcd.print("Eugen Vusak 4.H");
  delay(1000);
  lcd.clear();
  lcd.print("ZAKLJUCANO");
  delay(500);
  lcd.clear();
  lcd.print("UPISI LOZINKU: ");
  blink();
}

void loop() {
  in = analogRead(A5);                  //očitava vrijednost sa analognog ulaza A0 --> 0-1023 (0=0V, 1023=5V)
  stanje = vrati_stanje(in);            //vraća stanje s obzirom na očitanu vrijednost
  if (stanje == 1) {                    //ako je tipka pritisnuta...
    delay(50);
    tipka = vrati_tipku(in);            //vraća pritisnutu tipku s obzirom na očitanu vrijednost
    lcd.setCursor(i, 1);                //pomiče pokazivać na zaslonu s obzirom na brojilo "i"
    lcd.print(tipka);    //ispisuje pritisnutu tipku
    if (i != 0) {
      lcd.setCursor(i - 1, 1);              //pomiče pokazivać na zaslonu s obzirom na brojilo "i"
      lcd.print('*');
    }
    if (i <= 3) {
      digitalWrite(8 + i, HIGH);       //uključuje odgovarajuću LED diodu s obzirom na brojilo "i"
    }
    i++;                                //broji pritisnute tipke
    if (tipka == '#') {                 //ako je pritisnuta tipka '#' provjerava ispravnost lozinke
    provjera();
  }
    else if (tipka == '*') {            //ako je pritisnuta tipka '*'
      nt++;                             //povečava brojilo nt
      provjera();                       //i provjerava ispravnost lozinke
    }
    else {                              //ostale pritisnute tipke upisuje u polje pokušaj
      dodaj(tipka);
    }
  }
}

/*definicija funkcije vrati_stanje()*/
int vrati_stanje(int an0) {
  int st = 0;                           //zadano stanje tipaka je 0, tj. nepritisnuto
  delay(100);
  if (an0 > 40 && an0!=47 && an0!=55 && an0!=63) { //ako se na alanognom ulazu A0 pojavi napon
    st = 1;                             //stanje tipkala se postavlja u 1, tj. pritisnuto
  }
  return st;                            //vraća stanje tipkala
}

/*definicija funkcije dodaj()*/
void dodaj(char j) {
  if (z > 3) {                          //ako je z>3 upisano je više od 4 znaka...
    lcd.clear();
    lcd.print("Upisano je");              //upisano je previše znakova
    lcd.setCursor(0, 1);
    lcd.print("previse znakova!");
    for (i = 8; i <= 11; i++) {
      digitalWrite(i, LOW);             //isključuju se sve LED diode
    }
    digitalWrite(13, HIGH);
    for (i = 0; i < 4; i++) {           //briše se predhodni pokušaj
      pokusaj[i] = '\0';
    }
    z = 0;                              //brojilo se resetira
    i = 0;                              //brojilo se resetira
    delay(1000);                        //pričeka 0,3s kako bi servo došao u poziciju
    digitalWrite(13, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ZAKLJUCANO");
    delay(1000);                        //nakon 1s korisnik može ponovno upisivati lozinku
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UPISI LOZINKU: ");
    blink();
  }
  else if (nt == 0) {                   //ako je z<=3, a nije bila pritisnuta tipka '*'
    pokusaj[z] = j;                     //pritisnutu tipku upisuj u polje pokušaj
    z++;
  }
  else if (nt == 2) {                   //ako je z<=t, a bila je pritisnuta tipka '*'
    newpass[z] = j;                     //pritisnutu tipku upisuj u polje newpass
    z++;
  }
}

/*definicija funkcije provjera()*/
void provjera() {
  int c = 0;                            //brojilo
  lcd.clear();
  for (i = 0; i < 4; i++) {
    if (pokusaj[i] == EEPROM.read(i)) { //provjerava je li upisani znak jednak ekvivalentnom zapisanom u EEPROM
      c++;                              //broji točne znakove
    }
  }
  if (nt == 2) {                        //ako je bila pritisnuta tipka '*'
  for (i = 8; i <= 11; i++) {
      digitalWrite(i, LOW);             //isključuju se sve LED diode
    }
    for (i = 0; i < 4; i++) {
      EEPROM.write(i, newpass[i]);      //upisuj u EEPROM memoriju
    }
    lcd.setCursor(0, 0);
    lcd.print("PROMJENJENO");     //lozinka je promjenjena
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UPISI LOZINKU: ");
    blink();
    nt = 0;                             //brojilo se restarta
    c = 5;                              //brojilo se restarta
    z = 0;                              //brojilo se restarta
    i = 0;                              //brojilo se restarta
  }
  else if (nt == 3) {                   //ako je tipka '*' pritisnuta dva puta
  for (i = 8; i <= 11; i++) {
      digitalWrite(i, LOW);             //isključuju se sve LED diode
    }
    lcd.setCursor(0, 0);
    lcd.print("OTKAZANO");
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UPISI LOZINKU: ");
    blink();
    for (i = 0; i < 4; i++) {           //briše se predhodni pokušaj
      pokusaj[i] = '\0';
    }
    nt = 0;                             //brojilo se restarta
    c = 5;                              //brojilo se postavlja u 5 kako bi se preskočile slijedeće funkcije
    z = 0;                              //brojilo se restarta
    i = 0;                              //brojilo se restarta
  }
  if (c == 4) {                           //ukoliko su svi znakovi točni...
    lcd.clear();
    if (nt == 0) {                        //i nije pritisnuta tipka '*'
    for (i = 8; i <= 11; i++) {
        digitalWrite(i, LOW);             //isključuju se sve LED diode
      }
      lcd.setCursor(0, 0);
      lcd.print("Pristup dopusten");
      digitalWrite(12, HIGH);              //uključuje se zelena LED dioda
      for (i = 0; i < 4; i++) {           //briše se predhodni pokušaj
        pokusaj[i] = '\0';
      }
      z = 0;                              //brojilo se resetira
      i = 0;                              //brojilo se resetira
      delay(200);                         //pauza od 0,5s
      lcd.setCursor(0, 1);
      servo.write(90);
      lcd.print("OTKLJUCANO");
      delay(500);
      lcd.clear();
      lcd.print("Zakljucavanje za");
      l_time = millis();
      do {
        time = millis();
        if (((10000 - time + l_time) / 1000) + 1 < 10) {
          lcd.setCursor(7, 1);
          lcd.print("0");
          lcd.setCursor(8,1);
        }
        else {
          lcd.setCursor(7, 1);
        }
        lcd.print(((10000 - time + l_time) / 1000) + 1);
        in = analogRead(A5);
      } while ((in < 40 || in==43 || in==47 || in== 55 || in==59 || in==63) && time - l_time < 9999);
      lcd.clear();
      digitalWrite(12, LOW);               //isključuje se zelena LED dioda
      servo.write(180);                     //rotira servo u položaj kada su vrata zaključana
      delay(300);                         //pričeka 0,3s kako bi servo došao u poziciju
      lcd.setCursor(0, 0);
      lcd.print("ZAKLJUCANO");
      delay(1000);                        //nakon 1s korisnik može ponovno upisivati lozinku
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UPISI LOZINKU: ");
      blink();
    }
    else if (nt == 1) {                   //ukoliko su svi znakovi točni i pritisnuta je tipka '*'
    for (i = 8; i <= 11; i++) {
        digitalWrite(i, LOW);             //isključuju se sve LED diode
      }
      digitalWrite(12, HIGH);
      delay(500);
      nt = 2;                             //postavlja zastavicu nt
      z = 0;
      i = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NOVA LOZINKA: ");
      digitalWrite(12, LOW);
      blink();

    }
  }
  else if (c < 4) {                       //ukoliko svi znakovi nisu točni...
    if (nt == 0) {                        //i nije pritisnuta tipka '*'
    for (i = 8; i <= 11; i++) {
        digitalWrite(i, LOW);             //isključuju se sve LED diode
      }
      lcd.setCursor(0, 0);
      lcd.print("Pristup odbijen");
      digitalWrite(13, HIGH);              //uključuje se crvena LED dioda
      for (i = 0; i < 4; i++) {           //briše se predhodni pokušaj
        pokusaj[i] = '\0';
      }
      z = 0;                              //brojilo se resetira
      i = 0;                              //brojilo se resetira
      delay(1000);                        //nakon 1s vrata se zaključavaju
      digitalWrite(13, LOW);               //isključuje se crvena LED dioda
      servo.write(180);
      delay(300);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ZAKLJUCANO");
      delay(1000);                        //nakon 1s korisnik može ponovno upisivati lozinku
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UPISI LOZINKU: ");
      blink();
    }
    else if (nt == 1) {                   //ukoliko svi znakovi nisu točni i pritisnuta je tipka '*'
    for (i = 8; i <= 11; i++) {
        digitalWrite(i, LOW);             //isključuju se sve LED diode
      }
      nt = 0;                             //brojilo se resetira
      z = 0;                              //brojilo se resetira
      i = 0;                              //brojilo se resetira
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("KRIVA LOZINKA! ");
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UPISI LOZINKU: ");
      blink();
    }
  }
}
void blink() {
  delay(150);
  for (i = 8; i <= 11; i++) {            //pale se žute LED diode
    digitalWrite(i, HIGH);
  }
  delay(150);                             //stanka od 150ms
  for (i = 8; i <= 11; i++) {
    digitalWrite(i, LOW);                 //gase se žute LED diode
  }
  i = 0;
}

/*definicija funkcije vrati_tipku()*/
char vrati_tipku(int a0) {
  char t;
  delay(100);
  if (a0 > 35 && a0 < 70 ) {
    t = '1';
  }
  if (a0 > 70 && a0 < 110) {
    t = '2';
  }
  if (a0 > 110 && a0 < 165) {
    t = '3';
  }
  if (a0 > 165 && a0 < 255) {
    t = '4';
  }
  if (a0 > 255 && a0 < 363) {
    t = '5';
  }
  if (a0 > 363 && a0 < 482) {
    t = '6';
  }
  if (a0 > 482 && a0 < 623) {
    t = '7';
  }
  if (a0 > 623 && a0 < 740) {
    t = '8';
  }
  if (a0 > 740 && a0 < 825) {
    t = '9';
  }
  if (a0 > 825 && a0 < 900) {
    t = '*';
  }
  if (a0 > 900 && a0 < 945) {
    t = '0';
  }
  if (a0 > 945) {
    t = '#';
  }
  return t;
}
