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

#include "MEC_time_harmonic.h"

MS_ReluctanceElement::MS_ReluctanceElement()
{
    initialize();
}

MS_ReluctanceElement::MS_ReluctanceElement(ReluctanceElementType t, MEC_ElementGeometry g, double v, std::string text)
{
    initialize();

    type = t;
    label = text;

    setGeometry(g);
    setReluctivity(v);
}

void MS_ReluctanceElement::setGeometry(MEC_ElementGeometry g)
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

void MS_ReluctanceElement::setReluctivity(double v)
{
    reluctivity = v;
    computeReluctances();
}

void MS_ReluctanceElement::addAirgapPermeanceDown(MS_ReluctanceElement *element, double permenace)
{
    airgapElementsDown.push_back(element);
    airgapReluctanceDown.push_back(2.0 / permenace);
}

void MS_ReluctanceElement::addAirgapPermeanceUp(MS_ReluctanceElement *element, double permenace)
{
    airgapElementsUp.push_back(element);
    airgapReluctanceUp.push_back(2.0 / permenace);
}

void MS_ReluctanceElement::clearAirgapPermeances()
{
    airgapElementsDown.clear();
    airgapElementsUp.clear();
    airgapReluctanceDown.clear();
    airgapReluctanceUp.clear();
    airgapPermeanceDown.clear();
    airgapPermeanceUp.clear();
}

void MS_ReluctanceElement::updateReluctivity()
{
    reluctivity = nonlinearMaterial.getReluctivity(magneticFluxDensity);
    computeReluctances();
}

void MS_ReluctanceElement::addMMFSource(MS_MMFSource *source)
{
    MMFSources.push_back(source);
}

void MS_ReluctanceElement::connect(MS_ReluctanceElement *left, MS_ReluctanceElement *up, MS_ReluctanceElement *right, MS_ReluctanceElement *down)
{
    elementLeft = left;
    elementRight = right;
    elementUp = up;
    elementDown = down;
}

void MS_ReluctanceElement::computePermeance()
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

void MS_ReluctanceElement::computeFluxEquations()
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
                    std::complex<double> value = 0.5 * permeanceLeft * source->MMF;
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
                    std::complex<double> value = 0.5 * permeanceLeft * source->MMF;
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
                    std::complex<double> value = -0.5 * permeanceRight * source->MMF;
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
                    std::complex<double> value = -0.5 * permeanceRight * source->MMF;
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
                    std::complex<double> value = 0.5 * permeanceDown * source->MMF;
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
                    std::complex<double> value = 0.5 * permeanceDown * source->MMF;
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
                    std::complex<double> value = -0.5 * permeanceUp * source->MMF;
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
                    std::complex<double> value = -0.5 * permeanceUp * source->MMF;
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

MEC_ComplexEquation MS_ReluctanceElement::getFluxBalanceEquation()
{
    return EquationMagneticFluxBalance;
}

MEC_ComplexEquation MS_ReluctanceElement::getTangentionalFluxEquation()
{
    return EquationMagneticFluxTangential;
}

MEC_ComplexEquation MS_ReluctanceElement::getRadialFluxEquation()
{
    return EquationMagneticFluxRadial;
}

