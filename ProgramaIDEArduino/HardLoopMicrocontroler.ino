/*
 *  First simulation in the loop

 ESP12 (ESP8266) Programa Avanzado de Respuesta Serial
 * 
 * Este programa simula un sistema de sensores que responde a comandos específicos:
 * A: Simula lectura de un sensor de nivel (100-250 con patrón ondulante)
 * B: Simula lectura de temperatura (300-450 con cambios graduales)
 * C: Simula lectura de presión (500-650 con pequeños picos)
 * 
 * Incluye funciones de depuración y LED indicador de actividad.
 */


void setup() {
  // Configurar comunicación serial
  Serial.begin(115200);
  while (!Serial) {
    ; // Esperar a que se establezca la conexión

  }
  // Inicializar generador de números aleatorios
  randomSeed(analogRead(A0));
  
}


float inputs = 0;
float controls = 0;
float outputs = 0;

void loop() {
  inputs = entrada();
  // Deveria tambien calcularse el error
  controls = procesamiento(inputs);
  salida(controls);
}


// ############################################################################
// ------------------------ Funciones de un controlado digital 
// ############################################################################
/*
Para implementar un control de cualquier tipo, se necesitarán 3 tipos de funciones. 
Las configuraciones utilizar los recursos del microcontrolador en estas funciones debe configurarse previamente y dependeran del diseño y aplicacion.
*/

// ---------------------------------------- Entrada
float entrada(){
  return SimulateIn();
  //RealInput();
}

float SimulateIn() {
  union {
    byte b[4];
    float f;
  } data;
  int bytes_recibidos = 0;
  // Esperar hasta recibir los 4 bytes
  while (bytes_recibidos < 4) {
    if (Serial.available()) {
      data.b[bytes_recibidos++] = Serial.read();
    }
  }
  float valorEntrada = data.f;
  return valorEntrada;
}

float SimulateControlIn(){
  float valorRespuesta = -20.0;
  bool flag = true;
  
  while(flag) {
    // Esperar a que haya datos disponibles
    while (Serial.available() <= 0) {
      // Esperando datos
    }
    
    // Leer el primer carácter (encabezado)
    char comandoRecibido = Serial.read();
    
    // Si recibimos 'A', debemos leer un número real
    if (comandoRecibido == 'A') {
      // Esperar a que lleguen más datos que contengan el número
      delay(50); // Pequeña pausa para asegurar que todos los datos han llegado
      
      // Leer el número real como una cadena
      String numeroStr = "";
      while (Serial.available() > 0) {
        char c = Serial.read();
        // Solo añadir dígitos, punto decimal y signo negativo
        if (isDigit(c) || c == '.' || c == '-') {
          numeroStr += c;
        }
      }
      
      // Convertir la cadena a float si no está vacía
      if (numeroStr.length() > 0) {
        valorRespuesta = numeroStr.toFloat();
        flag = false;
      }
    } 
    // Mantener la compatibilidad con los otros comandos
    /*
    else if (comandoRecibido == 'B') {
      valorRespuesta = outputs - 15;
      flag = false;
    } 
    else if (comandoRecibido == 'C') {
      valorRespuesta = random(0, 750);
      flag = false;
    }
    */
    else {
      // Limpiar el buffer serial si recibimos algo que no reconocemos
      while (Serial.available() > 0) {
        Serial.read();
      }
    }
  }
  return valorRespuesta;
}


float RealInput(){
  return 66;
}

// ---------------------------------------- Procesamiento

float procesamiento(float error){
  return ControlTipoX(error);
  //return ControlPID(error);
  //return ControlDifuso(error);
}

float invHarmestain(float contr){
  float step = 0.01-contr;
  float aux_1 = step/2;
  float aux_2 = sin(200*PI*step);
  float E = 38.72*(0.005-aux_1+aux_2/(400*PI));
  return 1/E;

}

float ControlTipoX(float error){
   return error * 20;
}

// ---------------------------------------- Salida 
void salida(float control){
    SimulateOut(control);
}

void SimulateOut(float control){
    /*
    // Comtrol de entrada de agua negativa
    if(control < 0){
      control = 0;
    }
  */
     union {
      byte b[4];
      float f;
    } respuesta;
    respuesta.f = control;
    Serial.write(respuesta.b, 4);
}