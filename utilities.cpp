#include "utilities.h"

double getValueInPeridicity(double value, double L_lower, double L_upper)
{
    double span = L_upper - L_lower; // L_upper always greater than L_lower

    if ( value < L_lower ) {
        double module = L_lower - value;
        value = L_upper - module + (std::floor(module / span) * span);
        return value;
    } else if ( value > L_upper) {
        double module = value - L_upper;
        value = L_lower + module - (std::floor(module / span) * span);
        return value;
    }

    return value;
}

MEC_RealEquation MEC_RealEquation::operator <<(const MEC_RealEquation &t)
{
    MEC_RealEquation r = *this;

    for (unsigned i = 0; i < t.CoefficientsCount; ++i)
        r.addCoefficient(t.coefficientMatrixIndexes[i], t.coefficientMatrixValues[i]);
    r.addConstant(t.ConstantValue);

    return r;
}

MEC_RealEquation MEC_RealEquation::operator *(const double v)
{
    MEC_RealEquation r = *this;

    for(auto &i : r.coefficientMatrixValues)
        i *= v;
    r.ConstantValue *= v;

    return r;
}

void MEC_RealEquation::addCoefficient(unsigned index, double value)
{
    for (unsigned i = 0; i < CoefficientsCount; ++i) {
        if ( index < coefficientMatrixIndexes[i] ) {
            coefficientMatrixIndexes.insert(coefficientMatrixIndexes.begin() + i, index);
            coefficientMatrixValues.insert(coefficientMatrixValues.begin() + i, value);
            ++CoefficientsCount;
            return;
        }
        if (index == coefficientMatrixIndexes[i]) {
            coefficientMatrixValues[i] += value;
            return;
        }
    }
    coefficientMatrixIndexes.push_back(index);
    coefficientMatrixValues.push_back(value);
    ++CoefficientsCount;
}

void MEC_RealEquation::addConstant(double value)
{
    ConstantValue += value;
}

void MEC_RealEquation::clear()
{
    this->coefficientMatrixIndexes.clear();
    this->coefficientMatrixValues.clear();
    this->ConstantValue = 0.0;

    this->CoefficientsCount = 0;
}

void MEC_RealEquation::removeFirst()
{
    this->coefficientMatrixIndexes.erase(this->coefficientMatrixIndexes.begin());
    this->coefficientMatrixValues.erase(this->coefficientMatrixValues.begin());

    this->CoefficientsCount = 0;
}

MEC_ComplexEquation MEC_ComplexEquation::operator <<(const MEC_ComplexEquation &t)
{
    MEC_ComplexEquation r = *this;

    for (unsigned i = 0; i < t.CoefficientsCount; ++i)
        r.addCoefficient(t.coefficientMatrixIndexes[i], t.coefficientMatrixValues[i]);
    r.addConstant(t.ConstantValue);

    return r;
}

MEC_ComplexEquation MEC_ComplexEquation::operator *(const double v)
{
    MEC_ComplexEquation r = *this;

    for(auto &i : r.coefficientMatrixValues)
        i *= v;
    r.ConstantValue *= v;

    return r;
}

MEC_ComplexEquation MEC_ComplexEquation::operator *(const std::complex<double> v)
{
    MEC_ComplexEquation r = *this;

    for(auto &i : r.coefficientMatrixValues)
        i *= v;
    r.ConstantValue *= v;

    return r;
}

void MEC_ComplexEquation::addCoefficient(unsigned index, std::complex<double> value)
{
    for (unsigned i = 0; i < CoefficientsCount; ++i) {
        if ( index < coefficientMatrixIndexes[i] ) {
            coefficientMatrixIndexes.insert(coefficientMatrixIndexes.begin() + i, index);
            coefficientMatrixValues.insert(coefficientMatrixValues.begin() + i, value);
            ++CoefficientsCount;
            return;
        }
        if (index == coefficientMatrixIndexes[i]) {
            coefficientMatrixValues[i] += value;
            return;
        }
    }
    coefficientMatrixIndexes.push_back(index);
    coefficientMatrixValues.push_back(value);
    ++CoefficientsCount;
}

void MEC_ComplexEquation::addConstant(std::complex<double> value)
{
    ConstantValue += value;
}

