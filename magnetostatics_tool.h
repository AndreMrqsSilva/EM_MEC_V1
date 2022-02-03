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
#include <algorithm>
#include <vector>
#include <string>
#include <chrono>
#include <QDebug>
#include <QString>
#include <complex>
#include "utilities.h"
#include "miscellaneous.h"

enum MS_ElectricCircuitComponentType { MSECCT_notDefined, MSECCT_voltageSource, MSECCT_currentSource, MSECCT_impedance, MSECCT_coil};

struct MS_MMFSource;

struct MS_CoilSide;

class MS_ReluctanceElement
{
public:
    MS_ReluctanceElement();
    MS_ReluctanceElement(ReluctanceElementType t, MEC_ElementGeometry g, double v = 0.0, std::string text = "");

    // Element functions
    void setType(ReluctanceElementType t) { type = t; }
    void setGeometry(MEC_ElementGeometry g);
    void setReluctivity(double v);
    void setNonLinearMaterial(NonlinearMaterial m) { nonlinearMaterial = m; }
    void addAirgapPermeanceDown(MS_ReluctanceElement* element, double permenace);
    void addAirgapPermeanceUp(MS_ReluctanceElement* element, double permenace);
    void setLabel(std::string text) { label = text; }
    void setIndex(unsigned v) { index = v; }
    void clearAirgapPermeances();
    void updateReluctivity();

    ReluctanceElementType getType() { return type; }
    MEC_ElementGeometry getGeometry() { return geometry; }
    double getReluctivity() { return reluctivity; }
    double getReluctanceTangential() { return reluctanceTangential; }
    double getReluctanceRadial() { return  reluctanceRadial; }
    std::string getLabel() { return label; }
    unsigned getIndex() { return index; }

    // Mesh functions
    void addMMFSource(MS_MMFSource* source);
    void connect(MS_ReluctanceElement* left, MS_ReluctanceElement* up, MS_ReluctanceElement* right, MS_ReluctanceElement* down);
    void computePermeance();
    void computeFluxEquations();
    MEC_ComplexEquation getFluxBalanceEquation();
    MEC_ComplexEquation getTangentionalFluxEquation();
    MEC_ComplexEquation getRadialFluxEquation();

    // Element post-processing results
    void computeMagneticFluxes();
    double computeMagneticFluxDensity();
    double computeEnergy();
    double computeTangentialForce();
    double computeRadialForce();

    std::complex<double> magneticPotential; // result of the element magnetic potential
    std::complex<double> electricCurrent; // result of the element total current (A)
    std::complex<double> electricCurrentDensity; // result of the element total current density (A / m / m);
    std::complex<double> magneticFluxLeft; // result of the element magnetic flux (wb)
    std::complex<double> magneticFluxRight; // result of the element magnetic flux (wb)
    std::complex<double> magneticFluxUp; // result of the element magnetic flux (wb)
    std::complex<double> magneticFluxDown; // result of the element magnetic flux (wb)
    std::complex<double> magneticFluxTangential; // result of the element directional magnetic flux (wb)
    std::complex<double> magneticFluxRadial; // result of the element directional magnetic flux (wb)
    std::complex<double> magneticFluxDensityTangential; // result of the element magnetic flux density tangential component (B)
    std::complex<double> magneticFluxDensityRadial; // result of the element magnetic flux density radial component (B)
    double magneticFluxDensity; // result of the element magnetic flux density (B)
    double energy; // result of the element enery (J)
    double forceTX; // result of the element force tangential component (N)
    double forceRY; // result of the element force radial component (N)

    MS_ReluctanceElement* elementLeft; // pointer to the left element
    MS_ReluctanceElement* elementRight; // pointer to the right element
    MS_ReluctanceElement* elementUp; // pointer to the up element
    MS_ReluctanceElement* elementDown; // pointer to the down element

private:
    void initialize();
    void computeReluctances();
    double computeReluctanceTangential(double v);
    double computeReluctanceRadial(double v);

