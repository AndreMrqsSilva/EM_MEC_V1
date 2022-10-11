/*
 * ================================================================================================
 *  Copyright (c) 2021, the author Andre Marques Silva
 *
 *  All rights reserved.
 *
 *  The source code is distributed under BSD license, see the file License.txt
 *  at the top-level directory.
 * ================================================================================================
 *
 * Authors:
 *
 * Andre Marques Silva.
 *
 * Date:
 *
 * June 1, 2021.  Version 1.0.
 *
 * Acknowledgements:
 *
 * This work is result of a Ph.D. program under the surpervision of Professors:
 *  Fernando J.T.E. Ferreira
 *  Carlos Henggeler Antunes
 * at the Univerity of Coimbra
 *
 * This work was supported by the Portuguese Science and Technology
 * Foundation (FCT) under the scholarship SFRH/BD/137880/2018.
 *
 * Professor Andre M. S. Mendes provided the validation of MEC method through FEA.
 *
 * Notice:
 *
 * Copyright (c) 2021 by Andre Marques Silva.
 * All Rights Reserved.
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY
 * EXPRESSED OR IMPLIED. ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use, copy, modify, and/or distribute
 * this program, provided that the Copyright, this License, and the
 * availability of the original version is retained on all copies and made
 * accessible to the end-user of any code or package.
 *
 * Availability:
 *
 * This library is available at https://GitHub
 * ================================================================================================
*/

#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H

#include <cmath>
#include <complex>
#include <vector>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QString>

#define M_2PI 6.28318530718
#define U_AIR 1.25663706212E-6
#define RHO_COPPER20C 1.68E-8
#define RHO_ALUMINIUM20C 2.65E-8
#define RHO_COPPER100C 2.22E-8
#define RHO_ALUMINIUM100C 3.48E-8
#define DENSITY_COPPER 8.96E3
#define DENSITY_ALUMINIUM 2.7E3

enum ReluctanceElementType { RET_notDefined, RET_air, RET_coreLinear, RET_coreNonLinear, RET_coil, RET_magnet };

enum ReluctanceElementGeometry { REG_notDefined, REG_rectangle, REG_trapezoid, REG_sector };

enum MEC_MMFSourceOrientation { MMFSO_notDefined, MMFSO_tangential, MMFSO_radial };

enum NonlinearMaterialType { SILICONSTEEL_M250_50A };

double round_SuperLUPrecision(double v);

void linsysToCsv(std::vector<std::vector<double> > A, std::vector<double> B);
void DataToCsv(std::vector<std::vector<double> > data);

double calculator_resistance(double length, double resistivity, double area);

double calculator_inductance(double length, double height, double width, double permeability);

struct MEC_ElementGeometry
{
    MEC_ElementGeometry();

    void setGeometryRectangle(double h, double w, double l);
    void setGeometryTrapezoid(double h, double bW, double tW, double l);
    void setGeometrySector(double rT, double rB, double aT, double aB, double l);
    void setPosition(double tX, double rY);
    void setTangentialPosition(double theta);
    void setTangentialPositionWithOffset(double offset);

    ReluctanceElementGeometry shape;
    double position_tX; // cylindrical: tangential distance (radians) or cartesian: abscissa (meters)
    double position_rY; // cylindrical: radial distance (meters) or cartesian: ordinate (meters)
    double height; // element height
    double width; // element average width
    double widthBottom; // element bottom width
    double widthTop; // element top width
    double radiusBottom; // element bottom radius
    double radiusTop; // element top radius
    double arcBottom; // element bottom arc
    double arcTop; // element top arc
    double length; // element length (z-axis wise)
    double crossSectionalArea; // element z-axis wise cross sectional area
    double crossSectionalAreaTangential; // element tangential or x-axis wise cross sectional area
    double crossSectionalAreaRadial; // element radial or y-axis wise cross sectional area
    double volume; // element volume
};

struct ReportDataSolver
{
    ReportDataSolver() { iterations = 0; elapsedTime = 0.0; variance = 0.0; solverError = false; currentTime = 0.0; currentStep = 0; }
    unsigned iterations;
    double elapsedTime;
    double variance;
    bool solverError;
    std::vector<double> data;

    double currentTime;
    unsigned currentStep;
};

class NonlinearMaterial
{
public:
    NonlinearMaterial() { setMaterial(SILICONSTEEL_M250_50A); }
    NonlinearMaterial(NonlinearMaterialType t) { setMaterial(t); }
    void setMaterial(NonlinearMaterialType t);
    double getLinearReluctivity() { return linearReluctivity; }
    double getSaturatedReluctivity() { return saturatedReluctivity; }
    double getAirReluctivity() { return 795775.0; }
    double getReluctivity(double B);
    double getReluctivityDerivative(double B);
    double getIronLosses(double B, double f, double volume);
    double getIronLosses(double B_prev, double B_curr, double timeStep, double volume);
    double getDensity() { return density; }
private:
    NonlinearMaterialType type;
    double a, b, c, d; // v-B fiting function coefficients
    double k_h, k_c, k_e; // Bertotti Iron losses coefficients: (h) - hysteresis, (c) - eddy current losses, and (e) - excess losses
    double linearReluctivity;
    double saturatedReluctivity;
    double density;
};

#endif // MISCELLANEOUS_H
