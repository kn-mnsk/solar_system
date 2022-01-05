#Project: PY01_SolarSysGenData
"""
Generate motiondata only
Rewrite the original 'simulation_solarsystem_01.py' with turle, in use of matplotlib
"""

import sys
import argparse
import time
import os.path
import time
#import math
sys.path.append('../localtool')
from simulation_algorism import MotionAlgorism
import numpy as np 
from numpy.linalg import norm
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d import proj3d
from matplotlib.patches import Circle, Wedge, Polygon
from matplotlib.collections import PatchCollection
import matplotlib.pyplot as plt 
import matplotlib.animation as animation
from scipy.spatial.distance import squareform, pdist, cdist

Rootpath = "../../.globaldata/other/"

Starttime, Timestep = 0.00, 0.01    #unit:days
N = 2       # unit: a round of earth orbit
Repeats = int(365 * N / Timestep)  # unit: days

Algorism_name = "rungekutta2"
motiondataName = "motiondataArchive/solarsys_motiondata_" + Algorism_name + "_" + str(Repeats)
motiondataFile = Rootpath + motiondataName + ".csv"

class GenSolarSysMotiondata(MotionAlgorism):
   
    def __init__(self, starttime, timestep, repeats):
        MotionAlgorism.__init__(self, starttime, timestep, repeats)
        # values setting
        
        """ Sceince unit to display unit
               mass: Kg
               radius: Km
               initxy: Km
            initmov: m/second
        """
        #Gm: Gravity constant  6.67 x 10(-11) Nm(2)/kg(2)
        Gm = (6.67430*(10 **-11))
        self.Am = 1.495979  * (10 ** 11)         # astronomical unit in meter
        Am2= pow(self.Am, 2)
        self.Pau = 200                               # one au in pixel
        Px2 = pow(self.Pau, 2)
        Gpx = (Gm / Am2) * Px2           #Gm now converted to display unit

        # coeffcient to calculate velociy in pixel/day
        Ds = 60 * 60 * 24                       # seconds / day 
        self.Pday = (1 / self.Am) * self.Pau * Ds          # pixel / day: velocity(m/s) * Pday  : 
        self.Gpx = Gpx * (self.Pau * pow(Ds, 2) / self.Am) # Newton : converted to kg ･　pixel / day(2)

        self.algorism = {
            "feynman": self.feynman,
            "rungekutta1": self.rungekutta1,
            "rungekutta2": self.rungekutta2,
            "rungekutta3": self.rungekutta3,
            }


        self.systimer = starttime
        self.timestep = timestep
        self.repeats = repeats   # repeats number of time step
        
        #read planets constants file to set up database
        file_name = Rootpath + "solarsys_constantdata.csv"
        cols = list(np.arange(0, 26, 1))
        constantLabel = \
              ['planetId','planetName', 'color', 'size', 'shaderName', 'textName', 'satelliteOfId',  'scalerX',  'scalerY',  'scalerZ',  'mass', 'radius', 'obliquity', 'inclination', 'rotationPeriod', 'rotationVelocity', 'orbitalPeriod', 'orbitalSpeed', 'distanceToParent', 'distanceToParentAU', 'positionX', 'positionY', 'positionZ', 'velocityX', 'velocityY', 'velocityZ']

        # Create planets constants dataframe
        self.constant = pd.read_csv(file_name, header=0, names=constantLabel, usecols=cols)

        ##debug
        #self.constant.to_csv(path_or_buf='../data/other/motionsys_planets_constants_3d_debug.csv')
        #debug end

        self.planetId = list(self.constant.planetId)
        self.parentid = list(self.constant.satelliteOfId)
        self.numStars = len(self.constant.planetId)
        self.starNames = list(self.constant.planetName)
        self.nextdataLabel = ['repeat', 'systimer', 'planetId', 'planetName', 'satelliteOfId', 'scalerX',  'scalerY',  'scalerZ', 'mass', 'radius',  'scaledPositionX', 'scaledPositionY', 'scaledPositionZ', 'positionX', 'positionY', 'positionZ', 'velocityX', 'velocityY', 'velocityZ', 'accelerationX', 'accelerationY', 'accelerationZ']
        self.motiondataLabel = ['repeat', 'systimer', 'planetId',  'planetName', 'scaledPositionX', 'scaledPositionY', 'scaledPositionZ', 'positionX', 'positionY', 'positionZ', 'velocityX', 'velocityY', 'velocityZ','accelerationX', 'accelerationY', 'accelerationZ']


        #create nextdata and set values at 0.0 as initialdata
        self.nextdata = pd.DataFrame(None, columns=self.nextdataLabel )
      
        self.nextdata[['planetId', 'planetName', 'satelliteOfId', 'scalerX',  'scalerY',  'scalerZ', 'mass', 'radius','positionX', 'positionY', 'positionZ', 'velocityX', 'velocityY', 'velocityZ']] = self.constant[['planetId', 'planetName', 'satelliteOfId', 'scalerX',  'scalerY',  'scalerZ', 'mass', 'radius', 'positionX', 'positionY', 'positionZ', 'velocityX', 'velocityY', 'velocityZ']]
        
        self.nextdata.repeat = 0
        self.nextdata.systimer = 0.0
        self.nextdata = self.nextdata.replace(np.nan, 0.0)

        #convert to Pixcel
        self.nextdata.loc[:, 'radius'] *= (1000 / self.Am) * self.Pau  # in pixel
        self.nextdata.loc[:, ('positionX', 'positionY', 'positionZ')] *= (1000 / self.Am) * self.Pau  # in pixel
        self.nextdata.loc[:, ('velocityX', 'velocityY', 'velocityZ')] *= self.Pday   #note: timestep unit is day  pixel/day      
        # Scaling xx, yy and zz, and then set them to scaledXYZ
        self.nextdata = self.scalingPos(self.nextdata)

  
    def scalingPos(self, df):
        """
        dfdata: Pandas DataFrame
        Convert Au(astronomical unit to PX(pixel on window) in motiondata
        Must be used only after the completion of calculation velocity and accellation
        """
        C = df.loc[:, ('positionX', 'positionY', 'positionZ')].to_numpy()
        P = pd.concat([df[df.planetId==i].loc[:, ('positionX', 'positionY', 'positionZ')] for  i in df.satelliteOfId],  ignore_index=True).to_numpy()
        PX = df.loc[:, ( 'scalerX',  'scalerY',  'scalerZ')].to_numpy()
        df.loc[:, ('scaledPositionX', 'scaledPositionY', 'scaledPositionZ')] = (C - P) * PX + P

        return df

    
    def acceleration(self, posv3, *constants):
        """
        c: planets.constants  # no use
        d: motion data container
        pos: stars position vectors as argument
        pos = np.array(list(zip(d._xx, d._yy, d_zz)))
        see Feynman Physics
        calculate acceleration at time 0 and after
        a(t) = - Gpx * m * r / |r|(3)      a and r are vectors
                                                    r: vector (in pixel)
                                                    m : counter planet
                                                    Gpx: gravity constant in pixel
        """
        
        # calcualte position vectors for each star to others, with itself centered
        # deltaPos.ndim is 3-dimention
        R = np.array([posv3 - posv3[i-1,:] for i in self.planetId])

        #pair-wise distance
        distR = squareform(pdist(posv3))
        distR = distR.reshape((distR.shape + (1,)))  # conform to the same dimention as deltaPos for later calc.
        # counter mass
        mass = self.constant.mass.to_numpy() # although including self, mass * R makes itself none
        mass = mass.reshape((mass.shape + (1,)))  # conform to 2 dimensions
      
        with np.errstate(divide='ignore', invalid='ignore'):
            accel = (np.nan_to_num(self.Gpx * mass * R / np.power(distR, 3))).sum(axis=1)
        #accel = accel.sum(axis=1)   # we get acceleration by star

        return accel   # numpy ndarray

     
    def velocity(self, dummy):
        pass
              
 
    def calcMotiondata(self):
        """
        Calculate next data of position, velocity and acceleration by the current data
        and update motion data panel
        """
        motiondata = pd.DataFrame(None, columns=self.motiondataLabel)
        motiondata.to_csv(motiondataFile, index=False, mode='w') #header only

        motiondataDF = self.nextdata[self.motiondataLabel]
        motiondataList = [motiondataDF.copy()] # deep copy of initial data

   

        try:
            repeat = 0
            tic = time.perf_counter()

            while repeat < self.repeats:
                ##print('Data calculation counted = ', counter)
                if (repeat % 300) == 0 and repeat != 0: 
                    toc = time.perf_counter()
                    print('data calculated: %#6d days out of %d, elapsed:%#7.2f minutes, remaining:%#7.2f minutes' %(repeat, self.repeats, (toc-tic)/60.0, 0 if (repeat==0) else((self.repeats / repeat) * (toc -tic) / 60.0) - (toc-tic)/60.0))
                    motiondata = pd.concat(motiondataList)
                    motiondata.to_csv(motiondataFile, header=False, index=False, mode='a')
                    motiondataList = []

                #current time
                posV3In = self.nextdata.loc[:, ('positionX', 'positionY', 'positionZ')].astype(float).to_numpy()
                velV3In = self.nextdata.loc[:, ('velocityX', 'velocityY', 'velocityZ')].astype(float).to_numpy()
                
                #next time
                self.systimer += self.timestep
                repeat += 1
                self.nextdata['systimer'] = self.systimer
                self.nextdata['repeat'] = repeat

                self.nextdata.loc[:, ('positionX', 'positionY', 'positionZ')], self.nextdata.loc[:, ('velocityX', 'velocityY', 'velocityZ')], self.nextdata.loc[:, ( 'accelerationX', 'accelerationY', 'accelerationZ')] = self.algorism.get(Algorism_name)(posV3In, velV3In)

                self.nextdata = self.scalingPos(self.nextdata)
                motiondataDF = self.nextdata[self.motiondataLabel]
                motiondataList.append(motiondataDF.copy()) # deep copy

            print('data calculation completed: %#6d days' %(repeat))
            motiondata = pd.concat(motiondataList)
            motiondata.to_csv(motiondataFile, header=False, index=False, mode='a')
            motiondataList = []
            del motiondata

        except Exception  as e:
            print('Error In calcMotionData: ', e.args[0])
            print('motiondata so far generated to be dumped')
            motiondata = pd.concat(motiondataList)
            motiondata.to_csv(motiondataFile, header=False, index=False, mode='a')
            motiondataList = []
            del motiondata
            return False
        finally:
            return True
    

def main(starttime, timestep, repeats):

    solarsysdata = GenSolarSysMotiondata(starttime, timestep, repeats)

    if os.path.isfile(motiondataFile):
        print(motiondataFile + " EXISTS")
        print("No data generation, and program ends")
    else:
        print(motiondataFile + " NO EXISTANT - Start Data Generation, TAKES TIME!") 
        end = solarsysdata.calcMotiondata()
        if end==False:
            quit()
        else:
            print("Program appropriately completed ")
      
   
    
if __name__ == '__main__':

    main(Starttime, Timestep, Repeats)


