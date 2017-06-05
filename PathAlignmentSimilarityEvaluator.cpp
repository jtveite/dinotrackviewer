#include "PathAlignmentSimilarityEvaluator.h"

double PathAlignmentSimilarityEvaluator::evaluateSimilarity(VRPoint& a, VRPoint& b){
  return pow((a.positions[0].x - b.positions[0].x), 2);

}