void MS_ReluctanceElement::computeMagneticFluxes()
{
    std::complex<double> flux;

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

double MS_ReluctanceElement::computeMagneticFluxDensity()
{
    double BB_l = std::abs(magneticFluxLeft) / geometry.crossSectionalAreaTangential;
    double BB_r = std::abs(magneticFluxRight) / geometry.crossSectionalAreaTangential;
    double BB_u = std::abs(magneticFluxUp) / geometry.crossSectionalAreaRadial;
    double BB_d = std::abs(magneticFluxDown) / geometry.crossSectionalAreaRadial;

    BB_l *= BB_l; BB_r *= BB_r; BB_u *= BB_u; BB_d *= BB_d;

    magneticFluxDensityTangential = magneticFluxTangential / geometry.crossSectionalAreaTangential;
    magneticFluxDensityRadial = magneticFluxRadial / geometry.crossSectionalAreaRadial;
    magneticFluxDensity = std::sqrt((BB_l + BB_r + BB_u + BB_d) / 2.0);

    return magneticFluxDensity;
}

double MS_ReluctanceElement::computeEnergy()
{
    energy = 0.5 * reluctivity * magneticFluxDensity * magneticFluxDensity;

    return energy;
}

double MS_ReluctanceElement::computeTangentialForce()
{
    double sigma = std::abs(magneticFluxDensityRadial * magneticFluxDensityTangential) * std::cos(std::arg(magneticFluxDensityRadial) - std::arg(magneticFluxDensityTangential));

    forceTX = geometry.length * geometry.width * sigma / (U_AIR);

    return forceTX;
}

double MS_ReluctanceElement::computeRadialForce()
{
    forceRY = 0.0;

    return forceRY;
}

void MS_ReluctanceElement::initialize()
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
}

void MS_ReluctanceElement::computeReluctances()
{
    reluctanceRadial = reluctivity * constantReluctanceRadial;
    reluctanceTangential = reluctivity * constantReluctanceTangential;
}

MS_ElectricCircuitComponent::MS_ElectricCircuitComponent()
{
    Type = MSECCT_notDefined;
    Resistance = 0.0;
    Inductance = 0.0;
    ActivePower = 0.0;
    ReactivePower = 0.0;
    ApparentPower = 0.0;
    JouleLosses = 0.0;
    Current = 0.0;
    Voltage = 0.0;
    VoltageCoilInduced = 0.0;
    MeshCurrentSource = nullptr;
}

void MS_ElectricCircuitComponent::setVoltageSource(double frequency, double rms, double phase, std::string text, double resistance, double inductance)
{
    Type = MSECCT_voltageSource;
    Frequency = frequency;
    SignalRMS = rms;
    SignalPhase = phase;
    Signal = std::complex<double>(rms*std::cos(phase), rms*std::sin(phase));
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
    Impedance = std::complex<double>(resistance, M_2PI*frequency*inductance);
}

void MS_ElectricCircuitComponent::setCurrentSource(double frequency, double rms, double phase, std::string text, double resistance, double inductance)
{
    Type = MSECCT_currentSource;
    Frequency = frequency;
    SignalRMS = rms;
    SignalPhase = phase;
    Signal = std::complex<double>(rms*std::cos(phase), rms*std::sin(phase));
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
    Impedance = std::complex<double>(resistance, M_2PI*frequency*inductance);
}

void MS_ElectricCircuitComponent::setImpedance(double frequency, double resistance, double inductance, std::string text)
{
    Type = MSECCT_impedance;
    Frequency = frequency;
    SignalRMS = 0.0;
    SignalPhase = 0.0;
    Signal = 0.0;
    Label = text;
    Resistance = resistance;
    Inductance = inductance;
    Impedance = std::complex<double>(resistance, M_2PI*frequency*inductance);
}

void MS_ElectricCircuitComponent::setCoil(double frequency, std::string text, double resistance, double inductance)
{
    Type = MSECCT_coil;
    Label = text;
    Frequency = frequency;
    Resistance = resistance;
    Inductance = inductance;
    Impedance = std::complex<double>(resistance, M_2PI*frequency*inductance);
}

void MS_ElectricCircuitComponent::setCoil(double frequency, MS_CoilSide coil_side, std::string text, double resistance, double inductance)
{
    Type = MSECCT_coil;
    Label = text;
    Frequency = frequency;
    Resistance = resistance;
    Inductance = inductance;
    Impedance = std::complex<double>(resistance, M_2PI*frequency*inductance);
    CoilSides.clear();
    CoilSides.push_back(coil_side);
}