    // Element definitions
    unsigned index; // unique reluctance element identification number that is equal to the element row in the coefficient matrix
    std::string label; // element identification label (optional)
    ReluctanceElementType type; // element types: RET_air, RET_coreLinear, RET_coreNonLinear, RET_coil
    MEC_ElementGeometry geometry; // element geometries: REG_rectangle, REG_trapezoid, REG_sector
    double reluctivity; // element reluctivity (H/B)
    double constantReluctanceTangential; // element reluctance constant (tangentional wise)
    double constantReluctanceRadial; // element reluctance constant (radial wise)
    double reluctanceTangential; // element reluctance (tangentional wise)
    double reluctanceRadial; // element reluctance (radial wise)
    double permeanceLeft; // element permeance
    double permeanceRight; // element permeance
    double permeanceUp; // element permeance
    double permeanceDown; // element permeance
    std::vector<double> airgapPermeanceDown; // vector of the complete airgap permeances (element + airgap + element)
    std::vector<double> airgapPermeanceUp; // vector of the complete airgap permeances (element + airgap + element)
    std::vector<double> airgapReluctanceDown; // vector of the reluctances of the airgap structure only
    std::vector<double> airgapReluctanceUp; // vector of the reluctances of the airgap structure only
    std::vector<MS_MMFSource*> MMFSources; // element magnetomotive force sources

    std::vector<MS_ReluctanceElement*> airgapElementsDown; // vector of pointers to the airgap structure
    std::vector<MS_ReluctanceElement*> airgapElementsUp; // vector of pointers to the airgap structure
    MEC_ComplexEquation EquationMagneticFluxLeft;
    MEC_ComplexEquation EquationMagneticFluxRight;
    MEC_ComplexEquation EquationMagneticFluxUp;
    MEC_ComplexEquation EquationMagneticFluxDown;
    MEC_ComplexEquation EquationMagneticFluxTangential;
    MEC_ComplexEquation EquationMagneticFluxRadial;
    MEC_ComplexEquation EquationMagneticFluxBalance;
    NonlinearMaterial nonlinearMaterial;
};

struct MS_CoilSide
{
    MS_CoilSide() { Turns = 0; Polarity = 0; Area = 0.0; }
    void setTurns(double t) { Turns = t; }
    void setPolarity(int p) { Polarity = p; }
    void addActiveElement(MS_ReluctanceElement *e) { activeReluctanceElements.push_back(e); Area += e->getGeometry().crossSectionalArea; }
    void addPassiveElement(MS_ReluctanceElement *e) { passiveReluctanceElements.push_back(e); }
    std::vector<MS_ReluctanceElement*> activeReluctanceElements;
    std::vector<MS_ReluctanceElement*> passiveReluctanceElements;
    std::vector<MS_MMFSource*> MMFSources;
    double Turns;
    int Polarity;
    double Area;
};

class MS_ElectricCircuitComponent
{
public:
    MS_ElectricCircuitComponent();

