// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/globaldef.hpp>

#include <sgpp/optimization/tools/Printer.hpp>
#include <sgpp/optimization/optimizer/unconstrained/Rprop.hpp>

namespace SGPP {
  namespace optimization {
    namespace optimizer {

      Rprop::Rprop(
        ScalarFunction& f,
        ScalarFunctionGradient& fGradient,
        size_t maxItCount,
        float_t tolerance,
        float_t initialStepSize,
        float_t stepSizeIncreaseFactor,
        float_t stepSizeDecreaseFactor) :
        UnconstrainedOptimizer(f, maxItCount),
        fGradient(fGradient),
        theta(tolerance),
        initialAlpha(initialStepSize),
        rhoAlphaPlus(stepSizeIncreaseFactor),
        rhoAlphaMinus(stepSizeDecreaseFactor) {
      }

      void Rprop::optimize() {
        printer.printStatusBegin("Optimizing (Rprop)...");

        const size_t d = f.getDimension();

        xOpt.resize(0);
        fOpt = NAN;
        xHist.resize(0, d);
        fHist.resize(0);

        base::DataVector x(x0);
        float_t fx;
        base::DataVector gradFx(d);

        base::DataVector xOld(d);
        base::DataVector gradFxOld(d, 0.0);

        size_t k;
        base::DataVector alpha(d, initialAlpha);

        size_t breakIterationCounter = 0;
        const size_t BREAK_ITERATION_COUNTER_MAX = 10;

        for (k = 0; k < N; k++) {
          // calculate gradient and norm
          fx = fGradient.eval(x, gradFx);
          xOld = x;

          if (k == 0) {
            xHist.appendRow(x);
            fHist.append(fx);
          }

          for (size_t t = 0; t < d; t++) {
            const float_t dir = gradFx[t];
            const float_t dirProduct = dir * gradFxOld[t];

            float_t dirSign;

            if (dir > 0.0) {
              dirSign = 1.0;
            } else if (dir < 0.0) {
              dirSign = -1.0;
            } else {
              dirSign = 0.0;
            }

            if (dirProduct > 0) {
              alpha[t] *= rhoAlphaPlus;
              x[t] -= alpha[t] * dirSign;
              gradFxOld[t] = dir;
            } else if (dirProduct < 0) {
              alpha[t] *= rhoAlphaMinus;
              gradFxOld[t] = 0.0;
            } else {
              gradFxOld[t] = dir;
            }

            const float_t newXt = x[t] - alpha[t] * dirSign;

            if (newXt < 0.0) {
              alpha[t] = x[t];
              x[t] = 0.0;
            } else if (newXt > 1.0) {
              alpha[t] = 1.0 - x[t];
              x[t] = 1.0;
            } else {
              x[t] = newXt;
            }
          }

          // status printing
          printer.printStatusUpdate(
            std::to_string(k) + " evaluations, x = " + x.toString() +
            ", f(x) = " + std::to_string(fx));

          xHist.appendRow(x);
          fHist.append(fx);

          // take difference between old and new x
          xOld.sub(x);

          // stopping criterion:
          // stop if x difference is smaller than tolerance theta
          // in BREAK_ITERATION_COUNTER_MAX consecutive iterations
          if (xOld.l2Norm() < theta) {
            breakIterationCounter++;

            if (breakIterationCounter >= BREAK_ITERATION_COUNTER_MAX) {
              break;
            }
          } else {
            breakIterationCounter = 0;
          }
        }

        xOpt.resize(d);
        xOpt = x;
        fOpt = f.eval(x);
        printer.printStatusEnd();
      }

      ScalarFunctionGradient& Rprop::getObjectiveGradient() const {
        return fGradient;
      }

      float_t Rprop::getTolerance() const {
        return theta;
      }

      void Rprop::setTolerance(float_t tolerance) {
        theta = tolerance;
      }

      float_t Rprop::getInitialStepSize() const {
        return initialAlpha;
      }

      void Rprop::setInitialStepSize(float_t initialStepSize) {
        initialAlpha = initialStepSize;
      }

      float_t Rprop::getStepSizeIncreaseFactor() const {
        return rhoAlphaPlus;
      }

      void Rprop::setStepSizeIncreaseFactor(
        float_t stepSizeIncreaseFactor) {
        rhoAlphaPlus = stepSizeIncreaseFactor;
      }

      float_t Rprop::getStepSizeDecreaseFactor() const {
        return rhoAlphaMinus;
      }

      void Rprop::setStepSizeDecreaseFactor(
        float_t stepSizeDecreaseFactor) {
        rhoAlphaMinus = stepSizeDecreaseFactor;
      }

    }
  }
}