void MS_ElectricCircuitComponent::setCoil(double frequency, std::vector<MS_CoilSide> coil_sides, std::string text, double resistance, double inductance)
{
    Type = MSECCT_coil;
    Label = text;
    Frequency = frequency;
    Resistance = resistance;
    Inductance = inductance;
    Impedance = std::complex<double>(resistance, M_2PI*frequency*inductance);
    CoilSides = coil_sides;
}

void MS_ElectricCircuitComponent::setFrequency(double frequency)
{
    Frequency = frequency;
    Impedance = std::complex<double>(Resistance, M_2PI*frequency*Inductance);
}

double MS_ElectricCircuitComponent::computeActivePower()
{
    ActivePower = std::abs(Voltage * Current) * std::cos(std::arg(Voltage) - std::arg(Current));

    return ActivePower;
}

double MS_ElectricCircuitComponent::computeReactivePower()
{
    ReactivePower = std::abs(Voltage * Current) * std::sin(std::arg(Voltage) - std::arg(Current));

    return ReactivePower;

}

double MS_ElectricCircuitComponent::computeApparentPower()
{
    ApparentPower = std::abs(Voltage * Current);

    return ApparentPower;
}

double MS_ElectricCircuitComponent::computeJouleLosses()
{
    double I = std::abs(Current);
    JouleLosses = Resistance * I * I;

    return JouleLosses;
}

std::complex<double> MS_ElectricCircuitComponent::computeVoltageCoilInduced()
{
    VoltageCoilInduced = 0.0;
    for (unsigned coilSideIndex = 0; coilSideIndex < CoilSides.size(); ++coilSideIndex) {
        MS_CoilSide coilSide = CoilSides[coilSideIndex];
        for (unsigned currentElementIndex = 0; currentElementIndex < coilSide.activeReluctanceElements.size(); ++currentElementIndex) {
            MS_ReluctanceElement* element = coilSide.activeReluctanceElements[currentElementIndex];
            std::complex<double> EMF_Constant(0.0,-M_2PI * Frequency * coilSide.Polarity * coilSide.Turns * element->getGeometry().crossSectionalArea / coilSide.Area);
            VoltageCoilInduced += element->magneticFluxTangential * (EMF_Constant / 2.0);
            for (unsigned otherElementIndex = currentElementIndex + 1; otherElementIndex < coilSide.activeReluctanceElements.size(); ++otherElementIndex) {
                element = coilSide.activeReluctanceElements[otherElementIndex];
                VoltageCoilInduced += element->magneticFluxTangential * EMF_Constant;
            }
            for (unsigned passiveElementIndex = 0; passiveElementIndex < coilSide.passiveReluctanceElements.size(); ++passiveElementIndex) {
                element = coilSide.passiveReluctanceElements[passiveElementIndex];
                VoltageCoilInduced += element->magneticFluxTangential * EMF_Constant;
            }
        }
    }

    return VoltageCoilInduced;
}

void MS_ElectricCircuit::addComponent(MS_ElectricCircuitComponent *component, int polarity)
{
    if(component->getType() == MSECCT_currentSource)
        currentSourceMesh = true;
    components.push_back(component);
    polarities.push_back(polarity);
}

MS_MMFSource::MS_MMFSource()
{
    ConstantSource = false;
    Polarity = 0;
    ConstantMMF = 0.0;
    Current = 0.0;
    MMF = 0.0;
    ReluctanceElement = nullptr;
    CoilComponent = nullptr;
}

void MS_MMFSource::setPolarity(int polarity)
{
    Polarity = polarity;
}

void MS_MMFSource::setCurrent()
{
    if ( ConstantSource ) return;
    Current = CoilComponent->Current;
    MMF = ConstantMMF * Current;
}

void MS_MMFSource::setMMF(std::complex<double> current)
{
    Current = current;
    MMF = ConstantMMF * Current;
}

MS_AirgapConnection::MS_AirgapConnection()
{
    Permeance = 0.0;
    PermeanceMaximum = 0.0;
    FixedElement = nullptr;
    MovableElement = nullptr;
}

