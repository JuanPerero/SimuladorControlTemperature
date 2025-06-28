import numpy as np
from scipy.integrate import solve_ivp

class Kettle:
    """
    Clase que modelauna pava eléctrica (Electric kettle)
    
    Parámetros:
    -----------
    """
    
    def __init__(self, Ce, m, reference, dt=1, T0=25.0):
        self.Ce = Ce       # Calor especifico del liquido [J/(kg*ºC)]
        self.m = m         # masa del liquido en el resipiente [kg]
        self.dt= dt        # Intervalo de tiempo [s]
        self.T0 = T0       # Temperatura inicial del agua [m]
        self.ref = reference

        # Parametros del sistema
        self.pA = 2.373e-6
        self.pB = 2.357e-6
        self.pC = 1.98
        self.pD = -0.9802
        # Buffer de salidas
        self.Y1 = T0
        self.Y2 = T0
        # Buffer de entradas
        self.X1 = 0
        self.X2 = 0
        self.harmestain = 0


    def step(self,control_value):
        #def calentador(T_actual, Pe, A, C, dt):
        """
        Calcula el próximo valor de altura del tanque usando integración Euler.
        -----------
        """

        # Etapa de regulador de potencia
        ####theta = np.linspace(0, 0.01, 100)
        ####t = 0.01-theta
        ####P_part1 = t/2
        ####P_part2 = np.sin(2*50*2*np.pi*t)
        ####P = 3872* (0.005-P_part1+P_part2/(400*np.pi))

        step = 0.01-control_value
        aux_1 = step/2
        aux_2 = np.sin(200*np.pi*step)
        P = 3872* (0.005-aux_1+aux_2/(400*np.pi))
        E = 0.01*P

        self.harmestain 
        E = control_value

        ###### Podria hacerse en forma separada
        # Etapa del sistema
        # Sensor
        ######

        # Sistema y sensor unificado
        salida = self.pA*self.X1 + self.pB*self.X2 + self.pC*self.Y1 + self.pD * self.Y2
        # Actualizacion de los buffers
        self.Y2 = self.Y1
        self.Y1 = salida
        self.X2 = self.X1
        self.X1 = E
        return salida

  # Parametros del sistema
        self.pA = 2.373e-6
        self.pB = 2.357e-6
        self.pC = 1.98
        self.pD = -0.9802
        # Buffer de salidas
        self.Y1 = T0
        self.Y2 = T0
        # Buffer de entradas
        self.X1 = 0
        self.X2 = 0


    #def set_values(self, A, C, reference, h0):
    def set_values(self, Ce, m, reference, T0=25.0):
        # Actualizacion de los parametros del tanque
        self.Ce = Ce       # Calor especifico del liquido [J/(kg*ºC)]
        self.m = m         # masa del liquido en el resipiente [kg]
        self.ref = reference
        self.T0 = T0       # Temperatura inicial del agua [m]
        return

