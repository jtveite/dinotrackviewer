#include "SimilarityEvaluator.h"

class PathAlignmentSimilarityEvaluator : public SimilarityEvaluator{
public:
  virtual double evaluateSimilarity(VRPoint& a, VRPoint& b);

};
