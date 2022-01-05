#file: simulation_algorism.py


class MotionAlgorism:
    def __init__(self, starttime, timestep, repeats):
        self.starttime = starttime
        self.timestep = timestep
        # repeats and timestep can be time in day, hour or second
        # timestep must be either of 1, 0.1, 0.01
        self.repeats = int(repeats / timestep) #no remainder
        

    def acceleration(self, var, *constants):                # var: vector position, a function of time
        assert False, 'Function: acceleration must be defined'            #Or raise exception

    def velocity(self, var):                     # var: vector position, a function of time
        assert False, 'Function: velocity must be defined'            #Or raise exception


    def rungekutta1(self, p0, v0, *constants):  #value of integral f at a given var
        """
        4th order Runge-Kutta (RK4) method
        https://lpsa.swarthmore.edu/NumInt/NumIntFourth.html

        dy(t)/dt = y'(t) = f(y(t), t),    with y(t0) = y0 
        h: time step

        k1 = f(y*(t0), t0) = y'*(t0)              p*(t0) is an approximation to p(t0)
        k2 = f(y*(t0) + k1 * h/2, t0 + h/2) = y'*(t0) + k1 * h/2
        k3 = f(y*(t0) + k2 * h/2, t0 + h/2) = y'*(t0) + k2 * h/2
        k4 = f(y*(t0) + k3 * h, t0 + h) = y'*(t0) + k3 * h

        y*(t0+h) = y*(t0) + (k1*1/6 + k2*1/3 + k3*1/3 + k4*1/6) * h

        Example given as follows:
        dv/dt = a(t), dp/dt = v(t) , a = GM * mass / power(r, 2) 
        initial values : t = t0, p(t0) = p0,  v(t0) = v0, a(t0) = a0
        h: time step

        step1: at t0
            a0 = a(p0)
            v0 = v(t0) 
            p0 = p(t0)
        
        step2: at t0 + h/2
            v1 = v0 + a0 * h/2
            p1 = p0 + v1 * h/2 
            a1 = a(p1)

        step3: at t0 + h/2
            v2 = v0 + a1 * h/2
            p2 = p0 + v2 * h/2
            a2 = a(p2)

        step4: at t0 + h
            v3 = v0 +a2 * h
            p3 = p0 + v3 * h
            a3 = a(p3)

        finally,at t0 + h, we calculate:
        p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
        v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
        a = a(p)

        we get (p, v, a)

        Repeating the above steps, we get the trajectory of a motion
        """       
        
        h = self.timestep
        a0 = self.acceleration(p0)
        
        v1 = v0 + a0 * h / 2
        p1 = p0 + v1 * h / 2
        a1 = self.acceleration(p1)

        v2 = v0 + a1 * h / 2
        p2 = p0 + v2 * h / 2
        a2 = self.acceleration(p2)
        
        v3 = v0 + a2 * h
        p3 = p0 + v3 * h      
        a3 = self.acceleration(p3)

        v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
        p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
        a = self.acceleration(p)

        return ( p, v, a)  # at next time step
        
    def rungekutta2(self, p0, v0, *constants):  #value of integral f at a given var
        """
        4th order Runge-Kutta (RK4) method
        https://lpsa.swarthmore.edu/NumInt/NumIntFourth.html

        dy(t)/dt = y'(t) = f(y(t), t),    with y(t0) = y0 
        h: time step

        k1 = f(y*(t0), t0) = y'*(t0)              p*(t0) is an approximation to p(t0)
        k2 = f(y*(t0) + k1 * h/2, t0 + h/2) = y'*(t0) + k1 * h/2
        k3 = f(y*(t0) + k2 * h/2, t0 + h/2) = y'*(t0) + k2 * h/2
        k4 = f(y*(t0) + k3 * h, t0 + h) = y'*(t0) + k3 * h

        y*(t0+h) = y*(t0) + (k1*1/6 + k2*1/3 + k3*1/3 + k4*1/6) * h

        Example given as follows:
        dv/dt = a(t), dp/dt = v(t) , a = GM * mass / power(r, 2) 
        initial values : t = t0, p(t0) = p0,  v(t0) = v0, a(t0) = a0
        h: time step

        step1: at t0
            a0 = a(p0)
            v0 = v(t0) 
            p0 = p(t0)
        
        step2: at t0 + h/2
            v1 = v0 + a0 * h/2
            p1 = p0 + v1 * h/2 
            a1 = a(p1)

        step3: at t0 + h/2
            v2 = v0 + a1 * h/2
            p2 = p0 + v2 * h/2
            a2 = a(p2)

        step4: at t0 + h
            v3 = v0 +a2 * h
            p3 = p0 + v3 * h
            a3 = a(p3)

        finally,at t0 + h, we calculate:
        p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
        v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
        a = a(p)

        we get (p, v, a)

        Repeating the above steps, we get the trajectory of a motion
        """       
        
        h = self.timestep
        a0 = self.acceleration(p0)
        
        p1 = p0 + v0 * h / 2
        v1 = v0 + a0 * h / 2
        a1 = self.acceleration(p1)

        v2 = v0 + a1 * h / 2
        p2 = p0 + v1 * h / 2
        a2 = self.acceleration(p2)
        
        v3 = v0 + a2 * h
        p3 = p0 + v2 * h      
        a3 = self.acceleration(p3)

        v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
        p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
        a = self.acceleration(p)

        return ( p, v, a)  # at next time step
        
    def rungekutta3(self, p0, v0, *constants):  #value of integral f at a given var
        """
        4th order Runge-Kutta (RK4) method
        https://lpsa.swarthmore.edu/NumInt/NumIntFourth.html

        dy(t)/dt = y'(t) = f(y(t), t),    with y(t0) = y0 
        h: time step

        k1 = f(y*(t0), t0) = y'*(t0)              p*(t0) is an approximation to p(t0)
        k2 = f(y*(t0) + k1 * h/2, t0 + h/2) = y'*(t0) + k1 * h/2
        k3 = f(y*(t0) + k2 * h/2, t0 + h/2) = y'*(t0) + k2 * h/2
        k4 = f(y*(t0) + k3 * h, t0 + h) = y'*(t0) + k3 * h

        y*(t0+h) = y*(t0) + (k1*1/6 + k2*1/3 + k3*1/3 + k4*1/6) * h

        Example given as follows:
        dv/dt = a(t), dp/dt = v(t) , a = GM * mass / power(r, 2) 
        initial values : t = t0, p(t0) = p0,  v(t0) = v0, a(t0) = a0
        h: time step

        step1: at t0
            a0 = a(p0)
            v0 = v(t0) 
            p0 = p(t0)
        
        step2: at t0 + h/2
            v1 = v0 + a0 * h/2
            p1 = p0 + v1 * h/2 
            a1 = a(p1)

        step3: at t0 + h/2
            v2 = v0 + a1 * h/2
            p2 = p0 + v2 * h/2
            a2 = a(p2)

        step4: at t0 + h
            v3 = v0 +a2 * h
            p3 = p0 + v3 * h
            a3 = a(p3)

        finally,at t0 + h, we calculate:
        p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
        v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
        a = a(p)

        we get (p, v, a)

        Repeating the above steps, we get the trajectory of a motion
        """       
        
        h = self.timestep
        a0 = self.acceleration(p0)
        
        p1 = p0 + v0 * h / 2
        v1 = v0 + a0 * h / 2
        a1 = self.acceleration(p1)

        v2 = v1 + a1 * h / 2
        p2 = p1 + v1 * h / 2
        a2 = self.acceleration(p2)
        
        v3 = v0 + a0 * h
        p3 = p0 + v0 * h      
        a3 = self.acceleration(p3)

        v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
        p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
        a = self.acceleration(p)

        return ( p, v, a)  # at next time step
        

        
    def feynman(self, p0, v0,  *constants):
        """
                feyman physics: numeric calcutaion method     
        """
        h = self.timestep
        a0 = self.acceleration(p0)   # function of acceleration
        if (h == 0.0):
            v = v0 + a0 * h / 2
        else:
            v = v0 + a0 * h
            
        p = p0 + v * h
        a = self.acceleration(p)
        
        #DEBUG print('debug type dF, type(dF))
        return (p, v, a)