    // Component functions
    void setVoltageSource(double frequency, double rms, double phase, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCurrentSource(double frequency, double rms, double phase, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setImpedance(double frequency, double resistance = 0.0, double inductance = 0.0, std::string text = "");
    void setCoil(double frequency, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCoil(double frequency, MS_CoilSide coil_side, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCoil(double frequency, std::vector<MS_CoilSide> coil_sides, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setFrequency(double frequency);
    void addCoilSide(MS_CoilSide coil_side) { CoilSides.push_back(coil_side); }
    void setMeshCurrentSource(MS_ElectricCircuitComponent* source) { MeshCurrentSource = source; }
    void setMeshCurrent(std::vector<unsigned> meshIndexes, std::vector<int> meshPolarities) {MeshIndexes = meshIndexes; MeshPolarities = meshPolarities; }
    void addMeshCurrent(unsigned meshIndex, int meshPolarity) { MeshIndexes.push_back(meshIndex); MeshPolarities.push_back(meshPolarity); }
    void clearMeshCurrents() { MeshIndexes.clear(); MeshPolarities.clear(); }
    MS_ElectricCircuitComponentType getType() { return Type; }
    std::string getLabel() { return Label; }
    double getResistance() { return Resistance; }
    double getInductance() { return Inductance; }
    std::vector<MS_CoilSide> getCoilSides() { return CoilSides; }
    MS_ElectricCircuitComponent* getMeshCurrentSource() { return MeshCurrentSource; }
    bool belongsToCurrentSourceMesh() { if ( MeshCurrentSource == nullptr) return false; return true; }
    std::vector<MS_CoilSide> CoilSides;
    // Identification of currents
    std::vector<unsigned> getMeshIndexes() { return MeshIndexes; }
    std::vector<int> getMeshPolarities() { return MeshPolarities; }
    // Post-processing results
    double computeActivePower();
    double computeReactivePower();
    double computeApparentPower();
    double computeJouleLosses();
    std::complex<double> computeVoltageCoilInduced();
    double ActivePower, ReactivePower, ApparentPower;
    double JouleLosses; // result of the component Joule losses
    std::complex<double> Current; // result of the component driven current
    std::complex<double> Voltage; // result of the component terminals voltage
    std::complex<double> VoltageCoilInduced; // result of the coil element induced voltage (MDECCT_coil components only!)
    double Frequency;
    double SignalRMS;
    double SignalPhase;
    std::complex<double> Signal;
    std::complex<double> Impedance;

private:
    MS_ElectricCircuitComponentType Type; // component type
    std::string Label; // component label

    // Passive component
    double Resistance;
    double Inductance;

    // Identification of currents
    std::vector<unsigned> MeshIndexes; // indexes for the component currents values
    std::vector<int> MeshPolarities; // indexes for the component currents polarities
    MS_ElectricCircuitComponent* MeshCurrentSource;
};

struct MS_ElectricCircuit
{
    MS_ElectricCircuit() { index = 0; currentSourceMesh = false;}

    void setIndex(unsigned i) { index = i; }
    unsigned getIndex() { return index; }
    void addComponent(MS_ElectricCircuitComponent* component, int polarity);
    void clear() { index = 0; components.clear(); polarities.clear(); }
    unsigned getSize() { return static_cast<unsigned>(components.size()); }

    unsigned index;
    bool currentSourceMesh;
    std::vector<MS_ElectricCircuitComponent*> components;
    std::vector<int> polarities;
};

struct MS_MMFSource
{
    MS_MMFSource();

    void setConstantSource(bool enable = true) { ConstantSource = enable; }
    void setReluctanceElement(MS_ReluctanceElement* element) { ReluctanceElement = element; }
    void setCoilComponent(MS_ElectricCircuitComponent* coil) { CoilComponent = coil; }
    void setPolarity(int polarity);
    void setCurrent();
    void setMMF(std::complex<double> current);

    std::vector<unsigned> MeshIndexes; // indexes for the element currents values
    std::vector<int> MeshPolarities; // indexes for the element currents polarities
    MS_ReluctanceElement* ReluctanceElement;
    MS_ElectricCircuitComponent* CoilComponent;

    bool ConstantSource;
    int Polarity; // polarity of the source
    MEC_MMFSourceOrientation Orientation; // orientation of the source tangential or radial
    double ConstantMMF; // MMF constant of the source
    std::complex<double> Current; // result of the current of the source
    std::complex<double> MMF; // result of the MMF of the source
};

class MS_AirgapConnection
{
public:
    MS_AirgapConnection();
    MS_AirgapConnection(MS_ReluctanceElement* fixedElement, MS_ReluctanceElement* movableElement);
    void setElements(MS_ReluctanceElement* fixedElement, MS_ReluctanceElement* movableElement);
    double computePermeance(double movableBlockAngularPosition, double lowerEnd, double upperEnd);
    double getPermeance() { return Permeance; }
    MS_ReluctanceElement* FixedElement;
    MS_ReluctanceElement* MovableElement;
private:
    void computeMaximumPermeance();
    double Permeance;
    double PermeanceMaximum;
    MEC_ElementGeometry FixedElementGeometry, MovableElementGeometry;
};

class MS_AirgapStructure
{
public:
    MS_AirgapStructure();

    void setConnectionElements(std::vector<MS_ReluctanceElement*> fixedBlockElements, std::vector<MS_ReluctanceElement*> movableBlockElements);
    void setAirgapPermeances(double tangentialPosition);
    void setPeriodicities(int p) { Periodicities = p; }

private:
    void createAirgapElements();
    void computeAirgapPermeances(double rotorAngularPosition);

    int Periodicities;
    std::vector<std::vector<MS_ReluctanceElement*>> currentlyConnectedMovableBlockElements;
    std::vector<std::vector<double>> currentlyConnectedPermeances;
    std::vector<std::vector<MS_AirgapConnection>> airgapDirectConnectionElements;
    std::vector<MS_ReluctanceElement*> connectionElements_fixedBlock, connectionElements_movableBlock;
};

struct MS_ElectromagneticBlock
{
    MS_ElectromagneticBlock();

    void computeDensity();
    void computeTorque();

    // General data
    double Length; // block lenght, z-axis wise (m)
    double Position_tX; // block center position along the tangential- or x-axis (radian/ m)
    double Position_rY; // block center position along the radial- or y-axis (radian/ m)
    double Speed_tX; // block center speed along the tangential- or x-axis
    double Speed_rY; // block center speed along the radial- or y-axis
    double Frequency;
    unsigned TangentialElementsCount; // number of reluctance elements along the tangential axis
    unsigned RadialElementsCount; // number of reluctance elements along the radial axis
    unsigned Slices; // number of repetitive geometric combinations (slot and tooth)
    NonlinearMaterial MaterialNonLinear; // block nonlinear reluctance element material
    std::vector<MS_MMFSource*> MMFSources; // list of MMF sources
    std::vector<MS_ReluctanceElement*> ReluctanceElements; // list of the block reluctance elements
    std::vector<MS_ReluctanceElement*> AirgapLayerElements; // list of the block connecting elements to the airgap structure
    std::vector<MS_ReluctanceElement*> MagnetElements;
    std::vector<MS_ReluctanceElement*> NonlinearReluctanceElements; // list of the block nonlinear elements
    std::vector<MS_ElectricCircuitComponent*> CoilComponents; // list of the block coil components
    std::vector<MS_ElectricCircuitComponent*> PassiveComponents; // list of the block passive components
    std::vector<MS_ElectricCircuitComponent*> VoltageSourceComponents; // list of the block voltage source components
    std::vector<MS_ElectricCircuitComponent*> CurrentSourceComponents; // list of the block current source components
    std::vector<MS_ElectricCircuit> ElectricCircuits; // list of the block electric circuits
    double WindingDensity;

    // Post processing
    double TangentialForce; // block total tangential force
    double RadialForce; // block total radial force
    double WindingJouleLosses;
    double IronLosses; // block total iron losses
    double MagneticEnergy; // block total magnetic energy
    double Volume;
    double Weight;
};

class MS_Project
{
public:
    MS_Project();
    MS_Project(MS_ElectromagneticBlock fixed);
    MS_Project(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable);
    MS_Project(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable, int periodicity);
    ~MS_Project();

    void destroyProject();
    void loadProject(MS_ElectromagneticBlock fixed);
    void loadProject(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable);
    void loadProject(MS_ElectromagneticBlock fixed, MS_ElectromagneticBlock movable, int periodicity);
    void setPeriodicities(int p) { Periodicities = p;}
    void updateMovableBlockPosition(double tX, double rY) { MovableBlock.Position_tX = tX; MovableBlock.Position_rY = rY; }
    void updateMovableBlockFrequency(double frequency);
    MS_ElectromagneticBlock getFixedBlock() { return FixedBlock; }
    MS_ElectromagneticBlock getMovableBlock() { return MovableBlock; }
    ReportDataSolver solve(double relaxation = 0.1, unsigned iterations = 100, double error = 0.01);

private:
    void destroyBlock(MS_ElectromagneticBlock &block);
    void preProcessing();
    void fastPostProcessing();
    void postProcessing();
    void setMagneticScalarPotentials(MS_ElectromagneticBlock &block);
    void setElectricCurrentsAndMMFs(MS_ElectromagneticBlock &block);
    void setVoltages(MS_ElectromagneticBlock &block);
    void computeTorque(MS_ElectromagneticBlock &block);
    void computeIronLosses(MS_ElectromagneticBlock &block);
    void computeWindingJouleLosses(MS_ElectromagneticBlock &block);
    void assembleProject();
    void assembleMagneticCircuitDomain(MS_ElectromagneticBlock block);
    void assembleElectricCircuitDomain(MS_ElectromagneticBlock block);
    void saveData(std::vector<double> &data);
    int SuperLU(std::vector<MEC_ComplexEquation> equations, std::vector<std::complex<double>> &solutions);
    unsigned initiateMagneticScalarPotentialIndexes(MS_ElectromagneticBlock &block, unsigned offset);
    unsigned initiateMeshCurrentIndexes(MS_ElectromagneticBlock &block, unsigned offset);
    void initiateMMFSources(MS_ElectromagneticBlock &block);
    MS_ElectromagneticBlock FixedBlock;
    MS_ElectromagneticBlock MovableBlock;
    MS_AirgapStructure airgapStructure;

    bool isNonlinear;
    bool isSolved;
    int Periodicities;
    unsigned UnknownsCount;
    ZSuperLU_Data SuperLUData;
    std::vector<MEC_ComplexEquation> SystemOfEquations;
    std::vector<std::complex<double>> UnknownsVector;
};

#endif // MAGNETOSTATICS_TOOL_H
