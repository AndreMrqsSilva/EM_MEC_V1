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

#ifndef MAGNETOSTATICS_TOOL_H
#define MAGNETOSTATICS_TOOL_H

#include <cmath>
#include <complex>
#include <vector>
#include <string>
#include <chrono>
#include <QDebug>
#include "miscellaneous.h"
#include "superLU/slu_zdefs.h"

#define LINEARSYSTEMOFEQUATIONS_SOLVER_ZERO 1E-20
#define NONLINEARSOLVER_MAX_VARIANCE 0.01

enum MagnetostaticsElectricCircuitComponentType { MECCT_voltageSource, MECCT_impedance, MECCT_coil};

enum MagnetOrientation { MO_left, MO_up, MO_right, MO_down };

struct MagnetostaticsMMFSource;

struct MagnetostaticsReluctanceElement
{
    MagnetostaticsReluctanceElement();
    MagnetostaticsReluctanceElement(ReluctanceElementType elementType);
    // Functions
    void setGeometryRectangle(double h, double w, double l);
    void setGeometryTrapezoid(double h, double bW, double tW, double l);
    void setGeometrySector(double rT, double rB, double aT, double aB, double l);
    void setPosition(double TX, double RY) { centerTangentialPosition = TX; centerRadialPosition = RY; }
    void setReluctivity(double value);
    void setRemanence(double Br, MagnetOrientation o);
    void setLabel(std::string text) { label = text; }
    void connect(MagnetostaticsReluctanceElement* left, MagnetostaticsReluctanceElement* up, MagnetostaticsReluctanceElement* right, MagnetostaticsReluctanceElement* down);
    void computeReluctances();
    void addMMFSource(MagnetostaticsMMFSource* source);
    void computePermeance();
    // Variables
    unsigned index; // unique element identification
    std::string label; // label
    ReluctanceElementType type;
    ReluctanceElementGeometry shape;
    double centerTangentialPosition;
    double centerRadialPosition;
    double height;
    double width;
    double widthBottom;
    double widthTop;
    double radiusBottom;
    double radiusTop;
    double spanBottom;
    double spanTop;
    double length;
    double magnetRemanence;
    MagnetOrientation magnetOrientation;
    double magnetFluxSource;
    double area;
    double crossSectionalAreaRadial;
    double crossSectionalAreaTangential; // element areas (m2)
    double volume; // element volume (m3)
    double reluctivity; // element reluctivity (H/B)
    double constantReluctanceTangential;
    double constantReluctanceRadial;
    double reluctanceTangential;
    double reluctanceRadial; // element reluctance
    double permeanceLeft;
    double permeanceRight;
    double permeanceUp;
    double permeanceDown; // element permeance
    std::vector<MagnetostaticsMMFSource*> MMFSources;
    // Post-processing variables
    std::complex<double> magneticPotential; // result of the element magnetic potential
    std::complex<double> electricCurrent; // result of the element total current (A)
    std::complex<double> electricCurrentDensity; // result of the element total current density (A/m^2);
    std::complex<double> magneticFluxLeft;
    std::complex<double> magneticFluxRight;
    std::complex<double> magneticFluxUp;
    std::complex<double> magneticFluxDown; // result of the element fluxes (wb)
    std::complex<double> magneticFluxTangential;
    std::complex<double> magneticFluxRadial; // result of the element directional fluxes (wb)
    double magneticFluxDensity; // result of the element flux density (B)
    double energy; // result of the element enery (J)
    // Pointers
    MagnetostaticsReluctanceElement* elementLeft;
    MagnetostaticsReluctanceElement* elementRight;
    MagnetostaticsReluctanceElement* elementUp;
    MagnetostaticsReluctanceElement* elementDown;
};

