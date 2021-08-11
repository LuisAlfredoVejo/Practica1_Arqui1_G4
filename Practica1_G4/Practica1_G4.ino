#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <TimerOne.h>
#include "LedControl.h"

// Parola
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 2

// Pantalla
#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10

// Objeto de Parola
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Objeto Led Control
LedControl controlador = LedControl(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Mensaje de Texto
const int DIP1 = 9;              // pin para el dipswitch
const int DIP2 = 8;              // pin para el dipswitch


int estadoDip1 = 0;  // Estado en que se encuentra el dipswitch
int estadoDip2 = 0;  // Estado en que se encuentra el dipswitch


String mensajeTexto = "*P1 - GRUPO # 4 - SECCION A*";  // Texto estatico a mostrar en la matriz

String mensajeTextoD = "Dinamico";  //Texto Dinamico a mostrar en la matriz

int velocidadTexto = 50; //velocidad texto
int pausavel = 300;                   
int indexMensajeTexto = 0;            // Indice para la cadena de texto
int indexMensajeTextoD = 0;            // Indice para la cadena de texto

bool bLimpiarMatriz = false;  // Booleano para limpiar matriz, lo que esta en
                              // pantalla, no el buffer

bool bLimpiarMatrizD = false;  // Booleano para limpiar matriz, lo que esta en
                              // pantalla, no el buffer                              

textEffect_t efectoTxtD =
    PA_SCROLL_LEFT;  // Orientación del texto a mostrar en el mensaje
textPosition_t posTxtD = PA_LEFT;  // Posición del texto

textEffect_t efectoTxt =
    PA_SCROLL_LEFT;  // Orientación del texto a mostrar en el mensaje
textPosition_t posTxt = PA_LEFT;  // Posición del texto

// Start
const int BTN_START = 12;
int estadoStart = 0;

/*
  ESTADO GENENRAL DE LA APLICACION
  estado 0 letrero Estatico
  estado 1 letrero Dinamico
*/

int estadoGeneral = 0;

// variable temporal
int pantallaUso = 0;

// Cuenta los pulsos
volatile unsigned long contadorPulsos = 0;

//Variable para el pin analogo
int pinAnalogo = A0;

//Lista con los pines digitales del display
int listaPines[7] = {23, 25, 27, 29, 31, 33, 35};

//Variables para los numeros mostrados en el display
boolean cero[7] = {true, true, true, true, true, true, false};
boolean uno[7] = {false, true, true, false, false, false, false};
boolean dos[7] = {true, true, false, true, true, false, true};
boolean tres[7] = {true, true, true, true, false, false, true};
boolean cuatro[7] = {false, true, true, false, false, true, true};
boolean cinco[7] = {true, false, true, true, false, true, true};



void setup() {
  // put your setup code here, to run once:

    Serial.begin(9600);
    //Serial1.begin(9600);

    // Inicializando Parola
    configuracionParola();

    // Configuracion inicial timer
    configuracionTimer();

    // Configuracion inicial de pantalla
    configuracionPantantalla();

    //Inicializar Pines Digitales
    for(int i=0; i<8; i++){

    pinMode(listaPines[i], OUTPUT);
    digitalWrite(listaPines[i], LOW);
    }    

}

// configuracion inicial de Parola
void configuracionParola() {
    //
    pinMode(DIP1, INPUT_PULLUP);
    pinMode(DIP2, INPUT_PULLUP);
  
    P.begin();
}

void encenderDisplay(int posicion, boolean valor){
  if(valor){
    digitalWrite(listaPines[posicion], HIGH);
    }
  else{
    digitalWrite(listaPines[posicion], LOW);
    }
  
  }

void procesarPines(boolean arreglo[]){
  for(int i=0; i<8; i++){
    encenderDisplay(i, arreglo[i]);
    }
  }

void metodoPot(){
int lectura_pot = analogRead(pinAnalogo);
if(lectura_pot == 0){
  procesarPines(cero);
}
else if(lectura_pot>0 && lectura_pot<=200){
  procesarPines(uno);
  }
else if(lectura_pot>200 && lectura_pot<400){
  procesarPines(dos);
  }
else if(lectura_pot>400 && lectura_pot<614){
  procesarPines(tres);
  }
else if(lectura_pot>600 && lectura_pot<819){
  procesarPines(cuatro);
  }
else if(lectura_pot>800 && lectura_pot<1023){
  procesarPines(cinco);
  }
}

// Metodo encargado de la configuracion inicial de la pantalla 
void configuracionJoystick() {
    // configuracion de los pines de entrada
    pinMode(BTN_START, INPUT_PULLUP);
}

void configuracionTimer() {
    Timer1.initialize(150000);
    Timer1.attachInterrupt(cuentoPulsos);
}

void cuentoPulsos(void) {
    if (digitalRead(BTN_START)) {
        contadorPulsos++;
    } else {
        contadorPulsos = 0;
    }
}

// Metodo encargado de la configuracion inicial de la pantalla 
void configuracionPantantalla() {
    // configuracion inicial de la pantalla 0
    controlador.shutdown(0, false);  // desactiva el ahorro de energia
    controlador.setIntensity(0, 8);  // establece el brillo
    controlador.clearDisplay(0);     // limpia la pantalla

    // configuracion inicial de la pantalla 1
    controlador.shutdown(1, false);  // desactiva el ahorro de energia
    controlador.setIntensity(1, 8);  // establece el brillo
    controlador.clearDisplay(1);     // limpia la pantalla
}

void loop() {
  // put your main code here, to run repeatedly:
  metodoPot(); 
  if (estadoGeneral == 0) {
        //texto estatico
        mensajeTextoAPP();
    } else if (estadoGeneral == 1) {
        //texto dinamico
        mensajeTextoDAPP();
    }
    contadorPulsacion();
}

// funcion que cuenta tiempo de pulsacion
void contadorPulsacion() {
    unsigned long auxContadorPulsos;
    noInterrupts();
    auxContadorPulsos = contadorPulsos;
    interrupts();

    if (((float)auxContadorPulsos / 10) > 3) {
        // Serial.println("Se pulso durante 3 segundos");
        // modo letrero preciono 3 segundos cambio a
        if (estadoGeneral == 0) {
            estadoGeneral = 1;  // modo dinamico 
            Serial.println("Se cambio al modo dinamico");
        } else if (estadoGeneral == 1) {            
            estadoGeneral = 0;  // modo mensaje estatico
            Serial.println("Regresando al modo mensaje estatico");            
        }
        contadorPulsos = 0;
    }
  }


// Ejecucion Mensaje de Texto Estatico

void mensajeTextoAPP(void) {
    estadoDip1 = digitalRead(DIP1);
    estadoDip2 = digitalRead(DIP2);
    

    funcEfectoTexto();
    funcVelocidad();
    
    
}



// Método para subir la velocidad del mensaje de texto
void funcVelocidad() {
  int lectura_pot = analogRead(pinAnalogo);

    if (lectura_pot>=0 && lectura_pot<=200) {
            P.setSpeed(500); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("1");
            // asm("jmp 0x0000"); //reiniciar arduino
              
    }
    
    else if (lectura_pot>=200 && lectura_pot<=400) {
            P.setSpeed(300); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("2");
            // asm("jmp 0x0000"); //reiniciar arduino
              
    }

    else if (lectura_pot>=400 && lectura_pot<=600){
            P.setSpeed(150); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("3");
            // asm("jmp 0x0000"); //reiniciar arduino
    } 

     else if (lectura_pot>=600 && lectura_pot<=800){
            P.setSpeed(75); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("4");
            // asm("jmp 0x0000"); //reiniciar arduino
    } 

     else if (lectura_pot>=819 && lectura_pot<=999){
            P.setSpeed(5); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("5");
            // asm("jmp 0x0000"); //reiniciar arduino
    } 
  }


// Método para sentido del texto, izquierda o derecha, y letra por letra.
void funcEfectoTexto() {
    if (estadoDip1 == LOW && estadoDip2 == LOW) {
        efectoTxt = PA_SCROLL_LEFT;
        posTxt = PA_LEFT;
        P.setTextEffect(efectoTxt, efectoTxt);
        if (bLimpiarMatriz == false) {
            bLimpiarMatriz = true;
            P.displayClear();
        }
    } else if (estadoDip1 == LOW && estadoDip2 == HIGH) {
        efectoTxt = PA_SCROLL_RIGHT;
        posTxt = PA_RIGHT;
        P.setTextEffect(efectoTxt, efectoTxt);
        if (bLimpiarMatriz == true) {
            bLimpiarMatriz = false;
            P.displayClear();
        }
    } else if (estadoDip1 == HIGH) {
        P.print(mensajeTexto[indexMensajeTexto]);
        indexMensajeTexto++;
        if (indexMensajeTexto == mensajeTexto.length()) {
            indexMensajeTexto = 0;
        }
        delay(velocidadTexto);
        P.displayReset();
    }
    if (P.displayAnimate()) {
        P.displayText(mensajeTexto.c_str(), posTxt, velocidadTexto, P.getPause(),
                      efectoTxt, efectoTxt);
    }
}

// Ejecucion Mensaje de Texto Dinamico

void mensajeTextoDAPP(void) {
    estadoDip1 = digitalRead(DIP1);
    estadoDip2 = digitalRead(DIP2);
    

    funcEfectoTextoD();
    funcVelocidadTD();
    
    
}



// Método para subir la velocidad del mensaje de texto
void funcVelocidadTD() {
  int lectura_pot = analogRead(pinAnalogo);

    if (lectura_pot>=0 && lectura_pot<=200) {
            P.setSpeed(500); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("1");
            // asm("jmp 0x0000"); //reiniciar arduino
              
    }
    
    else if (lectura_pot>=200 && lectura_pot<=400) {
            P.setSpeed(300); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("2");
            // asm("jmp 0x0000"); //reiniciar arduino
              
    }

    else if (lectura_pot>=400 && lectura_pot<=600){
            P.setSpeed(150); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("3");
            // asm("jmp 0x0000"); //reiniciar arduino
    } 

     else if (lectura_pot>=600 && lectura_pot<=800){
            P.setSpeed(75); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("4");
            // asm("jmp 0x0000"); //reiniciar arduino
    } 

     else if (lectura_pot>=819 && lectura_pot<=999){
            P.setSpeed(5); // Aplicamos el cambio de velocidad
            //Serial.print("Se cambio la velocidad del mensaje(ms): ");
            //Serial.println("5");
            // asm("jmp 0x0000"); //reiniciar arduino
    } 
  }  


// Método para sentido del texto, izquierda o derecha, y letra por letra.
void funcEfectoTextoD() {
    if (estadoDip1 == LOW && estadoDip2 == LOW) {
        efectoTxt = PA_SCROLL_LEFT;
        posTxtD = PA_LEFT;
        P.setTextEffect(efectoTxt, efectoTxt);
        if (bLimpiarMatrizD == false) {
            bLimpiarMatrizD = true;
            P.displayClear();
        }
    } else if (estadoDip1 == LOW && estadoDip2 == HIGH) {
        efectoTxt = PA_SCROLL_RIGHT;
        posTxtD = PA_RIGHT;
        P.setTextEffect(efectoTxt, efectoTxt);
        if (bLimpiarMatrizD == true) {
            bLimpiarMatrizD = false;
            P.displayClear();
        }
    } else if (estadoDip1 == HIGH) {
        P.print(mensajeTextoD[indexMensajeTextoD]);
        indexMensajeTextoD++;
        if (indexMensajeTextoD == mensajeTextoD.length()) {
            indexMensajeTextoD = 0;
        }
        delay(velocidadTexto);
        P.displayReset();
    }
    if (P.displayAnimate()) {
        P.displayText(mensajeTextoD.c_str(), posTxtD, velocidadTexto, P.getPause(),
                      efectoTxt, efectoTxt);
    }
}