MS_AirgapConnection::MS_AirgapConnection(MS_ReluctanceElement *fixedElement, MS_ReluctanceElement *movableElement)
{
    Permeance = 0.0;
    PermeanceMaximum = 0.0;
    FixedElement = fixedElement;
    MovableElement = movableElement;
    FixedElementGeometry = fixedElement->getGeometry();
    MovableElementGeometry = movableElement->getGeometry();
    computeMaximumPermeance();
}

void MS_AirgapConnection::setElements(MS_ReluctanceElement *fixedElement, MS_ReluctanceElement *movableElement)
{
    Permeance = 0.0;
    PermeanceMaximum = 0.0;
    FixedElement = fixedElement;
    MovableElement = movableElement;
    FixedElementGeometry = fixedElement->getGeometry();
    MovableElementGeometry = movableElement->getGeometry();
    computeMaximumPermeance();
}

double MS_AirgapConnection::computePermeance(double movableBlockAngularPosition, double lowerEnd, double upperEnd)
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
    if (displacement < theta_m)
        Permeance = PermeanceMaximum;
    else {
        Permeance = PermeanceMaximum * std::exp(-std::pow(1.8*(displacement - theta_m) / (theta_z - theta_m), (2.0)));
        if (Permeance < (PermeanceMaximum * 0.00001))
            Permeance = 0.0;
    }
    return Permeance;
}

void MS_AirgapConnection::computeMaximumPermeance()
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

MS_AirgapStructure::MS_AirgapStructure()
{
    Periodicities = 1;
}

void MS_AirgapStructure::setConnectionElements(std::vector<MS_ReluctanceElement*> fixedBlockElements, std::vector<MS_ReluctanceElement*> movableBlockElements)
{
    connectionElements_fixedBlock = fixedBlockElements;
    connectionElements_movableBlock = movableBlockElements;

    createAirgapElements();
}

void MS_AirgapStructure::setAirgapPermeances(double tangentialPosition)
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

void MS_AirgapStructure::createAirgapElements()
{
    airgapDirectConnectionElements.resize(connectionElements_fixedBlock.size());
    for (unsigned fixedElement = 0; fixedElement < connectionElements_fixedBlock.size(); ++fixedElement) {
        airgapDirectConnectionElements[fixedElement].resize(connectionElements_movableBlock.size());
        for (unsigned movableElement = 0; movableElement < connectionElements_movableBlock.size(); ++movableElement) {
            airgapDirectConnectionElements[fixedElement][movableElement].setElements(connectionElements_fixedBlock[fixedElement], connectionElements_movableBlock[movableElement]);
        }
    }
}

