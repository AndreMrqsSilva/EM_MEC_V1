#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>
#include <complex>
#include <cmath>
#include <QDebug>
#include <QElapsedTimer>
#include "superLU/slu_ddefs.h"
#include "superLU/slu_zdefs.h"

#define SUPERLU_PRECISION 1.0E-64

double getValueInPeridicity(double value, double L_lower, double L_upper);

struct MEC_RealEquation
{
    MEC_RealEquation() { CoefficientsCount = 0; ConstantValue = 0.0; }
    MEC_RealEquation operator <<(const MEC_RealEquation& t);
    MEC_RealEquation operator *(const double v);
    void addCoefficient(unsigned index, double value);
    void addConstant(double value);
    void clear();
    void removeFirst();
    unsigned CoefficientsCount;
    std::vector<unsigned> coefficientMatrixIndexes;
    std::vector<double> coefficientMatrixValues;
    double ConstantValue;
};

struct MEC_ComplexEquation
{
    MEC_ComplexEquation() { CoefficientsCount = 0; ConstantValue = 0.0; }
    MEC_ComplexEquation operator <<(const MEC_ComplexEquation &t);
    MEC_ComplexEquation operator *(const double v);
    MEC_ComplexEquation operator *(const std::complex<double> v);
    void addCoefficient(unsigned index, std::complex<double> value);
    void addConstant(std::complex<double> value);
    void clear();
    unsigned CoefficientsCount;
    std::vector<unsigned> coefficientMatrixIndexes;
    std::vector<std::complex<double>> coefficientMatrixValues;
    std::complex<double> ConstantValue;
};

class ZSuperLU_Data
{
public:
    ZSuperLU_Data();
    ~ZSuperLU_Data() { close(); }

    void setSystemOfEquations(std::vector<MEC_ComplexEquation> equations);
    int initSolver(std::vector<std::complex<double>> &solutions);
    int repeatSolver(std::vector<std::complex<double>> &solutions);
    void close();
    bool samePattern() { return ((nner_curr == nner_prev) && (nnec_curr == nnec_prev)); }

private:
    bool used;
    unsigned size;
    std::vector<unsigned> nner_curr, nnec_curr, nner_prev, nnec_prev;
    std::vector<std::complex<double>> nnev;
    char           equed[1];
    SuperMatrix    X, L, U;
    GlobalLU_t 	   Glu;
    int            *perm_r;
    int            *perm_c;
    int            *etree;
    void           *work;
    int            info, lwork, nrhs, ldx;
    int            nnz;
    doublecomplex  *rhsb, *rhsx;
    double         *R, *C;
    double         *ferr, *berr;
    double         u, rpg, rcond;
    mem_usage_t    mem_usage;
    superlu_options_t options;
    SuperLUStat_t stat;
};

class DSuperLU_Data
{
public:
    DSuperLU_Data();
    ~DSuperLU_Data() { close(); }

    void setSystemOfEquations(std::vector<MEC_RealEquation> equations);
    int initSolver(std::vector<double> &solutions);
    int repeatSolver(std::vector<double> &solutions);
    void close();
    bool samePattern() { return ((nner_curr == nner_prev) && (nnec_curr == nnec_prev)); }

private:
    bool used;
    unsigned size;
    std::vector<unsigned> nner_curr, nnec_curr, nner_prev, nnec_prev;
    std::vector<double> nnev;
    char           equed[1];
    SuperMatrix    X, L, U;
    GlobalLU_t 	   Glu;
    int            *perm_r;
    int            *perm_c;
    int            *etree;
    void           *work;
    int            info, lwork, nrhs, ldx;
    int            nnz;
    double         *rhsb, *rhsx;
    double         *R, *C;
    double         *ferr, *berr;
    double         u, rpg, rcond;
    mem_usage_t    mem_usage;
    superlu_options_t options;
    SuperLUStat_t stat;
};

#endif // UTILITIES_H
