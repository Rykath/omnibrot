#!/usr/bin/python3
'''
Xtremebrot - deepixel
runpy/deepixel.py

create mandelbrot using the deepixel module
'''

import asdf     #Advanced Scientific Data Format - metadata+image
import numpy
import subprocess

data = {
    'type':     'Mandelbrot',
    'x-axis':   'Cr',
    'y-axis':   'Ci'
}

def test(imax,cr,ci):
    zr = cr
    zi = ci
    zr2 = zr*zr
    zi2 = zi*zi
    for i in range(imax):
        if zr2+zi2 > 4:
            return i
        zi = 2*zr*zi + ci
        zr = zr2 - zi2 + cr
        zi2 = zi*zi
        zr2 = zr*zr
    return imax

cornerR = -2
cornerI = -2
widthR = 4
widthI = 4
numX = 5
numY = 5

command = './deepixel.out'
iteration_max = 100
process_num = 1

image = []
for _ in range(numX):
    image.append([None]*numY)
print("\n".join([str(i) for i in image]))

cluster = [None]*process_num
clusterinfo = [None]*process_num

factorXR = widthR/numX
factorYI = widthI/numY
cornerR += 0.5*factorXR
cornerI += 0.5*factorYI

while True in [None in i for i in image]:
    for pi,p in enumerate(cluster):
        if p == None:
            print("process",pi,p)
            ret = False
            for x,xd in enumerate(image):
                for y,yd in enumerate(xd):
                    if yd == None:
                        cluster[pi] = subprocess.Popen([command,str(iteration_max),str(x*factorXR+cornerR),str(y*factorYI+cornerI)])
                        clusterinfo[pi] = [x,y]
                        ret = True
                    if ret:
                        break
                if ret:
                    break
        elif p.poll() != None:
            print("return",p.returncode)
            image[clusterinfo[pi][0]][clusterinfo[pi][1]] = [p.returncode,test(iteration_max,clusterinfo[pi][0]*factorXR+cornerR,clusterinfo[pi][1]*factorYI+cornerI)]
            #image[clusterinfo[pi][0]][clusterinfo[pi][1]] = p.returncode
            cluster[pi] = None
    #print(".")
    #print(" ".join([i.returncode for i in image]))

print("\n".join([str(i) for i in image]))