class MagnetostaticsAirgapConnection
{
public:
    MagnetostaticsAirgapConnection();
    MagnetostaticsAirgapConnection(MagnetostaticsReluctanceElement* sElement, MagnetostaticsReluctanceElement* rElement);
    void setElements(MagnetostaticsReluctanceElement* sElement, MagnetostaticsReluctanceElement* rElement);
    double computePermeance(double rotorAngularPosition);
    double permeance;
    MagnetostaticsReluctanceElement* statorElement;
    MagnetostaticsReluctanceElement* rotorElement;
private:
    void computeMaximumPermeance();
    double permeanceMaximum;
    double statorElementTangentialPosition, rotorElementTangentialPosition;
};

class MagnetostaticsAirgapStructure
{
public:
    MagnetostaticsAirgapStructure();
    unsigned getElementsCount() { return elementCount; }
    void setConnectionElements(std::vector<MagnetostaticsReluctanceElement*> sElements, std::vector<MagnetostaticsReluctanceElement*> rElements);
    void createAirgapElements();
    void computeAirgapPermeances(double rotorAngularPosition);
    double getAirgapReluctance(unsigned statorElementIndex, unsigned rotorConnectedElementIndex);
    std::vector<MagnetostaticsReluctanceElement*> getCurrentlyConnectedRotorElements(unsigned statorElementIndex) { return currentlyConnectedRotorElements[statorElementIndex]; }
private:
    unsigned elementCount;
    std::vector<std::vector<MagnetostaticsReluctanceElement*>> currentlyConnectedRotorElements;
    std::vector<std::vector<double>> currentlyConnectedPermeances;
    std::vector<std::vector<MagnetostaticsAirgapConnection>> airgapDirectConnectionElements;
    std::vector<MagnetostaticsReluctanceElement*> connectionElements_fixedBlock, connectionElements_movableBlock;
};

struct MagnetostaticsCoilSide
{
    std::vector<MagnetostaticsReluctanceElement*> activeReluctanceElements;
    std::vector<MagnetostaticsReluctanceElement*> passiveReluctanceElements;
    int turns;
    int polarity;
    double area;
};

struct MagnetostaticsElectricCircuitComponent
{
    MagnetostaticsElectricCircuitComponent() { }
    MagnetostaticsElectricCircuitComponent(MagnetostaticsElectricCircuitComponentType t) { type = t; }
    MagnetostaticsElectricCircuitComponentType type;
    std::string label; // label
    // Passive component
    double frequency; // circuit angular frequency
    double resistance;
    double inductance;
    std::complex<double> impedance; // component impedance
    // Coil element
    std::vector<MagnetostaticsCoilSide> coilSides;
    // Voltage source
    std::complex<double> voltageSource; // voltage source
    // Identification of currents
    std::vector<unsigned> currentIndexes; // indexes for the component currents values
    std::vector<int> currentPolarities; // indexes for the component currents polarities
    // Post-processing results
    std::complex<double> current; // result of the component driven current
    std::complex<double> voltage; // result of the component terminals voltage
    std::complex<double> voltageCoilInduced; // result of the coil element induced voltage
    double powerActive;
    double powerReactive;
    double powerApparent;
    double powerLosses;
};

struct MagnetostaticsMMFSource
{
    MagnetostaticsReluctanceElement* reluctanceElement;
    MagnetostaticsElectricCircuitComponent* electricCircuitComponent;
    int MMFSourcePolarity;
    double constantMMF;
    std::vector<unsigned> currentIndexes; // indexes for the element currents values
    std::vector<int> currentPolarities; // indexes for the element currents polarities
    std::complex<double> current;
    std::complex<double> MMF;
};

struct MagnetostaticsElectricCircuit
{
    void addComponent(MagnetostaticsElectricCircuitComponent* component, int polarity);
    void clear();
    std::vector<MagnetostaticsElectricCircuitComponent*> components;
    std::vector<int> polarities;
};