void MEC_ComplexEquation::clear()
{
    this->coefficientMatrixIndexes.clear();
    this->coefficientMatrixValues.clear();
    this->ConstantValue = 0.0;

    this->CoefficientsCount = 0;
}

ZSuperLU_Data::ZSuperLU_Data()
{
    used = false;
    size = 0;
    equed[0] = ' ';
    perm_r = nullptr;
    perm_c = nullptr;
    etree = nullptr;
    work = nullptr;
    rhsb = nullptr;
    rhsx = nullptr;
    R = nullptr;
    C = nullptr;
    ferr = nullptr;
    berr = nullptr;
}

void ZSuperLU_Data::setSystemOfEquations(std::vector<MEC_ComplexEquation> equations)
{
    size = static_cast<unsigned>(equations.size());
    nner_curr.clear(); nnec_curr.clear(); nnev.clear();
    // Set the vector of constants
    if ( !(rhsb = doublecomplexMalloc(static_cast<int>(size))) ) ABORT("Malloc fails for rhsb[].");
    rhsb[0].r = 0.0; rhsb[0].i = 0.0;
    for (unsigned i = 1; i < size; ++i) {
        rhsb[i].r = -equations[i].ConstantValue.real();
        rhsb[i].i = -equations[i].ConstantValue.imag();
    }
    // Sort the coefficient matrix for the fast Harwell-Boeing format computation
    std::vector<std::vector<std::complex<double>>> sortedMatrixValue(size);
    std::vector<std::vector<unsigned>> sortedMatrixIndex(size);
    unsigned nonnull = 0;
    for (unsigned equation = 0; equation < size; ++equation) {
        for (unsigned coefficient = 0; coefficient < equations[equation].CoefficientsCount; ++coefficient) {
            sortedMatrixIndex[equations[equation].coefficientMatrixIndexes[coefficient]].push_back(equation);
            sortedMatrixValue[equations[equation].coefficientMatrixIndexes[coefficient]].push_back(equations[equation].coefficientMatrixValues[coefficient]);
            nonnull++;
        }
    }
    // Set the coefficient matrix into the Harwell-Boeing format
    nnec_curr.push_back(0); nner_curr.push_back(0); nnev.push_back(1.0); // first row first column are set to zero except the first element
    for (unsigned column = 1; column < sortedMatrixValue.size(); ++column) {
        for (unsigned row = 0; row < sortedMatrixValue[column].size(); ++row) {
            if (std::abs(sortedMatrixValue[column][row]) > SUPERLU_PRECISION) {
                nnec_curr.push_back(column);
                nner_curr.push_back(sortedMatrixIndex[column][row]);
                nnev.push_back(sortedMatrixValue[column][row]);
            }
        }
    }
    for (unsigned r = 1; r < nner_curr.size(); ++r) {
        if (nner_curr[r] == 0) {
            nner_curr.erase(nner_curr.begin() + r);
            nnec_curr.erase(nnec_curr.begin() + r);
            nnev.erase(nnev.begin() + r--);
        }
    }
    // Set the number of non null elements
    nnz = static_cast<int>(nnec_curr.size());
}

