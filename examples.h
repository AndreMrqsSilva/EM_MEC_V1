#ifndef EXAMPLES_H
#define EXAMPLES_H

#include "magnetodynamics_tool.h"
#include "miscellaneous.h"

void test_MD_CoreMagnet();
void test_MD_CoreCoil();
void test_MD_SinglePhaseTransformer();
void test_MD_ThreePhaseTransformer();
void test_MD_SinglePhaseTransformerCurrentSoruce();
void test_MD_ThreePhaseTransformerCurrentSoruce();
MD_Project test_MD_SynIPM_CoggingTorque();
MD_Project test_MD_SynIPM_BackEMF();
MD_Project test_MD_SynIPM_ConstantSpeed();
MD_Project test_MD_SynRM_ConstantSpeed();

#endif // EXAMPLES_H