void MS_AirgapStructure::computeAirgapPermeances(double rotorAngularPosition)
{
    double minPeriodicity = 0.0;
    double maxPeriodicity = M_2PI / Periodicities;
    currentlyConnectedMovableBlockElements.resize(connectionElements_fixedBlock.size());
    currentlyConnectedPermeances.resize(connectionElements_fixedBlock.size());
    for (unsigned fixedElement = 0; fixedElement < connectionElements_fixedBlock.size(); ++fixedElement) {
        airgapDirectConnectionElements[fixedElement].resize(connectionElements_movableBlock.size());
        std::vector<MS_ReluctanceElement*> connectedMovableBlockElements;
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

MS_ElectromagneticBlock::MS_ElectromagneticBlock()
{
    Length = 0.0;
    Frequency = 0.0;
    Position_tX = 0.0;
    Position_rY = 0.0;
    Speed_tX = 0.0;
    Speed_rY = 0.0;
    TangentialElementsCount = 0.0;
    RadialElementsCount = 0.0;
    Slices = 0.0;

    TangentialForce = 0.0;
    RadialForce = 0.0;
    WindingJouleLosses = 0.0;
    IronLosses = 0.0;
    MagneticEnergy = 0.0;
    Weight = 0.0;
    Volume = 0.0;
}

void MS_ElectromagneticBlock::computeDensity()
{
    Weight = 0.0; Volume = 0.0;
    for (auto element : ReluctanceElements) {
        double volume = element->getGeometry().volume;
        Volume += volume;
        switch (element->getType()) {
        case RET_coreNonLinear:
            Weight += volume * MaterialNonLinear.getDensity();
            break;
        case RET_coil:
            Weight += volume * WindingDensity;
            break;
        default:
            break;
        }
    }
}

void MS_ElectromagneticBlock::computeTorque()
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

MS_Project::MS_Project()
{
    UnknownsCount = 0;
    isNonlinear = false;
    isSolved = false;
    Periodicities = 1;
}

MS_Project::MS_Project(MS_ElectromagneticBlock fixed)
{
    UnknownsCount = 0;
    FixedBlock = fixed;
    isNonlinear = false;
    isSolved = false;
    Periodicities = 1;

    preProcessing();
}

MS_Project::MS_Project(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable)
{
    UnknownsCount = 0;
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;
    Periodicities = 1;

    preProcessing();
}

MS_Project::MS_Project(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable, int periodicity)
{
    UnknownsCount = 0;
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;
    Periodicities = periodicity;

    preProcessing();
}

MS_Project::~MS_Project()
{

}

void MS_Project::destroyProject()
{
    destroyBlock(FixedBlock);
    destroyBlock(MovableBlock);
    SuperLUData.close();
    SystemOfEquations.clear();
    UnknownsVector.clear();
    isSolved = false;
}

void MS_Project::loadProject(MS_ElectromagneticBlock fixed)
{
    FixedBlock = fixed;
    isNonlinear = false;
    isSolved = false;

    preProcessing();
}

void MS_Project::loadProject(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable)
{
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;

    preProcessing();
}

void MS_Project::loadProject(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable, int periodicity)
{
    FixedBlock = fixed;
    MovableBlock = movable;
    isNonlinear = false;
    isSolved = false;
    Periodicities = periodicity;

    preProcessing();
}

void MS_Project::updateMovableBlockFrequency(double frequency)
{
    MovableBlock.Frequency = frequency;
    for (auto component : MovableBlock.CoilComponents) component->setFrequency(frequency);
    for (auto component : MovableBlock.PassiveComponents) component->setFrequency(frequency);
    for (auto component : MovableBlock.VoltageSourceComponents) component->setFrequency(frequency);
    for (auto component : MovableBlock.CurrentSourceComponents) component->setFrequency(frequency);
}

ReportDataSolver MS_Project::solve(double relaxation, unsigned iterations, double error)
{
    isSolved = false;

    ReportDataSolver solverReport;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::vector<double> prevBFixed(FixedBlock.NonlinearReluctanceElements.size(), 0.0);
    std::vector<double> prevBMovable(MovableBlock.NonlinearReluctanceElements.size(), 0.0);
    // Initiate the global system of equations
    SystemOfEquations.clear();
    UnknownsVector.clear();
    SystemOfEquations.resize(UnknownsCount);
    UnknownsVector.resize(UnknownsCount);

    airgapStructure.setAirgapPermeances(MovableBlock.Position_tX);
    solverReport.data.resize(8);
    // Create/update airgap structure
    if (isNonlinear) {
        double variationMax = error + 1.0;
        unsigned iteration = 0;
        while ((variationMax > error) && (iteration++ < iterations)) {
            variationMax = 0.0;
            assembleProject();
            if (SuperLU(SystemOfEquations, UnknownsVector) != 0) {
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                solverReport.elapsedTime =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                solverReport.variance = 0.0;
                solverReport.iterations = 0;
                solverReport.solverError = true;
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
                if (variation > variationMax)
                    variationMax = variation;
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
                if (variation > variationMax)
                    variationMax = variation;
                MovableBlock.NonlinearReluctanceElements[i]->setReluctivity(oldReluctivity + (relaxation * reluctivityDifferetial));
                prevBMovable[i] = MovableBlock.NonlinearReluctanceElements[i]->magneticFluxDensity;
            }
        }
        postProcessing();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        solverReport.elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        solverReport.variance = variationMax;
        solverReport.iterations = iteration;
        solverReport.solverError = false;
        saveData(solverReport.data);
    } else {
        assembleProject();
        if (SuperLU(SystemOfEquations, UnknownsVector) != 0) {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            solverReport.elapsedTime =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            solverReport.variance = 0.0;
            solverReport.iterations = 0;
            solverReport.solverError = true;
            return solverReport;
        }
        postProcessing();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        solverReport.elapsedTime =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        solverReport.variance = 0.0;
        solverReport.iterations = 1;
        solverReport.solverError = false;
    }

    return solverReport;
}

void MS_Project::destroyBlock(MS_ElectromagneticBlock &block)
{
    // variables
    block.Length = 0.0;
    block.Position_tX = 0.0;
    block.Position_rY = 0.0;
    block.Speed_tX = 0.0;
    block.Speed_rY = 0.0;
    block.TangentialElementsCount = 0;
    block.RadialElementsCount = 0;
    block.Slices = 0;
    block.TangentialForce = 0.0;
    block.RadialForce = 0.0;
    block.WindingJouleLosses = 0.0;
    block.IronLosses = 0.0;
    block.MagneticEnergy = 0.0;
    block.Frequency = 0.0;
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

void MS_Project::preProcessing()
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

void MS_Project::fastPostProcessing()
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
            component->Current = component->Signal;
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            std::complex<double> current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += static_cast<double>(polarities[k]) * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
    }
    for (auto MMFSource : FixedBlock.MMFSources)
        MMFSource->setCurrent();

    for (auto component : MovableBlock.CoilComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->Signal;
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            std::complex<double> current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += static_cast<double>(polarities[k]) * UnknownsVector[indexes[k]];
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

void MS_Project::postProcessing()
{
    setMagneticScalarPotentials(FixedBlock);
    setElectricCurrentsAndMMFs(FixedBlock);
    setVoltages(FixedBlock);
    computeWindingJouleLosses(FixedBlock);
    computeIronLosses(FixedBlock);

    setMagneticScalarPotentials(MovableBlock);
    setElectricCurrentsAndMMFs(MovableBlock);
    setVoltages(MovableBlock);
    computeWindingJouleLosses(MovableBlock);
    computeIronLosses(MovableBlock);
    computeTorque(MovableBlock);

    FixedBlock.computeDensity();
    MovableBlock.computeDensity();
}

void MS_Project::setMagneticScalarPotentials(MS_ElectromagneticBlock &block)
{
    for (auto element : block.ReluctanceElements)
        element->magneticPotential = UnknownsVector[element->getIndex()];
}

void MS_Project::setElectricCurrentsAndMMFs(MS_ElectromagneticBlock &block)
{
    for (auto component : block.VoltageSourceComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->Signal;
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            std::complex<double> current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += static_cast<double>(polarities[k]) * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
        component->Voltage = component->Signal;
    }
    for (auto component : block.CurrentSourceComponents) {
        component->Current = component->Signal;
    }
    for (auto component : block.PassiveComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->Signal;
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            std::complex<double> current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += static_cast<double>(polarities[k]) * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
        component->Voltage = component->Current * component->Impedance;
    }
    for (auto component : block.CoilComponents) {
        if ( component->belongsToCurrentSourceMesh() ) {
            component->Current = component->getMeshCurrentSource()->Signal;
        } else {
            std::vector<unsigned> indexes = component->getMeshIndexes();
            std::vector<int> polarities = component->getMeshPolarities();
            std::complex<double> current = 0.0;
            for (unsigned k = 0; k < indexes.size(); ++k) {
                current += static_cast<double>(polarities[k]) * UnknownsVector[indexes[k]];
            }
            component->Current = current;
        }
        component->Voltage = component->Current * component->Impedance;
    }
    for (auto MMFSource : block.MMFSources)
        MMFSource->setCurrent();
    for (auto element : block.ReluctanceElements) {
        element->computeMagneticFluxes();
        element->computeMagneticFluxDensity();
    }
}

void MS_Project::setVoltages(MS_ElectromagneticBlock &block)
{
    for (auto component : block.CurrentSourceComponents)
        component->Voltage = 0.0;
    for (auto component : block.VoltageSourceComponents)
        component->Voltage = component->Signal;
    for (auto component : block.PassiveComponents) {
        component->Voltage = component->Current * component->Impedance;
        if ( component->belongsToCurrentSourceMesh() )
            component->getMeshCurrentSource()->Voltage = -component->Voltage;
    }
    for (auto component : block.CoilComponents) {
        component->Voltage = component->Current * component->Impedance;
        component->computeVoltageCoilInduced();
        //component->VoltageCoilInduced *= Periodicities;
        component->Voltage += component->VoltageCoilInduced;
        if ( component->belongsToCurrentSourceMesh() )
            component->getMeshCurrentSource()->Voltage = -component->Voltage;
    }
}

void MS_Project::computeTorque(MS_ElectromagneticBlock &block)
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

void MS_Project::computeIronLosses(MS_ElectromagneticBlock &block)
{
    block.IronLosses = 0.0;
    for (auto element : block.NonlinearReluctanceElements)
        block.IronLosses += block.MaterialNonLinear.getIronLosses(element->magneticFluxDensity * M_SQRT2, block.Frequency, element->getGeometry().volume);
    block.IronLosses *= Periodicities;
}

void MS_Project::computeWindingJouleLosses(MS_ElectromagneticBlock &block)
{
    block.WindingJouleLosses = 0.0;
    for (auto coil : block.CoilComponents)
        block.WindingJouleLosses += coil->computeJouleLosses();
}

void MS_Project::assembleProject()
{
    // Assemble the global system of equations
    assembleMagneticCircuitDomain(FixedBlock);
    assembleMagneticCircuitDomain(MovableBlock);
    assembleElectricCircuitDomain(FixedBlock);
    assembleElectricCircuitDomain(MovableBlock);
}

void MS_Project::assembleMagneticCircuitDomain(MS_ElectromagneticBlock block)
{
    for (auto element : block.ReluctanceElements)
        element->computePermeance();
    for (auto element : block.ReluctanceElements) {
        element->computeFluxEquations();
        SystemOfEquations[element->getIndex()] = element->getFluxBalanceEquation();
    }
}

void MS_Project::assembleElectricCircuitDomain(MS_ElectromagneticBlock block)
{
    // Read circuits
    for (auto circuit : block.ElectricCircuits) {
        MEC_ComplexEquation equation;
        if ( circuit.currentSourceMesh ) {
            for (auto component : circuit.components) {
                if ( component->getType() == MSECCT_coil) {
                    std::complex<double> current = component->Signal; // current source value
                    for (auto coilSide : component->CoilSides) {
                        for (auto MMFSource : coilSide.MMFSources) {
                            MMFSource->setMMF(current); // Update MMF sources
                        }
                    }
                }
            }
        } else {
            for (unsigned componentIndex = 0; componentIndex < circuit.getSize(); ++componentIndex) {
                MS_ElectricCircuitComponent* component = circuit.components[componentIndex];
                // Impedance
                std::vector<unsigned> meshIndexes = component->getMeshIndexes();
                std::vector<int> meshPolarities = component->getMeshPolarities();
                int circuitPolarity = circuit.polarities[componentIndex];
                for (unsigned i = 0; i < meshIndexes.size(); ++i) {
                    if (meshPolarities[i] == circuitPolarity)
                        equation.addCoefficient(meshIndexes[i], component->Impedance);
                    else
                        equation.addCoefficient(meshIndexes[i], -component->Impedance);
                }
                // Voltage source
                if (component->getType() == MSECCT_voltageSource)
                    equation.addConstant(component->Signal * static_cast<double>(circuitPolarity));

                // Coil - back EMF (time derivative)
                if (component->getType() == MSECCT_coil) {
                    std::vector<MS_CoilSide> coilSides = component->getCoilSides();
                    for (unsigned coilSideIndex = 0; coilSideIndex < coilSides.size(); ++coilSideIndex) {
                        MS_CoilSide coilSide = coilSides[coilSideIndex];
                        for (unsigned currentElementIndex = 0; currentElementIndex < coilSide.activeReluctanceElements.size(); ++currentElementIndex) {
                            MS_ReluctanceElement* element = coilSide.activeReluctanceElements[currentElementIndex];
                            std::complex<double> EMF_Constant(0.0, -M_2PI * component->Frequency * circuit.polarities[componentIndex] * coilSide.Polarity * coilSide.Turns * element->getGeometry().crossSectionalArea / (coilSide.Area));
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
                }
            }
            SystemOfEquations[circuit.getIndex()] = equation;
        }
    }
}

void MS_Project::saveData(std::vector<double> &data)
{
    data[0] = FixedBlock.VoltageSourceComponents[0]->computeActivePower() + FixedBlock.VoltageSourceComponents[1]->computeActivePower() + FixedBlock.VoltageSourceComponents[2]->computeActivePower();
    data[1] = FixedBlock.WindingJouleLosses;
    data[2] = FixedBlock.IronLosses;
    data[3] = MovableBlock.WindingJouleLosses;
    data[4] = MovableBlock.IronLosses;
    data[5] = FixedBlock.Weight + MovableBlock.Weight;
    data[6] = (std::abs(FixedBlock.VoltageSourceComponents[0]->Voltage) + std::abs(FixedBlock.VoltageSourceComponents[1]->Voltage) + std::abs(FixedBlock.VoltageSourceComponents[2]->Voltage)) / 3.0;
    data[7] = (std::abs(FixedBlock.VoltageSourceComponents[0]->Current) + std::abs(FixedBlock.VoltageSourceComponents[1]->Current) + std::abs(FixedBlock.VoltageSourceComponents[2]->Current)) / 3.0;
}

int MS_Project::SuperLU(std::vector<MEC_ComplexEquation> equations, std::vector<std::complex<double>> &solutions)
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

unsigned MS_Project::initiateMagneticScalarPotentialIndexes(MS_ElectromagneticBlock &block, unsigned offset)
{
    unsigned i;
    for (i = 0; i < block.ReluctanceElements.size(); ++i)
        block.ReluctanceElements[i]->setIndex(i + offset);
    return i + offset;
}

unsigned MS_Project::initiateMeshCurrentIndexes(MS_ElectromagneticBlock &block, unsigned offset)
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
            if (currentSource->getType() == MSECCT_currentSource) {
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

void MS_Project::initiateMMFSources(MS_ElectromagneticBlock &block)
{
    // MMF sources due to coil components
    for (unsigned component = 0; component < block.CoilComponents.size(); ++component) {
        MS_ElectricCircuitComponent* coilComponent = block.CoilComponents[component];
        for (unsigned coilSide = 0; coilSide < coilComponent->CoilSides.size(); ++coilSide) {
            double totalTurns = coilComponent->CoilSides[coilSide].Turns;
            double coilSideArea = coilComponent->CoilSides[coilSide].Area;
            double MMFConstant = 0.0;
            for (unsigned element = 0; element < coilComponent->CoilSides[coilSide].activeReluctanceElements.size(); ++element) {
                MS_MMFSource* MMFSource = new MS_MMFSource;
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
                MS_MMFSource* MMFSource = new MS_MMFSource;
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
