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

#ifndef MAGNETODYNAMICS_TOOL_H
#define MAGNETODYNAMICS_TOOL_H

#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <chrono>
#include <QDebug>
#include <QString>
#include "utilities.h"
#include "miscellaneous.h"

enum MD_ElectricCircuitComponentType { MDECCT_notDefined, MDECCT_voltageSource, MDECCT_currentSource, MDECCT_impedance, MDECCT_coil};

struct MD_MMFSource;

struct MD_CoilSide;

class MD_ReluctanceElement
{
public:
    MD_ReluctanceElement();
    MD_ReluctanceElement(ReluctanceElementType t, MEC_ElementGeometry g, double v = 0.0, std::string text = "");
    MD_ReluctanceElement(ReluctanceElementType t, MEC_ElementGeometry g, double Br, MEC_MMFSourceOrientation o, int polarity, double v = 0.0, std::string text = "");

    // Element functions
    void setType(ReluctanceElementType t) { type = t; }
    void setGeometry(MEC_ElementGeometry g);
    void setReluctivity(double v);
    void setNonLinearMaterial(NonlinearMaterial m) { nonlinearMaterial = m; }
    void setRemanence(double Br, MEC_MMFSourceOrientation o, int polarity);
    void addAirgapPermeanceDown(MD_ReluctanceElement* element, double permenace);
    void addAirgapPermeanceUp(MD_ReluctanceElement* element, double permenace);
    void setLabel(std::string text) { label = text; }
    void setIndex(unsigned v) { index = v; }
    void clearAirgapPermeances();
    void updateReluctivity();

    ReluctanceElementType getType() { return type; }
    MEC_ElementGeometry getGeometry() { return geometry; }
    double getReluctivity() { return reluctivity; }
    double getReluctanceTangential() { return reluctanceTangential; }
    double getReluctanceRadial() { return  reluctanceRadial; }
    double getRemanentMagnetization() { return RemanentMagnetization; }
    MEC_MMFSourceOrientation getMagnetOrientation() { return MagnetOrientation; }
    int getMagnetPolarity() { return MagnetPolarity; }
    double getMagnetMMFSource() { return MagnetMMFSource; }
    std::string getLabel() { return label; }
    unsigned getIndex() { return index; }

    // Mesh functions
    void addMMFSource(MD_MMFSource* source);
    void connect(MD_ReluctanceElement* left, MD_ReluctanceElement* up, MD_ReluctanceElement* right, MD_ReluctanceElement* down);
    void computePermeance();
    void computeFluxEquations();
    MEC_RealEquation getFluxBalanceEquation();
    MEC_RealEquation getTangentionalFluxEquation();
    MEC_RealEquation getRadialFluxEquation();

    // Element post-processing results
    void computeMagneticFluxes();
    double computeMagneticFluxDensity();
    double computeEnergy();
    double computeTangentialForce();
    double computeRadialForce();

    double magneticPotential; // result of the element magnetic potential
    double electricCurrent; // result of the element total current (A)
    double electricCurrentDensity; // result of the element total current density (A / m / m);
    double magneticFluxLeft; // result of the element magnetic flux (wb)
    double magneticFluxRight; // result of the element magnetic flux (wb)
    double magneticFluxUp; // result of the element magnetic flux (wb)
    double magneticFluxDown; // result of the element magnetic flux (wb)
    double magneticFluxTangential; // result of the element directional magnetic flux (wb)
    double magneticFluxRadial; // result of the element directional magnetic flux (wb)
    double magneticFluxDensity; // result of the element magnetic flux density (B)
    double magneticFluxDensityTangential; // result of the element magnetic flux density tangential component (B)
    double magneticFluxDensityRadial; // result of the element magnetic flux density radial component (B)
    double energy; // result of the element enery (J)
    double forceTX; // result of the element force tangential component (N)
    double forceRY; // result of the element force radial component (N)
    double magneticFluxDensityPrevious; // result of the element magnetic flux density (B) at the previous iteration

    MD_ReluctanceElement* elementLeft; // pointer to the left element
    MD_ReluctanceElement* elementRight; // pointer to the right element
    MD_ReluctanceElement* elementUp; // pointer to the up element
    MD_ReluctanceElement* elementDown; // pointer to the down element

private:
    void initialize();
    void computeReluctances();
    double computeReluctanceTangential(double v);
    double computeReluctanceRadial(double v);
    double computeMagneticFluxDensityDerivative_tangential(bool h_l, bool h_r);

