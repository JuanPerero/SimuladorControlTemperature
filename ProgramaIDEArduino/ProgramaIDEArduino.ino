/*
 *  Programa de simulación control in the loop

 probado en ESP12 (ESP8266) y Arduino UNO
 * 
 *
 */

void setup() {
  Serial.begin(115200);

  init_controler();

  while (!Serial) {
    ; // Esperar a que se establezca la conexión
  }
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
  union {
    byte b[4];
    float f;
  } data;

  while (true) {
    // Esperar a que llegue al menos 1 byte
    if (Serial.available()) {
      char header = Serial.read();

      // Comando especial: "XX"
      if (header == 'X') {
        // Esperar el segundo carácter
        while (!Serial.available()) {}
        char second = Serial.read();
        if (second == 'X') {
          init_controler();
        }
      }
      // Si el header es 'F', espera 4 bytes para el float
      else if (header == 'F') {
        int bytes_recibidos = 0;
        while (bytes_recibidos < 4) {
          if (Serial.available()) {
            data.b[bytes_recibidos++] = Serial.read();
          }
        }
        return data.f;
      }
    }
  }
}

// ---------------------------------------- Procesamiento

float s_control = 0;
float procesamiento(float error){
  s_control = ControlPID(error);
  //return invHarmestain(s_control);
  return s_control;
}

float invHarmestain(float contr){
  float step = 0.01-contr;
  float aux_1 = step/2;
  float aux_2 = sin(200*PI*step);
  float E = 38.72*(0.005-aux_1+aux_2/(400*PI));
  return 1/E;
}


float control_old = 0;
float q0 = 9.16385;
float q1 = -16.6625;
float q2 = 7.57385;
float e_old1 = 0;
float e_old2 = 0;

float ControlPID(float error){
   float control = q0*error + q1*e_old1 + q2*e_old2 + control_old;
   control_old = control;
   e_old2 = e_old1;
   e_old1 = error;
   
   //Serial.print("error: "); Serial.print(error); Serial.print(", ");
   //Serial.print("e_old1: "); Serial.print(e_old1); Serial.print(", ");
   //Serial.print("e_old2: "); Serial.print(e_old2); Serial.print(", ");
   //Serial.print("control_old: "); Serial.println(control_old);  // usa println al final
   //Serial.print("control: "); Serial.println(control);  // usa println al final
   return control;
}

void init_controler(){
  control_old = 0;
  e_old1 = 0;
  e_old2 = 0;
}

// ---------------------------------------- Salida 
void salida(float control){
     union {
      byte b[4];
      float f;
    } respuesta;
    respuesta.f = control;
    Serial.write("R");
    Serial.write(respuesta.b, 4);
    Serial.write("C");
}