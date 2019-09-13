"""
Learner
==========================================

"""

__version__ = "0.1"

__all__ = ["Interpolant", "ANOVAInterpolant"]

__author__ = "Fabian Franzelin, fabian.franzelin@ipvs.uni-stuttgart.de"

from pysgpp.extensions.datadriven.uq.learner.builder.CGSolverDescriptor import CGSolverDescriptor
from pysgpp.extensions.datadriven.uq.learner.builder.GridDescriptor import GridDescriptor

from pysgpp.extensions.datadriven.uq.learner.Interpolant import Interpolant
from pysgpp.extensions.datadriven.uq.learner.Learner import Learner, LearnerEvents