    // Element definitions
    unsigned index; // unique reluctance element identification number that is equal to the element row in the coefficient matrix
    std::string label; // element identification label (optional)
    ReluctanceElementType type; // element types: RET_air, RET_coreLinear, RET_coreNonLinear, RET_coil, RET_magnet
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
    double RemanentMagnetization; // magnet remanent magnetization (T)
    double MagnetMMFSource; // magnet permanent MMF source value
    MEC_MMFSourceOrientation MagnetOrientation; // magnet orientation (tangential or radial)
    int MagnetPolarity; // magnet polarity +1 or -1
    std::vector<MD_MMFSource*> MMFSources; // element magnetomotive force sources

    std::vector<MD_ReluctanceElement*> airgapElementsDown; // vector of pointers to the airgap structure
    std::vector<MD_ReluctanceElement*> airgapElementsUp; // vector of pointers to the airgap structure
    MEC_RealEquation EquationMagneticFluxLeft;
    MEC_RealEquation EquationMagneticFluxRight;
    MEC_RealEquation EquationMagneticFluxUp;
    MEC_RealEquation EquationMagneticFluxDown;
    MEC_RealEquation EquationMagneticFluxTangential;
    MEC_RealEquation EquationMagneticFluxRadial;
    MEC_RealEquation EquationMagneticFluxBalance;
    NonlinearMaterial nonlinearMaterial;
};

class MEC_SignalSource
{
public:
    MEC_SignalSource() { size = 0; step = 0; }

    void clear() { size = 0; pulses.clear(); }
    void reset() { step = 0; }
    void addPulse(double value) { pulses.push_back(value); }
    void replacePulse(unsigned i, double value) { pulses[i] = value; }
    double readPulse();
    double nextPulse();
    double getPulse(unsigned i) { return pulses[i]; }

private:
    unsigned size;
    unsigned step;
    std::vector<double> pulses;
};

struct MD_CoilSide
{
    MD_CoilSide() { Turns = 0; Polarity = 0; Area = 0.0; }
    void setTurns(double t) { Turns = t; }
    void setPolarity(int p) { Polarity = p; }
    void addActiveElement(MD_ReluctanceElement *e) { activeReluctanceElements.push_back(e); Area += e->getGeometry().crossSectionalArea; }
    void addPassiveElement(MD_ReluctanceElement *e) { passiveReluctanceElements.push_back(e); }
    std::vector<MD_ReluctanceElement*> activeReluctanceElements;
    std::vector<MD_ReluctanceElement*> passiveReluctanceElements;
    std::vector<MD_MMFSource*> MMFSources;
    double Turns;
    int Polarity;
    double Area;
};

class MD_ElectricCircuitComponent
{
public:
    MD_ElectricCircuitComponent();

