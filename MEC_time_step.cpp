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

#include "MEC_time_step.h"


MD_ReluctanceElement::MD_ReluctanceElement()
{
    initialize();
}

MD_ReluctanceElement::MD_ReluctanceElement(ReluctanceElementType t, MEC_ElementGeometry g, double v, std::string text)
{
    initialize();

    type = t;
    label = text;

    setGeometry(g);
    setReluctivity(v);
}

MD_ReluctanceElement::MD_ReluctanceElement(ReluctanceElementType t, MEC_ElementGeometry g, double Br, MEC_MMFSourceOrientation o, int polarity, double v, std::string text)
{
    initialize();

    type = t;
    label = text;

    setGeometry(g);
    setReluctivity(v);
    setRemanence(Br, o, polarity);
}

void MD_ReluctanceElement::setGeometry(MEC_ElementGeometry g)
{
    geometry = g;

    if (geometry.shape == REG_rectangle) {
        constantReluctanceRadial = geometry.height / (geometry.width * geometry.length);
        constantReluctanceTangential = geometry.width / (geometry.height * geometry.length);
    }
    if (geometry.shape == REG_trapezoid) {
        constantReluctanceRadial = geometry.height * std::log(geometry.widthBottom / geometry.widthTop) / ((geometry.widthBottom - geometry.widthTop) * geometry.length);
        constantReluctanceTangential = (geometry.widthBottom - geometry.widthTop) / (geometry.height * std::log(geometry.widthBottom / geometry.widthTop) * geometry.length);
    }
    if (geometry.shape == REG_sector) {
        double b = geometry.arcBottom - (geometry.radiusBottom * (geometry.arcTop - geometry.arcBottom) / (geometry.radiusTop - geometry.radiusBottom));
        double ln = std::log((geometry.radiusTop * geometry.arcBottom) / (geometry.radiusBottom * geometry.arcTop));
        constantReluctanceRadial = ln / (b * geometry.length);
        constantReluctanceTangential = b / (geometry.length * ln);
    }
}

void MD_ReluctanceElement::setReluctivity(double v)
{
    reluctivity = v;
    computeReluctances();
}

void MD_ReluctanceElement::setRemanence(double Br, MEC_MMFSourceOrientation o, int polarity)
{
    RemanentMagnetization = Br;
    MagnetOrientation = o;
    MagnetPolarity = polarity;
    if (MagnetOrientation == MMFSO_tangential)
        MagnetMMFSource = RemanentMagnetization * reluctanceTangential * geometry.crossSectionalAreaTangential;
    if (MagnetOrientation == MMFSO_radial)
        MagnetMMFSource = RemanentMagnetization * reluctanceRadial * geometry.crossSectionalAreaRadial;
}

void MD_ReluctanceElement::addAirgapPermeanceDown(MD_ReluctanceElement *element, double permenace)
{
    airgapElementsDown.push_back(element);
    airgapReluctanceDown.push_back(2.0 / permenace);
}

void MD_ReluctanceElement::addAirgapPermeanceUp(MD_ReluctanceElement *element, double permenace)
{
    airgapElementsUp.push_back(element);
    airgapReluctanceUp.push_back(2.0 / permenace);
}

void MD_ReluctanceElement::clearAirgapPermeances()
{
    airgapElementsDown.clear();
    airgapElementsUp.clear();
    airgapReluctanceDown.clear();
    airgapReluctanceUp.clear();
    airgapPermeanceDown.clear();
    airgapPermeanceUp.clear();
}

void MD_ReluctanceElement::updateReluctivity()
{
    reluctivity = nonlinearMaterial.getReluctivity(magneticFluxDensity);
    computeReluctances();
}

void MD_ReluctanceElement::addMMFSource(MD_MMFSource *source)
{
    MMFSources.push_back(source);
}

void MD_ReluctanceElement::connect(MD_ReluctanceElement *left, MD_ReluctanceElement *up, MD_ReluctanceElement *right, MD_ReluctanceElement *down)
{
    if (type == RET_magnet) {
        if (MagnetOrientation == MMFSO_radial) {
            elementUp = up;
            elementDown = down;
        }
        if (MagnetOrientation == MMFSO_tangential) {
            elementRight = right;
            elementLeft = left;
        }
    } else {
        elementLeft = left;
        if (elementLeft != nullptr) {
            if ((elementLeft->type == RET_magnet) && (elementLeft->MagnetOrientation == MMFSO_radial))
                elementLeft = nullptr;
        }
        elementRight = right;
        if (elementRight != nullptr) {
            if ((elementRight->type == RET_magnet) && (elementRight->MagnetOrientation == MMFSO_radial))
                elementRight = nullptr;
        }
        elementUp = up;
        if (elementUp != nullptr) {
            if ((elementUp->type == RET_magnet) && (elementUp->MagnetOrientation == MMFSO_tangential))
                elementUp = nullptr;
        }
        elementDown = down;
        if (elementDown != nullptr) {
            if ((elementDown->type == RET_magnet) && (elementDown->MagnetOrientation == MMFSO_tangential))
                elementDown = nullptr;
        }
    }
}

void MD_ReluctanceElement::computePermeance()
{
    if (elementLeft != nullptr)
        permeanceLeft = 2.0 / (reluctanceTangential + elementLeft->reluctanceTangential);
    else
        permeanceLeft = 2.0 / reluctanceTangential;
    if (elementRight != nullptr)
        permeanceRight = 2.0 / (reluctanceTangential + elementRight->reluctanceTangential);
    else
        permeanceRight = 2.0 / reluctanceTangential;
    if (elementUp != nullptr)
        permeanceUp = 2.0 / (reluctanceRadial + elementUp->reluctanceRadial);
    else
        permeanceUp = 2.0 / reluctanceRadial;
    if (elementDown != nullptr)
        permeanceDown = 2.0 / (reluctanceRadial + elementDown->reluctanceRadial);
    else
        permeanceDown = 2.0 / reluctanceRadial;
    airgapPermeanceDown.resize(airgapReluctanceDown.size());
    for (unsigned i = 0; i < airgapElementsDown.size(); ++i) {
        airgapPermeanceDown[i] = 2.0 / (reluctanceRadial + airgapElementsDown[i]->reluctanceRadial + airgapReluctanceDown[i]);
    }
    airgapPermeanceUp.resize(airgapReluctanceUp.size());
    for (unsigned i = 0; i < airgapElementsUp.size(); ++i) {
        airgapPermeanceUp[i] = 2.0 / (reluctanceRadial + airgapElementsUp[i]->reluctanceRadial + airgapReluctanceUp[i]);
    }
}

