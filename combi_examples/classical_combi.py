#!/usr/bin/python
#/*
 #* clasical_combi.cpp
 #*
 #*  Created on: Aug 20, 2010
 #*      Author: Aliz Nagy
 #*
 #*      A program presenting some examples with the basic functions introduced with the combination technique and their usage
 #*      The concepts exemplified in this piece of code include the construction of a sparse grid(with the create***Grid method of the grid class),
 #*      generation of a regular sparse grid, assignment of function values to the gridpoints, the construction of a set of fullgrids coreesponding to
 #*      the sparse grid, the decomposition of the sparse grid into the fullgrids(assignment of the right function values to the fullgrid points),
 #*      the evaluation of the function on the fullgrids, the result interpolated using the combined values on the grids, the recomposition of the fullgrids
 #*      into a sparse grid(also with the combinational formula), and comparing the result obtained on full grids with the result obtained on a sparse grid
 #*      and the actual function value
 #*/
# import modules
import sys
import re
# append trunk/bin to search path for modules
sys.path.append('../lib/pysgpp')
from pysgpp import DataVector, Grid, FullGrid, FullGridSet, createOperationHierarchisation
from ctypes import *





# Create a new Grid(LinearGrid, LinearBoundary,or LinearTrapezoidBoundary)
dim = 3
grid = Grid.createSquareRootGrid(dim)
gridStorage = grid.getStorage()
print "dimensionality:         %d" % (gridStorage.dim())

# Generate a new regular grid structure corresponding to the type of the chosen grid
level = 4
gridGen = grid.createGridGenerator()
gridGen.regular(level)
print "number of grid points:  %d" % (gridStorage.size())




# Create coefficient vector of type DataVector
alpha = DataVector(gridStorage.size())
print "length of alpha-vector: %d" % (len(alpha))

# Set the function values for every member of the DataVector
f = lambda x0,x1,x2: 1.0+(0.25*(x0-0.7)*(x0-0.7)+2.0)+(0.25*(x1-0.7)*(x1-0.7)+2.0)+(0.25*(x2-0.7)*(x2-0.7)+2.0)
for i in xrange(gridStorage.size()):
    gp = gridStorage.get(i)
    alpha[i] = f(gp.abs(0),gp.abs(1),gp.abs(2))
# Create the set of fullgrids into which the grid decomposes, the gridType can be "linear", "linearBoundary" or "linearTrapezoidBoundary
fgs = FullGridSet(dim,level,grid.getType())
print "Number of grids",fgs.getSize()
print "The grids:"
#This prints the levels of all fullgrids
fgs.printGridSet()

#Assign the function values to every gridpoint of the fullgrids, using the values from the sparse grid
# This step can be replaced by the manual assignment of function values to every gridpoint in the fullgrids
fgs.deCompose(gridStorage,alpha)
#Create a new datavector which contains the coordinates of the point we want to interpolate
p=DataVector(dim);
p[0] = 0.91;
p[1] = 0.23;
p[2] = 0.76;
#We now verify if the decomposition of the sparse grid was correct,i.e. see if every gridpoint of the fullgrids contains the correct function value,
#and evaluate the function
for i in xrange(fgs.getSize()):
    fg=fgs.at(i)  
    m=fg.getSize()    
    for j in xrange(m):
        d=f(fg.getCoord(0,j),fg.getCoord(1,j),fg.getCoord(2,j))        
        #If the function value in the fullgrid point is different from the value already assigned we will signal an error(if this happens we did something wrong)
        if (d!=fg.get(j)): print "Error:",d,"!=",fg.get(j)
        #Evaluates the fullgrid in an arbitrary point, and assigns the resulting value to the field variable value of the fullgrid,
        #The same value is returned by the function and can be accesed later through the function call fullgrid.val()
        fg.eval(p)

       
 #Combines the interpolated results on the fullgrids into one value, which in case of function interpolation equals the value on the sparse grid itself
 #We print the interpolation value as Uc(p)
 #This and the evals on fullgrids can be replaced by the eval() function of the fullgriids
print "Uc(p)=",fgs.combinedResult()
#We will now verify if the same value is obtained on them sparse grid
beta = DataVector(gridStorage.size())        
#We will recompose the values of the gridpoints into the sparse grid structure using the same coefficients as with the combinedResult function
#This step can also be replaced by the direct assignment of function values to the sparse grid points as we showed it before
fgs.reCompose(gridStorage,beta)
createOperationHierarchisation(grid).doHierarchisation(beta)    
# evaluate
opEval = createOperationEval(grid)
#We print the value interpolated on the sparse grid and also the real value of the function in the given point
print "Usg(p) =", opEval.eval(beta, p)
print "f(p)=",f(p[0],p[1],p[2])
