#include "TFT_eSPI.h" //TFT LCD library
// #include <ServoAlternative.h>

//  -> onda sinusoidal
#include"seeed_line_chart.h" //include the library
TFT_eSPI tft;

doubles data; //Initilising a doubles type to store data
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite

#define MIN_T 1000                         // minima duracion del periodo de la onda en milisegundos (1 segundo)
#define MAX_T 10000                        // maxima duracion del periodo de la onda en milisegundos (10 segundos)
#define POT A0                             // pin analogico del potenciometro

unsigned long T;                           // periodo de la onda
unsigned long _delay;                      // duracion de un paso
double angulo;                             // angulo en radianes de 0 a 2pi
double deltaAngulo;                        // incremento del angulo en un paso
double pasos;

// -> onda sinusoidal

int ledRight = D4;
int ledLeft = D3;

int PonteUno = A0;
int PonteDos = A1;
int PonteModo = A5;
int max_size_dos;
int valuePtDos;

byte valueMode = 0;
int valuePuro;

int button = D7;
byte valueButton;
bool onDisplay = false;

// -> raspi

// Campos contiene la cadena formateada, con separador ",".
String campos = "197:167:46:50";

unsigned long myTime;
unsigned long oldMyTime;
bool ledStatus = false;

// s en un puntero auxiar necesario.
char *s;

// <- raspi

// Servo myservo;  // create servo object to control a servo

bool fantasmaDetectado = false;
byte seccionLed = 0;

void setup() {

  Serial.begin(9600); //Start serial communication

  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  pinMode(PonteUno, INPUT);
  pinMode(PonteDos, INPUT);
  pinMode(PonteModo, INPUT);
  pinMode(button, INPUT);
  // myservo.attach(D2);  // attaches the servo on pin 9 to the servo object
  // myservo.write(0);

  // -> pantalla de la Wio terminal

  tft.begin(); //Start TFT LCD
  tft.setRotation(3); //Set TFT LCD rotation
  spr.createSprite(TFT_HEIGHT, TFT_WIDTH);

  tft.setFreeFont(&FreeSansBold24pt7b);
  tft.fillScreen(TFT_RED); //Red background
  tft.setTextColor(TFT_WHITE);          //sets the text colour to black
  tft.setTextSize(1);
  tft.drawString("MoonMakers", 13, 100);

  delay(3000);

  tft.fillScreen(TFT_RED);
  tft.fillScreen(TFT_BLACK);
}

void loop() {

  // -> onda sinusoidal

  if (onDisplay == true) {

    // myservo.write(90);

    tomarValoresPotenciometros();

    // -> modeo onda
    if (valueMode == 1) {

      digitalWrite(ledLeft, 0);
      digitalWrite(ledRight, 0);

      spr.fillScreen(TFT_WHITE);

      angulo = 0;                                             // ponemos el angulo a cero
      T = map(analogRead(POT), 0, 1023, MIN_T, MAX_T);        // leemos T del potenciometro cambiando al rango definido
      pasos = T / 80.0;

      _delay = T / pasos;                                     // calculamos el delay de acuerdo al T medido y a los pasos
      deltaAngulo = 2.0 * PI / pasos;                         // calculamos el angulo incremental

      while (angulo < 2 * PI) {                               // mientras no haya completado un ciclo
        // mientras no haya completado un ciclo

        OnButton();

        //las siguientes dos lineas deben ser modificadas de acuerdo al graficador que utilicen o si quieren sacar por PWM o un DAC

        spr.fillSprite(TFT_WHITE);
        if (data.size() >= max_size_dos) { // == max_size
          data.pop();//this is used to remove the first read variable
        }

        data.push(int( (valuePtDos * 0.5) *  (sin(angulo) * 64 + 62 ))); //read variables and store in data

        tomarValoresPotenciometros();

        //Settings for the line graph
        auto content = line_chart(-25, 0); //(x,y) where the line graph begins // 25
        content
        .height(tft.height() + 13) //actual height of the line chart
        .width(tft.width() + 25) //actual width of the line chart
        .based_on(0.0) //Starting point of y-axis, must be a float
        .show_circle(false) //drawing a cirle at each point, default is on.
        .value(data) //passing through the data to line graph
        .color(TFT_BLACK) //Setting the color for the line
        .draw();

        pantallaEstiloGiroscopio();

        // mostramos en pantalla
        spr.pushSprite(0, 0);

        angulo += deltaAngulo;                                // incrementamos el angulo en un delta correspondiente a un paso

        if (onDisplay == false || valueMode == 2 ) {
          data = {};
          break;

        }

      }
    }

    // modo localisador de fantasmas
    if (valueMode == 2) {

      spr.fillScreen(TFT_WHITE);
      spr.fillRect(75, 0, 250, 250, TFT_WHITE);


      //      spr.pushSprite(0, 0);


      OnButton();

      if (Serial.available()) { //Si está disponible
        String myString  = Serial.readString();

        if (myString.length() > 0 ) {
          //String myString = String(c);

          String xval = getValue(myString, ':', 0);
          String yval = getValue(myString, ':', 1);
          String wval = getValue(myString, ':', 2);
          String hval = getValue(myString, ':', 3);

          /*
            Serial.println("Y:" + yval);
            Serial.print("X:" + xval);
            Serial.println("w:" + wval);
            Serial.print("h:" + hval);
          */

          seccionLed = map(xval.toInt(), 0, 400, 1, 3 );
          if (seccionLed == 1) {
            digitalWrite(ledLeft, 150);
            digitalWrite(ledRight, 0);
          }
          if (seccionLed  == 2) {
            digitalWrite(ledLeft, 150);
            digitalWrite(ledRight, 150);
          }
          if (seccionLed == 3) {
            digitalWrite(ledLeft, 0);
            digitalWrite(ledRight, 150);
          }


          spr.fillCircle(map(xval.toInt(), 0, 400, 0, TFT_WIDTH ), map(yval.toInt(), 0, 400, 0, TFT_HEIGHT), 10, TFT_RED); //A black circle origin at (160, 120)

          fantasmaDetectado = true;

        } else {
          seccionLed = 0;

          digitalWrite(ledLeft, LOW);
          digitalWrite(ledRight, LOW);
        }


      }

      pantallaEstiloLocalisador();


      spr.pushSprite(0, 0);

      if (fantasmaDetectado == true) {
        fantasmaDetectado = false;
        delay(1000);
      }

      ajusteTiempo();

      if (ledStatus == true) {
        digitalWrite(ledLeft, 150);
        digitalWrite(ledRight, 150);
      } else {
        digitalWrite(ledLeft, 0);
        digitalWrite(ledRight, 0);
      }



    }


    if (valueMode == 0) {
      Serial.println("-> valueMode is 0");
    }

  } else {

    OnButton();

    tft.fillScreen(TFT_BLACK);

    // myservo.write(0);

  }


}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void OnButton() {
  valueButton = digitalRead(button);
  if (valueButton == 0) {

    onDisplay = ((onDisplay == true) ? false : true );
    Serial.print(" -> onDisplay ->");
    Serial.println(onDisplay);
    Serial.println("- Button 1 -");

    delay(1000);
  }
}

