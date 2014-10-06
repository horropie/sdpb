#ifndef SDP_BOOTSTRAP_SDP_H_
#define SDP_BOOTSTRAP_SDP_H_

#include <vector>
#include <iostream>
#include <ostream>
#include "types.h"
#include "util.h"
#include "Vector.h"
#include "Matrix.h"
#include "Polynomial.h"

using std::vector;
using std::ostream;

class IndexTuple {
public:
  int p;
  int r;
  int s;
  int k;
  IndexTuple(int p, int r, int s, int k): p(p), r(r), s(s), k(k) {}
  IndexTuple() {}
};

class SDP {
public:
  vector<Matrix> bilinearBases;
  Matrix FreeVarMatrix;
  Vector primalObjective;
  Vector dualObjective;
  Real objectiveConst;
  vector<int> dimensions;
  vector<int> degrees;
  vector<vector<int> > blocks;
  vector<vector<IndexTuple> > constraintIndices;

  vector<int> psdMatrixBlockDims() const {
    vector<int> dims;
    for (unsigned int j = 0; j < dimensions.size(); j++)
      for (vector<int>::const_iterator b = blocks[j].begin(); b != blocks[j].end(); b++)
        dims.push_back(bilinearBases[*b].rows * dimensions[j]);
    return dims;
  }

  vector<int> bilinearPairingBlockDims() const {
    vector<int> dims;
    for (unsigned int j = 0; j < dimensions.size(); j++)
      for (vector<int>::const_iterator b = blocks[j].begin(); b != blocks[j].end(); b++)
        dims.push_back(bilinearBases[*b].cols * dimensions[j]);
    return dims;
  }

  vector<int> schurBlockDims() const {
    vector<int> dims;
    for (unsigned int j = 0; j < dimensions.size(); j++)
      dims.push_back(constraintIndices[j].size());
    return dims;
  }

  void initializeConstraintIndices() {
    int p = 0;
    for (unsigned int j = 0; j < dimensions.size(); j++) {
      constraintIndices.push_back(vector<IndexTuple>(0));

      for (int s = 0; s < dimensions[j]; s++) {
        for (int r = 0; r <= s; r++) {
          for (int k = 0; k <= degrees[j]; k++) {
            constraintIndices[j].push_back(IndexTuple(p, r, s, k));
            p++;
          }
        }
      }
    }
    assert(p == (int)primalObjective.size());
  }

  friend ostream& operator<<(ostream& os, const SDP& sdp) {
    os << "SDP(bilinearBases = " << sdp.bilinearBases
       << ", FreeVarMatrix = " << sdp.FreeVarMatrix
       << ", primalObjective = " << sdp.primalObjective
       << ", dualObjective = " << sdp.dualObjective
       << ", dimensions = " << sdp.dimensions
       << ", degrees = " << sdp.degrees
       << ", blocks = " << sdp.blocks
       << ")";
    
    return os;
  }
};

class SampledMatrixPolynomial {
public:
  int dim;
  int degree;
  Matrix constraintMatrix;
  Vector constraintConstants;
  vector<Matrix> bilinearBases;
};

SampledMatrixPolynomial samplePolynomialVectorMatrix(const PolynomialVectorMatrix &m,
                                                     const vector<Polynomial> &bilinearBasis,
                                                     const vector<Real> &samplePoints,
                                                     const vector<Real> &rescalings);

SDP bootstrapSDP(const Vector &objective,
                 const Real &objectiveConst,
                 const vector<SampledMatrixPolynomial> &sampledMatrixPols);

SDP bootstrapPolynomialSDP(const Vector &affineObjective,
                           const vector<PolynomialVectorMatrix> &polVectorMatrices,
                           const vector<Polynomial> &bilinearBasis,
                           const vector<Real> &samplePoints,
                           const vector<Real> &sampleScalings);

#endif  // SDP_BOOTSTRAP_SDP_H_