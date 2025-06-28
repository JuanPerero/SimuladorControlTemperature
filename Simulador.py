from flask import Flask, render_template, request, jsonify, send_from_directory
from flask_socketio import SocketIO
import threading
import serial
import struct
import time
import re
from Tankmodel import *
from Tempmodel import *

# Configuración del puerto serie
ser = serial.Serial(
    #port='/dev/ttyUSB0',              # Ajustar al puerto correcto
    #baudrate=115200,
    #bytesize=serial.EIGHTBITS,
    #parity=serial.PARITY_NONE,
    #stopbits=serial.STOPBITS_ONE,
    #timeout=1
)

app = Flask(__name__, static_folder='static')
socketio = SocketIO(app)

# Variable para almacenar el último valor recibido
ultimo_valor = 0
# Flag para controlar ejecución del programa
global ejecutando
ejecutando = True

################################################################################
#####                   RUTAS DEL SERVIDOR FLASK                           #####
################################################################################

@app.route('/')
def index():
    return render_template('indexSimulator.html')

@app.route('/meter_animation.js')
def serve_tank_animation():
    return app.send_static_file('meter_animation.js')
@app.route('/configuraciones.html')
def serve_configuration():
    return app.send_static_file('configuraciones.html')


# Ruta para enviar comandos al puerto serial
'''
@app.route('/enviar_comando', methods=['POST'])
def enviar_comando():
    comando = request.json.get('comando', '')
    try:
        # Enviar el comando al puerto serial
        ser.on_send(str(comando))
        print("Conectado\r") #jsonify({"status": "success", "mensaje": f"Comando '{comando}' enviado"})
        return 
    except Exception as e:
        print("ERROR\r")
        return #jsonify({"status": "error", "mensaje": str(e)}), 500
'''



################################################################################
#####                   CONTROL DE ACTIVIDAD DE LA WEB                     #####
################################################################################
# In place of something like this
#@app.before_first_request
#def first_request():
#    print("BEFORE FIRST REQUEST")

@app.before_request
def second_request():
    print("BEFORE Rec")

################################################################################
#####                   FUNCION DE INICIALIZACION SERIAL                   #####
################################################################################

@socketio.on('startSimulation')
def Inicializacion(data): 
    print("LLEGO EL LLAMADO")
    print("Parámetros recibidos:", data)
    isopen = init_serial(data)
    if isopen:
        init_simulation(data)


def init_serial(params):
    try:
        ser.port = params['serialPort']
        ser.baudrate = params['baudRate']
        ser.open()
        print("Puerto serie abierto")
        socketio.emit('serialStatus', {'status': 'connected'}, to=request.sid)
        return True
    except serial.SerialException as e:
        print(f"Error al abrir el puerto serie: {e}")
        socketio.emit('serialStatus', {'status': 'error'}, to=request.sid)
        return False


################################################################################
#####                   FUNCIONES DE INICIALIZACION DE LA SIMULACION       #####
################################################################################


global Instancia_simulacion
Instancia_simulacion = None        
def init_simulation(params):
    global Instancia_simulacion
    Tinicial = params['systemParam1']
    CtteCe = params['systemParam2']
    CtteM = params['systemParam3']
    referencia = params['systemParam4']
    # Convertit todos los valores a float
    Tinicial = float(Tinicial)
    CtteCe = float(CtteCe)
    CtteM = float(CtteM)
    referencia = float(referencia)
    Instancia_simulacion = Kettle(CtteCe, CtteM, referencia, T0=Tinicial)

def set_reference(valor):
    # Agregar la carga de la referencia desde la web. 
     socketio.emit('change_ref', {'valor': valor})
     # Tambien vas a necesitar el del cambio de parametros



################################################################################
#####                   FUNCIONES DE CAMBIO DE PARAMETROS                  #####
################################################################################

@socketio.on('changeparameter')
def Change_parameters(data): 
    global ejecutando
    if ejecutando:
        print("Cambio de parámetros:", data)
        global Instancia_simulacion
        #Tinicial = params['systemParam1']
        CtteCe = params['systemParam2']
        CtteM = params['systemParam3']
        referencia = params['systemParam4']
        # Convertit todos los valores a float
        #Tinicial = float(Tinicial)
        CtteCe = float(CtteCe)
        CtteM = float(CtteM)
        referencia = float(referencia)
        Instancia_simulacion.set_values(CtteCe, CtteM, referencia, Instancia_simulacion.T0)
    


################################################################################
#####                   REINICIO DEL SISTEMA                               #####
################################################################################

@socketio.on('resetsimulation')
def clear_all_resources(data):
    # Limpiar instancia de simulacion  
    global Instancia_simulacion
    del Instancia_simulacion
    Instancia_simulacion = None
    # cerrar puerto serie
    ser.close()
    # Deshabilitacion de bandera de ejecucion
    global ejecutando  
    ejecutando = False



################################################################################
################################################################################

#lock_serial = threading.Lock()
y_sense = 0
def ciclo_comando_y_lectura():
    y_sense = Instancia_simulacion.T0 
    #Enviar referencia
    global ejecutando  
    while ejecutando:
        try:
            #####################################################
            ## Este segmento deberia estar en el microcontrolador
            #####################################################
            # Calculo del Error para enviar al controlador
            error = Instancia_simulacion.ref - y_sense
            #####################################################

            # Envio data del sensor simulado
            envio_serial(error)          #    envio_serial(y_sense) 
            # Recepcion del dato del controlador
            ctrl_sig = recep_serial()
            # Calculo sobre el modelo
            y_sense = Instancia_simulacion.step(ctrl_sig)
            p_cicle = Instancia_simulacion.harmestain
            # Guardado del calculo para enviarlo en la siguiente iteracion
            socketio.emit('actualizacion_valor', {'valor': y_sense, 'ctrl': ctrl_sig, 'ciclo': p_cicle})
            time.sleep(Instancia_simulacion.dt)
        except Exception as e:
            print(f"[ERROR GENERAL] {e}")
            time.sleep(10)


def envio_serial(valor_float):
    # Codifica el float en 4 bytes
    data_bytes = struct.pack('f', valor_float)
    ser.write(data_bytes)

def recep_serial():
    # Lee 4 bytes del puerto serial y decodifica a float
    data = ser.read(4)
    if len(data) < 4:
        print("Advertencia: no se recibieron 4 bytes")
        return 0.0
    valor_float = struct.unpack('f', data)[0]
    return valor_float


def iniciar_servidor_flask():
    print("ejecutar el servidor Flask en un hilo separado")
    socketio.run(app, debug=False, host='0.0.0.0', port=5000)


if __name__ == '__main__':
    try:
        # Iniciar servidor Flask, ¿crea su propio hilo? NO se, pero si no lo hago en hilo separado no avanza la ejecucion
        serial_thread = threading.Thread(target=iniciar_servidor_flask, daemon=True)
        serial_thread.start()

        while True:
            if Instancia_simulacion is None: # is not None:
                time.sleep(2)
            else:
                print("Inicio de instancia de simulación")
                ejecutando = True
                ciclo_comando_y_lectura()
        
    except KeyboardInterrupt:
        print("Programa detenido por el usuario")
    except Exception as e:
        print(f"Error en el programa principal: {e}")
    finally:
        ejecutando = False
        print("Finalizando programa...")