    // Component functions
    void setVolatageSource(MEC_SignalSource signal, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCurrentSource(std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCurrentSource(MEC_SignalSource signal, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setImpedance(double resistance = 0.0, double inductance = 0.0, std::string text = "");
    void setCoil(std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCoil(MD_CoilSide coil_side, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCoil(std::vector<MD_CoilSide> coil_sides, std::string text = "", double resistance = 0.0, double inductance = 0.0);
    void setCurrentPrevious() { CurrentPrevious = Current; }
    void setLinkageFluxPrevious() { LinkageFluxPrevious = LinkageFlux; }
    void addCoilSide(MD_CoilSide coil_side) { CoilSides.push_back(coil_side); }
    void setMeshCurrentSource(MD_ElectricCircuitComponent* source) { MeshCurrentSource = source; }
    void setMeshCurrent(std::vector<unsigned> meshIndexes, std::vector<int> meshPolarities) {MeshIndexes = meshIndexes; MeshPolarities = meshPolarities; }
    void addMeshCurrent(unsigned meshIndex, int meshPolarity) { MeshIndexes.push_back(meshIndex); MeshPolarities.push_back(meshPolarity); }
    void clearMeshCurrents() { MeshIndexes.clear(); MeshPolarities.clear(); }
    void setCurrentSignal(MEC_SignalSource signal) { CurrentSource = signal; }
    MD_ElectricCircuitComponentType getType() { return Type; }
    std::string getLabel() { return Label; }
    double getResistance() { return Resistance; }
    double getInductance() { return Inductance; }
    std::vector<MD_CoilSide> getCoilSides() { return CoilSides; }
    MEC_SignalSource getVoltageSourceSignal() { return VoltageSource; }
    MEC_SignalSource getCurrentSourceSignal() { return CurrentSource; }
    MD_ElectricCircuitComponent* getMeshCurrentSource() { return MeshCurrentSource; }
    bool belongsToCurrentSourceMesh() { if ( MeshCurrentSource == nullptr) return false; return true; }
    double readPulse();
    double nextPulse();
    std::vector<MD_CoilSide> CoilSides;
    // Identification of currents
    std::vector<unsigned> getMeshIndexes() { return MeshIndexes; }
    std::vector<int> getMeshPolarities() { return MeshPolarities; }
    // Post-processing results
    double computePower() { Power = Voltage * Current; return Power; }
    double computeJouleLosses() { JouleLosses = Resistance * Current * Current; return JouleLosses; }
    double computeLinkageFlux(double timeStep);
    double computeVoltageCoilInduced();
    double Power;
    double JouleLosses; // result of the component Joule losses
    double Current; // result of the component driven current
    double CurrentPrevious;
    double Voltage; // result of the component terminals voltage
    double VoltageCoilInduced; // result of the coil element induced voltage (MDECCT_coil components only!)
    double LinkageFlux;
    double LinkageFluxPrevious;

private:
    MD_ElectricCircuitComponentType Type; // component type
    std::string Label; // component label

    // Passive component
    double Resistance;
    double Inductance;

    // Voltage source
    MEC_SignalSource VoltageSource; // voltage source

    // Current source
    MEC_SignalSource CurrentSource; // current source

    // Identification of currents
    std::vector<unsigned> MeshIndexes; // indexes for the component currents values
    std::vector<int> MeshPolarities; // indexes for the component currents polarities
    MD_ElectricCircuitComponent* MeshCurrentSource;
};

struct MD_ElectricCircuit
{
    MD_ElectricCircuit() { index = 0; currentSourceMesh = false;}

    void setIndex(unsigned i) { index = i; }
    unsigned getIndex() { return index; }
    void addComponent(MD_ElectricCircuitComponent* component, int polarity);
    void clear() { index = 0; components.clear(); polarities.clear(); }
    unsigned getSize() { return static_cast<unsigned>(components.size()); }

    unsigned index;
    bool currentSourceMesh;
    std::vector<MD_ElectricCircuitComponent*> components;
    std::vector<int> polarities;
};

struct MD_MMFSource
{
    MD_MMFSource();

    void setConstantSource(bool enable = true) { ConstantSource = enable; }
    void setReluctanceElement(MD_ReluctanceElement* element) { ReluctanceElement = element; }
    void setCoilComponent(MD_ElectricCircuitComponent* coil) { CoilComponent = coil; }
    void setPolarity(int polarity);
    void setCurrent();
    void setMMF(double current);

    std::vector<unsigned> MeshIndexes; // indexes for the element currents values
    std::vector<int> MeshPolarities; // indexes for the element currents polarities
    MD_ReluctanceElement* ReluctanceElement;
    MD_ElectricCircuitComponent* CoilComponent;

    bool ConstantSource;
    int Polarity; // polarity of the source
    MEC_MMFSourceOrientation Orientation; // orientation of the source tangential or radial
    double ConstantMMF; // MMF constant of the source
    double Current; // result of the current of the source
    double MMF; // result of the MMF of the source
};

class MD_AirgapConnection
{
public:
    MD_AirgapConnection();
    MD_AirgapConnection(MD_ReluctanceElement* fixedElement, MD_ReluctanceElement* movableElement);
    void setElements(MD_ReluctanceElement* fixedElement, MD_ReluctanceElement* movableElement);
    double computePermeance(double movableBlockAngularPosition, double lowerEnd, double upperEnd);
    double getPermeance() { return Permeance; }
    MD_ReluctanceElement* FixedElement;
    MD_ReluctanceElement* MovableElement;
private:
    void computeMaximumPermeance();
    double Permeance;
    double PermeanceMaximum;
    MEC_ElementGeometry FixedElementGeometry, MovableElementGeometry;
};

class MD_AirgapStructure
{
public:
    MD_AirgapStructure();

    void setConnectionElements(std::vector<MD_ReluctanceElement*> fixedBlockElements, std::vector<MD_ReluctanceElement*> movableBlockElements);
    void setAirgapPermeances(double tangentialPosition);
    void setPeriodicities(int p) { Periodicities = p; }

private:
    void createAirgapElements();
    void computeAirgapPermeances(double rotorAngularPosition);

    int Periodicities;
    std::vector<std::vector<MD_ReluctanceElement*>> currentlyConnectedMovableBlockElements;
    std::vector<std::vector<double>> currentlyConnectedPermeances;
    std::vector<std::vector<MD_AirgapConnection>> airgapDirectConnectionElements;
    std::vector<MD_ReluctanceElement*> connectionElements_fixedBlock, connectionElements_movableBlock;
};

struct MD_ElectromagneticBlock
{
    MD_ElectromagneticBlock();

    void computeDensity();
    void computeTorque();

    // General data
    double Length; // block lenght, z-axis wise (m)
    double Position_tX; // block center position along the tangential- or x-axis (radian/ m)
    double Position_rY; // block center position along the radial- or y-axis (radian/ m)
    double Speed_tX; // block center speed along the tangential- or x-axis
    double Speed_rY; // block center speed along the radial- or y-axis
    double Acceleration_tX; // block center acceleration along the tangential- or x-axis
    double Acceleration_rY; // block center acceleration along the radial- or y-axis
    double Inertia; // block inertia (kg.m.m), non-fixed speed applications
    unsigned TangentialElementsCount; // number of reluctance elements along the tangential axis
    unsigned RadialElementsCount; // number of reluctance elements along the radial axis
    unsigned Slices; // number of repetitive geometric combinations (slot and tooth)
    NonlinearMaterial MaterialNonLinear; // block nonlinear reluctance element material
    std::vector<MD_MMFSource*> MMFSources; // list of MMF sources
    std::vector<MD_ReluctanceElement*> ReluctanceElements; // list of the block reluctance elements
    std::vector<MD_ReluctanceElement*> AirgapLayerElements; // list of the block connecting elements to the airgap structure
    std::vector<MD_ReluctanceElement*> MagnetElements;
    std::vector<MD_ReluctanceElement*> NonlinearReluctanceElements; // list of the block nonlinear elements
    std::vector<MD_ElectricCircuitComponent*> CoilComponents; // list of the block coil components
    std::vector<MD_ElectricCircuitComponent*> PassiveComponents; // list of the block passive components
    std::vector<MD_ElectricCircuitComponent*> VoltageSourceComponents; // list of the block voltage source components
    std::vector<MD_ElectricCircuitComponent*> CurrentSourceComponents; // list of the block current source components
    std::vector<MD_ElectricCircuit> ElectricCircuits; // list of the block electric circuits
    double WindingDensity;
    double PermanentMagnetDensity;

    // Post processing
    double TangentialForce; // block total tangential force
    double RadialForce; // block total radial force
    double IronLosses; // block total iron losses
    double PermanentMagnetLosses; // block losses in the permanent magnets due to eddy currents
    double MagneticEnergy; // block total magnetic energy
    double Volume;
    double Weight;
};

class MD_Project
{
public:
    MD_Project();
    MD_Project(MD_ElectromagneticBlock fixed);
    MD_Project(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable);
    MD_Project(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable, int periodicity);
    ~MD_Project();

    void destroyProject();
    void loadProject(MD_ElectromagneticBlock fixed);
    void loadProject(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable);
    void loadProject(MD_ElectromagneticBlock fixed, MD_ElectromagneticBlock movable, int periodicity);
    void setPeriodicities(int p) { Periodicities = p;}
    void updateMovableBlockPosition(double tX, double rY) { MovableBlock.Position_tX = tX; MovableBlock.Position_rY = rY; }
    MD_ElectromagneticBlock getFixedBlock() { return FixedBlock; }
    MD_ElectromagneticBlock getMovableBlock() { return MovableBlock; }
    std::vector<ReportDataSolver> solve(double timeStep, unsigned steps, double relaxation = 0.1, unsigned iterations = 100, double error = 0.01);
    void solveField(double relaxation = 0.1, unsigned iterations = 100, double error = 0.01);

private:
    void destroyBlock(MD_ElectromagneticBlock &block);
    void preProcessing();
    void fastPostProcessing();
    void postProcessing();
    void setPreviousValues();
    void setMagneticScalarPotentials(MD_ElectromagneticBlock &block);
    void setElectricCurrentsAndMMFs(MD_ElectromagneticBlock &block);
    void setVoltages(MD_ElectromagneticBlock &block);
    void computeTorque(MD_ElectromagneticBlock &block);
    void computeIronLosses(MD_ElectromagneticBlock &block);
    void computePermanentMagnetLosses(MD_ElectromagneticBlock &block);
    void assembleProject();
    void assembleMagneticCircuitDomain(MD_ElectromagneticBlock block);
    void assembleElectricCircuitDomain(MD_ElectromagneticBlock block);
    void updateSignals();
    void updateMechanics();
    void saveData(std::vector<double> &data);
    int SuperLU(std::vector<MEC_RealEquation> equations, std::vector<double> &solutions);
    unsigned initiateMagneticScalarPotentialIndexes(MD_ElectromagneticBlock &block, unsigned offset);
    unsigned initiateMeshCurrentIndexes(MD_ElectromagneticBlock &block, unsigned offset);
    void initiateMMFSources(MD_ElectromagneticBlock &block);
    MD_ElectromagneticBlock FixedBlock;
    MD_ElectromagneticBlock MovableBlock;
    MD_AirgapStructure airgapStructure;

    bool isNonlinear;
    bool isSolved;
    int Periodicities;
    double TimeStep;
    unsigned CurrentStep, Steps;
    unsigned UnknownsCount;
    DSuperLU_Data SuperLUData;
    std::vector<MEC_RealEquation> SystemOfEquations;
    std::vector<double> UnknownsVector;
};

#endif // MAGNETODYNAMICS_TOOL_H
