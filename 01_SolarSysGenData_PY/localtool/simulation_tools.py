#file: simulation_tools.py in e:\code\exercise



from turtle import Turtle, Screen, Vec2D as V2D
from random import randrange
import math

class XYcoordinates():
    """
    Draw X-Y coordinates on the screen
    """
    def __init__(self, sysname, llx, lly, urx, ury, scupdate, canvasupdate):
        self.sysname = sysname
        self.llx, self.lly, self.urx, self.ury = llx, lly, urx, ury
        
        self.wn = Screen(); self.wn.reset(); self.wn.title(sysname)
        self.wn.tracer(scupdate, canvasupdate)  #Screen update frequency control
        self.wn.setup(width=1.0, height=1.0, startx = None, starty = None)

        '''screen color setting'''
        self.sr = randrange(256)
        self.sg = randrange(256)
        self.sb = randrange(256)
        self.sc = "#" + ('%.2x' %self.sr) + ('%.2x' %self.sg) + ('%.2x' %self.sb)
        #suspended: self.wn.bgcolor(self.sc)        
        """
        ''' 'Bang' shape registration    '''
        self.wn.register_shape('Bang', ((15, 0), (5, 5), (0, 15), (-5, 5),
                                                    (-15, 0), (-5, -5), (0, -15), (5, -5)))
        """
        
        xyturtle = Turtle(visible=False)
        self.drawXY(xyturtle)

        self.wn.onkeypress(self.wn.bye, key ="q") # quit if pressed q
        self.wn.listen()

    def drawXY(self, xyturtle): 
        #X-Y axisis
        xyturtle.pensize(2)
        lxlimit = (self.llx, 0); rxlimit = (self.urx, 0); uylimit = (0, self.ury); lylimit = (0, self.lly)
        xyturtle.penup();       xyturtle.goto(lxlimit)
        xyturtle.pendown();   xyturtle.dot(10, 'black'); xyturtle.goto(rxlimit); xyturtle.dot(10, 'black')
        xyturtle.penup();       xyturtle.goto(uylimit)
        xyturtle.pendown();   xyturtle.dot(10, 'black'); xyturtle.goto(lylimit); xyturtle.dot(10, 'black')

        #detail measurement on each axis
        axis ='X'
        xyturtle.penup(); xyturtle.goto(lxlimit); xyturtle.pendown()
        self.axisMeasure(xyturtle, axis, 10, 5, ((self.urx - self.llx)/10 -1))
        axis ='Y'
        xyturtle.penup(); xyturtle.goto(lylimit); xyturtle.pendown()
        self.axisMeasure(xyturtle, axis, 10, 5, ((self.ury - self.lly)/10 -1))       
        
    def axisMeasure(self, xyturtle, axis, width, height, howmany):
        if  howmany == 0: return
        if axis == 'X':
            xyturtle.setheading(0)          #east           
        if axis == 'Y':
            xyturtle.setheading(90)         #north

        xyturtle.forward(width); xyturtle.right(90)
        xyturtle.forward(height/2); xyturtle.left(180)
        xyturtle.forward(height); xyturtle.backward(height/2)

        howmany += -1
        self.axisMeasure(xyturtle, axis, width, height, howmany)


class OnKeyPress:
    """
    function creator for turte.onkeypress
    """
    previouskey = 0                         #previously pressed key
    def __init__(self, key, motionobj):
        self.key = key
        self.motionsys= motionobj.motionsys            

    def __call__(self):    
        '''Clear wirteturtles'''
        self.motionsys.clearDispFormat()
        #print('prev. key: ', OnKeyPress.previouskey,
          #            'self key: ', self.key, '  cleared')        #debug
        OnKeyPress.previouskey = self.key
            
class V2D(tuple):
    """A 2 dimensional vector class, used as a helper class
    for implementing turtle graphics.
    May be useful for turtle graphics programs also.
    Derived from tuple, so a vector is a tuple!

    Provides (for a, b vectors, k number):
       a+b vector addition
       a-b vector subtraction
       a*b inner product
       k*a and a*k multiplication with scalar
       |a| absolute value of a
       a.rotate(angle) rotation
    """
    def __new__(cls, x, y):
        return tuple.__new__(cls, (x, y))
    def __add__(self, other):
        return V2D(self[0]+other[0], self[1]+other[1])
    def __mul__(self, other):
        if isinstance(other, V2D):
            return self[0]*other[0]+self[1]*other[1]
        return V2D(self[0]*other, self[1]*other)
    def __rmul__(self, other):
        if isinstance(other, int) or isinstance(other, float):
            return V2D(self[0]*other, self[1]*other)
    def __sub__(self, other):
        return V2D(self[0]-other[0], self[1]-other[1])
    def __neg__(self):
        return V2D(-self[0], -self[1])
    def __abs__(self):
        return (self[0]**2 + self[1]**2)**0.5
    def rotate(self, angle):
        """rotate self counterclockwise by angle
        """
        perp = V2D(-self[1], self[0])
        angle = angle * math.pi / 180.0
        c, s = math.cos(angle), math.sin(angle)
        return V2D(self[0]*c+perp[0]*s, self[1]*c+perp[1]*s)
    def __getnewargs__(self):
        return (self[0], self[1])
    def __repr__(self):
        return "(%.2f,%.2f)" % self


class V3D(tuple):
    """A 3 dimensional vector class, used as a helper class
    for implementing turtle graphics.
    May be useful for turtle graphics programs also.
    Derived from tuple, so a vector is a tuple!

    Provides (for a, b vectors, k number):
       a+b vector addition
       a-b vector subtraction
       a*b inner product
       k*a and a*k multiplication with scalar
       |a| absolute value of a
       a.rotate(angle) rotation
       a.cross(b) cross(or vector) product    a.cross(b) perpendicular to a and b
    """
    def __new__(cls, x, y, z):
        return tuple.__new__(cls, (x, y, z))
    def __add__(self, other):
        return V3D(self[0]+other[0], self[1]+other[1], self[2]+other[2] )
    def __mul__(self, other):
        if isinstance(other, V3D):
            return self[0]*other[0]+self[1]*other[1] + self[2]*other[2]
        return V3D(self[0]*other, self[1]*other, self[2]*other)
    def __rmul__(self, other):
        if isinstance(other, int) or isinstance(other, float):
            return V3D(self[0]*other, self[1]*other, self[2]*other)
    def __sub__(self, other):
        return V3D(self[0]-other[0], self[1]-other[1], self[2]-other[2])
    def __neg__(self):
        return V3D(-self[0], -self[1], -self[2])
    def __abs__(self):
        return (self[0]**2 + self[1]**2 + self[2]**2)**0.5
    def rotate(self, angle, axis='z'):
        """rotate self counterclockwise by angle
        """
        if axis == 'z':
            perp = V3D(-self[1], self[0], self[2])
            angle = angle * math.pi / 180.0     # convertion to radian
            c, s = math.cos(angle), math.sin(angle)
            return V3D(self[0]*c+perp[0]*s, self[1]*c+perp[1]*s, self[2])
    def __getnewargs__(self):
        return (self[0], self[1], self[2])
    def __repr__(self):
        return "(%.2f,%.2f, %.2f)" % self
    def cross(self, other):
        return V3D(self[1]*other[2] - self[2]*other[1],
                         self[2]*other[0] - self[0]*other[2],
                         self[0]*other[1] - self[1]*other[0])
    
 