struct MagnetostaticsProjectBlock
{
    double frequency;
    double length;
    double angularPosition;
    double torque;
    double ironLosses;
    unsigned slices;
    unsigned tangentialElementsCount;
    unsigned radialElementsCount;
    NonlinearMaterial nonlinearMaterial;
    std::vector<MagnetostaticsMMFSource*> MMFSources;
    std::vector<MagnetostaticsReluctanceElement*> reluctanceElements;
    std::vector<MagnetostaticsReluctanceElement*> airgapLayerElements;
    std::vector<MagnetostaticsReluctanceElement*> nonlinearReluctanceElements;
    std::vector<MagnetostaticsElectricCircuitComponent*> coilComponents;
    std::vector<MagnetostaticsElectricCircuitComponent*> passiveComponents;
    std::vector<MagnetostaticsElectricCircuitComponent*> powerSupplyComponents;
    std::vector<MagnetostaticsElectricCircuit*> electricCircuits;
};

class MagnetostaticsElectricMachineProject
{
public:
    MagnetostaticsElectricMachineProject();
    // Functions
    void destroyProject();
    void cleanProject();
    void updateFrequency_fixedBlock(double f);
    void updateFrequency_movableBlock(double angularFrequency);
    void updatePosition_movableBlock(double radians);
    void postProcessingLevel1();
    void postProcessingLevel2();
    double getResult_powerActiveInput();
    double getResult_powerApparentInput();
    double getResult_torqueStressTensor();
    double getResult_lossesConductionStator();
    double getResult_lossesConductionRotor();
    double getResult_lossesCoreStator();
    double getResult_lossesCoreRotor();
private:
    void computeElementFlux();
    void computeElementFluxDensity();
    void computeElementCurrentDensity();
    void computeElementEnergy();
    void computeElectromagneticTorque();
    void computeElectricDomainQuantities();
    void computeIronLosses();
    void computeVoltage(MagnetostaticsElectricCircuitComponent* component);
    void computePowerQuantities(MagnetostaticsElectricCircuitComponent* component);
public:
    // Variables
    bool isSolved;
    bool isNonlinearSystem;
    MagnetostaticsProjectBlock fixedBlock;
    MagnetostaticsProjectBlock movableBlock;
    MagnetostaticsAirgapStructure airgap;
};

class MagnetostaticsElectricMachineProcessor
{
public:
    MagnetostaticsElectricMachineProcessor();
    ~MagnetostaticsElectricMachineProcessor() { project.destroyProject(); }
    void setProject(MagnetostaticsElectricMachineProject p) { project = p; }
    void loadProject(MagnetostaticsElectricMachineProject p);
    ReportDataSolver solve(double relaxation, unsigned MaxIterations);
    void getProject(MagnetostaticsElectricMachineProject &p) { p = project; }
private:
    void assembleSystemOfEquations();
    void assembleBlockSystemOfEquations(MagnetostaticsProjectBlock block, unsigned MD_startingIndex, unsigned ED_startingIndex);
    void assembleStatorSystemOfEquations();
    void assembleRotorSystemOfEquations();
    void assembleAirgapSystemOfEquations();
    int solveLinearSystemEquations(std::vector<std::vector<std::complex<double>>> A, std::vector<std::complex<double>> &X, std::vector<std::complex<double>> B);
    void setResultsToProject();
    unsigned unknownsNumber;
    unsigned startingIndex_statorElements; // unknowns indexation magnetic potential - stator elements
    unsigned startingIndex_rotorElements; // unknowns indexation magnetic potential - rotor elements
    unsigned startingIndex_statorCircuits; // unknowns indexation electric current - stator circuits
    unsigned startingIndex_rotorCircuits; // unknowns indexation electric current - rotor circuits
    MagnetostaticsElectricMachineProject project;
    std::vector<std::vector<std::complex<double>>> stiffnessMatrix; // stiffness matrix
    std::vector<std::complex<double>> unknownsVector;
    std::vector<std::complex<double>> constantsVector;
};

#endif // MAGNETOSTATICS_TOOL_H