void tomarValoresPotenciometros() {
  int valUno = analogRead(PonteUno);
  int valUDos = analogRead(PonteDos);

  max_size_dos = map(valUno, 0, 1023, 25, 50);
  valuePtDos = map(valUDos, 0, 1023, 1 , 5);

  valuePuro = analogRead(PonteModo);
  if (valuePuro > 50 &&  valuePuro < 120) {
    valueMode = 2;
  } else if (valuePuro < 60) {
    valueMode = 1;
  }
  //valueMode = map(analogRead(PonteModo), 0, 1023, 1, 2);

  Serial.print("--> valueMode ");
  Serial.println(valueMode);
}

void ajusteTiempo() {

  myTime = millis();

  long diferencia = myTime - oldMyTime;

  Serial.print("--> diferencia: ");
  Serial.println(diferencia);
  Serial.print("--> ledStatus :" );
  Serial.println(ledStatus);

  if (diferencia >= 400 ) {
    oldMyTime = myTime;
    ledStatus = !ledStatus;
  }

}

void pantallaEstiloGiroscopio() {
  /*
           estructura de basica del PKE onda
  */

  spr.drawFastVLine(tft.width() / 2, 0, tft.height(), TFT_BLACK); //Verical line
  spr.drawFastHLine(tft.width() / 2 - 30, 10, 60, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 20, 30, 40, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 30, tft.height() - 10, 60, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 20, tft.height() - 30, 40, TFT_BLACK);

  spr.drawFastHLine(0, tft.height() / 2, tft.width(), TFT_BLACK); //Horizontal line
  spr.drawFastVLine(10, tft.height() / 2 - 30, 60, TFT_BLACK);
  spr.drawFastVLine(30, tft.height() / 2 - 20, 40, TFT_BLACK);
  spr.drawFastVLine(tft.width() - 10, tft.height() / 2 - 30, 60, TFT_BLACK);
  spr.drawFastVLine(tft.width() - 30, tft.height() / 2 - 20, 40, TFT_BLACK);
}

void pantallaEstiloLocalisador() {
  /*
        estructura de rastreo de fantasma
  */

  //centro
  spr.drawFastVLine(tft.width() / 2 , tft.height() / 2 - 40,  30, TFT_BLACK);
  spr.drawFastVLine(tft.width() / 2 , tft.height() / 2 + 10,  30, TFT_BLACK);

  spr.drawFastHLine(tft.width() / 2 - 40, tft.height() / 2 ,  30, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 + 10, tft.height() / 2 ,  30, TFT_BLACK);

  //interceccion
  //-- vertical
  spr.drawFastHLine(tft.width() / 2 - 20, 30, 40, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 20, 45, 40, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 20, 60, 40, TFT_BLACK);

  spr.drawFastHLine(tft.width() / 2 - 20, tft.height() - 30, 40, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 20, tft.height() - 45, 40, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 20, tft.height() - 60, 40, TFT_BLACK);

  //-- horizontal
  spr.drawFastVLine(30, tft.height() / 2 - 20, 40, TFT_BLACK);
  spr.drawFastVLine(45, tft.height() / 2 - 20, 40, TFT_BLACK);
  spr.drawFastVLine(60, tft.height() / 2 - 20, 40, TFT_BLACK);

  spr.drawFastVLine(tft.width() - 30, tft.height() / 2 - 20, 40, TFT_BLACK);
  spr.drawFastVLine(tft.width() - 45, tft.height() / 2 - 20, 40, TFT_BLACK);
  spr.drawFastVLine(tft.width() - 60, tft.height() / 2 - 20, 40, TFT_BLACK);

  // esquinas
  spr.drawFastVLine(10, tft.height() / 2 - 40, 80, TFT_BLACK);
  spr.drawFastVLine(tft.width() - 10, tft.height() / 2 - 40, 80, TFT_BLACK);

  spr.drawFastHLine(tft.width() / 2 - 40, 10, 80, TFT_BLACK);
  spr.drawFastHLine(tft.width() / 2 - 40, tft.height() - 10, 80, TFT_BLACK);

}
