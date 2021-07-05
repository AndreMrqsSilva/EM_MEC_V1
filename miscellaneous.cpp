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

#include "MEC_IM_V2021/miscellaneous.h"

double calculator_resistance(double length, double resistivity, double area)
{
    return ((resistivity * length) / area);
}

double calculator_inductance(double length, double height, double width, double permeability)
{
    return (1.5 * permeability * height * length / width);
}

void NonlinearMaterial::setMaterial(NonlinearMaterialType t)
{
    type = t;
    if ( type == SILICONSTEEL_M250_50A) {
        a = 0.1791;
        b = 5.723;
        c = 1.678E8;
        d = 3903.0;
        k_h = 74.62;
        k_c = 1.046;
        k_e = 4.974;
        linearReluctivity = 50.0;
        saturatedReluctivity = 1.703E4;
    }
}

double NonlinearMaterial::getReluctivity(double B)
{
    double v = a*std::exp(b*B) + std::log(c*B+d); // MATLAB: a*exp(b*x)+log(c*x+d)
    if (v > 795775.0)
        v = 40000.0;
    return v;
}

double NonlinearMaterial::getIronLosses(double B, double f, double volume)
{
    double losses_h, losses_c, losses_e;
    losses_h = k_h * f * B;
    losses_c = k_c * (f*f) * (B*B);
    losses_e = k_e * std::pow(f*B, 1.5);
    return volume * (losses_h + losses_c + losses_e);
}