void MD_ReluctanceElement::computeFluxEquations()
{
    // Clear equations
    EquationMagneticFluxLeft.clear();
    EquationMagneticFluxUp.clear();
    EquationMagneticFluxRight.clear();
    EquationMagneticFluxDown.clear();
    EquationMagneticFluxTangential.clear();
    EquationMagneticFluxRadial.clear();
    EquationMagneticFluxBalance.clear();
    // Left side element
    if(elementLeft != nullptr)
    {
        EquationMagneticFluxLeft.addCoefficient(index, permeanceLeft);
        EquationMagneticFluxLeft.addCoefficient(elementLeft->index, -permeanceLeft);
        for(auto source : MMFSources)
        {
            if (source->Orientation == MMFSO_tangential) {
                if (source->ConstantSource) {
                    double value = 0.5 * permeanceLeft * source->MMF;
                    EquationMagneticFluxLeft.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = 0.5 * permeanceLeft * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxLeft.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
        for(auto source : elementLeft->MMFSources)
        {
            if (source->Orientation == MMFSO_tangential) {
                if (source->ConstantSource) {
                    double value = 0.5 * permeanceLeft * source->MMF;
                    EquationMagneticFluxLeft.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = 0.5 * permeanceLeft * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxLeft.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
    }
    // Right side element
    if(elementRight != nullptr)
    {
        EquationMagneticFluxRight.addCoefficient(index, permeanceRight);
        EquationMagneticFluxRight.addCoefficient(elementRight->index, -permeanceRight);
        for(auto source : MMFSources)
        {
            if (source->Orientation == MMFSO_tangential) {
                if (source->ConstantSource) {
                    double value = -0.5 * permeanceRight * source->MMF;
                    EquationMagneticFluxRight.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = -0.5 * permeanceRight * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxRight.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
        for(auto source : elementRight->MMFSources)
        {
            if (source->Orientation == MMFSO_tangential) {
                if (source->ConstantSource) {
                    double value = -0.5 * permeanceRight * source->MMF;
                    EquationMagneticFluxRight.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = -0.5 * permeanceRight * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxRight.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
    }
    // Down side element
    if(elementDown != nullptr)
    {
        EquationMagneticFluxDown.addCoefficient(index, permeanceDown);
        EquationMagneticFluxDown.addCoefficient(elementDown->index, -permeanceDown);
        for(auto source : MMFSources)
        {
            if (source->Orientation == MMFSO_radial) {
                if (source->ConstantSource) {
                    double value = 0.5 * permeanceDown * source->MMF;
                    EquationMagneticFluxDown.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = 0.5 * permeanceDown * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxDown.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
        for(auto source : elementDown->MMFSources)
        {
            if (source->Orientation == MMFSO_radial) {
                if (source->ConstantSource) {
                    double value = 0.5 * permeanceDown * source->MMF;
                    EquationMagneticFluxDown.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = 0.5 * permeanceDown * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxDown.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
    }
    if (!airgapElementsDown.empty()) {
        for (unsigned i = 0; i < airgapElementsDown.size(); ++i) {
            EquationMagneticFluxDown.addCoefficient(index, airgapPermeanceDown[i]);
            EquationMagneticFluxDown.addCoefficient(airgapElementsDown[i]->index, -airgapPermeanceDown[i]);
        }
    }
    // Up side element
    if(elementUp != nullptr)
    {
        EquationMagneticFluxUp.addCoefficient(index, permeanceUp);
        EquationMagneticFluxUp.addCoefficient(elementUp->index, -permeanceUp);
        for(auto source : MMFSources)
        {
            if (source->Orientation == MMFSO_radial) {
                if (source->ConstantSource) {
                    double value = -0.5 * permeanceUp * source->MMF;
                    EquationMagneticFluxUp.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = -0.5 * permeanceUp * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxUp.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
        for (auto source : elementUp->MMFSources) {
            if (source->Orientation == MMFSO_radial) {
                if (source->ConstantSource) {
                    double value = -0.5 * permeanceUp * source->MMF;
                    EquationMagneticFluxUp.addConstant(value);
                } else {
                    for(unsigned current = 0; current < source->MeshIndexes.size(); ++current) {
                        double value = -0.5 * permeanceUp * source->ConstantMMF * double(source->MeshPolarities[current]);
                        EquationMagneticFluxUp.addCoefficient(source->MeshIndexes[current], value);
                    }
                }
            }
        }
    }
    if (!airgapElementsUp.empty()) {
        for (unsigned i = 0; i < airgapElementsUp.size(); ++i) {
            EquationMagneticFluxUp.addCoefficient(index, airgapPermeanceUp[i]);
            EquationMagneticFluxUp.addCoefficient(airgapElementsUp[i]->index, -airgapPermeanceUp[i]);
        }
    }

    EquationMagneticFluxTangential = (EquationMagneticFluxRight << (EquationMagneticFluxLeft*-1.0)) * 0.5;
    EquationMagneticFluxRadial = (EquationMagneticFluxUp << (EquationMagneticFluxDown*-1.0)) * 0.5;
    EquationMagneticFluxBalance = EquationMagneticFluxLeft << EquationMagneticFluxRight << EquationMagneticFluxDown << EquationMagneticFluxUp;
}

MEC_RealEquation MD_ReluctanceElement::getFluxBalanceEquation()
{    
    return EquationMagneticFluxBalance;
}

MEC_RealEquation MD_ReluctanceElement::getTangentionalFluxEquation()
{
    return EquationMagneticFluxTangential;
}

MEC_RealEquation MD_ReluctanceElement::getRadialFluxEquation()
{
    return EquationMagneticFluxRadial;
}

void MD_ReluctanceElement::computeMagneticFluxes()
{
    double flux;

    // Left side element
    flux = 0.0;
    if (elementLeft != nullptr) {
        flux += permeanceLeft * (magneticPotential - elementLeft->magneticPotential);
        for (auto source : MMFSources) {
            if (source->Orientation == MMFSO_tangential) {
                flux += 0.5 * permeanceLeft * source->MMF;
            }
        }
        for (auto source : elementLeft->MMFSources) {
            if (source->Orientation == MMFSO_tangential) {
                flux += 0.5 * permeanceLeft * source->MMF;
            }
        }
    }
    magneticFluxLeft = flux;

    // Right side element
    flux = 0.0;
    if (elementRight != nullptr) {
        flux += permeanceRight * (magneticPotential - elementRight->magneticPotential);
        for (auto source : MMFSources) {
            if (source->Orientation == MMFSO_tangential) {
                flux -= 0.5 * permeanceRight * source->MMF;
            }
        }
        for (auto source : elementRight->MMFSources) {
            if (source->Orientation == MMFSO_tangential) {
                flux -= 0.5 * permeanceRight * source->MMF;
            }
        }
    }
    magneticFluxRight = flux;

    // Down side element
    flux = 0.0;
    if (elementDown != nullptr) {
        flux += permeanceDown * (magneticPotential - elementDown->magneticPotential);
        for (auto source : MMFSources) {
            if (source->Orientation == MMFSO_radial) {
                flux += 0.5 * permeanceDown * source->MMF;
            }
        }
        for(auto source : elementDown->MMFSources)
        {
            if (source->Orientation == MMFSO_radial) {
                flux += 0.5 * permeanceDown * source->MMF;
            }
        }
    }
    if (!airgapElementsDown.empty()) {
        for (unsigned i = 0; i < airgapElementsDown.size(); ++i) {
            flux += airgapPermeanceDown[i] * (magneticPotential - airgapElementsDown[i]->magneticPotential);
        }
    }
    magneticFluxDown = flux;

    // Up side element
    flux = 0.0;
    if (elementUp != nullptr) {
        flux += permeanceUp * (magneticPotential - elementUp->magneticPotential);
        for (auto source : MMFSources) {
            if (source->Orientation == MMFSO_radial) {
                flux -= 0.5 * permeanceUp * source->MMF;
            }
        }
        for(auto source : elementUp->MMFSources)
        {
            if (source->Orientation == MMFSO_radial) {
                flux -= 0.5 * permeanceUp * source->MMF;
            }
        }
    }
    if (!airgapElementsUp.empty()) {
        for (unsigned i = 0; i < airgapElementsUp.size(); ++i) {
            flux += airgapPermeanceUp[i] * (magneticPotential - airgapElementsUp[i]->magneticPotential);
        }
    }
    magneticFluxUp = flux;

    magneticFluxTangential = (magneticFluxRight - magneticFluxLeft) / 2.0;
    magneticFluxRadial = (magneticFluxUp - magneticFluxDown) / 2.0;
}

double MD_ReluctanceElement::computeMagneticFluxDensity()
{
    double BB_l = magneticFluxLeft / geometry.crossSectionalAreaTangential;
    double BB_r = magneticFluxRight / geometry.crossSectionalAreaTangential;
    double BB_u = magneticFluxUp / geometry.crossSectionalAreaRadial;
    double BB_d = magneticFluxDown / geometry.crossSectionalAreaRadial;

    BB_l *= BB_l; BB_r *= BB_r; BB_u *= BB_u; BB_d *= BB_d;

    magneticFluxDensityTangential = magneticFluxTangential / geometry.crossSectionalAreaTangential;
    magneticFluxDensityRadial = magneticFluxRadial / geometry.crossSectionalAreaRadial;
    magneticFluxDensity = std::sqrt((BB_l + BB_r + BB_u + BB_d) / 2.0);

    return magneticFluxDensity;
}

double MD_ReluctanceElement::computeEnergy()
{
    energy = 0.5 * reluctivity * magneticFluxDensity * magneticFluxDensity;

    return energy;
}

double MD_ReluctanceElement::computeTangentialForce()
{
    forceTX = geometry.length * geometry.width * (magneticFluxDensityRadial * magneticFluxDensityTangential) / (U_AIR);

    return forceTX;
}

double MD_ReluctanceElement::computeRadialForce()
{
    double dFry = magneticFluxDensityRadial*magneticFluxDensityRadial;
    double dFtx = magneticFluxDensityTangential*magneticFluxDensityTangential;
    forceRY = geometry.length * geometry.width * (dFry - dFtx) / (2.0 * U_AIR);

    return forceRY;
}

void MD_ReluctanceElement::initialize()
{
    index = 0;
    type = RET_notDefined;
    reluctivity = 0.0;
    constantReluctanceTangential = 0.0;
    constantReluctanceRadial = 0.0;
    reluctanceTangential = 0.0;
    reluctanceRadial = 0.0;
    permeanceLeft = 0.0;
    permeanceRight = 0.0;
    permeanceUp = 0.0;
    permeanceDown = 0.0;
    RemanentMagnetization = 0.0;
    MagnetOrientation = MMFSO_notDefined;
    MagnetPolarity = 0;
    MagnetMMFSource = 0.0;
    elementLeft = nullptr;
    elementRight = nullptr;
    elementUp = nullptr;
    elementDown = nullptr;

    magneticPotential = 0.0;
    electricCurrent = 0.0;
    electricCurrentDensity = 0.0;
    magneticFluxLeft = 0.0;
    magneticFluxRight = 0.0;
    magneticFluxUp = 0.0;
    magneticFluxDown = 0.0;
    magneticFluxTangential = 0.0;
    magneticFluxRadial = 0.0;
    magneticFluxDensity = 0.0;
    magneticFluxDensityTangential = 0.0;
    magneticFluxDensityRadial = 0.0;
    energy = 0.0;
    forceTX = 0.0;
    forceRY = 0.0;

    magneticFluxDensityPrevious = 0.0;
}

void MD_ReluctanceElement::computeReluctances()
{
    reluctanceRadial = reluctivity * constantReluctanceRadial;
    reluctanceTangential = reluctivity * constantReluctanceTangential;
}

double MD_ReluctanceElement::computeMagneticFluxDensityDerivative_tangential(bool h_l, bool h_r)
{
    double h, B_h;
    double variation = 1E-3;
    double F_l = magneticFluxLeft;
    double F_r = magneticFluxRight;
    double P_l = permeanceLeft;
    double P_r = permeanceRight;
    if (h_r && h_l) {
        h = (std::abs(magneticPotential) > 0.0) ? magneticPotential * variation : variation;
        double BB_r = (((F_r / P_r) + h) * P_r) / geometry.crossSectionalAreaTangential;
        double BB_l = (((F_l / P_l) + h) * P_l) / geometry.crossSectionalAreaTangential;
        double BB_u = magneticFluxUp / geometry.crossSectionalAreaRadial;
        double BB_d = magneticFluxDown / geometry.crossSectionalAreaRadial;
        BB_r *= BB_r; BB_l *= BB_l; BB_u *= BB_u; BB_d *= BB_d;
        B_h = std::sqrt((BB_r + BB_l + BB_u + BB_d) / 2.0);
    } else if (h_l) {
        if (elementLeft != nullptr)
            h = (std::abs(elementLeft->magneticPotential) > 0.0) ? elementLeft->magneticPotential * variation : variation;
        else
            h = 0.0;
        double BB_r = magneticFluxRight / geometry.crossSectionalAreaTangential;
        double BB_l = (((F_l / P_l) - h) * P_l) / geometry.crossSectionalAreaTangential;
        double BB_u = magneticFluxUp / geometry.crossSectionalAreaRadial;
        double BB_d = magneticFluxDown / geometry.crossSectionalAreaRadial;
        BB_r *= BB_r; BB_l *= BB_l; BB_u *= BB_u; BB_d *= BB_d;
        B_h = std::sqrt((BB_r + BB_l + BB_u + BB_d) / 2.0);
    } else {
        if (elementRight != nullptr)
            h = (std::abs(elementRight->magneticPotential) > 0.0) ? elementRight->magneticPotential * variation : variation;
        else
            h = 0.0;
        double BB_r = (((F_r / P_r) - h) * P_r) / geometry.crossSectionalAreaTangential;
        double BB_l = magneticFluxLeft / geometry.crossSectionalAreaTangential;
        double BB_u = magneticFluxUp / geometry.crossSectionalAreaRadial;
        double BB_d = magneticFluxDown / geometry.crossSectionalAreaRadial;
        BB_r *= BB_r; BB_l *= BB_l; BB_u *= BB_u; BB_d *= BB_d;
        B_h = std::sqrt((BB_r + BB_l + BB_u + BB_d) / 2.0);
    }
    return ((B_h - magneticFluxDensity) / h);
}

double MEC_SignalSource::readPulse()
{
    return pulses[step];
}

double MEC_SignalSource::nextPulse()
{
    double pulse = pulses[step++];
    if (step == pulses.size())
        step = 0;

    return pulse;
}

MD_ElectricCircuitComponent::MD_ElectricCircuitComponent()
{
    Type = MDECCT_notDefined;
    Resistance = 0.0;
    Inductance = 0.0;
    Power = 0.0;
    JouleLosses = 0.0;
    Current = 0.0;
    CurrentPrevious = 0.0;
    Voltage = 0.0;
    VoltageCoilInduced = 0.0;
    LinkageFlux = 0.0;
    LinkageFluxPrevious = 0.0;
    MeshCurrentSource = nullptr;
}

void MD_ElectricCircuitComponent::setVolatageSource(MEC_SignalSource signal, std::string text, double resistance, double inductance)
{
    Type = MDECCT_voltageSource;
    VoltageSource = signal;
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
}

void MD_ElectricCircuitComponent::setCurrentSource(std::string text, double resistance, double inductance)
{
    Type = MDECCT_currentSource;
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
}

void MD_ElectricCircuitComponent::setCurrentSource(MEC_SignalSource signal, std::string text, double resistance, double inductance)
{
    Type = MDECCT_currentSource;
    CurrentSource = signal;
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
}

void MD_ElectricCircuitComponent::setImpedance(double resistance, double inductance, std::string text)
{
    Type = MDECCT_impedance;
    Resistance = resistance;
    Inductance = inductance;
    Label = text;
}

void MD_ElectricCircuitComponent::setCoil(std::string text, double resistance, double inductance)
{
    Type = MDECCT_coil;
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
}

void MD_ElectricCircuitComponent::setCoil(MD_CoilSide coil_side, std::string text, double resistance, double inductance)
{
    Type = MDECCT_coil;
    CoilSides.clear();
    CoilSides.push_back(coil_side);
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
}

void MD_ElectricCircuitComponent::setCoil(std::vector<MD_CoilSide> coil_sides, std::string text, double resistance, double inductance)
{
    Type = MDECCT_coil;
    CoilSides = coil_sides;
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
}

double MD_ElectricCircuitComponent::readPulse()
{
    if (Type == MDECCT_voltageSource) {
        return VoltageSource.readPulse();
    }
    if (Type == MDECCT_currentSource) {
        return CurrentSource.readPulse();
    }
    return 0.0;
}

double MD_ElectricCircuitComponent::nextPulse()
{
    if (Type == MDECCT_voltageSource) {
        return VoltageSource.nextPulse();
    }
    if (Type == MDECCT_currentSource) {
        return CurrentSource.nextPulse();
    }
    return 0.0;
}

double MD_ElectricCircuitComponent::computeLinkageFlux(double timeStep)
{
    LinkageFlux = 0;
    for (unsigned coilSideIndex = 0; coilSideIndex < CoilSides.size(); ++coilSideIndex) {
        MD_CoilSide coilSide = CoilSides[coilSideIndex];
        for (unsigned currentElementIndex = 0; currentElementIndex < coilSide.activeReluctanceElements.size(); ++currentElementIndex) {
            MD_ReluctanceElement* element = coilSide.activeReluctanceElements[currentElementIndex];
            double EMF_Constant = (-1.0 * coilSide.Polarity * coilSide.Turns * element->getGeometry().crossSectionalArea) / (coilSide.Area * timeStep);
            LinkageFlux += element->magneticFluxTangential * (EMF_Constant / 2.0);
            for (unsigned otherElementIndex = currentElementIndex + 1; otherElementIndex < coilSide.activeReluctanceElements.size(); ++otherElementIndex) {
                element = coilSide.activeReluctanceElements[otherElementIndex];
                LinkageFlux += element->magneticFluxTangential * EMF_Constant;
            }
            for (unsigned passiveElementIndex = 0; passiveElementIndex < coilSide.passiveReluctanceElements.size(); ++passiveElementIndex) {
                element = coilSide.passiveReluctanceElements[passiveElementIndex];
                LinkageFlux += element->magneticFluxTangential * EMF_Constant;
            }
        }
    }

    return LinkageFlux;
}

double MD_ElectricCircuitComponent::computeVoltageCoilInduced()
{
    VoltageCoilInduced = LinkageFlux - LinkageFluxPrevious;

    return VoltageCoilInduced;
}

void MD_ElectricCircuit::addComponent(MD_ElectricCircuitComponent *component, int polarity)
{
    if(component->getType() == MDECCT_currentSource)
        currentSourceMesh = true;
    components.push_back(component);
    polarities.push_back(polarity);
}

MD_MMFSource::MD_MMFSource()
{
    ConstantSource = false;
    Polarity = 0;
    ConstantMMF = 0.0;
    Current = 0.0;
    MMF = 0.0;
    ReluctanceElement = nullptr;
    CoilComponent = nullptr;
}

void MD_MMFSource::setPolarity(int polarity)
{
    Polarity = polarity;
}

void MD_MMFSource::setCurrent()
{
    if ( ConstantSource ) return;
    Current = CoilComponent->Current;
    MMF = ConstantMMF * Current;
}

void MD_MMFSource::setMMF(double current)
{
    Current = current;
    MMF = ConstantMMF * Current;
}

MD_AirgapConnection::MD_AirgapConnection()
{
    Permeance = 0.0;
    PermeanceMaximum = 0.0;
    FixedElement = nullptr;
    MovableElement = nullptr;
}

MD_AirgapConnection::MD_AirgapConnection(MD_ReluctanceElement *fixedElement, MD_ReluctanceElement *movableElement)
{
    Permeance = 0.0;
    PermeanceMaximum = 0.0;
    FixedElement = fixedElement;
    MovableElement = movableElement;
    FixedElementGeometry = fixedElement->getGeometry();
    MovableElementGeometry = movableElement->getGeometry();
    computeMaximumPermeance();
}

void MD_AirgapConnection::setElements(MD_ReluctanceElement *fixedElement, MD_ReluctanceElement *movableElement)
{
    Permeance = 0.0;
    PermeanceMaximum = 0.0;
    FixedElement = fixedElement;
    MovableElement = movableElement;
    FixedElementGeometry = fixedElement->getGeometry();
    MovableElementGeometry = movableElement->getGeometry();
    computeMaximumPermeance();
}

double MD_AirgapConnection::computePermeance(double movableBlockAngularPosition, double lowerEnd, double upperEnd)
{
    // Set the rotor angular to the stator element angular position
    double span = upperEnd - lowerEnd;
    double halfSpan = span / 2.0;
    double fixedTheta = FixedElementGeometry.position_tX;
    double movableTheta = (MovableElementGeometry.position_tX + movableBlockAngularPosition);
    movableTheta = getValueInPeridicity(movableTheta, lowerEnd, upperEnd);
    double displacement = std::abs(fixedTheta - movableTheta);
    if ( displacement > halfSpan)
        displacement = std::abs(displacement - span);
    double theta_m = std::abs(FixedElementGeometry.arcBottom - MovableElementGeometry.arcTop) / 2.0;
    double theta_z = (FixedElementGeometry.arcBottom + MovableElementGeometry.arcTop) / 2.0;
    // Calculate permeance
//    if (displacement < theta_m)
//        Permeance = PermeanceMaximum;
//    else if (displacement < theta_z) {
//        //Permeance = PermeanceMaximum * (displacement - theta_z) / (theta_m - theta_z);
//        Permeance = PermeanceMaximum * (1.0 + std::cos(M_PI * (displacement - theta_m) / (theta_z - theta_m))) / 2.0;
//    } else
//        Permeance = 0.0;
//    else if (displacement < theta_z)
//        Permeance = PermeanceMaximum * (1.0 + std::cos(M_PI * (displacement - theta_m) / (theta_z - theta_m))) / 2.0;
//    else
//        Permeance = 0.0;
    if (displacement < theta_m)
        Permeance = PermeanceMaximum;
    else {
        Permeance = PermeanceMaximum * std::exp(-std::pow((displacement - theta_m) / (theta_z - theta_m), (2.0)));
        if (Permeance < (PermeanceMaximum * 0.00001))
            Permeance = 0.0;
    }
//    Permeance = PermeanceMaximum * std::exp(-std::pow(displacement / theta_z, (6.0)));
//    if (Permeance < (PermeanceMaximum * 0.00001))
//        Permeance = 0.0;
    return Permeance;
}

void MD_AirgapConnection::computeMaximumPermeance()
{
    double angularBottom = FixedElementGeometry.arcBottom;
    double angularTop = FixedElementGeometry.arcBottom;
    double radiusBottom = MovableElementGeometry.radiusTop;
    double radiusTop = FixedElementGeometry.radiusBottom;
    double length = FixedElementGeometry.length;
    double b = angularBottom - (radiusBottom * (angularTop - angularBottom) / (radiusTop - radiusBottom));
    double ln = std::log((radiusTop*angularBottom) / (radiusBottom*angularTop));
    double constantReluctanceRadial = ln / (b * length);

    PermeanceMaximum = U_AIR / constantReluctanceRadial;
}

MD_AirgapStructure::MD_AirgapStructure()
{
    Periodicities = 1;
}

void MD_AirgapStructure::setConnectionElements(std::vector<MD_ReluctanceElement*> fixedBlockElements, std::vector<MD_ReluctanceElement*> movableBlockElements)
{
    connectionElements_fixedBlock = fixedBlockElements;
    connectionElements_movableBlock = movableBlockElements;

    createAirgapElements();
}

void MD_AirgapStructure::setAirgapPermeances(double tangentialPosition)
{
    for (auto element : connectionElements_fixedBlock)
        element->clearAirgapPermeances();
    for (auto element : connectionElements_movableBlock)
        element->clearAirgapPermeances();

    computeAirgapPermeances(tangentialPosition);

    for (unsigned i = 0; i < connectionElements_fixedBlock.size(); ++i) {
        for (unsigned j = 0; j < currentlyConnectedMovableBlockElements[i].size(); ++j) {
            double permeance = currentlyConnectedPermeances[i][j];
            connectionElements_fixedBlock[i]->addAirgapPermeanceDown(currentlyConnectedMovableBlockElements[i][j], permeance);
            currentlyConnectedMovableBlockElements[i][j]->addAirgapPermeanceUp(connectionElements_fixedBlock[i], permeance);
        }
    }
}

void MD_AirgapStructure::createAirgapElements()
{
    airgapDirectConnectionElements.resize(connectionElements_fixedBlock.size());
    for (unsigned fixedElement = 0; fixedElement < connectionElements_fixedBlock.size(); ++fixedElement) {
        airgapDirectConnectionElements[fixedElement].resize(connectionElements_movableBlock.size());
        for (unsigned movableElement = 0; movableElement < connectionElements_movableBlock.size(); ++movableElement) {
            airgapDirectConnectionElements[fixedElement][movableElement].setElements(connectionElements_fixedBlock[fixedElement], connectionElements_movableBlock[movableElement]);
        }
    }
}

void MD_AirgapStructure::computeAirgapPermeances(double rotorAngularPosition)
{
    double minPeriodicity = 0.0;
    double maxPeriodicity = M_2PI / Periodicities;
    currentlyConnectedMovableBlockElements.resize(connectionElements_fixedBlock.size());
    currentlyConnectedPermeances.resize(connectionElements_fixedBlock.size());
    for (unsigned fixedElement = 0; fixedElement < connectionElements_fixedBlock.size(); ++fixedElement) {
        airgapDirectConnectionElements[fixedElement].resize(connectionElements_movableBlock.size());
        std::vector<MD_ReluctanceElement*> connectedMovableBlockElements;
        std::vector<double> connectedPermenances;
        for (unsigned movableElement = 0; movableElement < connectionElements_movableBlock.size(); ++movableElement) {
            double permeance = airgapDirectConnectionElements[fixedElement][movableElement].computePermeance(rotorAngularPosition, minPeriodicity, maxPeriodicity);
            if (permeance > 0.0) {
                connectedMovableBlockElements.push_back(airgapDirectConnectionElements[fixedElement][movableElement].MovableElement);
                connectedPermenances.push_back(airgapDirectConnectionElements[fixedElement][movableElement].getPermeance());
            }
        }
        currentlyConnectedMovableBlockElements[fixedElement] = connectedMovableBlockElements;
        currentlyConnectedPermeances[fixedElement] = connectedPermenances;
    }
}

MD_ElectromagneticBlock::MD_ElectromagneticBlock()
{
    Length = 0.0;
    Position_tX = 0.0;
    Position_rY = 0.0;
    Speed_tX = 0.0;
    Speed_rY = 0.0;
    Acceleration_tX = 0.0;
    Acceleration_rY = 0.0;
    Inertia = 0.0;
    TangentialElementsCount = 0.0;
    RadialElementsCount = 0.0;
    Slices = 0.0;

    TangentialForce = 0.0;
    RadialForce = 0.0;
    IronLosses = 0.0;
    MagneticEnergy = 0.0;
    PermanentMagnetLosses = 0.0;
    Weight = 0.0;
    Volume = 0.0;
}

void MD_ElectromagneticBlock::computeDensity()
{
    Weight = 0.0; Volume = 0.0;
    for (auto element : ReluctanceElements) {
        double volume = element->getGeometry().volume;
        Volume += volume;
        switch (element->getType()) {
        case RET_coreNonLinear:
            Weight += volume * MaterialNonLinear.getDensity();
            break;
        case RET_magnet:
            Weight += volume * 7400; // Ferrite = 5000 kg / m3; Neodymium = 7400 kg / m3. PermanentMagnetDensity;
            break;
        case RET_coil:
            Weight += volume * WindingDensity;
            break;
        default:
            break;
        }
    }
}

void MD_ElectromagneticBlock::computeTorque()
{
    if (!AirgapLayerElements.empty()) {
        double force = 0.0;
        for (auto element : AirgapLayerElements) {
            element->computeTangentialForce();
            force += element->forceTX;
        }
        TangentialForce = force;
    }
}

MD_Project::MD_Project()
{
    CurrentStep = 0;
    Steps = 0;
    TimeStep = 0;
    UnknownsCount = 0;
    isNonlinear = false;
    isSolved = false;
    Periodicities = 1;
}

MD_Project::MD_Project(MD_ElectromagneticBlock fixed)
{
    CurrentStep = 0;
    Steps = 0;
    TimeStep = 0;
    UnknownsCount = 0;
    FixedBlock = fixed;
    isNonlinear = false;
    isSolved = false;
    Periodicities = 1;

    preProcessing();
}

MD_Project::MD_Project(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable)
{
    CurrentStep = 0;
    Steps = 0;
    TimeStep = 0;
    UnknownsCount = 0;
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;
    Periodicities = 1;

    preProcessing();
}

MD_Project::MD_Project(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable, int periodicity)
{
    CurrentStep = 0;
    Steps = 0;
    TimeStep = 0;
    UnknownsCount = 0;
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;
    Periodicities = periodicity;

    preProcessing();
}

MD_Project::~MD_Project()
{

}

void MD_Project::destroyProject()
{
    destroyBlock(FixedBlock);
    destroyBlock(MovableBlock);
    SuperLUData.close();
    SystemOfEquations.clear();
    UnknownsVector.clear();
    isSolved = false;
}

void MD_Project::loadProject(MD_ElectromagneticBlock fixed)
{
    FixedBlock = fixed;
    isNonlinear = false;
    isSolved = false;

    preProcessing();
}

void MD_Project::loadProject(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable)
{
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;

    preProcessing();
}

void MD_Project::loadProject(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable, int periodicity)
{
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;
    Periodicities = periodicity;

    preProcessing();
}

std::vector<ReportDataSolver> MD_Project::solve(double timeStep, unsigned steps, double relaxation, unsigned iterations, double error)
{
    TimeStep = timeStep;
    Steps = steps;
    CurrentStep = 0;
    isSolved = false;

    std::vector<ReportDataSolver> solverReport(Steps);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::vector<double> prevBFixed(FixedBlock.NonlinearReluctanceElements.size(), 0.0);
    std::vector<double> prevBMovable(MovableBlock.NonlinearReluctanceElements.size(), 0.0);
    // Initiate the global system of equations
    SystemOfEquations.clear();
    UnknownsVector.clear();
    SystemOfEquations.resize(UnknownsCount);
    UnknownsVector.resize(UnknownsCount);

    for (unsigned step = CurrentStep; step < Steps; ++step) {
        double time = step * timeStep;
        setPreviousValues();
        double B;
        airgapStructure.setAirgapPermeances(MovableBlock.Position_tX);
        solverReport[step].data.resize(9);
        // Create/update airgap structure
        if (isNonlinear) {
            double variationMax = error + 1.0;
            unsigned iteration = 0;
            while ((variationMax > error) && (iteration++ < iterations)) {
                variationMax = 0.0;
                assembleProject();
                if (SuperLU(SystemOfEquations, UnknownsVector) != 0) {
                    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                    solverReport[step].elapsedTime =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                    solverReport[step].variance = 0.0;
                    solverReport[step].iterations = 0;
                    solverReport[step].solverError = true;
                    solverReport[step].currentStep = step;
                    solverReport[step].currentTime = time;
                    return solverReport;
                }
                fastPostProcessing();
                for (unsigned i = 0; i < FixedBlock.NonlinearReluctanceElements.size(); ++i) {
                    double oldReluctivity = FixedBlock.NonlinearReluctanceElements[i]->getReluctivity();
                    double newReluctivity = FixedBlock.MaterialNonLinear.getReluctivity(FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity);
                    double reluctivityDifferetial = newReluctivity - oldReluctivity;
                    double variation = 1.0;
                    if (prevBFixed[i] > 0.0)
                        variation = std::abs(FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity - prevBFixed[i]) / prevBFixed[i];
                    if (variation > variationMax) {
                        variationMax = variation;
                        B = FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
                    }
                    FixedBlock.NonlinearReluctanceElements[i]->setReluctivity(oldReluctivity + (relaxation * reluctivityDifferetial));
                    prevBFixed[i] = FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
                }
                for (unsigned i = 0; i < MovableBlock.NonlinearReluctanceElements.size(); ++i) {
                    double oldReluctivity = MovableBlock.NonlinearReluctanceElements[i]->getReluctivity();
                    double newReluctivity = MovableBlock.MaterialNonLinear.getReluctivity(MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity);
                    double reluctivityDifferetial = newReluctivity - oldReluctivity;
                    double variation = 1.0;
                    if (prevBMovable[i] > 0.0)
                        variation = std::abs(MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity - prevBMovable[i]) / prevBMovable[i];
                    if (variation > variationMax) {
                        variationMax = variation;
                        B = MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
                    }
                    MovableBlock.NonlinearReluctanceElements[i]->setReluctivity(oldReluctivity + (relaxation * reluctivityDifferetial));
                    prevBMovable[i] = MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
                }
            }
            postProcessing();
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            solverReport[step].elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            solverReport[step].variance = variationMax;
            solverReport[step].iterations = iteration;
            solverReport[step].solverError = false;
            solverReport[step].currentStep = step;
            solverReport[step].currentTime = time;
            saveData(solverReport[step].data);
            updateSignals();
            updateMechanics();
        } else {
            assembleProject();
            if (SuperLU(SystemOfEquations, UnknownsVector) != 0) {
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                solverReport[step].elapsedTime =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                solverReport[step].variance = 0.0;
                solverReport[step].iterations = 0;
                solverReport[step].solverError = true;
                solverReport[step].currentStep = step;
                solverReport[step].currentTime = time;
                return solverReport;
            }
            postProcessing();
            updateSignals();
            updateMechanics();
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            solverReport[step].elapsedTime =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            solverReport[step].variance = 0.0;
            solverReport[step].iterations = 1;
            solverReport[step].solverError = false;
            solverReport[step].currentStep = step;
            solverReport[step].currentTime = time;
        }
        CurrentStep = step;
    }

    FixedBlock.computeDensity();
    MovableBlock.computeDensity();

    return solverReport;
}

void MD_Project::solveField(double relaxation, unsigned iterations, double error)
{
    isSolved = false;
    std::vector<double> prevBFixed(FixedBlock.NonlinearReluctanceElements.size(), 0.0);
    std::vector<double> prevBMovable(MovableBlock.NonlinearReluctanceElements.size(), 0.0);
    // Initiate the global system of equations
    SystemOfEquations.clear();
    UnknownsVector.clear();
    SystemOfEquations.resize(UnknownsCount);
    UnknownsVector.resize(UnknownsCount);
    setPreviousValues();
    double B;
    airgapStructure.setAirgapPermeances(MovableBlock.Position_tX);
    // Create/update airgap structure
    double variationMax = error + 1.0;
    unsigned iteration = 0;
    while ((variationMax > error) && (iteration++ < iterations)) {
        variationMax = 0.0;
        assembleProject();
        if (SuperLU(SystemOfEquations, UnknownsVector) != 0) {
            return;
        }
        fastPostProcessing();
        for (unsigned i = 0; i < FixedBlock.NonlinearReluctanceElements.size(); ++i) {
            double oldReluctivity = FixedBlock.NonlinearReluctanceElements[i]->getReluctivity();
            double newReluctivity = FixedBlock.MaterialNonLinear.getReluctivity(FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity);
            double reluctivityDifferetial = newReluctivity - oldReluctivity;
            double variation = 1.0;
            if (prevBFixed[i] > 0.0)
                variation = std::abs(FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity - prevBFixed[i]) / prevBFixed[i];
            if (variation > variationMax) {
                variationMax = variation;
                B = FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
            }
            FixedBlock.NonlinearReluctanceElements[i]->setReluctivity(oldReluctivity + (relaxation * reluctivityDifferetial));
            prevBFixed[i] = FixedBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
        }
        for (unsigned i = 0; i < MovableBlock.NonlinearReluctanceElements.size(); ++i) {
            double oldReluctivity = MovableBlock.NonlinearReluctanceElements[i]->getReluctivity();
            double newReluctivity = MovableBlock.MaterialNonLinear.getReluctivity(MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity);
            double reluctivityDifferetial = newReluctivity - oldReluctivity;
            double variation = 1.0;
            if (prevBMovable[i] > 0.0)
                variation = std::abs(MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity - prevBMovable[i]) / prevBMovable[i];
            if (variation > variationMax) {
                variationMax = variation;
                B = MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
            }
            MovableBlock.NonlinearReluctanceElements[i]->setReluctivity(oldReluctivity + (relaxation * reluctivityDifferetial));
            prevBMovable[i] = MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
        }
    }
    // Field Postprocessing
    setMagneticScalarPotentials(FixedBlock);
    setElectricCurrentsAndMMFs(FixedBlock);
    setMagneticScalarPotentials(MovableBlock);
    setElectricCurrentsAndMMFs(MovableBlock);
    isSolved = true;
}

void MD_Project::destroyBlock(MD_ElectromagneticBlock &block)
{
    // variables
    block.Length = 0.0;
    block.Position_tX = 0.0;
    block.Position_rY = 0.0;
    block.Speed_tX = 0.0;
    block.Speed_rY = 0.0;
    block.Acceleration_tX = 0.0;
    block.Acceleration_rY = 0.0;
    block.Inertia = 0.0;
    block.TangentialElementsCount = 0;
    block.RadialElementsCount = 0;
    block.Slices = 0;
    block.TangentialForce = 0.0;
    block.RadialForce = 0.0;
    block.IronLosses = 0.0;
    block.MagneticEnergy = 0.0;
    block.PermanentMagnetLosses = 0.0;
    block.Weight = 0.0;
    block.Volume = 0.0;
    // pointers
    for (auto object : block.MMFSources) delete object;
    for (auto object : block.ReluctanceElements) delete object;
    for (auto object : block.CoilComponents) delete object;
    for (auto object : block.PassiveComponents) delete object;
    for (auto object : block.VoltageSourceComponents) delete object;
    for (auto object : block.CurrentSourceComponents) delete object;
    block.MMFSources.clear();
    block.ReluctanceElements.clear();
    block.AirgapLayerElements.clear();
    block.MagnetElements.clear();
    block.NonlinearReluctanceElements.clear();
    block.CoilComponents.clear();
    block.PassiveComponents.clear();
    block.VoltageSourceComponents.clear();
    block.CurrentSourceComponents.clear();
    block.ElectricCircuits.clear();
}

void MD_Project::preProcessing()
{
    unsigned blockStartingIndex = 0;
    // Magnetic domain
    blockStartingIndex = initiateMagneticScalarPotentialIndexes(FixedBlock, blockStartingIndex);
    blockStartingIndex = initiateMagneticScalarPotentialIndexes(MovableBlock, blockStartingIndex);
    // Electric domain
    blockStartingIndex = initiateMeshCurrentIndexes(FixedBlock, blockStartingIndex);
    initiateMMFSources(FixedBlock);
    blockStartingIndex = initiateMeshCurrentIndexes(MovableBlock, blockStartingIndex);
    initiateMMFSources(MovableBlock);
    UnknownsCount = blockStartingIndex;
    if ( !(FixedBlock.NonlinearReluctanceElements.empty() && MovableBlock.NonlinearReluctanceElements.empty()) )
        isNonlinear = true;
    // Airgap structure
    airgapStructure.setPeriodicities(Periodicities);
    if (!(FixedBlock.AirgapLayerElements.empty() || MovableBlock.AirgapLayerElements.empty()))
        airgapStructure.setConnectionElements(FixedBlock.AirgapLayerElements, MovableBlock.AirgapLayerElements);
}

void MD_Project::fastPostProcessing()
{
    // Set magnetic scalar potentials
    for (auto element : FixedBlock.ReluctanceElements) {
        element->magneticPotential = UnknownsVector[element->getIndex()];
    }
    for (auto element : MovableBlock.ReluctanceElements) {
        element->magneticPotential = UnknownsVector[element->getIndex()];
    }

    // Set MMF forces (setting the coil currents)
    for (auto component : FixedBlock.CoilComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->getMeshCurrentSource()->readPulse();
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            double current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += polarities[k] * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
    }
    for (auto MMFSource : FixedBlock.MMFSources)
        MMFSource->setCurrent();

    for (auto component : MovableBlock.CoilComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->getMeshCurrentSource()->readPulse();
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            double current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += polarities[k] * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
    }
    for (auto MMFSource : MovableBlock.MMFSources)
        MMFSource->setCurrent();

    // Set magnetic fluxes
    for (auto element : FixedBlock.NonlinearReluctanceElements) {
        element->computeMagneticFluxes();
        element->computeMagneticFluxDensity();
    }
    for (auto element : MovableBlock.NonlinearReluctanceElements) {
        element->computeMagneticFluxes();
        element->computeMagneticFluxDensity();
    }
}

void MD_Project::postProcessing()
{
    setMagneticScalarPotentials(FixedBlock);
    setElectricCurrentsAndMMFs(FixedBlock);
    setVoltages(FixedBlock);
    computeIronLosses(FixedBlock);
    computePermanentMagnetLosses(FixedBlock);

    setMagneticScalarPotentials(MovableBlock);
    setElectricCurrentsAndMMFs(MovableBlock);
    setVoltages(MovableBlock);
    computeTorque(MovableBlock);
    computeIronLosses(MovableBlock);
    computePermanentMagnetLosses(MovableBlock);
}

void MD_Project::setPreviousValues()
{
    // Set currents for the electric circuit components
    for (auto voltageSource : FixedBlock.VoltageSourceComponents) {
        voltageSource->setCurrentPrevious();
    }
    for (auto passiveComponent : FixedBlock.PassiveComponents) {
        passiveComponent->setCurrentPrevious();
    }
    for (auto coilComponent : FixedBlock.CoilComponents) {
        coilComponent->setCurrentPrevious();
    }
    for (auto coil : FixedBlock.CoilComponents) {
        coil->setLinkageFluxPrevious();
    }

    for (auto voltageSource : MovableBlock.VoltageSourceComponents) {
        voltageSource->setCurrentPrevious();
    }
    for (auto passiveComponent : MovableBlock.PassiveComponents) {
        passiveComponent->setCurrentPrevious();
    }
    for (auto coilComponent : MovableBlock.CoilComponents) {
        coilComponent->setCurrentPrevious();
    }
    for (auto coil : MovableBlock.CoilComponents) {
        coil->setLinkageFluxPrevious();
    }

    for (auto element : FixedBlock.ReluctanceElements) {
        element->magneticFluxDensityPrevious = element->magneticFluxDensity;
    }
    for (auto element : MovableBlock.ReluctanceElements) {
        element->magneticFluxDensityPrevious = element->magneticFluxDensity;
    }
}

void MD_Project::setMagneticScalarPotentials(MD_ElectromagneticBlock &block)
{
    for (auto element : block.ReluctanceElements)
        element->magneticPotential = UnknownsVector[element->getIndex()];
}

void MD_Project::setElectricCurrentsAndMMFs(MD_ElectromagneticBlock &block)
{
    for (auto component : block.VoltageSourceComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->getMeshCurrentSource()->readPulse();
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            double current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += polarities[k] * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
        component->Voltage = component->readPulse();
    }
    for (auto component : block.CurrentSourceComponents) {
        component->Current = component->readPulse();
    }
    for (auto component : block.PassiveComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->getMeshCurrentSource()->readPulse();
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            double current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += polarities[k] * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
        double voltageDueToResitance = component->getResistance() * component->Current;
        double voltageDueToInductance = component->getInductance() * (component->Current - component->CurrentPrevious) / TimeStep;
        component->Voltage = voltageDueToResitance + voltageDueToInductance;
    }
    for (auto component : block.CoilComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->getMeshCurrentSource()->readPulse();
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            double current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += polarities[k] * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
        double voltageDueToResitance = component->getResistance() * component->Current;
        double voltageDueToInductance = component->getInductance() * (component->Current - component->CurrentPrevious) / TimeStep;
        component->Voltage = voltageDueToResitance + voltageDueToInductance;
    }
    for (auto MMFSource : block.MMFSources)
        MMFSource->setCurrent();
    for (auto element : block.ReluctanceElements) {
        element->computeMagneticFluxes();
        element->computeMagneticFluxDensity();
    }
}

void MD_Project::setVoltages(MD_ElectromagneticBlock &block)
{
    for (auto component : block.CurrentSourceComponents)
        component->Voltage = 0.0;
    for (auto component : block.VoltageSourceComponents)
        component->Voltage = component->readPulse();
    for (auto component : block.PassiveComponents) {
        double voltageDueToResitance = component->getResistance() * component->Current;
        double voltageDueToInductance = component->getInductance() * (component->Current - component->CurrentPrevious) / TimeStep;
        component->Voltage = voltageDueToResitance + voltageDueToInductance;
        if ( component->belongsToCurrentSourceMesh() )
            component->getMeshCurrentSource()->Voltage = -component->Voltage;
    }
    for (auto component : block.CoilComponents) {
        double voltageDueToResitance = component->getResistance() * component->Current;
        double voltageDueToInductance = component->getInductance() * (component->Current - component->CurrentPrevious) / TimeStep;
        component->computeLinkageFlux(TimeStep);
        component->computeVoltageCoilInduced();
        component->VoltageCoilInduced *= Periodicities;
        component->Voltage = voltageDueToResitance + voltageDueToInductance + component->VoltageCoilInduced;
        if ( component->belongsToCurrentSourceMesh() )
            component->getMeshCurrentSource()->Voltage = -component->Voltage;
    }
}

void MD_Project::computeTorque(MD_ElectromagneticBlock &block)
{
    if (!block.AirgapLayerElements.empty()) {
        double force = 0.0;
        for (auto element : block.AirgapLayerElements) {
            element->computeTangentialForce();
            force += element->forceTX;
        }
        block.TangentialForce = force * block.AirgapLayerElements.front()->getGeometry().position_rY;
    }
    block.TangentialForce *= Periodicities;
}

void MD_Project::computeIronLosses(MD_ElectromagneticBlock &block)
{
    block.IronLosses = 0.0;
    for (auto element : block.NonlinearReluctanceElements)
        block.IronLosses += block.MaterialNonLinear.getIronLosses(element->magneticFluxDensityPrevious, element->magneticFluxDensity, TimeStep, element->getGeometry().volume);
    block.IronLosses *= Periodicities;
}

void MD_Project::computePermanentMagnetLosses(MD_ElectromagneticBlock &block)
{
    block.PermanentMagnetLosses = 0.0;
    for (auto element : block.MagnetElements) {
        double width = element->getGeometry().width;
        double resistivity = 1.4E-6;
        double losses = (( width * width ) / ( resistivity )) * std::pow( (element->magneticFluxDensity - element->magneticFluxDensityPrevious) / TimeStep , 2.0);
        block.PermanentMagnetLosses += losses * element->getGeometry().volume;
    }
    block.PermanentMagnetLosses *= Periodicities;
}

void MD_Project::assembleProject()
{
    // Assemble the global system of equations
    assembleMagneticCircuitDomain(FixedBlock);
    assembleMagneticCircuitDomain(MovableBlock);
    assembleElectricCircuitDomain(FixedBlock);
    assembleElectricCircuitDomain(MovableBlock);
}

void MD_Project::assembleMagneticCircuitDomain(MD_ElectromagneticBlock block)
{
    for (auto element : block.ReluctanceElements)
        element->computePermeance();
    for (auto element : block.ReluctanceElements) {
        element->computeFluxEquations();
        SystemOfEquations[element->getIndex()] = element->getFluxBalanceEquation();
    }
}

void MD_Project::assembleElectricCircuitDomain(MD_ElectromagneticBlock block)
{
    // Read circuits
    for (auto circuit : block.ElectricCircuits) {
        MEC_RealEquation equation;
        if ( circuit.currentSourceMesh ) {
            for (auto component : circuit.components) {
                if ( component->getType() == MDECCT_coil) {
                    double current = component->getMeshCurrentSource()->readPulse(); // current source value
                    for (auto coilSide : component->CoilSides) {
                        for (auto MMFSource : coilSide.MMFSources) {
                            MMFSource->setMMF(current); // Update MMF sources
                        }
                    }
                }
            }
        } else {
            for (unsigned componentIndex = 0; componentIndex < circuit.getSize(); ++componentIndex) {
                MD_ElectricCircuitComponent* component = circuit.components[componentIndex];
                // Impedance (time derivative)
                if (component->getResistance() != 0.0 || component->getInductance() != 0.0) {
                    std::vector<unsigned> meshIndexes = component->getMeshIndexes();
                    std::vector<int> meshPolarities = component->getMeshPolarities();
                    int circuitPolarity = circuit.polarities[componentIndex];
                    double previousValue, currentValue;
                    // Resistance
                    for (unsigned i = 0; i < meshIndexes.size(); ++i) {
                        currentValue = component->getResistance();
                        if (meshPolarities[i] == circuitPolarity)
                            equation.addCoefficient(meshIndexes[i], currentValue);
                        else
                            equation.addCoefficient(meshIndexes[i], -currentValue);
                    }
                    // Inductance (time derivative)
                    for (unsigned i = 0; i < meshIndexes.size(); ++i) {
                        double currentValue = component->getInductance() / TimeStep;
                        if (meshPolarities[i] == circuitPolarity)
                            equation.addCoefficient(meshIndexes[i], currentValue);
                        else
                            equation.addCoefficient(meshIndexes[i], -currentValue);
                    }
                    previousValue = component->CurrentPrevious * component->getInductance() / TimeStep;
                    equation.addConstant(-previousValue * circuit.polarities[componentIndex]);
                }

                // Voltage source
                if (component->getType() == MDECCT_voltageSource) {
                    equation.addConstant(component->readPulse() * circuit.polarities[componentIndex]);
                }

                // Coil - back EMF (time derivative)
                if (component->getType() == MDECCT_coil) {
                    std::vector<MD_CoilSide> coilSides = component->getCoilSides();
                    for (unsigned coilSideIndex = 0; coilSideIndex < coilSides.size(); ++coilSideIndex) {
                        MD_CoilSide coilSide = coilSides[coilSideIndex];
                        for (unsigned currentElementIndex = 0; currentElementIndex < coilSide.activeReluctanceElements.size(); ++currentElementIndex) {
                            MD_ReluctanceElement* element = coilSide.activeReluctanceElements[currentElementIndex];
                            double EMF_Constant = -circuit.polarities[componentIndex] * coilSide.Polarity * coilSide.Turns * element->getGeometry().crossSectionalArea / (coilSide.Area * TimeStep);
                            equation = equation << (element->getTangentionalFluxEquation() * (EMF_Constant / 2.0));
                            for (unsigned otherElementIndex = currentElementIndex + 1; otherElementIndex < coilSide.activeReluctanceElements.size(); ++otherElementIndex) {
                                element = coilSide.activeReluctanceElements[otherElementIndex];
                                equation = equation << (element->getTangentionalFluxEquation() * (EMF_Constant));
                            }
                            for (unsigned passiveElementIndex = 0; passiveElementIndex < coilSide.passiveReluctanceElements.size(); ++passiveElementIndex) {
                                element = coilSide.passiveReluctanceElements[passiveElementIndex];
                                equation = equation << (element->getTangentionalFluxEquation() * (EMF_Constant));
                            }
                        }
                    }
                    equation.addConstant(-component->LinkageFluxPrevious * circuit.polarities[componentIndex]);
                }
            }
            SystemOfEquations[circuit.getIndex()] = equation;
        }
    }
}

void MD_Project::updateSignals()
{
    for (auto signal : FixedBlock.VoltageSourceComponents)
        signal->nextPulse();
    for (auto signal : FixedBlock.CurrentSourceComponents)
        signal->nextPulse();

    for (auto signal : MovableBlock.VoltageSourceComponents)
        signal->nextPulse();
    for (auto signal : MovableBlock.CurrentSourceComponents)
        signal->nextPulse();
}

void MD_Project::updateMechanics()
{
    if ( MovableBlock.Speed_tX > 0.0 ) {
        MovableBlock.Position_tX += MovableBlock.Speed_tX * TimeStep;
    }
}

void MD_Project::saveData(std::vector<double> &data)
{
    data[0] = FixedBlock.CurrentSourceComponents[0]->Voltage;
    data[1] = FixedBlock.CurrentSourceComponents[1]->Voltage;
    data[2] = FixedBlock.CurrentSourceComponents[2]->Voltage;
    data[3] = FixedBlock.CurrentSourceComponents[0]->Current;
    data[4] = FixedBlock.CurrentSourceComponents[1]->Current;
    data[5] = FixedBlock.CurrentSourceComponents[2]->Current;
    data[6] = FixedBlock.IronLosses;
    data[7] = MovableBlock.IronLosses;
    data[8] = MovableBlock.PermanentMagnetLosses;
}

int MD_Project::SuperLU(std::vector<MEC_RealEquation> equations, std::vector<double> &solutions)
{
    int info = 0;

    SuperLUData.setSystemOfEquations(equations);

    if ( SuperLUData.samePattern() )
        info = SuperLUData.repeatSolver(solutions);
    else {
        SuperLUData.close();
        info = SuperLUData.initSolver(solutions);
    }

    return info;
}

unsigned MD_Project::initiateMagneticScalarPotentialIndexes(MD_ElectromagneticBlock &block, unsigned offset)
{
    unsigned i;
    for (i = 0; i < block.ReluctanceElements.size(); ++i)
        block.ReluctanceElements[i]->setIndex(i + offset);
    return i + offset;
}

unsigned MD_Project::initiateMeshCurrentIndexes(MD_ElectromagneticBlock &block, unsigned offset)
{
    unsigned equations = 0;

    for (unsigned i = 0; i < block.ElectricCircuits.size(); ++i) {
        if ( block.ElectricCircuits[i].currentSourceMesh ) { // current source mesh
            for (unsigned j = 0; j < block.ElectricCircuits[i].components.size(); ++j)
                block.ElectricCircuits[i].components[j]->addMeshCurrent(i, block.ElectricCircuits[i].polarities[j]);
            block.ElectricCircuits[i].setIndex(0);
        } else { // non current source mesh
            for (unsigned j = 0; j < block.ElectricCircuits[i].components.size(); ++j)
                block.ElectricCircuits[i].components[j]->addMeshCurrent(equations + offset, block.ElectricCircuits[i].polarities[j]);
            block.ElectricCircuits[i].setIndex(equations + offset);
            ++equations;
        }
    }
    for (auto circuit : block.ElectricCircuits) {
        for (auto currentSource : circuit.components) {
            if (currentSource->getType() == MDECCT_currentSource) {
                for (auto component : circuit.components) {
                    if (component == currentSource) continue;
                    if (currentSource->getMeshIndexes() == component->getMeshIndexes())
                        component->setMeshCurrentSource(currentSource);
                }
            }
        }
    }
    return equations + offset;
}

void MD_Project::initiateMMFSources(MD_ElectromagneticBlock &block)
{
    // MMF sources due to permanent magnets
    for (unsigned element = 0; element < block.MagnetElements.size(); ++element) {
        MD_ReluctanceElement* magnet = block.MagnetElements[element];
        MD_MMFSource* MMFSource = new MD_MMFSource;
        MMFSource->CoilComponent = nullptr;
        MMFSource->ReluctanceElement = magnet;
        MMFSource->Orientation = magnet->getMagnetOrientation();
        MMFSource->Polarity = magnet->getMagnetPolarity();
        MMFSource->ConstantMMF = 0.0;
        MMFSource->MMF = magnet->getMagnetMMFSource() * MMFSource->Polarity;
        MMFSource->ConstantSource = true;
        block.MMFSources.push_back(MMFSource);
        MMFSource->ReluctanceElement->addMMFSource(MMFSource);
    }

    // MMF sources due to coil components
    for (unsigned component = 0; component < block.CoilComponents.size(); ++component) {
        MD_ElectricCircuitComponent* coilComponent = block.CoilComponents[component];
        for (unsigned coilSide = 0; coilSide < coilComponent->CoilSides.size(); ++coilSide) {
            double totalTurns = coilComponent->CoilSides[coilSide].Turns;
            double coilSideArea = coilComponent->CoilSides[coilSide].Area;
            double MMFConstant = 0.0;
            for (unsigned element = 0; element < coilComponent->CoilSides[coilSide].activeReluctanceElements.size(); ++element) {
                MD_MMFSource* MMFSource = new MD_MMFSource;
                MMFSource->CoilComponent = coilComponent;
                MMFSource->ReluctanceElement = coilComponent->CoilSides[coilSide].activeReluctanceElements[element];
                MMFSource->MeshIndexes = coilComponent->getMeshIndexes();
                MMFSource->MeshPolarities = coilComponent->getMeshPolarities();
                MMFSource->Orientation = MMFSO_tangential;
                MMFSource->Polarity = coilComponent->CoilSides[coilSide].Polarity;
                double deltaMMFConstant = totalTurns * (MMFSource->ReluctanceElement->getGeometry().crossSectionalArea / coilSideArea);
                MMFSource->ConstantMMF = MMFSource->Polarity * (MMFConstant + (0.5 * deltaMMFConstant));
                MMFConstant += deltaMMFConstant;
                block.MMFSources.push_back(MMFSource);
                coilComponent->CoilSides[coilSide].MMFSources.push_back(MMFSource);
                MMFSource->ReluctanceElement->addMMFSource(MMFSource);
                if ( coilComponent->belongsToCurrentSourceMesh() ) {
                    MMFSource->ConstantSource = true;
                } else {
                    MMFSource->ConstantSource = false;
                }
            }
            for (unsigned element = 0; element < coilComponent->CoilSides[coilSide].passiveReluctanceElements.size(); ++element) {
                MD_MMFSource* MMFSource = new MD_MMFSource;
                MMFSource->CoilComponent = coilComponent;
                MMFSource->ReluctanceElement = coilComponent->CoilSides[coilSide].passiveReluctanceElements[element];
                MMFSource->MeshIndexes = coilComponent->getMeshIndexes();
                MMFSource->MeshPolarities = coilComponent->getMeshPolarities();
                MMFSource->Orientation = MMFSO_tangential;
                MMFSource->Polarity = coilComponent->CoilSides[coilSide].Polarity;
                MMFSource->ConstantMMF = MMFSource->Polarity * MMFConstant;
                block.MMFSources.push_back(MMFSource);
                coilComponent->CoilSides[coilSide].MMFSources.push_back(MMFSource);
                MMFSource->ReluctanceElement->addMMFSource(MMFSource);
                if ( coilComponent->belongsToCurrentSourceMesh() ) {
                    MMFSource->ConstantSource = true;
                } else {
                    MMFSource->ConstantSource = false;
                }
            }
        }
    }
}