int ZSuperLU_Data::initSolver(std::vector<std::complex<double> > &solutions)
{
    SuperMatrix A, B;
    lwork = 0;
    nrhs  = 1;

    set_default_options(&options);

    doublecomplex *a;
    int *asub, *xa;
    if ( !(a = doublecomplexMalloc(nnz)) ) ABORT("Malloc fails for a[].");
    if ( !(asub = intMalloc(nnz)) ) ABORT("Malloc fails for asub[].");
    if ( !(xa = intMalloc(size+1)) ) ABORT("Malloc fails for xa[].");
    int previousColumn = 0;
    xa[0] = 0;
    for (int i = 0; i < nnz; ++i) {
        a[i].r = nnev[i].real(); a[i].i = nnev[i].imag();
        asub[i] = nner_curr[i];
        if(previousColumn != nnec_curr[i])
            xa[++previousColumn] = i;
    }
    xa[size] = nnz; // Harwell-Boeing format

    zCreate_CompCol_Matrix(&A, size, size, nnz, a, asub, xa, SLU_NC, SLU_Z, SLU_GE);

    if ( !(rhsx = doublecomplexMalloc(size * nrhs)) ) ABORT("Malloc fails for rhsx[].");
    zCreate_Dense_Matrix(&B, size, nrhs, rhsb, size, SLU_DN, SLU_Z, SLU_GE);
    zCreate_Dense_Matrix(&X, size, nrhs, rhsx, size, SLU_DN, SLU_Z, SLU_GE);

    if ( !(perm_c = intMalloc(size)) ) ABORT("Malloc fails for perm_c[].");
    if ( !(perm_r = intMalloc(size)) ) ABORT("Malloc fails for perm_r[].");
    if ( !(etree = intMalloc(size)) ) ABORT("Malloc fails for etree[].");
    if ( !(R = (double *) SUPERLU_MALLOC(A.nrow * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for R[].");
    if ( !(C = (double *) SUPERLU_MALLOC(A.ncol * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for C[].");
    if ( !(ferr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for ferr[].");
    if ( !(berr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for berr[].");

    StatInit(&stat);

    zgssvx(&options, &A, perm_c, perm_r, etree, equed, R, C,
           &L, &U, work, lwork, &B, &X, &rpg, &rcond, ferr, berr,
           &Glu, &mem_usage, &stat, &info);
    if ( info == 0 || info == size+1 ) {
        doublecomplex *sol = static_cast<doublecomplex*>((static_cast<DNformat*>( X.Store)->nzval ));
        for (int i = 0; i < X.nrow; ++i) {
            solutions[i].real(sol[i].r); solutions[i].imag(sol[i].i);
        }
    }

    StatFree(&stat);
    Destroy_CompCol_Matrix(&A);
    Destroy_Dense_Matrix(&B);
    used = true;

    nner_prev = nner_curr;
    nnec_prev = nnec_curr;

    return info;
}

int ZSuperLU_Data::repeatSolver(std::vector<std::complex<double> > &solutions)
{
    SuperMatrix A, B;

    options.Fact = SamePattern_SameRowPerm;
    StatInit(&stat);

    doublecomplex *a;
    int *asub, *xa;
    if ( !(a = doublecomplexMalloc(nnz)) ) ABORT("Malloc fails for a[].");
    if ( !(asub = intMalloc(nnz)) ) ABORT("Malloc fails for asub[].");
    if ( !(xa = intMalloc(size+1)) ) ABORT("Malloc fails for xa[].");
    int previousColumn = 0;
    xa[0] = 0;
    for (int i = 0; i < nnz; ++i) {
        a[i].r = nnev[i].real(); a[i].i = nnev[i].imag();
        asub[i] = nner_curr[i];
        if(previousColumn != nnec_curr[i])
            xa[++previousColumn] = i;
    }
    xa[size] = nnz; // Harwell-Boeing format

    zCreate_CompCol_Matrix(&A, size, size, nnz, a, asub, xa, SLU_NC, SLU_Z, SLU_GE);
    zCreate_Dense_Matrix(&B, size, nrhs, rhsb, size, SLU_DN, SLU_Z, SLU_GE);

    zgssvx(&options, &A, perm_c, perm_r, etree, equed, R, C,
           &L, &U, work, lwork, &B, &X, &rpg, &rcond, ferr, berr,
           &Glu, &mem_usage, &stat, &info);

    if ( (info == 0) || (info == size + 1) ) {
        doublecomplex *sol = static_cast<doublecomplex*>((static_cast<DNformat*>( X.Store)->nzval ));
        for (int i = 0; i < X.nrow; ++i) {
            solutions[i].real(sol[i].r); solutions[i].imag(sol[i].i);
        }
    }

    StatFree(&stat);
    Destroy_CompCol_Matrix(&A);
    Destroy_Dense_Matrix(&B);
    used = true;

    return info;
}

void ZSuperLU_Data::close()
{
    if (used) {
        SUPERLU_FREE (etree);
        SUPERLU_FREE (perm_r);
        SUPERLU_FREE (perm_c);
        SUPERLU_FREE (R);
        SUPERLU_FREE (C);
        SUPERLU_FREE (ferr);
        SUPERLU_FREE (berr);
        Destroy_Dense_Matrix(&X);
        if ( lwork == 0 ) { /* Deallocate storage associated with L and U. */
            Destroy_SuperNode_Matrix(&L);
            Destroy_CompCol_Matrix(&U);
        } else if ( lwork > 0 ) {
            SUPERLU_FREE(work);
        }
        nner_prev.clear();
        nnec_prev.clear();
        used = false;
    }
}


DSuperLU_Data::DSuperLU_Data()
{
    used = false;
    size = 0;
    equed[0] = ' ';
    perm_r = nullptr;
    perm_c = nullptr;
    etree = nullptr;
    work = nullptr;
    rhsb = nullptr;
    rhsx = nullptr;
    R = nullptr;
    C = nullptr;
    ferr = nullptr;
    berr = nullptr;
}

void DSuperLU_Data::setSystemOfEquations(std::vector<MEC_RealEquation> equations)
{
    size = static_cast<unsigned>(equations.size());
    nner_curr.clear(); nnec_curr.clear(); nnev.clear();
    // Set the vector of constants
    if ( !(rhsb = doubleMalloc(static_cast<int>(size))) ) ABORT("Malloc fails for rhsb[].");
    rhsb[0] = 0.0;
    for (unsigned i = 1; i < size; ++i) {
        rhsb[i] = -equations[i].ConstantValue;
    }
    // Sort the coefficient matrix for the fast Harwell-Boeing format computation
    std::vector<std::vector<double>> sortedMatrixValue(size);
    std::vector<std::vector<unsigned>> sortedMatrixIndex(size);
    unsigned nonnull = 0;
    for (unsigned equation = 0; equation < size; ++equation) {
        for (unsigned coefficient = 0; coefficient < equations[equation].CoefficientsCount; ++coefficient) {
            sortedMatrixIndex[equations[equation].coefficientMatrixIndexes[coefficient]].push_back(equation);
            sortedMatrixValue[equations[equation].coefficientMatrixIndexes[coefficient]].push_back(equations[equation].coefficientMatrixValues[coefficient]);
            nonnull++;
        }
    }
    // Set the coefficient matrix into the Harwell-Boeing format
    nnec_curr.push_back(0); nner_curr.push_back(0); nnev.push_back(1.0); // first row first column are set to zero except the first element
    for (unsigned column = 1; column < sortedMatrixValue.size(); ++column) {
        for (unsigned row = 0; row < sortedMatrixValue[column].size(); ++row) {
            if (std::abs(sortedMatrixValue[column][row]) > SUPERLU_PRECISION) {
                nnec_curr.push_back(column);
                nner_curr.push_back(sortedMatrixIndex[column][row]);
                nnev.push_back(sortedMatrixValue[column][row]);
            }
        }
    }
    for (unsigned r = 1; r < nner_curr.size(); ++r) {
        if (nner_curr[r] == 0) {
            nner_curr.erase(nner_curr.begin() + r);
            nnec_curr.erase(nnec_curr.begin() + r);
            nnev.erase(nnev.begin() + r--);
        }
    }
    // Set the number of non null elements
    nnz = static_cast<int>(nnec_curr.size());
}

int DSuperLU_Data::initSolver(std::vector<double> &solutions)
{
    SuperMatrix A, B;
    lwork = 0;
    nrhs  = 1;

    set_default_options(&options);
    options.SymmetricMode = YES;

    double *a;
    int *asub, *xa;
    if ( !(a = doubleMalloc(nnz)) ) ABORT("Malloc fails for a1[].");
    if ( !(asub = intMalloc(nnz)) ) ABORT("Malloc fails for asub1[].");
    if ( !(xa = intMalloc(size+1)) ) ABORT("Malloc fails for xa1[].");
    int previousColumn = 0;
    xa[0] = 0;
    for (int i = 0; i < nnz; ++i) {
        a[i] = nnev[i];
        asub[i] = nner_curr[i];
        if(previousColumn != nnec_curr[i])
            xa[++previousColumn] = i;
    }
    xa[size] = nnz; // Harwell-Boeing format

    dCreate_CompCol_Matrix(&A, size, size, nnz, a, asub, xa, SLU_NC, SLU_D, SLU_GE);

    if ( !(rhsx = doubleMalloc(static_cast<int>(size))) ) ABORT("Malloc fails for rhsx[].");
    dCreate_Dense_Matrix(&B, size, nrhs, rhsb, size, SLU_DN, SLU_D, SLU_GE);
    dCreate_Dense_Matrix(&X, size, nrhs, rhsx, size, SLU_DN, SLU_D, SLU_GE);

    if ( !(perm_c = intMalloc(size)) ) ABORT("Malloc fails for perm_c[].");
    if ( !(perm_r = intMalloc(size)) ) ABORT("Malloc fails for perm_r[].");
    if ( !(etree = intMalloc(size)) ) ABORT("Malloc fails for etree[].");
    if ( !(R = (double *) SUPERLU_MALLOC(A.nrow * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for R[].");
    if ( !(C = (double *) SUPERLU_MALLOC(A.ncol * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for C[].");
    if ( !(ferr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for ferr[].");
    if ( !(berr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for berr[].");

    StatInit(&stat);

    dgssvx(&options, &A, perm_c, perm_r, etree, equed, R, C,
           &L, &U, work, lwork, &B, &X, &rpg, &rcond, ferr, berr,
           &Glu, &mem_usage, &stat, &info);
    if ( info == 0 || info == size+1 ) {
        double *sol = static_cast<double*>((static_cast<DNformat*>(X.Store))->nzval);
        for (int i = 0; i < X.nrow; ++i)
            solutions[i] = sol[i];
    }

    StatFree(&stat);
    Destroy_CompCol_Matrix(&A);
    Destroy_Dense_Matrix(&B);
    used = true;

    nner_prev = nner_curr;
    nnec_prev = nnec_curr;

    return info;
}

int DSuperLU_Data::repeatSolver(std::vector<double> &solutions)
{
    SuperMatrix A, B;
    options.Fact = SamePattern_SameRowPerm;

    StatInit(&stat);

    double *a;
    int *asub, *xa;
    if ( !(a = doubleMalloc(nnz)) ) ABORT("Malloc fails for a1[].");
    if ( !(asub = intMalloc(nnz)) ) ABORT("Malloc fails for asub1[].");
    if ( !(xa = intMalloc(size+1)) ) ABORT("Malloc fails for xa1[].");
    int previousColumn = 0;
    xa[0] = 0;
    for (int i = 0; i < nnz; ++i) {
        a[i] = nnev[i];
        asub[i] = nner_curr[i];
        if(previousColumn != nnec_curr[i])
            xa[++previousColumn] = i;
    }
    xa[size] = nnz; // Harwell-Boeing format

    dCreate_CompCol_Matrix(&A, size, size, nnz, a, asub, xa, SLU_NC, SLU_D, SLU_GE);
    dCreate_Dense_Matrix(&B, size, nrhs, rhsb, size, SLU_DN, SLU_D, SLU_GE);

    dgssvx(&options, &A, perm_c, perm_r, etree, equed, R, C,
           &L, &U, work, lwork, &B, &X, &rpg, &rcond, ferr, berr,
           &Glu, &mem_usage, &stat, &info);

    if ( (info == 0) || (info == size + 1) ) {
        double *sol = static_cast<double*>((static_cast<DNformat*>(X.Store))->nzval);
        for (int i = 0; i < X.nrow; ++i)
            solutions[i] = sol[i];
    }

    StatFree(&stat);
    Destroy_CompCol_Matrix(&A);
    Destroy_Dense_Matrix(&B);
    used = true;

    return info;
}

void DSuperLU_Data::close()
{
    if (used) {
        SUPERLU_FREE (etree);
        SUPERLU_FREE (perm_r);
        SUPERLU_FREE (perm_c);
        SUPERLU_FREE (R);
        SUPERLU_FREE (C);
        SUPERLU_FREE (ferr);
        SUPERLU_FREE (berr);
        Destroy_Dense_Matrix(&X);
        if ( lwork == 0 ) { /* Deallocate storage associated with L and U. */
            Destroy_SuperNode_Matrix(&L);
            Destroy_CompCol_Matrix(&U);
        } else if ( lwork > 0 ) {
            SUPERLU_FREE(work);
        }
        nner_prev.clear();
        nnec_prev.clear();
        used = false;
    }
}
