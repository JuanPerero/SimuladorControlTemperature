/*
 *  Programa de simulación control in the loop

 probado en ESP12 (ESP8266) y Arduino UNO
 * 
 *
 */


// Inicializacion del microcontrolador
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
  // Deberia tambien calcularse el error, pero requiere enviar la referencia
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
float h_control = 0;
float procesamiento(float error){
  s_control = ControlPID(error);
  //h_control = invHarmestain(s_control);
  h_control = s_control;

  // Convertir los valores a String
  String errorStr = String(error, 4);       // 4 decimales
  String sControlStr = String(s_control, 4);
  String hControlStr = String(h_control, 4);

  // Enviar por Serial como strings
  Serial.println("Error: " + errorStr);
  Serial.println("s_control: " + sControlStr);
  Serial.println("h_control: " + hControlStr);
  return h_control;
}


// ------------------------ Codigo necesario para el calculo de la inversa ---------------------------------------------
const int TABLE_SIZE = 11;
const float x_vals[TABLE_SIZE] = {0.00, 0.001     , 0.002     , 0.003     , 0.004     , 0.005     , 0.006     , 0.007     , 0.008     , 0.009     , 0.010  };
const float y_vals[TABLE_SIZE] = {0.00, 0.00124893, 0.00941567, 0.02877567, 0.05932893, 0.09680000, 0.13427107, 0.16482433, 0.18418433, 0.19235107, 0.1936 };

float invHarmestain(float contr){
    // Fuera de rango
    if (contr <= y_vals[0]) 
            return x_vals[0];
    if (contr >= y_vals[TABLE_SIZE - 1]) 
            return x_vals[TABLE_SIZE - 1];

    // Buscar intervalo donde y_vals[i] <= contr < y_vals[i+1]
    for (int i = 0; i < TABLE_SIZE - 1; ++i) {
        if (contr >= y_vals[i] && contr <= y_vals[i+1]) {
            float dy = y_vals[i+1] - y_vals[i];
            float dx = x_vals[i+1] - x_vals[i];
            if (dy == 0) 
                return x_vals[i]; 
            float t = (contr - y_vals[i]) / dy;
            return x_vals[i] + t * dx;
        }
    }
    return -1;
}

// ###########################################################################################################
// --------------------- Implementar las funciones necesarias del controlador ------------------------------
// ###########################################################################################################
// - la funcion ControlPID(float error) recibe un float, el error, que será procesado. 
// - Se calcula la salida y se retorna como float
float control_old = 0;
//float q0 = 28861.6;
//float q1 = -56945.9;
//float q2 =  28085.6;
float q0 = 81724.47;
float q1 = -162527.26;
float q2 =  80805.31;

float e_old1 = 0;
float e_old2 = 0;

void init_controler(){
  control_old = 0;
  e_old1 = 0;
  e_old2 = 0;
}

float ControlPID(float error){
   float control = q0*error + q1*e_old1 + q2*e_old2 + control_old;
   control_old = control;
   e_old2 = e_old1;
   e_old1 = error;
   return control;
}
// ###########################################################################################################


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