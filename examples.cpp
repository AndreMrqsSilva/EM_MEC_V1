#include "examples.h"

// |------||------||------|
// | E_C1 || E_M1 || E_C2 |
// |------||------||------|
// |------||------||------|
// | E_C3 || E_A1 || E_C4 |
// |------||------||------|
// |------||------||------|
// | E_C5 || E_C6 || E_C7 |
// |------||------||------|

void test_MD_CoreMagnet()
{
    MD_Project project;
    MEC_ElementGeometry geometry_core, geometry_magnet;
    MD_ElectromagneticBlock device;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    geometry_core.setGeometryRectangle(0.01, 0.035, 0.01);
    geometry_magnet.setGeometryRectangle(0.01, 0.01, 0.01);

    // Device setup
    device.MaterialNonLinear = coreMaterial;
    device.Length = 0.01;
    device.Position_tX = 0.015; device.Position_rY = 0.015;
    device.TangentialElementsCount = 3;
    device.RadialElementsCount = 3;

    // Set core elements
    geometry_core.setPosition(0.005, 0.025);
    MD_ReluctanceElement* E_C1 = new MD_ReluctanceElement(RET_coreNonLinear, geometry_core, coreMaterial.getLinearReluctivity(), "CORE_1");
    device.ReluctanceElements.push_back(E_C1);
    device.NonlinearReluctanceElements.push_back(E_C1);
    geometry_core.setPosition(0.005, 0.025);
    MD_ReluctanceElement* E_C2 = new MD_ReluctanceElement(RET_coreNonLinear, geometry_core, coreMaterial.getLinearReluctivity(), "CORE_2");
    device.ReluctanceElements.push_back(E_C2);
    device.NonlinearReluctanceElements.push_back(E_C2);

    // Set permanent magnet elements
    geometry_magnet.setPosition(0.015, 0.025);
    MD_ReluctanceElement* E_M1 = new MD_ReluctanceElement(RET_magnet, geometry_magnet, 2.8, MMFSO_tangential, 1, 1.0 / (U_AIR * 1.04), "MAGNET_1");
    device.ReluctanceElements.push_back(E_M1);
    device.MagnetElements.push_back(E_M1);

    // Connect elements
    E_C1->connect(E_C2, nullptr, E_M1, nullptr);
    E_C2->connect(E_M1, nullptr, E_C1, nullptr);
    E_M1->connect(E_C1, nullptr, E_C2, nullptr);

    project.loadProject(device);
    project.solve(0.0, 1, 0.1, 100, 0.0001);

//    MD_Project project;
//    MEC_ElementGeometry geometry;
//    MD_ElectromagneticBlock device;
//    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

//    geometry.setGeometryRectangle(0.01, 0.01, 0.01);

//    // Device setup
//    device.MaterialNonLinear = coreMaterial;
//    device.Length = 0.01;
//    device.Position_tX = 0.015; device.Position_rY = 0.015;
//    device.TangentialElementsCount = 3;
//    device.RadialElementsCount = 3;

//    // Set core elements
//    geometry.setPosition(0.005, 0.025);
//    MD_ReluctanceElement* E_C1 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_1");
//    device.ReluctanceElements.push_back(E_C1);
//    device.NonlinearReluctanceElements.push_back(E_C1);
//    geometry.setPosition(0.025, 0.025);
//    MD_ReluctanceElement* E_C2 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_2");
//    device.ReluctanceElements.push_back(E_C2);
//    device.NonlinearReluctanceElements.push_back(E_C2);
//    geometry.setPosition(0.005, 0.015);
//    MD_ReluctanceElement* E_C3 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_3");
//    device.ReluctanceElements.push_back(E_C3);
//    device.NonlinearReluctanceElements.push_back(E_C3);
//    geometry.setPosition(0.025, 0.015);
//    MD_ReluctanceElement* E_C4 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_4");
//    device.ReluctanceElements.push_back(E_C4);
//    device.NonlinearReluctanceElements.push_back(E_C4);
//    geometry.setPosition(0.005, 0.005);
//    MD_ReluctanceElement* E_C5 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_5");
//    device.ReluctanceElements.push_back(E_C5);
//    device.NonlinearReluctanceElements.push_back(E_C5);
//    geometry.setPosition(0.015, 0.005);
//    MD_ReluctanceElement* E_C6 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_6");
//    device.ReluctanceElements.push_back(E_C6);
//    device.NonlinearReluctanceElements.push_back(E_C6);
//    geometry.setPosition(0.025, 0.005);
//    MD_ReluctanceElement* E_C7 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_7");
//    device.ReluctanceElements.push_back(E_C7);
//    device.NonlinearReluctanceElements.push_back(E_C7);

//    // Set air elements
//    geometry.setPosition(0.015, 0.015);
//    MD_ReluctanceElement* E_A1 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_1");
//    device.ReluctanceElements.push_back(E_A1);

//    // Set permanent magnet elements
//    geometry.setPosition(0.015, 0.025);
//    MD_ReluctanceElement* E_M1 = new MD_ReluctanceElement(RET_magnet, geometry, 1.1, MMFSO_tangential, 1, 1.0 / (U_AIR * 1.04), "MAGNET_1");
//    device.ReluctanceElements.push_back(E_M1);
//    device.MagnetElements.push_back(E_M1);

//    // Connect elements
//    E_C1->connect(nullptr, nullptr, E_M1, E_C3);
//    E_C2->connect(E_M1, nullptr, nullptr, E_C4);
//    E_C3->connect(nullptr, E_C1, E_A1, E_C5);
//    E_C4->connect(E_A1, E_C2, nullptr, E_C7);
//    E_C5->connect(nullptr, E_C3, E_C6, nullptr);
//    E_C6->connect(E_C5, E_A1, E_C7, nullptr);
//    E_C7->connect(E_C6, E_C4, nullptr, nullptr);
//    E_A1->connect(E_C3, E_M1, E_C4, E_C6);
//    E_M1->connect(E_C1, nullptr, E_C2, nullptr);

//    project.loadProject(device);
//    project.solveDCMagnetostatics(0.7, 200);
}


// |------||------||------|
// | E_C1 || E_C2 || E_C3 |
// |------||------||------|
// |------||------||------|
// | E_C4 || E_B1 || E_C5 |
// |------||------||------|
// |------||------||------|
// | E_C6 || E_C7 || E_C8 |
// |------||------||------|

void test_MD_CoreCoil()
{
    MD_Project project;
    MEC_ElementGeometry geometry;
    MD_ElectromagneticBlock device;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    geometry.setGeometryRectangle(0.01, 0.01, 0.01);

    // Device setup
    device.MaterialNonLinear = coreMaterial;
    device.Length = 0.01;
    device.Position_tX = 0.015; device.Position_rY = 0.015;
    device.TangentialElementsCount = 3;
    device.RadialElementsCount = 3;

    // Set core elements
    geometry.setPosition(0.005, 0.025);
    MD_ReluctanceElement* E_C1 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_1");
    device.ReluctanceElements.push_back(E_C1);
    device.NonlinearReluctanceElements.push_back(E_C1);
    geometry.setPosition(0.015, 0.025);
    MD_ReluctanceElement* E_C2 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_2");
    device.ReluctanceElements.push_back(E_C2);
    device.NonlinearReluctanceElements.push_back(E_C2);
    geometry.setPosition(0.025, 0.025);
    MD_ReluctanceElement* E_C3 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_3");
    device.ReluctanceElements.push_back(E_C3);
    device.NonlinearReluctanceElements.push_back(E_C3);
    geometry.setPosition(0.005, 0.015);
    MD_ReluctanceElement* E_C4 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_4");
    device.ReluctanceElements.push_back(E_C4);
    device.NonlinearReluctanceElements.push_back(E_C4);
    geometry.setPosition(0.025, 0.015);
    MD_ReluctanceElement* E_C5 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_5");
    device.ReluctanceElements.push_back(E_C5);
    device.NonlinearReluctanceElements.push_back(E_C5);
    geometry.setPosition(0.005, 0.005);
    MD_ReluctanceElement* E_C6 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_6");
    device.ReluctanceElements.push_back(E_C6);
    device.NonlinearReluctanceElements.push_back(E_C6);
    geometry.setPosition(0.015, 0.005);
    MD_ReluctanceElement* E_C7 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_7");
    device.ReluctanceElements.push_back(E_C7);
    device.NonlinearReluctanceElements.push_back(E_C7);
    geometry.setPosition(0.025, 0.005);
    MD_ReluctanceElement* E_C8 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_8");
    device.ReluctanceElements.push_back(E_C8);
    device.NonlinearReluctanceElements.push_back(E_C8);

    // Set coil elements
    geometry.setPosition(0.015, 0.015);
    MD_ReluctanceElement* E_B1 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_1");
    device.ReluctanceElements.push_back(E_B1);

    // Connect elements
    E_C1->connect(nullptr, nullptr, E_C2, E_C4);
    E_C2->connect(E_C1, nullptr, E_C3, E_B1);
    E_C3->connect(E_C2, nullptr, nullptr, E_C5);
    E_C4->connect(nullptr, E_C1, E_B1, E_C6);
    E_C5->connect(E_B1, E_C3, nullptr, E_C8);
    E_C6->connect(nullptr, E_C4, E_C7, nullptr);
    E_C7->connect(E_C6, E_B1, E_C8, nullptr);
    E_C8->connect(E_C7, E_C5, nullptr, nullptr);
    E_B1->connect(E_C4, E_C2, E_C5, E_C7);

    // Electric circuit
    unsigned steps = 201;
    unsigned periodSteps = 40; // 5 periods
    double timeStep = 0.0005;

    MEC_SignalSource signal;
    for (unsigned t = 0; t < periodSteps; ++t) {
        double time = t * timeStep;
        signal.addPulse(5.0 * M_SQRT2 * std::sin(M_2PI * 50 * time));
    }
    MD_ElectricCircuitComponent *voltageSource = new MD_ElectricCircuitComponent;
    voltageSource->setVolatageSource(signal,"VOLTAGE_SOURCE");
    device.VoltageSourceComponents.push_back(voltageSource);

    MD_CoilSide coilside;
    coilside.setTurns(100);
    coilside.setPolarity(1);
    coilside.addActiveElement(E_B1);
    coilside.addPassiveElement(E_C2);
    MD_ElectricCircuitComponent *coil = new MD_ElectricCircuitComponent;
    coil->setCoil(coilside, "COIL", 1.0, 0.0005);
    device.CoilComponents.push_back(coil);

    MD_ElectricCircuit electricCircuit;
    electricCircuit.addComponent(voltageSource, -1);
    electricCircuit.addComponent(coil, 1);

    device.ElectricCircuits.push_back(electricCircuit);

    project.loadProject(device);
    project.solve(timeStep, steps, 0.28, 150, 0.001);
}

void test_MD_SinglePhaseTransformer()
{
    MD_Project project;
    MEC_ElementGeometry geometry;
    MD_ElectromagneticBlock device;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    geometry.setGeometryRectangle(0.01, 0.01, 0.01);

    // Device setup
    device.MaterialNonLinear = coreMaterial;
    device.Length = 0.01;
    device.Position_tX = 0.015; device.Position_rY = 0.015;
    device.TangentialElementsCount = 3;
    device.RadialElementsCount = 3;

    // Set core elements
    geometry.setPosition(0.005, 0.025);
    MD_ReluctanceElement* E_C1 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_1");
    device.ReluctanceElements.push_back(E_C1);
    device.NonlinearReluctanceElements.push_back(E_C1);
    geometry.setPosition(0.015, 0.025);
    MD_ReluctanceElement* E_C2 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_2");
    device.ReluctanceElements.push_back(E_C2);
    device.NonlinearReluctanceElements.push_back(E_C2);
    geometry.setPosition(0.025, 0.025);
    MD_ReluctanceElement* E_C3 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_3");
    device.ReluctanceElements.push_back(E_C3);
    device.NonlinearReluctanceElements.push_back(E_C3);
    geometry.setPosition(0.005, 0.015);
    MD_ReluctanceElement* E_C4 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_4");
    device.ReluctanceElements.push_back(E_C4);
    device.NonlinearReluctanceElements.push_back(E_C4);
    geometry.setPosition(0.025, 0.015);
    MD_ReluctanceElement* E_C5 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_5");
    device.ReluctanceElements.push_back(E_C5);
    device.NonlinearReluctanceElements.push_back(E_C5);
    geometry.setPosition(0.005, 0.005);
    MD_ReluctanceElement* E_C6 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_6");
    device.ReluctanceElements.push_back(E_C6);
    device.NonlinearReluctanceElements.push_back(E_C6);
    geometry.setPosition(0.015, 0.005);
    MD_ReluctanceElement* E_C7 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_7");
    device.ReluctanceElements.push_back(E_C7);
    device.NonlinearReluctanceElements.push_back(E_C7);
    geometry.setPosition(0.025, 0.005);
    MD_ReluctanceElement* E_C8 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_8");
    device.ReluctanceElements.push_back(E_C8);
    device.NonlinearReluctanceElements.push_back(E_C8);

    // Set coil elements
    geometry.setPosition(0.015, 0.015);
    MD_ReluctanceElement* E_B1 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_1");
    device.ReluctanceElements.push_back(E_B1);

    // Connect elements
    E_C1->connect(nullptr, nullptr, E_C2, E_C4);
    E_C2->connect(E_C1, nullptr, E_C3, E_B1);
    E_C3->connect(E_C2, nullptr, nullptr, E_C5);
    E_C4->connect(nullptr, E_C1, E_B1, E_C6);
    E_C5->connect(E_B1, E_C3, nullptr, E_C8);
    E_C6->connect(nullptr, E_C4, E_C7, nullptr);
    E_C7->connect(E_C6, E_B1, E_C8, nullptr);
    E_C8->connect(E_C7, E_C5, nullptr, nullptr);
    E_B1->connect(E_C4, E_C2, E_C5, E_C7);

    // Electric circuit
    unsigned steps = 201;
    unsigned periodSteps = 40; // 5 periods
    double timeStep = 0.0005;

    MEC_SignalSource signal;
    for (unsigned t = 0; t < periodSteps; ++t) {
        double time = t * timeStep;
        signal.addPulse(5 * M_SQRT2 * std::sin(M_2PI * 50 * time));
    }
    MD_ElectricCircuitComponent *voltageSource = new MD_ElectricCircuitComponent;
    voltageSource->setVolatageSource(signal,"VOLTAGE_SOURCE");
    device.VoltageSourceComponents.push_back(voltageSource);
    MD_CoilSide coilsidePrimary;
    coilsidePrimary.setTurns(100);
    coilsidePrimary.setPolarity(1);
    coilsidePrimary.addActiveElement(E_B1);
    coilsidePrimary.addPassiveElement(E_C2);
    MD_ElectricCircuitComponent *primary = new MD_ElectricCircuitComponent;
    primary->setCoil(coilsidePrimary, "COIL_PRIMARY", 1.0, 0.0005);
    device.CoilComponents.push_back(primary);
    MD_ElectricCircuit electricCircuitPrimary;
    electricCircuitPrimary.addComponent(voltageSource, -1);
    electricCircuitPrimary.addComponent(primary, 1);
    device.ElectricCircuits.push_back(electricCircuitPrimary);

    MD_CoilSide coilsideSecondary;
    coilsideSecondary.setTurns(30);
    coilsideSecondary.setPolarity(1);
    coilsideSecondary.addActiveElement(E_B1);
    coilsideSecondary.addPassiveElement(E_C2);
    MD_ElectricCircuitComponent *secondary = new MD_ElectricCircuitComponent;
    secondary->setCoil(coilsideSecondary, "COIL_SECONDARY", 0.5, 0.00005);
    device.CoilComponents.push_back(secondary);
    MD_ElectricCircuit electricCircuitSecondary;
    electricCircuitSecondary.addComponent(secondary, 1);
    device.ElectricCircuits.push_back(electricCircuitSecondary);

    project.loadProject(device);
    project.solve(timeStep, steps, 0.2, 150, 0.001);
}

void test_MD_ThreePhaseTransformer()
{
    MD_Project project;
    MEC_ElementGeometry geometry;
    MD_ElectromagneticBlock device;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    geometry.setGeometryRectangle(0.01, 0.01, 0.01);

    // Device setup
    device.MaterialNonLinear = coreMaterial;
    device.Length = 0.01;
    device.Position_tX = 0.0; device.Position_rY = 0.0;
    device.TangentialElementsCount = 9;
    device.RadialElementsCount = 3;

    // Set core elements
    geometry.setPosition(-0.02, 0.01);
    MD_ReluctanceElement* E_C1 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_1");
    device.ReluctanceElements.push_back(E_C1);
    device.NonlinearReluctanceElements.push_back(E_C1);
    geometry.setPosition(-0.01, 0.01);
    MD_ReluctanceElement* E_C2 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_2");
    device.ReluctanceElements.push_back(E_C2);
    device.NonlinearReluctanceElements.push_back(E_C2);
    geometry.setPosition(0.0, 0.01);
    MD_ReluctanceElement* E_C3 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_3");
    device.ReluctanceElements.push_back(E_C3);
    device.NonlinearReluctanceElements.push_back(E_C3);
    geometry.setPosition(0.01, 0.01);
    MD_ReluctanceElement* E_C4 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_4");
    device.ReluctanceElements.push_back(E_C4);
    device.NonlinearReluctanceElements.push_back(E_C4);
    geometry.setPosition(0.02, 0.01);
    MD_ReluctanceElement* E_C5 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_5");
    device.ReluctanceElements.push_back(E_C5);
    device.NonlinearReluctanceElements.push_back(E_C5);
    geometry.setPosition(-0.02, 0.0);
    MD_ReluctanceElement* E_C6 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_6");
    device.ReluctanceElements.push_back(E_C6);
    device.NonlinearReluctanceElements.push_back(E_C6);
    geometry.setPosition(0.0, 0.0);
    MD_ReluctanceElement* E_C7 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_7");
    device.ReluctanceElements.push_back(E_C7);
    device.NonlinearReluctanceElements.push_back(E_C7);
    geometry.setPosition(0.02, 0.0);
    MD_ReluctanceElement* E_C8 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_8");
    device.ReluctanceElements.push_back(E_C8);
    device.NonlinearReluctanceElements.push_back(E_C8);
    geometry.setPosition(-0.02, -0.01);
    MD_ReluctanceElement* E_C9 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_9");
    device.ReluctanceElements.push_back(E_C9);
    device.NonlinearReluctanceElements.push_back(E_C9);
    geometry.setPosition(-0.01, -0.01);
    MD_ReluctanceElement* E_C10 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_10");
    device.ReluctanceElements.push_back(E_C10);
    device.NonlinearReluctanceElements.push_back(E_C10);
    geometry.setPosition(0.0, -0.01);
    MD_ReluctanceElement* E_C11 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_11");
    device.ReluctanceElements.push_back(E_C11);
    device.NonlinearReluctanceElements.push_back(E_C11);
    geometry.setPosition(0.01, -0.01);
    MD_ReluctanceElement* E_C12 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_12");
    device.ReluctanceElements.push_back(E_C12);
    device.NonlinearReluctanceElements.push_back(E_C12);
    geometry.setPosition(0.02, -0.01);
    MD_ReluctanceElement* E_C13 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_13");
    device.ReluctanceElements.push_back(E_C13);
    device.NonlinearReluctanceElements.push_back(E_C13);

    // Set air elements
    geometry.setPosition(-0.04, 0.01);
    MD_ReluctanceElement* E_A1 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_1");
    device.ReluctanceElements.push_back(E_A1);
    geometry.setPosition(-0.03, 0.01);
    MD_ReluctanceElement* E_A2 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_2");
    device.ReluctanceElements.push_back(E_A2);
    geometry.setPosition(0.03, 0.01);
    MD_ReluctanceElement* E_A3 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_3");
    device.ReluctanceElements.push_back(E_A3);
    geometry.setPosition(0.04, 0.01);
    MD_ReluctanceElement* E_A4 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_4");
    device.ReluctanceElements.push_back(E_A4);
    geometry.setPosition(-0.04, 0.0);
    MD_ReluctanceElement* E_A5 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_5");
    device.ReluctanceElements.push_back(E_A5);
    geometry.setPosition(0.04, 0.0);
    MD_ReluctanceElement* E_A6 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_6");
    device.ReluctanceElements.push_back(E_A6);
    geometry.setPosition(-0.04, -0.01);
    MD_ReluctanceElement* E_A7 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_7");
    device.ReluctanceElements.push_back(E_A7);
    geometry.setPosition(-0.03, -0.01);
    MD_ReluctanceElement* E_A8 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_8");
    device.ReluctanceElements.push_back(E_A8);
    geometry.setPosition(0.03, -0.01);
    MD_ReluctanceElement* E_A9 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_9");
    device.ReluctanceElements.push_back(E_A9);
    geometry.setPosition(0.04, -0.01);
    MD_ReluctanceElement* E_A10 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_10");
    device.ReluctanceElements.push_back(E_A10);

    // Set coil elements
    geometry.setPosition(-0.03, 0.0);
    MD_ReluctanceElement* E_B1 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_1");
    device.ReluctanceElements.push_back(E_B1);
    geometry.setPosition(-0.01, 0.0);
    MD_ReluctanceElement* E_B2 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_2");
    device.ReluctanceElements.push_back(E_B2);
    geometry.setPosition(0.01, 0.0);
    MD_ReluctanceElement* E_B3 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_3");
    device.ReluctanceElements.push_back(E_B3);
    geometry.setPosition(0.03, 0.0);
    MD_ReluctanceElement* E_B4 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_4");
    device.ReluctanceElements.push_back(E_B4);

    // Connect elements
    E_C1->connect(E_A2, nullptr, E_C2, E_C6);
    E_C2->connect(E_C1, nullptr, E_C3, E_B2);
    E_C3->connect(E_C2, nullptr, E_C4, E_C7);
    E_C4->connect(E_C3, nullptr, E_C5 ,E_B3);
    E_C5->connect(E_C4, nullptr, E_A3, E_C8);
    E_C6->connect(E_B1, E_C1, E_B2, E_C9);
    E_C7->connect(E_B2, E_C3, E_B3, E_C11);
    E_C8->connect(E_B3, E_C5, E_B4, E_C13);
    E_C9->connect(E_A8, E_C6, E_C10, nullptr);
    E_C10->connect(E_C9, E_B2, E_C11, nullptr);
    E_C11->connect(E_C10, E_C7, E_C12, nullptr);
    E_C12->connect(E_C11, E_B3, E_C13, nullptr);
    E_C13->connect(E_C12, E_C8, E_A9, nullptr);
    E_A1->connect(nullptr, nullptr, E_A2, E_A5);
    E_A2->connect(E_A1, nullptr, E_C1, E_B1);
    E_A3->connect(E_C5, nullptr, E_A4, E_B4);
    E_A4->connect(E_A3, nullptr, nullptr, E_A6);
    E_A5->connect(nullptr, E_A1, E_B1, E_A7);
    E_A6->connect(E_B4, E_A4, nullptr, E_A10);
    E_A7->connect(nullptr, E_A5, E_A8, nullptr);
    E_A8->connect(E_A7, E_B1, E_C9, nullptr);
    E_A9->connect(E_C13, E_B4, E_A10, nullptr);
    E_A10->connect(E_A9, E_A6, nullptr, nullptr);
    E_B1->connect(E_A5, E_A2, E_C6, E_A8);
    E_B2->connect(E_C6, E_C2, E_C7, E_C10);
    E_B3->connect(E_C7, E_C4, E_C8, E_C12);
    E_B4->connect(E_C8, E_A3, E_A6, E_A9);

    // Electric circuits
    unsigned steps = 201;
    unsigned periodSteps = 40; // 5 periods
    double timeStep = 0.0005;

    // Definition of the voltage sources
    MEC_SignalSource signal_V1, signal_V2, signal_V3;
    for (unsigned t = 0; t < periodSteps; ++t) {
        double time = t * timeStep;
        signal_V1.addPulse(5.0 * M_SQRT2 * std::sin((M_2PI * 50 * time)));
        signal_V2.addPulse(5.0 * M_SQRT2 * std::sin((M_2PI * 50 * time) - (M_2PI / 3.0)));
        signal_V3.addPulse(5.0 * M_SQRT2 * std::sin((M_2PI * 50 * time) - (2.0 * M_2PI / 3.0)));
    }
    MD_ElectricCircuitComponent *voltageSource_V1 = new MD_ElectricCircuitComponent;
    voltageSource_V1->setVolatageSource(signal_V1,"VOLTAGE_SOURCE_V1");
    device.VoltageSourceComponents.push_back(voltageSource_V1);
    MD_ElectricCircuitComponent *voltageSource_V2 = new MD_ElectricCircuitComponent;
    voltageSource_V2->setVolatageSource(signal_V2,"VOLTAGE_SOURCE_V2");
    device.VoltageSourceComponents.push_back(voltageSource_V2);
    MD_ElectricCircuitComponent *voltageSource_V3 = new MD_ElectricCircuitComponent;
    voltageSource_V3->setVolatageSource(signal_V3,"VOLTAGE_SOURCE_V3");
    device.VoltageSourceComponents.push_back(voltageSource_V3);

    // Definition of the coil sides
    MD_CoilSide coilsideB1_PP; // Primary phase A positive
    coilsideB1_PP.setTurns(100);
    coilsideB1_PP.setPolarity(1);
    coilsideB1_PP.addActiveElement(E_B1);
    coilsideB1_PP.addPassiveElement(E_A2);
    MD_CoilSide coilsideB1_PN; // Primary phase A negative
    coilsideB1_PN.setTurns(100);
    coilsideB1_PN.setPolarity(-1);
    coilsideB1_PN.addActiveElement(E_B2);
    coilsideB1_PN.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_PP; // Primary phase B positive
    coilsideB2_PP.setTurns(100);
    coilsideB2_PP.setPolarity(1);
    coilsideB2_PP.addActiveElement(E_B2);
    coilsideB2_PP.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_PN; // Primary phase B negative
    coilsideB2_PN.setTurns(100);
    coilsideB2_PN.setPolarity(-1);
    coilsideB2_PN.addActiveElement(E_B3);
    coilsideB2_PN.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_PP; // Primary phase C positive
    coilsideB3_PP.setTurns(100);
    coilsideB3_PP.setPolarity(1);
    coilsideB3_PP.addActiveElement(E_B3);
    coilsideB3_PP.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_PN; // Primary phase C negative
    coilsideB3_PN.setTurns(100);
    coilsideB3_PN.setPolarity(-1);
    coilsideB3_PN.addActiveElement(E_B4);
    coilsideB3_PN.addPassiveElement(E_A3);
    MD_CoilSide coilsideB1_SP; // Secondary phase A positive
    coilsideB1_SP.setTurns(30);
    coilsideB1_SP.setPolarity(1);
    coilsideB1_SP.addActiveElement(E_B1);
    coilsideB1_SP.addPassiveElement(E_A2);
    MD_CoilSide coilsideB1_SN; // Secondary phase A negative
    coilsideB1_SN.setTurns(30);
    coilsideB1_SN.setPolarity(-1);
    coilsideB1_SN.addActiveElement(E_B2);
    coilsideB1_SN.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_SP; // Secondary phase B positive
    coilsideB2_SP.setTurns(30);
    coilsideB2_SP.setPolarity(1);
    coilsideB2_SP.addActiveElement(E_B2);
    coilsideB2_SP.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_SN; // Secondary phase B negative
    coilsideB2_SN.setTurns(30);
    coilsideB2_SN.setPolarity(-1);
    coilsideB2_SN.addActiveElement(E_B3);
    coilsideB2_SN.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_SP; // Secondary phase C positive
    coilsideB3_SP.setTurns(30);
    coilsideB3_SP.setPolarity(1);
    coilsideB3_SP.addActiveElement(E_B3);
    coilsideB3_SP.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_SN; // Secondary phase C negative
    coilsideB3_SN.setTurns(30);
    coilsideB3_SN.setPolarity(-1);
    coilsideB3_SN.addActiveElement(E_B4);
    coilsideB3_SN.addPassiveElement(E_A3);

    // Definition of the coils
    MD_ElectricCircuitComponent *primaryA = new MD_ElectricCircuitComponent;
    primaryA->setCoil("COIL_PRIMARY_A", 1.0, 0.0005);
    primaryA->addCoilSide(coilsideB1_PP);
    primaryA->addCoilSide(coilsideB1_PN);
    device.CoilComponents.push_back(primaryA);
    MD_ElectricCircuitComponent *primaryB = new MD_ElectricCircuitComponent;
    primaryB->setCoil("COIL_PRIMARY_B", 1.0, 0.0005);
    primaryB->addCoilSide(coilsideB2_PP);
    primaryB->addCoilSide(coilsideB2_PN);
    device.CoilComponents.push_back(primaryB);
    MD_ElectricCircuitComponent *primaryC = new MD_ElectricCircuitComponent;
    primaryC->setCoil("COIL_PRIMARY_C", 1.0, 0.0005);
    primaryC->addCoilSide(coilsideB3_PP);
    primaryC->addCoilSide(coilsideB3_PN);
    device.CoilComponents.push_back(primaryC);
    MD_ElectricCircuitComponent *secondaryA = new MD_ElectricCircuitComponent;
    secondaryA->setCoil("COIL_SECONDARY_A", 0.5, 0.00005);
    secondaryA->addCoilSide(coilsideB1_SP);
    secondaryA->addCoilSide(coilsideB1_SN);
    device.CoilComponents.push_back(secondaryA);
    MD_ElectricCircuitComponent *secondaryB = new MD_ElectricCircuitComponent;
    secondaryB->setCoil("COIL_SECONDARY_B", 0.5, 0.00005);
    secondaryB->addCoilSide(coilsideB2_SP);
    secondaryB->addCoilSide(coilsideB2_SN);
    device.CoilComponents.push_back(secondaryB);
    MD_ElectricCircuitComponent *secondaryC = new MD_ElectricCircuitComponent;
    secondaryC->setCoil("COIL_SECONDARY_C", 0.5, 0.00005);
    secondaryC->addCoilSide(coilsideB3_SP);
    secondaryC->addCoilSide(coilsideB3_SN);
    device.CoilComponents.push_back(secondaryC);

    // Primary electric circuits definition
    MD_ElectricCircuit primaryMesh1;
    primaryMesh1.addComponent(voltageSource_V1, -1);
    primaryMesh1.addComponent(primaryA, 1);
    primaryMesh1.addComponent(primaryB, -1);
    primaryMesh1.addComponent(voltageSource_V2, 1);
    device.ElectricCircuits.push_back(primaryMesh1);
    MD_ElectricCircuit primaryMesh2;
    primaryMesh2.addComponent(voltageSource_V2, -1);
    primaryMesh2.addComponent(primaryB, 1);
    primaryMesh2.addComponent(primaryC, -1);
    primaryMesh2.addComponent(voltageSource_V3, 1);
    device.ElectricCircuits.push_back(primaryMesh2);

    // Secondary electric circuits definition
    MD_ElectricCircuit secondaryMesh1;
    secondaryMesh1.addComponent(secondaryA, 1);
    secondaryMesh1.addComponent(secondaryB, -1);
    device.ElectricCircuits.push_back(secondaryMesh1);
    MD_ElectricCircuit secondaryMesh2;
    secondaryMesh2.addComponent(secondaryB, 1);
    secondaryMesh2.addComponent(secondaryC, -1);
    device.ElectricCircuits.push_back(secondaryMesh2);

    // Project solving
    project.loadProject(device);
    project.solve(timeStep, steps, 0.3, 250, 0.001);
}

void test_MD_SinglePhaseTransformerCurrentSoruce()
{
    MD_Project project;
    MEC_ElementGeometry geometry;
    MD_ElectromagneticBlock device;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    geometry.setGeometryRectangle(0.01, 0.01, 0.01);

    // Device setup
    device.MaterialNonLinear = coreMaterial;
    device.Length = 0.01;
    device.Position_tX = 0.015; device.Position_rY = 0.015;
    device.TangentialElementsCount = 3;
    device.RadialElementsCount = 3;

    // Set core elements
    geometry.setPosition(0.005, 0.025);
    MD_ReluctanceElement* E_C1 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_1");
    device.ReluctanceElements.push_back(E_C1);
    device.NonlinearReluctanceElements.push_back(E_C1);
    geometry.setPosition(0.015, 0.025);
    MD_ReluctanceElement* E_C2 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_2");
    device.ReluctanceElements.push_back(E_C2);
    device.NonlinearReluctanceElements.push_back(E_C2);
    geometry.setPosition(0.025, 0.025);
    MD_ReluctanceElement* E_C3 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_3");
    device.ReluctanceElements.push_back(E_C3);
    device.NonlinearReluctanceElements.push_back(E_C3);
    geometry.setPosition(0.005, 0.015);
    MD_ReluctanceElement* E_C4 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_4");
    device.ReluctanceElements.push_back(E_C4);
    device.NonlinearReluctanceElements.push_back(E_C4);
    geometry.setPosition(0.025, 0.015);
    MD_ReluctanceElement* E_C5 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_5");
    device.ReluctanceElements.push_back(E_C5);
    device.NonlinearReluctanceElements.push_back(E_C5);
    geometry.setPosition(0.005, 0.005);
    MD_ReluctanceElement* E_C6 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_6");
    device.ReluctanceElements.push_back(E_C6);
    device.NonlinearReluctanceElements.push_back(E_C6);
    geometry.setPosition(0.015, 0.005);
    MD_ReluctanceElement* E_C7 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_7");
    device.ReluctanceElements.push_back(E_C7);
    device.NonlinearReluctanceElements.push_back(E_C7);
    geometry.setPosition(0.025, 0.005);
    MD_ReluctanceElement* E_C8 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_8");
    device.ReluctanceElements.push_back(E_C8);
    device.NonlinearReluctanceElements.push_back(E_C8);

    // Set coil elements
    geometry.setPosition(0.015, 0.015);
    MD_ReluctanceElement* E_B1 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_1");
    device.ReluctanceElements.push_back(E_B1);

    // Connect elements
    E_C1->connect(nullptr, nullptr, E_C2, E_C4);
    E_C2->connect(E_C1, nullptr, E_C3, E_B1);
    E_C3->connect(E_C2, nullptr, nullptr, E_C5);
    E_C4->connect(nullptr, E_C1, E_B1, E_C6);
    E_C5->connect(E_B1, E_C3, nullptr, E_C8);
    E_C6->connect(nullptr, E_C4, E_C7, nullptr);
    E_C7->connect(E_C6, E_B1, E_C8, nullptr);
    E_C8->connect(E_C7, E_C5, nullptr, nullptr);
    E_B1->connect(E_C4, E_C2, E_C5, E_C7);

    // Electric circuit
    unsigned steps = 201;
    unsigned periodSteps = 40; // 5 periods
    double timeStep = 0.0005;

    MEC_SignalSource signal;
    for (unsigned t = 0; t < periodSteps; ++t) {
        double time = t * timeStep;
        signal.addPulse(2.0 * M_SQRT2 * std::sin(M_2PI * 50.0 * time));
    }
    MD_ElectricCircuitComponent *currentSource = new MD_ElectricCircuitComponent;
    currentSource->setCurrentSource(signal,"CURRENT_SOURCE");
    device.CurrentSourceComponents.push_back(currentSource);
    MD_CoilSide coilsidePrimary;
    coilsidePrimary.setTurns(100);
    coilsidePrimary.setPolarity(1);
    coilsidePrimary.addActiveElement(E_B1);
    coilsidePrimary.addPassiveElement(E_C2);
    MD_ElectricCircuitComponent *primary = new MD_ElectricCircuitComponent;
    primary->setCoil(coilsidePrimary, "COIL_PRIMARY", 1.0, 0.0005);
    device.CoilComponents.push_back(primary);
    MD_ElectricCircuit electricCircuitPrimary;
    electricCircuitPrimary.addComponent(currentSource, -1);
    electricCircuitPrimary.addComponent(primary, 1);
    device.ElectricCircuits.push_back(electricCircuitPrimary);

    MD_CoilSide coilsideSecondary;
    coilsideSecondary.setTurns(30);
    coilsideSecondary.setPolarity(1);
    coilsideSecondary.addActiveElement(E_B1);
    coilsideSecondary.addPassiveElement(E_C2);
    MD_ElectricCircuitComponent *secondary = new MD_ElectricCircuitComponent;
    secondary->setCoil(coilsideSecondary, "COIL_SECONDARY", 0.5, 0.00005);
    device.CoilComponents.push_back(secondary);
    MD_ElectricCircuit electricCircuitSecondary;
    electricCircuitSecondary.addComponent(secondary, 1);
    device.ElectricCircuits.push_back(electricCircuitSecondary);

    project.loadProject(device);
    project.solve(timeStep, steps, 0.2, 150, 0.001);
}

void test_MD_ThreePhaseTransformerCurrentSoruce()
{
    MD_Project project;
    MEC_ElementGeometry geometry;
    MD_ElectromagneticBlock device;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    geometry.setGeometryRectangle(0.01, 0.01, 0.01);

    // Device setup
    device.MaterialNonLinear = coreMaterial;
    device.Length = 0.01;
    device.Position_tX = 0.0; device.Position_rY = 0.0;
    device.TangentialElementsCount = 9;
    device.RadialElementsCount = 3;

    // Set core elements
    geometry.setPosition(-0.02, 0.01);
    MD_ReluctanceElement* E_C1 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_1");
    device.ReluctanceElements.push_back(E_C1);
    device.NonlinearReluctanceElements.push_back(E_C1);
    geometry.setPosition(-0.01, 0.01);
    MD_ReluctanceElement* E_C2 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_2");
    device.ReluctanceElements.push_back(E_C2);
    device.NonlinearReluctanceElements.push_back(E_C2);
    geometry.setPosition(0.0, 0.01);
    MD_ReluctanceElement* E_C3 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_3");
    device.ReluctanceElements.push_back(E_C3);
    device.NonlinearReluctanceElements.push_back(E_C3);
    geometry.setPosition(0.01, 0.01);
    MD_ReluctanceElement* E_C4 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_4");
    device.ReluctanceElements.push_back(E_C4);
    device.NonlinearReluctanceElements.push_back(E_C4);
    geometry.setPosition(0.02, 0.01);
    MD_ReluctanceElement* E_C5 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_5");
    device.ReluctanceElements.push_back(E_C5);
    device.NonlinearReluctanceElements.push_back(E_C5);
    geometry.setPosition(-0.02, 0.0);
    MD_ReluctanceElement* E_C6 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_6");
    device.ReluctanceElements.push_back(E_C6);
    device.NonlinearReluctanceElements.push_back(E_C6);
    geometry.setPosition(0.0, 0.0);
    MD_ReluctanceElement* E_C7 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_7");
    device.ReluctanceElements.push_back(E_C7);
    device.NonlinearReluctanceElements.push_back(E_C7);
    geometry.setPosition(0.02, 0.0);
    MD_ReluctanceElement* E_C8 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_8");
    device.ReluctanceElements.push_back(E_C8);
    device.NonlinearReluctanceElements.push_back(E_C8);
    geometry.setPosition(-0.02, -0.01);
    MD_ReluctanceElement* E_C9 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_9");
    device.ReluctanceElements.push_back(E_C9);
    device.NonlinearReluctanceElements.push_back(E_C9);
    geometry.setPosition(-0.01, -0.01);
    MD_ReluctanceElement* E_C10 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_10");
    device.ReluctanceElements.push_back(E_C10);
    device.NonlinearReluctanceElements.push_back(E_C10);
    geometry.setPosition(0.0, -0.01);
    MD_ReluctanceElement* E_C11 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_11");
    device.ReluctanceElements.push_back(E_C11);
    device.NonlinearReluctanceElements.push_back(E_C11);
    geometry.setPosition(0.01, -0.01);
    MD_ReluctanceElement* E_C12 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_12");
    device.ReluctanceElements.push_back(E_C12);
    device.NonlinearReluctanceElements.push_back(E_C12);
    geometry.setPosition(0.02, -0.01);
    MD_ReluctanceElement* E_C13 = new MD_ReluctanceElement(RET_coreNonLinear, geometry, coreMaterial.getSaturatedReluctivity(), "CORE_13");
    device.ReluctanceElements.push_back(E_C13);
    device.NonlinearReluctanceElements.push_back(E_C13);

    // Set air elements
    geometry.setPosition(-0.04, 0.01);
    MD_ReluctanceElement* E_A1 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_1");
    device.ReluctanceElements.push_back(E_A1);
    geometry.setPosition(-0.03, 0.01);
    MD_ReluctanceElement* E_A2 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_2");
    device.ReluctanceElements.push_back(E_A2);
    geometry.setPosition(0.03, 0.01);
    MD_ReluctanceElement* E_A3 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_3");
    device.ReluctanceElements.push_back(E_A3);
    geometry.setPosition(0.04, 0.01);
    MD_ReluctanceElement* E_A4 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_4");
    device.ReluctanceElements.push_back(E_A4);
    geometry.setPosition(-0.04, 0.0);
    MD_ReluctanceElement* E_A5 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_5");
    device.ReluctanceElements.push_back(E_A5);
    geometry.setPosition(0.04, 0.0);
    MD_ReluctanceElement* E_A6 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_6");
    device.ReluctanceElements.push_back(E_A6);
    geometry.setPosition(-0.04, -0.01);
    MD_ReluctanceElement* E_A7 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_7");
    device.ReluctanceElements.push_back(E_A7);
    geometry.setPosition(-0.03, -0.01);
    MD_ReluctanceElement* E_A8 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_8");
    device.ReluctanceElements.push_back(E_A8);
    geometry.setPosition(0.03, -0.01);
    MD_ReluctanceElement* E_A9 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_9");
    device.ReluctanceElements.push_back(E_A9);
    geometry.setPosition(0.04, -0.01);
    MD_ReluctanceElement* E_A10 = new MD_ReluctanceElement(RET_air, geometry, 1.0 / U_AIR, "AIR_10");
    device.ReluctanceElements.push_back(E_A10);

    // Set coil elements
    geometry.setPosition(-0.03, 0.0);
    MD_ReluctanceElement* E_B1 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_1");
    device.ReluctanceElements.push_back(E_B1);
    geometry.setPosition(-0.01, 0.0);
    MD_ReluctanceElement* E_B2 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_2");
    device.ReluctanceElements.push_back(E_B2);
    geometry.setPosition(0.01, 0.0);
    MD_ReluctanceElement* E_B3 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_3");
    device.ReluctanceElements.push_back(E_B3);
    geometry.setPosition(0.03, 0.0);
    MD_ReluctanceElement* E_B4 = new MD_ReluctanceElement(RET_coil, geometry, 1.0 / U_AIR, "COIL_4");
    device.ReluctanceElements.push_back(E_B4);

    // Connect elements
    E_C1->connect(E_A2, nullptr, E_C2, E_C6);
    E_C2->connect(E_C1, nullptr, E_C3, E_B2);
    E_C3->connect(E_C2, nullptr, E_C4, E_C7);
    E_C4->connect(E_C3, nullptr, E_C5 ,E_B3);
    E_C5->connect(E_C4, nullptr, E_A3, E_C8);
    E_C6->connect(E_B1, E_C1, E_B2, E_C9);
    E_C7->connect(E_B2, E_C3, E_B3, E_C11);
    E_C8->connect(E_B3, E_C5, E_B4, E_C13);
    E_C9->connect(E_A8, E_C6, E_C10, nullptr);
    E_C10->connect(E_C9, E_B2, E_C11, nullptr);
    E_C11->connect(E_C10, E_C7, E_C12, nullptr);
    E_C12->connect(E_C11, E_B3, E_C13, nullptr);
    E_C13->connect(E_C12, E_C8, E_A9, nullptr);
    E_A1->connect(nullptr, nullptr, E_A2, E_A5);
    E_A2->connect(E_A1, nullptr, E_C1, E_B1);
    E_A3->connect(E_C5, nullptr, E_A4, E_B4);
    E_A4->connect(E_A3, nullptr, nullptr, E_A6);
    E_A5->connect(nullptr, E_A1, E_B1, E_A7);
    E_A6->connect(E_B4, E_A4, nullptr, E_A10);
    E_A7->connect(nullptr, E_A5, E_A8, nullptr);
    E_A8->connect(E_A7, E_B1, E_C9, nullptr);
    E_A9->connect(E_C13, E_B4, E_A10, nullptr);
    E_A10->connect(E_A9, E_A6, nullptr, nullptr);
    E_B1->connect(E_A5, E_A2, E_C6, E_A8);
    E_B2->connect(E_C6, E_C2, E_C7, E_C10);
    E_B3->connect(E_C7, E_C4, E_C8, E_C12);
    E_B4->connect(E_C8, E_A3, E_A6, E_A9);

    // Electric circuits
    unsigned steps = 201;
    unsigned periodSteps = 40; // 5 periods
    double timeStep = 0.0005;

    // Definition of the voltage sources
    MEC_SignalSource signal_V1, signal_V2, signal_V3;
    for (unsigned t = 0; t < periodSteps; ++t) {
        double time = t * timeStep;
        signal_V1.addPulse(2.0 * M_SQRT2 * std::sin((M_2PI * 50 * time)));
        signal_V2.addPulse(2.0 * M_SQRT2 * std::sin((M_2PI * 50 * time) - (M_2PI / 3.0)));
        signal_V3.addPulse(2.0 * M_SQRT2 * std::sin((M_2PI * 50 * time) - (2.0 * M_2PI / 3.0)));
    }
    MD_ElectricCircuitComponent *currentSource_V1 = new MD_ElectricCircuitComponent;
    currentSource_V1->setCurrentSource(signal_V1,"CURRENT_SOURCE_V1");
    device.CurrentSourceComponents.push_back(currentSource_V1);
    MD_ElectricCircuitComponent *currentSource_V2 = new MD_ElectricCircuitComponent;
    currentSource_V2->setCurrentSource(signal_V2,"CURRENT_SOURCE_V2");
    device.CurrentSourceComponents.push_back(currentSource_V2);
    MD_ElectricCircuitComponent *currentSource_V3 = new MD_ElectricCircuitComponent;
    currentSource_V3->setCurrentSource(signal_V3,"CURRENT_SOURCE_V3");
    device.CurrentSourceComponents.push_back(currentSource_V3);

    // Definition of the coil sides
    MD_CoilSide coilsideB1_PP; // Primary phase A positive
    coilsideB1_PP.setTurns(100);
    coilsideB1_PP.setPolarity(1);
    coilsideB1_PP.addActiveElement(E_B1);
    coilsideB1_PP.addPassiveElement(E_A2);
    MD_CoilSide coilsideB1_PN; // Primary phase A negative
    coilsideB1_PN.setTurns(100);
    coilsideB1_PN.setPolarity(-1);
    coilsideB1_PN.addActiveElement(E_B2);
    coilsideB1_PN.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_PP; // Primary phase B positive
    coilsideB2_PP.setTurns(100);
    coilsideB2_PP.setPolarity(1);
    coilsideB2_PP.addActiveElement(E_B2);
    coilsideB2_PP.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_PN; // Primary phase B negative
    coilsideB2_PN.setTurns(100);
    coilsideB2_PN.setPolarity(-1);
    coilsideB2_PN.addActiveElement(E_B3);
    coilsideB2_PN.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_PP; // Primary phase C positive
    coilsideB3_PP.setTurns(100);
    coilsideB3_PP.setPolarity(1);
    coilsideB3_PP.addActiveElement(E_B3);
    coilsideB3_PP.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_PN; // Primary phase C negative
    coilsideB3_PN.setTurns(100);
    coilsideB3_PN.setPolarity(-1);
    coilsideB3_PN.addActiveElement(E_B4);
    coilsideB3_PN.addPassiveElement(E_A3);
    MD_CoilSide coilsideB1_SP; // Secondary phase A positive
    coilsideB1_SP.setTurns(30);
    coilsideB1_SP.setPolarity(1);
    coilsideB1_SP.addActiveElement(E_B1);
    coilsideB1_SP.addPassiveElement(E_A2);
    MD_CoilSide coilsideB1_SN; // Secondary phase A negative
    coilsideB1_SN.setTurns(30);
    coilsideB1_SN.setPolarity(-1);
    coilsideB1_SN.addActiveElement(E_B2);
    coilsideB1_SN.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_SP; // Secondary phase B positive
    coilsideB2_SP.setTurns(30);
    coilsideB2_SP.setPolarity(1);
    coilsideB2_SP.addActiveElement(E_B2);
    coilsideB2_SP.addPassiveElement(E_C2);
    MD_CoilSide coilsideB2_SN; // Secondary phase B negative
    coilsideB2_SN.setTurns(30);
    coilsideB2_SN.setPolarity(-1);
    coilsideB2_SN.addActiveElement(E_B3);
    coilsideB2_SN.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_SP; // Secondary phase C positive
    coilsideB3_SP.setTurns(30);
    coilsideB3_SP.setPolarity(1);
    coilsideB3_SP.addActiveElement(E_B3);
    coilsideB3_SP.addPassiveElement(E_C4);
    MD_CoilSide coilsideB3_SN; // Secondary phase C negative
    coilsideB3_SN.setTurns(30);
    coilsideB3_SN.setPolarity(-1);
    coilsideB3_SN.addActiveElement(E_B4);
    coilsideB3_SN.addPassiveElement(E_A3);

    // Definition of the coils
    MD_ElectricCircuitComponent *primaryA = new MD_ElectricCircuitComponent;
    primaryA->setCoil("COIL_PRIMARY_A", 1.0, 0.0005);
    primaryA->addCoilSide(coilsideB1_PP);
    primaryA->addCoilSide(coilsideB1_PN);
    device.CoilComponents.push_back(primaryA);
    MD_ElectricCircuitComponent *primaryB = new MD_ElectricCircuitComponent;
    primaryB->setCoil("COIL_PRIMARY_B", 1.0, 0.0005);
    primaryB->addCoilSide(coilsideB2_PP);
    primaryB->addCoilSide(coilsideB2_PN);
    device.CoilComponents.push_back(primaryB);
    MD_ElectricCircuitComponent *primaryC = new MD_ElectricCircuitComponent;
    primaryC->setCoil("COIL_PRIMARY_C", 1.0, 0.0005);
    primaryC->addCoilSide(coilsideB3_PP);
    primaryC->addCoilSide(coilsideB3_PN);
    device.CoilComponents.push_back(primaryC);
    MD_ElectricCircuitComponent *secondaryA = new MD_ElectricCircuitComponent;
    secondaryA->setCoil("COIL_SECONDARY_A", 0.5, 0.00005);
    secondaryA->addCoilSide(coilsideB1_SP);
    secondaryA->addCoilSide(coilsideB1_SN);
    device.CoilComponents.push_back(secondaryA);
    MD_ElectricCircuitComponent *secondaryB = new MD_ElectricCircuitComponent;
    secondaryB->setCoil("COIL_SECONDARY_B", 0.5, 0.00005);
    secondaryB->addCoilSide(coilsideB2_SP);
    secondaryB->addCoilSide(coilsideB2_SN);
    device.CoilComponents.push_back(secondaryB);
    MD_ElectricCircuitComponent *secondaryC = new MD_ElectricCircuitComponent;
    secondaryC->setCoil("COIL_SECONDARY_C", 0.5, 0.00005);
    secondaryC->addCoilSide(coilsideB3_SP);
    secondaryC->addCoilSide(coilsideB3_SN);
    device.CoilComponents.push_back(secondaryC);

    // Primary electric circuits definition
    MD_ElectricCircuit primaryMesh1;
    primaryMesh1.addComponent(currentSource_V1, -1);
    primaryMesh1.addComponent(primaryA, 1);
    primaryMesh1.addComponent(primaryB, -1);
    primaryMesh1.addComponent(currentSource_V2, 1);
    device.ElectricCircuits.push_back(primaryMesh1);
    MD_ElectricCircuit primaryMesh2;
    primaryMesh2.addComponent(currentSource_V2, -1);
    primaryMesh2.addComponent(primaryB, 1);
    primaryMesh2.addComponent(primaryC, -1);
    primaryMesh2.addComponent(currentSource_V3, 1);
    device.ElectricCircuits.push_back(primaryMesh2);

    // Secondary electric circuits definition
    MD_ElectricCircuit secondaryMesh1;
    secondaryMesh1.addComponent(secondaryA, 1);
    secondaryMesh1.addComponent(secondaryB, -1);
    device.ElectricCircuits.push_back(secondaryMesh1);
    MD_ElectricCircuit secondaryMesh2;
    secondaryMesh2.addComponent(secondaryB, 1);
    secondaryMesh2.addComponent(secondaryC, -1);
    device.ElectricCircuits.push_back(secondaryMesh2);

    // Project solving
    project.loadProject(device);
    project.solve(timeStep, steps, 0.3, 250, 0.001);
}

MD_Project test_MD_SynIPM_CoggingTorque()
{
    MD_Project project;
    MD_ElectromagneticBlock stator, rotor;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Stator geometry definition                                         * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    stator.MaterialNonLinear = coreMaterial;
    stator.Length = 0.075;
    stator.Position_tX = 0.0; stator.Position_rY = 0.0;
    stator.Slices = 2 * 6; // 6 slices per pole
    stator.TangentialElementsCount = 2 * stator.Slices;
    stator.RadialElementsCount = 5;
    double statorSliceSpan = M_2PI / (2.0 * 48.0);
    double statorStart = 0.1091 / 2.0;


    // Set element geometries
    std::vector<MEC_ElementGeometry> geometry_SS(5);
    geometry_SS[0].setGeometrySector(0.0936, 0.0926, 0.0214, 0.0216, 0.075);
    geometry_SS[1].setGeometrySector(0.0956, 0.0936, 0.0628, 0.0214, 0.075);
    geometry_SS[2].setGeometrySector(0.1177, 0.0956, 0.0756, 0.0628, 0.075);
    geometry_SS[3].setGeometrySector(0.1202, 0.1177, 0.0499, 0.0756, 0.075);
    geometry_SS[4].setGeometrySector(0.1410, 0.1202, 0.0652, 0.0499, 0.075);

    std::vector<MEC_ElementGeometry> geometry_ST(5);
    geometry_ST[0].setGeometrySector(0.0936, 0.0926, 0.1100, 0.1091, 0.075);
    geometry_ST[1].setGeometrySector(0.0956, 0.0936, 0.0680, 0.1100, 0.075);
    geometry_ST[2].setGeometrySector(0.1177, 0.0956, 0.0552, 0.0680, 0.075);
    geometry_ST[3].setGeometrySector(0.1202, 0.1177, 0.0807, 0.0552, 0.075);
    geometry_ST[4].setGeometrySector(0.1410, 0.1202, 0.0652, 0.0807, 0.075);

    MD_ReluctanceElement* element = nullptr;
    std::string label;
    unsigned coreElemCount = 0;
    unsigned airElemCount = 0;
    unsigned coilElemCount = 0;
    for (unsigned stack = stator.RadialElementsCount; stack > 4; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_SS[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
        }
    }
    for (unsigned stack = stator.RadialElementsCount - 1; stack > 2; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            label = ("COIL_" + std::to_string(coilElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coil, geometry_SS[stack-1], 1.0 / U_AIR, label);
            stator.ReluctanceElements.push_back(element);
        }
    }
    for (unsigned stack = stator.RadialElementsCount - 3; stack > 0; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            if (stack == 1) stator.AirgapLayerElements.push_back(element);
            label = ("AIR_" + std::to_string(airElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_air, geometry_SS[stack-1], 1.0 / U_AIR, label);
            stator.ReluctanceElements.push_back(element);
            //if (stack == 1) stator.AirgapLayerElements.push_back(element); // additional airgap layer elements (slot opening)
        }
    }

    // Connect elements
    unsigned sector = 0;
    for (unsigned stack = stator.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = stator.ReluctanceElements[sector];
        MD_ReluctanceElement *left = stator.ReluctanceElements[ ((stator.RadialElementsCount -  stack + 1) * stator.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < stator.TangentialElementsCount; ++tangential) {
            element = stator.ReluctanceElements[sector];
            if ( stack == stator.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, nullptr);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);

            }
            ++sector;
        }
    }
    for (auto sac : stator.ReluctanceElements) {
        std::string left, up, right, down;
        if ( sac->elementLeft == nullptr)
            left = "unconnected";
        else
            left = sac->elementLeft->getLabel();
        if ( sac->elementUp == nullptr)
            up = "unconnected";
        else
            up = sac->elementUp->getLabel();
        if ( sac->elementRight == nullptr)
            right = "unconnected";
        else
            right = sac->elementRight->getLabel();
        if ( sac->elementDown == nullptr)
            down = "unconnected";
        else
            down = sac->elementDown->getLabel();
//        qDebug() << QString::fromStdString(sac->getLabel()) << ":" << QString::fromStdString(left) << QString::fromStdString(up) << QString::fromStdString(right) << QString::fromStdString(down);
    }
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Rotor geometry definition                                          * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    rotor.MaterialNonLinear = coreMaterial;
    rotor.Length = 0.075;
    rotor.Position_tX = (0.1091/2.0) + ((0.0 * M_PI) / 180.0); rotor.Position_rY = 0.0;
    rotor.Slices = 2;
    rotor.TangentialElementsCount = rotor.Slices * 26;
    rotor.RadialElementsCount = 10;
    rotor.Speed_tX = (0.25 * M_PI) / 180.0;

    unsigned r_airgap_element = 0;
    unsigned r_air_element = 0;
    unsigned r_core_element = 0;
    unsigned r_PM_element = 0;
    double arc = M_2PI / (8.0 * rotor.TangentialElementsCount / rotor.Slices); // rotor element arc
    double remanentMagnetization = 1.6;

    // Airgap layer
    // r_outer = 0.0923 and r_inner = 0.092 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("AIRGAP_" + std::to_string(r_airgap_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.09200001, 0.092, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
        rotor.ReluctanceElements.push_back(element);
        rotor.AirgapLayerElements.push_back(element);
    }

    // Core external layer
    // r_outer = 0.092 (m) and r_inner = 0.091 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.092, 0.091, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }

    // Rotor active layer
    // r_outer = 0.091 (m) and r_inner = 0.0769 (m)
    double r_prev = 0.091;
    double rowArea = M_2PI * ((0.091 * 0.091) - (0.0769 * 0.0769)) / (rotor.RadialElementsCount * 8.0);
    for (unsigned r = 0; r < rotor.RadialElementsCount; ++r) {
        unsigned c_rep = 0;
        double r_curr = std::sqrt((r_prev * r_prev) - (8.0 * rowArea / M_2PI));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(r_prev, r_curr, arc, arc, rotor.Length);
        int PM_polarity = 1;
        for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
            geo.setTangentialPositionWithOffset(c * arc);
            if ( r == 0 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 1 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 2 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 3 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 4 ) {
                if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 5 ) {
                if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 6 ) {
                if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 7 ) {
                if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 8 ) {
                if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 9 ) {
                if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( c < (rotor.TangentialElementsCount / 4)) {
                if (c < ((rotor.TangentialElementsCount / 4) - 1))
                    ++c_rep;
            } else if ( c < (rotor.TangentialElementsCount / 2)) {
                if (c < ((rotor.TangentialElementsCount / 2) - 1)) {
                    --c_rep;
                    PM_polarity = -1;
                }
            } else if ( c < (3 * rotor.TangentialElementsCount / 4)) {
                if (c < (( 3* rotor.TangentialElementsCount / 4) - 1))
                    ++c_rep;
            } else {
                --c_rep;
                PM_polarity = 1;
            }
        }
        r_prev = r_curr;
    }

    // Core back iron layer 1
    // r_outer = 0.0769 (m) and r_inner = 0.07 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.0769, 0.07, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }
    // Core back iron layer 2
    // r_outer = 0.07 (m) and r_inner = 0.056 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.07, 0.056, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }
    rotor.RadialElementsCount += 4; // airgap layer + external layer + back iron layer

    // Connect elements
    sector = 0;
    for (unsigned stack = rotor.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = rotor.ReluctanceElements[sector];
        MD_ReluctanceElement *left = rotor.ReluctanceElements[ ((rotor.RadialElementsCount - stack + 1) * rotor.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < rotor.TangentialElementsCount; ++tangential) {
            MD_ReluctanceElement* element = rotor.ReluctanceElements[sector];
            if ( stack == rotor.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); // element->connect(rotor.ReluctanceElements[stackRight], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, nullptr); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], nullptr);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);

            }
            ++sector;
        }
    }

    // Solve the project
    project.loadProject(stator, rotor);
    project.solve(1.0, 31, 0.05, 250, 0.0001);

    return project;
}

MD_Project test_MD_SynIPM_BackEMF()
{
    MD_Project project;
    MD_ElectromagneticBlock stator, rotor;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Stator geometry definition                                         * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    stator.MaterialNonLinear = coreMaterial;
    stator.Length = 0.075;
    stator.Position_tX = 0.0; stator.Position_rY = 0.0;
    stator.Slices = 2 * 6; // 6 slices per pole
    stator.TangentialElementsCount = 2 * stator.Slices;
    stator.RadialElementsCount = 5;
    double statorSliceSpan = M_2PI / (2.0 * 48.0);
    double statorStart = 0.1091 / 2.0;


    // Set element geometries
    std::vector<MEC_ElementGeometry> geometry_SS(5);
    geometry_SS[0].setGeometrySector(0.0936, 0.0926, 0.0214, 0.0216, 0.075);
    geometry_SS[1].setGeometrySector(0.0956, 0.0936, 0.0628, 0.0214, 0.075);
    geometry_SS[2].setGeometrySector(0.1177, 0.0956, 0.0756, 0.0628, 0.075);
    geometry_SS[3].setGeometrySector(0.1202, 0.1177, 0.0499, 0.0756, 0.075);
    geometry_SS[4].setGeometrySector(0.1410, 0.1202, 0.0652, 0.0499, 0.075);

    std::vector<MEC_ElementGeometry> geometry_ST(5);
    geometry_ST[0].setGeometrySector(0.0936, 0.0926, 0.1100, 0.1091, 0.075);
    geometry_ST[1].setGeometrySector(0.0956, 0.0936, 0.0680, 0.1100, 0.075);
    geometry_ST[2].setGeometrySector(0.1177, 0.0956, 0.0552, 0.0680, 0.075);
    geometry_ST[3].setGeometrySector(0.1202, 0.1177, 0.0807, 0.0552, 0.075);
    geometry_ST[4].setGeometrySector(0.1410, 0.1202, 0.0652, 0.0807, 0.075);

    MD_ReluctanceElement* element = nullptr;
    std::string label;
    unsigned coreElemCount = 0;
    unsigned airElemCount = 0;
    unsigned coilElemCount = 0;
    for (unsigned stack = stator.RadialElementsCount; stack > 4; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_SS[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
        }
    }
    for (unsigned stack = stator.RadialElementsCount - 1; stack > 2; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            label = ("COIL_" + std::to_string(coilElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coil, geometry_SS[stack-1], 1.0 / U_AIR, label);
            stator.ReluctanceElements.push_back(element);
        }
    }
    for (unsigned stack = stator.RadialElementsCount - 3; stack > 0; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            if (stack == 1) stator.AirgapLayerElements.push_back(element);
            label = ("AIR_" + std::to_string(airElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_air, geometry_SS[stack-1], 1.0 / U_AIR, label);
            stator.ReluctanceElements.push_back(element);
        }
    }

    // Connect elements
    unsigned sector = 0;
    for (unsigned stack = stator.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = stator.ReluctanceElements[sector];
        MD_ReluctanceElement *left = stator.ReluctanceElements[ ((stator.RadialElementsCount -  stack + 1) * stator.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < stator.TangentialElementsCount; ++tangential) {
            element = stator.ReluctanceElements[sector];
            if ( stack == stator.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, nullptr);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);

            }
            ++sector;
        }
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Rotor geometry definition                                          * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    rotor.MaterialNonLinear = coreMaterial;
    rotor.Length = 0.075;
    rotor.Position_tX = (0.1091/2.0) + (3.0 * M_PI / 8.0);// 2.0 * (3.75 * M_PI / 180.0);
    rotor.Position_rY = 0.0;
    rotor.Slices = 2;
    rotor.TangentialElementsCount = rotor.Slices * 26;
    rotor.RadialElementsCount = 10;
    rotor.Speed_tX = (1000.0 / 60.0) * M_2PI;

    unsigned r_airgap_element = 0;
    unsigned r_air_element = 0;
    unsigned r_core_element = 0;
    unsigned r_PM_element = 0;
    double arc = M_2PI / (8.0 * rotor.TangentialElementsCount / rotor.Slices); // rotor element arc
    double remanentMagnetization = 1.2;

    // Airgap layer
    // r_outer = 0.0923 and r_inner = 0.092 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("AIRGAP_" + std::to_string(r_airgap_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.09200001, 0.092, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
        rotor.ReluctanceElements.push_back(element);
        rotor.AirgapLayerElements.push_back(element);
    }

    // Core external layer
    // r_outer = 0.092 (m) and r_inner = 0.091 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.092, 0.091, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }

    // Rotor active layer
    // r_outer = 0.091 (m) and r_inner = 0.0769 (m)
    double r_prev = 0.091;
    double rowArea = M_2PI * ((0.091 * 0.091) - (0.0769 * 0.0769)) / (rotor.RadialElementsCount * 8.0);
    for (unsigned r = 0; r < rotor.RadialElementsCount; ++r) {
        unsigned c_rep = 0;
        double r_curr = std::sqrt((r_prev * r_prev) - (8.0 * rowArea / M_2PI));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(r_prev, r_curr, arc, arc, rotor.Length);
        int PM_polarity = 1;
        for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
            geo.setTangentialPositionWithOffset(c * arc);
            if ( r == 0 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 1 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 2 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 3 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 4 ) {
                if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 5 ) {
                if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 6 ) {
                if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 7 ) {
                if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 8 ) {
                if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 9 ) {
                if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( c < (rotor.TangentialElementsCount / 4)) {
                if (c < ((rotor.TangentialElementsCount / 4) - 1))
                    ++c_rep;
            } else if ( c < (rotor.TangentialElementsCount / 2)) {
                if (c < ((rotor.TangentialElementsCount / 2) - 1)) {
                    --c_rep;
                    PM_polarity = -1;
                }
            } else if ( c < (3 * rotor.TangentialElementsCount / 4)) {
                if (c < (( 3* rotor.TangentialElementsCount / 4) - 1))
                    ++c_rep;
            } else {
                --c_rep;
                PM_polarity = 1;
            }
        }
        r_prev = r_curr;
    }

    // Core back iron layer 1
    // r_outer = 0.0769 (m) and r_inner = 0.07 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.0769, 0.07, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }
    // Core back iron layer 2
    // r_outer = 0.07 (m) and r_inner = 0.056 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.07, 0.056, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }
    rotor.RadialElementsCount += 4; // airgap layer + external layer + back iron layer

    // Connect elements
    sector = 0;
    for (unsigned stack = rotor.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = rotor.ReluctanceElements[sector];
        MD_ReluctanceElement *left = rotor.ReluctanceElements[ ((rotor.RadialElementsCount - stack + 1) * rotor.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < rotor.TangentialElementsCount; ++tangential) {
            MD_ReluctanceElement* element = rotor.ReluctanceElements[sector];
            if ( stack == rotor.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); // element->connect(rotor.ReluctanceElements[stackRight], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, nullptr); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], nullptr);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);

            }
            ++sector;
        }
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                    Electric circuit definitions                                         * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    std::vector<MD_CoilSide> CS_A(4);
    CS_A[0].setTurns(13);
    CS_A[0].setPolarity(1);
    CS_A[0].addPassiveElement(stator.ReluctanceElements[1]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[51]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[26]);
    CS_A[1].setTurns(13);
    CS_A[1].setPolarity(1);
    CS_A[1].addPassiveElement(stator.ReluctanceElements[3]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[53]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[28]);
    CS_A[2].setTurns(13);
    CS_A[2].setPolarity(-1);
    CS_A[2].addPassiveElement(stator.ReluctanceElements[13]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[63]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[38]);
    CS_A[3].setTurns(13);
    CS_A[3].setPolarity(-1);
    CS_A[3].addPassiveElement(stator.ReluctanceElements[15]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[65]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[40]);

    std::vector<MD_CoilSide> CS_B(4);
    CS_B[0].setTurns(13);
    CS_B[0].setPolarity(-1);
    CS_B[0].addPassiveElement(stator.ReluctanceElements[5]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[55]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[30]);
    CS_B[1].setTurns(13);
    CS_B[1].setPolarity(-1);
    CS_B[1].addPassiveElement(stator.ReluctanceElements[7]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[57]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[32]);
    CS_B[2].setTurns(13);
    CS_B[2].setPolarity(1);
    CS_B[2].addPassiveElement(stator.ReluctanceElements[17]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[67]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[42]);
    CS_B[3].setTurns(13);
    CS_B[3].setPolarity(1);
    CS_B[3].addPassiveElement(stator.ReluctanceElements[19]);
    CS_B[3].addActiveElement(stator.ReluctanceElements[69]);
    CS_B[3].addActiveElement(stator.ReluctanceElements[44]);

    std::vector<MD_CoilSide> CS_C(4);
    CS_C[0].setTurns(13);
    CS_C[0].setPolarity(1);
    CS_C[0].addPassiveElement(stator.ReluctanceElements[9]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[59]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[34]);
    CS_C[1].setTurns(13);
    CS_C[1].setPolarity(1);
    CS_C[1].addPassiveElement(stator.ReluctanceElements[11]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[61]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[36]);
    CS_C[2].setTurns(13);
    CS_C[2].setPolarity(-1);
    CS_C[2].addPassiveElement(stator.ReluctanceElements[21]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[71]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[46]);
    CS_C[3].setTurns(13);
    CS_C[3].setPolarity(-1);
    CS_C[3].addPassiveElement(stator.ReluctanceElements[23]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[73]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[48]);

    MD_ElectricCircuitComponent* Coil_A = new MD_ElectricCircuitComponent;
    Coil_A->setCoil(CS_A,"COIL_PHASE_A", 0.088, 0.000159);
    stator.CoilComponents.push_back(Coil_A);

    MD_ElectricCircuitComponent* Coil_B = new MD_ElectricCircuitComponent;
    Coil_B->setCoil(CS_B,"COIL_PHASE_B", 0.088, 0.000159);
    stator.CoilComponents.push_back(Coil_B);

    MD_ElectricCircuitComponent* Coil_C = new MD_ElectricCircuitComponent;
    Coil_C->setCoil(CS_C,"COIL_PHASE_C", 0.088, 0.000159);
    stator.CoilComponents.push_back(Coil_C);

    // Solve the project
    project.loadProject(stator, rotor);
    project.solve((0.5 * M_PI / 180.0) / rotor.Speed_tX , 1 * 2, 0.05, 250, 0.01);

    return project;
}

MD_Project test_MD_SynIPM_ConstantSpeed()
{
    MD_Project project;
    MD_ElectromagneticBlock stator, rotor;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Stator geometry definition                                         * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    stator.MaterialNonLinear = coreMaterial;
    stator.Length = 0.075;
    stator.Position_tX = 0.0; stator.Position_rY = 0.0;
    stator.Slices = 2 * 6; // 6 slices per pole
    stator.TangentialElementsCount = 2 * stator.Slices;
    stator.RadialElementsCount = 5;
    double statorSliceSpan = M_2PI / (2.0 * 48.0);
    double statorStart = 0.1091 / 2.0;


    // Set element geometries
    std::vector<MEC_ElementGeometry> geometry_SS(5);
    geometry_SS[0].setGeometrySector(0.0936, 0.0926, 0.0214, 0.0216, 0.075);
    geometry_SS[1].setGeometrySector(0.0956, 0.0936, 0.0628, 0.0214, 0.075);
    geometry_SS[2].setGeometrySector(0.1177, 0.0956, 0.0756, 0.0628, 0.075);
    geometry_SS[3].setGeometrySector(0.1202, 0.1177, 0.0499, 0.0756, 0.075);
    geometry_SS[4].setGeometrySector(0.1410, 0.1202, 0.0652, 0.0499, 0.075);

    std::vector<MEC_ElementGeometry> geometry_ST(5);
    geometry_ST[0].setGeometrySector(0.0936, 0.0926, 0.1100, 0.1091, 0.075);
    geometry_ST[1].setGeometrySector(0.0956, 0.0936, 0.0680, 0.1100, 0.075);
    geometry_ST[2].setGeometrySector(0.1177, 0.0956, 0.0552, 0.0680, 0.075);
    geometry_ST[3].setGeometrySector(0.1202, 0.1177, 0.0807, 0.0552, 0.075);
    geometry_ST[4].setGeometrySector(0.1410, 0.1202, 0.0652, 0.0807, 0.075);

    MD_ReluctanceElement* element = nullptr;
    std::string label;
    unsigned coreElemCount = 0;
    unsigned airElemCount = 0;
    unsigned coilElemCount = 0;
    for (unsigned stack = stator.RadialElementsCount; stack > 4; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_SS[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
        }
    }
    for (unsigned stack = stator.RadialElementsCount - 1; stack > 2; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            label = ("COIL_" + std::to_string(coilElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coil, geometry_SS[stack-1], 1.0 / U_AIR, label);
            stator.ReluctanceElements.push_back(element);
        }
    }
    for (unsigned stack = stator.RadialElementsCount - 3; stack > 0; --stack) {
        double pos = statorStart;
        for (unsigned slice = 0; slice < stator.Slices; ++slice) {
            label = ("CORE_" + std::to_string(coreElemCount++));
            geometry_ST[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_ST[stack-1], coreMaterial.getSaturatedReluctivity(), label);
            stator.ReluctanceElements.push_back(element);
            stator.NonlinearReluctanceElements.push_back(element);
            if (stack == 1) stator.AirgapLayerElements.push_back(element);
            label = ("AIR_" + std::to_string(airElemCount++));
            geometry_SS[stack-1].setTangentialPosition(pos);
            pos += statorSliceSpan;
            element = new MD_ReluctanceElement(RET_air, geometry_SS[stack-1], 1.0 / U_AIR, label);
            stator.ReluctanceElements.push_back(element);
        }
    }

    // Connect elements
    unsigned sector = 0;
    for (unsigned stack = stator.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = stator.ReluctanceElements[sector];
        MD_ReluctanceElement *left = stator.ReluctanceElements[ ((stator.RadialElementsCount -  stack + 1) * stator.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < stator.TangentialElementsCount; ++tangential) {
            element = stator.ReluctanceElements[sector];
            if ( stack == stator.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, nullptr);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);

            }
            ++sector;
        }
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Rotor geometry definition                                          * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    rotor.MaterialNonLinear = coreMaterial;
    rotor.Length = 0.075;
    //rotor.Position_tX = (0.1091/2.0) + (3.0 * M_PI / 8.0) + (7.5 * M_PI / 180.0) - (9.0 * M_PI / 180.0);
    rotor.Slices = 2;
    rotor.TangentialElementsCount = rotor.Slices * 26;
    rotor.RadialElementsCount = 10;
    rotor.Speed_tX = (1200.0 / 60.0) * M_2PI;
    rotor.Position_tX = -(13.0 * M_PI_4 / 26.0) + (0.1091/2.0);
    rotor.Position_rY = 0.0;

    unsigned r_airgap_element = 0;
    unsigned r_air_element = 0;
    unsigned r_core_element = 0;
    unsigned r_PM_element = 0;
    double arc = M_2PI / (8.0 * rotor.TangentialElementsCount / rotor.Slices); // rotor element arc
    double remanentMagnetization = 1.2;

    // Airgap layer
    // r_outer = 0.0923 and r_inner = 0.092 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("AIRGAP_" + std::to_string(r_airgap_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.09200001, 0.092, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
        rotor.ReluctanceElements.push_back(element);
        rotor.AirgapLayerElements.push_back(element);
    }

    // Core external layer
    // r_outer = 0.092 (m) and r_inner = 0.091 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.092, 0.091, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }

    // Rotor active layer
    // r_outer = 0.091 (m) and r_inner = 0.0769 (m)
    double r_prev = 0.091;
    double rowArea = M_2PI * ((0.091 * 0.091) - (0.0769 * 0.0769)) / (rotor.RadialElementsCount * 8.0);
    for (unsigned r = 0; r < rotor.RadialElementsCount; ++r) {
        unsigned c_rep = 0;
        double r_curr = std::sqrt((r_prev * r_prev) - (8.0 * rowArea / M_2PI));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(r_prev, r_curr, arc, arc, rotor.Length);
        int PM_polarity = 1;
        for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
            geo.setTangentialPositionWithOffset(c * arc);
            if ( r == 0 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 1 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 2 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 3 ) {
                if ( c_rep == 10) { // air
                    std::string label = ("AIR_" + std::to_string(r_air_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_air, geo, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 4 ) {
                if ( c_rep == 9) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 5 ) {
                if ( c_rep == 8) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 7) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 6 ) {
                if ( c_rep == 6) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 7 ) {
                if ( c_rep == 5) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 8 ) {
                if ( c_rep == 4) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 3) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( r == 9 ) {
                if ( c_rep == 2) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 1) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else if ( c_rep == 0) { // PM
                    std::string label = ("PM_" + std::to_string(r_PM_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_magnet, geo, remanentMagnetization, MMFSO_radial, PM_polarity, 1.0 / (U_AIR * 1.05), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.MagnetElements.push_back(element);
                } else {
                    std::string label = ("CORE_" + std::to_string(r_core_element++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            }
            if ( c < (rotor.TangentialElementsCount / 4)) {
                if (c < ((rotor.TangentialElementsCount / 4) - 1))
                    ++c_rep;
            } else if ( c < (rotor.TangentialElementsCount / 2)) {
                if (c < ((rotor.TangentialElementsCount / 2) - 1)) {
                    --c_rep;
                    PM_polarity = -1;
                }
            } else if ( c < (3 * rotor.TangentialElementsCount / 4)) {
                if (c < (( 3* rotor.TangentialElementsCount / 4) - 1))
                    ++c_rep;
            } else {
                --c_rep;
                PM_polarity = 1;
            }
        }
        r_prev = r_curr;
    }

    // Core back iron layer 1
    // r_outer = 0.0769 (m) and r_inner = 0.07 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.0769, 0.07, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }
    // Core back iron layer 2
    // r_outer = 0.07 (m) and r_inner = 0.056 (m)
    for (unsigned c = 0; c < rotor.TangentialElementsCount; ++c) {
        std::string label = ("CORE_" + std::to_string(r_core_element++));
        MEC_ElementGeometry geo;
        geo.setGeometrySector(0.07, 0.056, arc, arc, rotor.Length);
        geo.setTangentialPositionWithOffset(c * arc);
        MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geo, coreMaterial.getSaturatedReluctivity(), label);
        rotor.ReluctanceElements.push_back(element);
        rotor.NonlinearReluctanceElements.push_back(element);
    }
    rotor.RadialElementsCount += 4; // airgap layer + external layer + back iron layer

    // Connect elements
    sector = 0;
    for (unsigned stack = rotor.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = rotor.ReluctanceElements[sector];
        MD_ReluctanceElement *left = rotor.ReluctanceElements[ ((rotor.RadialElementsCount - stack + 1) * rotor.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < rotor.TangentialElementsCount; ++tangential) {
            MD_ReluctanceElement* element = rotor.ReluctanceElements[sector];
            if ( stack == rotor.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); // element->connect(rotor.ReluctanceElements[stackRight], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, nullptr); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], nullptr);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);

            }
            ++sector;
        }
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                    Electric circuit definitions                                         * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    double gamma = ((15.0+21.0*15.0) * M_PI / 180.0);
    double frequency = rotor.Speed_tX * 4.0 / M_2PI; // 4 pole paires
    double omega = M_2PI * frequency;
    double I_max = 200.0;
    double txStep = 1;
    double lowerLimit = 0.0;
    double upperLimit = 92.0;
    unsigned steps = ( upperLimit - lowerLimit ) / txStep;
    double timeStep = (1.0 * M_PI / 180.0) / rotor.Speed_tX;
    MEC_SignalSource I_A, I_B, I_C;

    double time = 0;
    for (unsigned i = 0; i < steps; ++i) {
        I_A.addPulse(I_max * std::sin((omega * time) + gamma));
        I_C.addPulse(I_max * std::sin((omega * time) + gamma - (2.0 * M_2PI / 3.0)));
        I_B.addPulse(I_max * std::sin((omega * time) + gamma - (4.0 * M_2PI / 3.0)));
        time += timeStep;
    }

    MD_ElectricCircuitComponent *currentSource_A = new MD_ElectricCircuitComponent;
    currentSource_A->setCurrentSource(I_A,"CURRENT_SOURCE_A");
    stator.CurrentSourceComponents.push_back(currentSource_A);
    MD_ElectricCircuitComponent *currentSource_B = new MD_ElectricCircuitComponent;
    currentSource_B->setCurrentSource(I_B,"CURRENT_SOURCE_B");
    stator.CurrentSourceComponents.push_back(currentSource_B);
    MD_ElectricCircuitComponent *currentSource_C = new MD_ElectricCircuitComponent;
    currentSource_C->setCurrentSource(I_C,"CURRENT_SOURCE_C");
    stator.CurrentSourceComponents.push_back(currentSource_C);

    std::vector<MD_CoilSide> CS_A(4);
    CS_A[0].setTurns(13);
    CS_A[0].setPolarity(1);
    CS_A[0].addPassiveElement(stator.ReluctanceElements[1]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[51]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[26]);
    CS_A[1].setTurns(13);
    CS_A[1].setPolarity(1);
    CS_A[1].addPassiveElement(stator.ReluctanceElements[3]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[53]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[28]);
    CS_A[2].setTurns(13);
    CS_A[2].setPolarity(-1);
    CS_A[2].addPassiveElement(stator.ReluctanceElements[13]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[63]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[38]);
    CS_A[3].setTurns(13);
    CS_A[3].setPolarity(-1);
    CS_A[3].addPassiveElement(stator.ReluctanceElements[15]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[65]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[40]);

    std::vector<MD_CoilSide> CS_B(4);
    CS_B[0].setTurns(13);
    CS_B[0].setPolarity(-1);
    CS_B[0].addPassiveElement(stator.ReluctanceElements[5]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[55]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[30]);
    CS_B[1].setTurns(13);
    CS_B[1].setPolarity(-1);
    CS_B[1].addPassiveElement(stator.ReluctanceElements[7]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[57]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[32]);
    CS_B[2].setTurns(13);
    CS_B[2].setPolarity(1);
    CS_B[2].addPassiveElement(stator.ReluctanceElements[17]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[67]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[42]);
    CS_B[3].setTurns(13);
    CS_B[3].setPolarity(1);
    CS_B[3].addPassiveElement(stator.ReluctanceElements[19]);
    CS_B[3].addActiveElement(stator.ReluctanceElements[69]);
    CS_B[3].addActiveElement(stator.ReluctanceElements[44]);

    std::vector<MD_CoilSide> CS_C(4);
    CS_C[0].setTurns(13);
    CS_C[0].setPolarity(1);
    CS_C[0].addPassiveElement(stator.ReluctanceElements[9]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[59]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[34]);
    CS_C[1].setTurns(13);
    CS_C[1].setPolarity(1);
    CS_C[1].addPassiveElement(stator.ReluctanceElements[11]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[61]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[36]);
    CS_C[2].setTurns(13);
    CS_C[2].setPolarity(-1);
    CS_C[2].addPassiveElement(stator.ReluctanceElements[21]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[71]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[46]);
    CS_C[3].setTurns(13);
    CS_C[3].setPolarity(-1);
    CS_C[3].addPassiveElement(stator.ReluctanceElements[23]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[73]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[48]);

    MD_ElectricCircuitComponent* Coil_A = new MD_ElectricCircuitComponent;
    Coil_A->setCoil(CS_A,"COIL_PHASE_A", 0.0881, 0.000159);
    stator.CoilComponents.push_back(Coil_A);

    MD_ElectricCircuitComponent* Coil_B = new MD_ElectricCircuitComponent;
    Coil_B->setCoil(CS_B,"COIL_PHASE_B", 0.0881, 0.000159);
    stator.CoilComponents.push_back(Coil_B);

    MD_ElectricCircuitComponent* Coil_C = new MD_ElectricCircuitComponent;
    Coil_C->setCoil(CS_C,"COIL_PHASE_C", 0.0881, 0.000159);
    stator.CoilComponents.push_back(Coil_C);

    // Primary electric circuits definition
    MD_ElectricCircuit mesh1;
    mesh1.addComponent(currentSource_A, -1);
    mesh1.addComponent(Coil_A, 1);
    mesh1.addComponent(Coil_C, -1);
    mesh1.addComponent(currentSource_B, 1);
    stator.ElectricCircuits.push_back(mesh1);
    MD_ElectricCircuit mesh2;
    mesh2.addComponent(currentSource_B, -1);
    mesh2.addComponent(Coil_C, 1);
    mesh2.addComponent(Coil_B, -1);
    mesh2.addComponent(currentSource_C, 1);
    stator.ElectricCircuits.push_back(mesh2);

    // Solve the project
    project.loadProject(stator, rotor);
    std::vector<ReportDataSolver> report = project.solve(timeStep, steps, 0.05, 200, 0.01);
    std::vector<std::vector<double>> data;
    data.resize(report.size());
    for (unsigned i = 0; i < report.size(); ++i)
        data[i] = report[i].data;
    DataToCsv(data);

    return project;
}

MD_Project test_MD_SynRM_ConstantSpeed()
{
    MD_Project project;
    MD_ElectromagneticBlock stator, rotor;
    NonlinearMaterial coreMaterial(SILICONSTEEL_M250_50A);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Stator geometry definition                                         * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    stator.MaterialNonLinear = coreMaterial;
    stator.Length = 0.1;
    stator.Position_tX = 0.0; stator.Position_rY = 0.0;
    stator.Slices = 35 * 2;
    stator.TangentialElementsCount = stator.Slices;
    stator.RadialElementsCount = 7;
    MD_ReluctanceElement* element = nullptr;
    double statorRadii[] = {0.160, 0.1205, 0.11517, 0.10983, 0.1045, 0.099167, 0.093833, 0.0885};
    double elementArc = M_PI / stator.Slices;
    std::string label;
    unsigned coreElemCount = 0;
    unsigned airElemCount = 0;
    unsigned coilElemCount = 0;
    for (unsigned row = 0; row < stator.RadialElementsCount; ++row) {
        for (unsigned column = 0; column < stator.TangentialElementsCount; ++column) {
            MEC_ElementGeometry geometry_element;
            geometry_element.setGeometrySector(statorRadii[row], statorRadii[row + 1], elementArc, elementArc, stator.Length);
            geometry_element.setPosition((column * elementArc) + (elementArc / 2.0), (statorRadii[row] + statorRadii[row + 1]) / 2.0);
            unsigned r = row;
            unsigned c = column % (stator.Slices / 2);
            if ( c > (stator.Slices / 4)) c = (stator.Slices / 2) - c - 1;

            if ( r == 0 ) { // core elements
                label = ("CORE_" + std::to_string(coreElemCount++));
                MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                stator.ReluctanceElements.push_back(element);
                stator.NonlinearReluctanceElements.push_back(element);
            } else if ( r == 1 || r == 2 ) { // coil elements
                if ( c == 1 || c == 2 || c == 3 || c == 4 ||
                     c == 7 || c == 8 || c == 9 || c == 10 ||
                     c == 13 || c == 14 || c == 15 || c == 16 ) {
                    label = ("COIL_" + std::to_string(coilElemCount++));
                    element = new MD_ReluctanceElement(RET_coil, geometry_element, 1.0 / U_AIR, label);
                    stator.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    stator.ReluctanceElements.push_back(element);
                    stator.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 3 || r == 4 || r == 5) { // coil elements
                if ( c == 1 || c == 2 || c == 3 || c == 4 ||
                     c == 7 || c == 8 || c == 9 || c == 10 ||
                     c == 13 || c == 14 || c == 15 ) {
                    label = ("COIL_" + std::to_string(coilElemCount++));
                    element = new MD_ReluctanceElement(RET_coil, geometry_element, 1.0 / U_AIR, label);
                    stator.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    stator.ReluctanceElements.push_back(element);
                    stator.NonlinearReluctanceElements.push_back(element);
                }
            } else { // air elements
                if ( c == 2 || c == 3 || c == 8 || c == 9 ||
                     c == 14 || c == 15 ) {
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    stator.ReluctanceElements.push_back(element);
                    stator.AirgapLayerElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    stator.ReluctanceElements.push_back(element);
                    stator.NonlinearReluctanceElements.push_back(element);
                    stator.AirgapLayerElements.push_back(element);
                }
            }
        }
    }
    // Connect elements
    unsigned sector = 0;
    for (unsigned stack = stator.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = stator.ReluctanceElements[sector];
        MD_ReluctanceElement *left = stator.ReluctanceElements[ ((stator.RadialElementsCount - stack + 1) * stator.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < stator.TangentialElementsCount; ++tangential) {
            MD_ReluctanceElement* element = stator.ReluctanceElements[sector];
            if ( stack == stator.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]); //element->connect(stator.ReluctanceElements[sector+1], nullptr, stator.ReluctanceElements[stackLeft], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]); // element->connect(stator.ReluctanceElements[stackRight], nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], nullptr, stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, nullptr); //element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[stackLeft], nullptr);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr); //element->connect(stator.ReluctanceElements[stackRight], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], left, stator.ReluctanceElements[sector + stator.TangentialElementsCount]); //element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[stackLeft], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else if (tangential == stator.TangentialElementsCount - 1)
                    element->connect(right, stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]); //element->connect(stator.ReluctanceElements[stackRight], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);
                else
                    element->connect(stator.ReluctanceElements[sector+1], stator.ReluctanceElements[sector - stator.TangentialElementsCount], stator.ReluctanceElements[sector-1], stator.ReluctanceElements[sector + stator.TangentialElementsCount]);

            }
            ++sector;
        }
    }


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                      Rotor geometry definition                                          * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    rotor.MaterialNonLinear = coreMaterial;
    rotor.Length = 0.1;
    rotor.Speed_tX = (1500.0 / 60.0) * M_2PI;
    rotor.Position_tX = 0.0; stator.Position_rY = 0.0;
    rotor.Slices = 35 * 2;
    rotor.TangentialElementsCount = stator.Slices;
    rotor.RadialElementsCount = 11;
    double rotorRadii[] = {0.0875, 0.082227, 0.077954, 0.073681, 0.06948, 0.065135, 0.060861, 0.056588, 0.052315, 0.048042, 0.043769, 0.035};
    coreElemCount = 0;
    airElemCount = 0;
    coilElemCount = 0;
    for (unsigned row = 0; row < rotor.RadialElementsCount; ++row) {
        for (unsigned column = 0; column < rotor.TangentialElementsCount; ++column) {
            MEC_ElementGeometry geometry_element;
            geometry_element.setGeometrySector(rotorRadii[row], rotorRadii[row + 1], elementArc, elementArc, rotor.Length);
            geometry_element.setPosition((column * elementArc) + (elementArc / 2.0), (rotorRadii[row] + rotorRadii[row + 1]) / 2.0);
            unsigned r = row;
            unsigned c = column % (rotor.Slices / 2);
            if ( c > (rotor.Slices / 4)) c = (rotor.Slices / 2) - c - 1;

            if ( r == 0 ) {
                if ( c == 0 || c == 3 || c == 6 || c == 7 || c == 11 || c == 12 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.AirgapLayerElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                    rotor.AirgapLayerElements.push_back(element);
                }
            } else if ( r == 1) {
                if ( c == 0 || c == 1 || c == 4 || c == 7 || c == 8 || c == 13 ||
                     c == 14 || c == 15 || c == 16 || c == 17 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 2) {
                if ( c == 1 || c == 5 || c == 9 || c == 10  ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 3) {
                if ( c == 1 || c == 2 || c == 5 || c == 6 || c == 10 || c == 11 ||
                     c == 12 || c == 13 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 4) {
                if ( c == 2 || c == 3 || c == 7 || c == 8 || c == 14 || c == 15 ||
                     c == 16 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 5) {
                if ( c == 3 || c == 4 || c == 9 || c == 10 || c == 11 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 6) {
                if ( c == 4 || c == 5 || c == 12 || c == 13 || c == 14 || c == 15 ||
                     c == 16 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 7) {
                if ( c == 6 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 8) {
                if ( c == 7 || c == 8 || c == 9 || c == 10 || c == 11 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else if ( r == 9) {
                if ( c == 11 || c == 12 || c == 13 || c == 14 || c == 15 || c == 16 ) { // air elements
                    label = ("AIR_" + std::to_string(airElemCount++));
                    element = new MD_ReluctanceElement(RET_air, geometry_element, 1.0 / U_AIR, label);
                    rotor.ReluctanceElements.push_back(element);
                } else { // core elements
                    label = ("CORE_" + std::to_string(coreElemCount++));
                    MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                    rotor.ReluctanceElements.push_back(element);
                    rotor.NonlinearReluctanceElements.push_back(element);
                }
            } else {
                label = ("CORE_" + std::to_string(coreElemCount++));
                MD_ReluctanceElement* element = new MD_ReluctanceElement(RET_coreNonLinear, geometry_element, coreMaterial.getSaturatedReluctivity(), label);
                rotor.ReluctanceElements.push_back(element);
                rotor.NonlinearReluctanceElements.push_back(element);
            }
        }
    }
    // Connect elements
    sector = 0;
    for (unsigned stack = rotor.RadialElementsCount; stack > 0; --stack) {
        MD_ReluctanceElement *right = rotor.ReluctanceElements[sector];
        MD_ReluctanceElement *left = rotor.ReluctanceElements[ ((rotor.RadialElementsCount - stack + 1) * rotor.TangentialElementsCount) - 1];
        for (unsigned tangential = 0; tangential < rotor.TangentialElementsCount; ++tangential) {
            MD_ReluctanceElement* element = rotor.ReluctanceElements[sector];
            if ( stack == rotor.RadialElementsCount ) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); // element->connect(rotor.ReluctanceElements[stackRight], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], nullptr, rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
            } else if (stack == 1) {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, nullptr); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], nullptr);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], nullptr);
            } else {
                if ( tangential == 0)
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], left, rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[stackLeft], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else if (tangential == rotor.TangentialElementsCount - 1)
                    element->connect(right, rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]); //element->connect(rotor.ReluctanceElements[stackRight], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);
                else
                    element->connect(rotor.ReluctanceElements[sector+1], rotor.ReluctanceElements[sector - rotor.TangentialElementsCount], rotor.ReluctanceElements[sector-1], rotor.ReluctanceElements[sector + rotor.TangentialElementsCount]);

            }
            ++sector;
        }
    }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * *                                    Electric circuit definitions                                         * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    double gamma = ((30.0) * M_PI / 180.0);
    double frequency = rotor.Speed_tX * 2.0 / M_2PI; // 2 pole paires
    double omega = M_2PI * frequency;
    double I_max = 50.0;
    double txStep = 2;
    double lowerLimit = 0.0;
    double upperLimit = 184.0;
    unsigned steps = ( upperLimit - lowerLimit ) / txStep;
    double timeStep = (2.0 * M_PI / 180.0) / rotor.Speed_tX;
    MEC_SignalSource I_A, I_B, I_C;

    double time = 0;
    for (unsigned i = 0; i < steps; ++i) {
        I_A.addPulse(I_max * std::sin((omega * time) + gamma));
        I_B.addPulse(I_max * std::sin((omega * time) + gamma - (2.0 * M_2PI / 3.0)));
        I_C.addPulse(I_max * std::sin((omega * time) + gamma - (4.0 * M_2PI / 3.0)));
        time += timeStep;
    }

    MD_ElectricCircuitComponent *currentSource_A = new MD_ElectricCircuitComponent;
    currentSource_A->setCurrentSource(I_A,"CURRENT_SOURCE_A");
    stator.CurrentSourceComponents.push_back(currentSource_A);
    MD_ElectricCircuitComponent *currentSource_B = new MD_ElectricCircuitComponent;
    currentSource_B->setCurrentSource(I_B,"CURRENT_SOURCE_B");
    stator.CurrentSourceComponents.push_back(currentSource_B);
    MD_ElectricCircuitComponent *currentSource_C = new MD_ElectricCircuitComponent;
    currentSource_C->setCurrentSource(I_C,"CURRENT_SOURCE_C");
    stator.CurrentSourceComponents.push_back(currentSource_C);

    std::vector<MD_CoilSide> CS_A(16);
    CS_A[0].setTurns(6.5); // coil-side A-1
    CS_A[0].setPolarity(1);
    CS_A[0].addPassiveElement(stator.ReluctanceElements[0+1]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[350+1]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[280+1]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[210+1]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[140+1]);
    CS_A[0].addActiveElement(stator.ReluctanceElements[70+1]);
    CS_A[1].setTurns(6.5); // coil-side A-2
    CS_A[1].setPolarity(1);
    CS_A[1].addPassiveElement(stator.ReluctanceElements[0+2]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[350+2]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[280+2]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[210+2]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[140+2]);
    CS_A[1].addActiveElement(stator.ReluctanceElements[70+2]);
    CS_A[2].setTurns(6.5); // coil-side A-3
    CS_A[2].setPolarity(1);
    CS_A[2].addPassiveElement(stator.ReluctanceElements[0+3]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[350+3]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[280+3]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[210+3]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[140+3]);
    CS_A[2].addActiveElement(stator.ReluctanceElements[70+3]);
    CS_A[3].setTurns(6.5); // coil-side A-4
    CS_A[3].setPolarity(1);
    CS_A[3].addPassiveElement(stator.ReluctanceElements[0+4]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[350+4]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[280+4]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[210+4]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[140+4]);
    CS_A[3].addActiveElement(stator.ReluctanceElements[70+4]);
    CS_A[4].setTurns(6.5); // coil-side A-5
    CS_A[4].setPolarity(1);
    CS_A[4].addPassiveElement(stator.ReluctanceElements[0+7]);
    CS_A[4].addActiveElement(stator.ReluctanceElements[350+7]);
    CS_A[4].addActiveElement(stator.ReluctanceElements[280+7]);
    CS_A[4].addActiveElement(stator.ReluctanceElements[210+7]);
    CS_A[4].addActiveElement(stator.ReluctanceElements[140+7]);
    CS_A[4].addActiveElement(stator.ReluctanceElements[70+7]);
    CS_A[5].setTurns(6.5); // coil-side A-6
    CS_A[5].setPolarity(1);
    CS_A[5].addPassiveElement(stator.ReluctanceElements[0+8]);
    CS_A[5].addActiveElement(stator.ReluctanceElements[350+8]);
    CS_A[5].addActiveElement(stator.ReluctanceElements[280+8]);
    CS_A[5].addActiveElement(stator.ReluctanceElements[210+8]);
    CS_A[5].addActiveElement(stator.ReluctanceElements[140+8]);
    CS_A[5].addActiveElement(stator.ReluctanceElements[70+8]);
    CS_A[6].setTurns(6.5); // coil-side A-7
    CS_A[6].setPolarity(1);
    CS_A[6].addPassiveElement(stator.ReluctanceElements[0+9]);
    CS_A[6].addActiveElement(stator.ReluctanceElements[350+9]);
    CS_A[6].addActiveElement(stator.ReluctanceElements[280+9]);
    CS_A[6].addActiveElement(stator.ReluctanceElements[210+9]);
    CS_A[6].addActiveElement(stator.ReluctanceElements[140+9]);
    CS_A[6].addActiveElement(stator.ReluctanceElements[70+9]);
    CS_A[7].setTurns(6.5); // coil-side A-8
    CS_A[7].setPolarity(1);
    CS_A[7].addPassiveElement(stator.ReluctanceElements[0+10]);
    CS_A[7].addActiveElement(stator.ReluctanceElements[350+10]);
    CS_A[7].addActiveElement(stator.ReluctanceElements[280+10]);
    CS_A[7].addActiveElement(stator.ReluctanceElements[210+10]);
    CS_A[7].addActiveElement(stator.ReluctanceElements[140+10]);
    CS_A[7].addActiveElement(stator.ReluctanceElements[70+10]);
    CS_A[8].setTurns(6.5); // coil-side A-9
    CS_A[8].setPolarity(-1);
    CS_A[8].addPassiveElement(stator.ReluctanceElements[0+1+35]);
    CS_A[8].addActiveElement(stator.ReluctanceElements[350+1+35]);
    CS_A[8].addActiveElement(stator.ReluctanceElements[280+1+35]);
    CS_A[8].addActiveElement(stator.ReluctanceElements[210+1+35]);
    CS_A[8].addActiveElement(stator.ReluctanceElements[140+1+35]);
    CS_A[8].addActiveElement(stator.ReluctanceElements[70+1+35]);
    CS_A[9].setTurns(6.5); // coil-side A-10
    CS_A[9].setPolarity(-1);
    CS_A[9].addPassiveElement(stator.ReluctanceElements[0+2+35]);
    CS_A[9].addActiveElement(stator.ReluctanceElements[350+2+35]);
    CS_A[9].addActiveElement(stator.ReluctanceElements[280+2+35]);
    CS_A[9].addActiveElement(stator.ReluctanceElements[210+2+35]);
    CS_A[9].addActiveElement(stator.ReluctanceElements[140+2+35]);
    CS_A[9].addActiveElement(stator.ReluctanceElements[70+2+35]);
    CS_A[10].setTurns(6.5); // coil-side A-11
    CS_A[10].setPolarity(-1);
    CS_A[10].addPassiveElement(stator.ReluctanceElements[0+3+35]);
    CS_A[10].addActiveElement(stator.ReluctanceElements[350+3+35]);
    CS_A[10].addActiveElement(stator.ReluctanceElements[280+3+35]);
    CS_A[10].addActiveElement(stator.ReluctanceElements[210+3+35]);
    CS_A[10].addActiveElement(stator.ReluctanceElements[140+3+35]);
    CS_A[10].addActiveElement(stator.ReluctanceElements[70+3+35]);
    CS_A[11].setTurns(6.5); // coil-side A-12
    CS_A[11].setPolarity(-1);
    CS_A[11].addPassiveElement(stator.ReluctanceElements[0+4+35]);
    CS_A[11].addActiveElement(stator.ReluctanceElements[350+4+35]);
    CS_A[11].addActiveElement(stator.ReluctanceElements[280+4+35]);
    CS_A[11].addActiveElement(stator.ReluctanceElements[210+4+35]);
    CS_A[11].addActiveElement(stator.ReluctanceElements[140+4+35]);
    CS_A[11].addActiveElement(stator.ReluctanceElements[70+4+35]);
    CS_A[12].setTurns(6.5); // coil-side A-13
    CS_A[12].setPolarity(-1);
    CS_A[12].addPassiveElement(stator.ReluctanceElements[0+7+35]);
    CS_A[12].addActiveElement(stator.ReluctanceElements[350+7+35]);
    CS_A[12].addActiveElement(stator.ReluctanceElements[280+7+35]);
    CS_A[12].addActiveElement(stator.ReluctanceElements[210+7+35]);
    CS_A[12].addActiveElement(stator.ReluctanceElements[140+7+35]);
    CS_A[12].addActiveElement(stator.ReluctanceElements[70+7+35]);
    CS_A[13].setTurns(6.5); // coil-side A-14
    CS_A[13].setPolarity(-1);
    CS_A[13].addPassiveElement(stator.ReluctanceElements[0+8+35]);
    CS_A[13].addActiveElement(stator.ReluctanceElements[350+8+35]);
    CS_A[13].addActiveElement(stator.ReluctanceElements[280+8+35]);
    CS_A[13].addActiveElement(stator.ReluctanceElements[210+8+35]);
    CS_A[13].addActiveElement(stator.ReluctanceElements[140+8+35]);
    CS_A[13].addActiveElement(stator.ReluctanceElements[70+8+35]);
    CS_A[14].setTurns(6.5); // coil-side A-15
    CS_A[14].setPolarity(-1);
    CS_A[14].addPassiveElement(stator.ReluctanceElements[0+9+35]);
    CS_A[14].addActiveElement(stator.ReluctanceElements[350+9+35]);
    CS_A[14].addActiveElement(stator.ReluctanceElements[280+9+35]);
    CS_A[14].addActiveElement(stator.ReluctanceElements[210+9+35]);
    CS_A[14].addActiveElement(stator.ReluctanceElements[140+9+35]);
    CS_A[14].addActiveElement(stator.ReluctanceElements[70+9+35]);
    CS_A[15].setTurns(6.5); // coil-side A-16
    CS_A[15].setPolarity(-1);
    CS_A[15].addPassiveElement(stator.ReluctanceElements[0+10+35]);
    CS_A[15].addActiveElement(stator.ReluctanceElements[350+10+35]);
    CS_A[15].addActiveElement(stator.ReluctanceElements[280+10+35]);
    CS_A[15].addActiveElement(stator.ReluctanceElements[210+10+35]);
    CS_A[15].addActiveElement(stator.ReluctanceElements[140+10+35]);
    CS_A[15].addActiveElement(stator.ReluctanceElements[70+10+35]);

    std::vector<MD_CoilSide> CS_B(16);
    CS_B[0].setTurns(6.5); // coil-side B-1
    CS_B[0].setPolarity(-1);
    CS_B[0].addPassiveElement(stator.ReluctanceElements[0+13]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[350+13]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[280+13]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[210+13]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[140+13]);
    CS_B[0].addActiveElement(stator.ReluctanceElements[70+13]);
    CS_B[1].setTurns(6.5); // coil-side B-2
    CS_B[1].setPolarity(-1);
    CS_B[1].addPassiveElement(stator.ReluctanceElements[0+14]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[350+14]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[280+14]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[210+14]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[140+14]);
    CS_B[1].addActiveElement(stator.ReluctanceElements[70+14]);
    CS_B[2].setTurns(6.5); // coil-side B-3
    CS_B[2].setPolarity(-1);
    CS_B[2].addPassiveElement(stator.ReluctanceElements[0+15]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[350+15]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[280+15]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[210+15]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[140+15]);
    CS_B[2].addActiveElement(stator.ReluctanceElements[70+15]);
    CS_B[3].setTurns(6.5); // coil-side B-4
    CS_B[3].setPolarity(-1);
    CS_B[3].addPassiveElement(stator.ReluctanceElements[0+16]);
    CS_B[3].addActiveElement(stator.ReluctanceElements[140+16]);
    CS_B[3].addActiveElement(stator.ReluctanceElements[70+16]);
    CS_B[4].setTurns(6.5); // coil-side B-5
    CS_B[4].setPolarity(-1);
    CS_B[4].addPassiveElement(stator.ReluctanceElements[0+18]);
    CS_B[4].addActiveElement(stator.ReluctanceElements[140+18]);
    CS_B[4].addActiveElement(stator.ReluctanceElements[70+18]);
    CS_B[5].setTurns(6.5); // coil-side B-6
    CS_B[5].setPolarity(-1);
    CS_B[5].addPassiveElement(stator.ReluctanceElements[0+19]);
    CS_B[5].addActiveElement(stator.ReluctanceElements[350+19]);
    CS_B[5].addActiveElement(stator.ReluctanceElements[280+19]);
    CS_B[5].addActiveElement(stator.ReluctanceElements[210+19]);
    CS_B[5].addActiveElement(stator.ReluctanceElements[140+19]);
    CS_B[5].addActiveElement(stator.ReluctanceElements[70+19]);
    CS_B[6].setTurns(6.5); // coil-side B-7
    CS_B[6].setPolarity(-1);
    CS_B[6].addPassiveElement(stator.ReluctanceElements[0+20]);
    CS_B[6].addActiveElement(stator.ReluctanceElements[350+20]);
    CS_B[6].addActiveElement(stator.ReluctanceElements[280+20]);
    CS_B[6].addActiveElement(stator.ReluctanceElements[210+20]);
    CS_B[6].addActiveElement(stator.ReluctanceElements[140+20]);
    CS_B[6].addActiveElement(stator.ReluctanceElements[70+20]);
    CS_B[7].setTurns(6.5); // coil-side B-8
    CS_B[7].setPolarity(-1);
    CS_B[7].addPassiveElement(stator.ReluctanceElements[0+21]);
    CS_B[7].addActiveElement(stator.ReluctanceElements[350+21]);
    CS_B[7].addActiveElement(stator.ReluctanceElements[280+21]);
    CS_B[7].addActiveElement(stator.ReluctanceElements[210+21]);
    CS_B[7].addActiveElement(stator.ReluctanceElements[140+21]);
    CS_B[7].addActiveElement(stator.ReluctanceElements[70+21]);
    CS_B[8].setTurns(6.5); // coil-side B-9
    CS_B[8].setPolarity(1);
    CS_B[8].addPassiveElement(stator.ReluctanceElements[0+13+35]);
    CS_B[8].addActiveElement(stator.ReluctanceElements[350+13+35]);
    CS_B[8].addActiveElement(stator.ReluctanceElements[280+13+35]);
    CS_B[8].addActiveElement(stator.ReluctanceElements[210+13+35]);
    CS_B[8].addActiveElement(stator.ReluctanceElements[140+13+35]);
    CS_B[8].addActiveElement(stator.ReluctanceElements[70+13+35]);
    CS_B[9].setTurns(6.5); // coil-side B-10
    CS_B[9].setPolarity(1);
    CS_B[9].addPassiveElement(stator.ReluctanceElements[0+14+35]);
    CS_B[9].addActiveElement(stator.ReluctanceElements[350+14+35]);
    CS_B[9].addActiveElement(stator.ReluctanceElements[280+14+35]);
    CS_B[9].addActiveElement(stator.ReluctanceElements[210+14+35]);
    CS_B[9].addActiveElement(stator.ReluctanceElements[140+14+35]);
    CS_B[9].addActiveElement(stator.ReluctanceElements[70+14+35]);
    CS_B[10].setTurns(6.5); // coil-side B-11
    CS_B[10].setPolarity(1);
    CS_B[10].addPassiveElement(stator.ReluctanceElements[0+15+35]);
    CS_B[10].addActiveElement(stator.ReluctanceElements[350+15+35]);
    CS_B[10].addActiveElement(stator.ReluctanceElements[280+15+35]);
    CS_B[10].addActiveElement(stator.ReluctanceElements[210+15+35]);
    CS_B[10].addActiveElement(stator.ReluctanceElements[140+15+35]);
    CS_B[10].addActiveElement(stator.ReluctanceElements[70+15+35]);
    CS_B[11].setTurns(6.5); // coil-side B-12
    CS_B[11].setPolarity(1);
    CS_B[11].addPassiveElement(stator.ReluctanceElements[0+16+35]);
    CS_B[11].addActiveElement(stator.ReluctanceElements[140+16+35]);
    CS_B[11].addActiveElement(stator.ReluctanceElements[70+16+35]);
    CS_B[12].setTurns(6.5); // coil-side B-13
    CS_B[12].setPolarity(1);
    CS_B[12].addPassiveElement(stator.ReluctanceElements[0+18+35]);
    CS_B[12].addActiveElement(stator.ReluctanceElements[140+18+35]);
    CS_B[12].addActiveElement(stator.ReluctanceElements[70+18+35]);
    CS_B[13].setTurns(6.5); // coil-side B-14
    CS_B[13].setPolarity(1);
    CS_B[13].addPassiveElement(stator.ReluctanceElements[0+19+35]);
    CS_B[13].addActiveElement(stator.ReluctanceElements[350+19+35]);
    CS_B[13].addActiveElement(stator.ReluctanceElements[280+19+35]);
    CS_B[13].addActiveElement(stator.ReluctanceElements[210+19+35]);
    CS_B[13].addActiveElement(stator.ReluctanceElements[140+19+35]);
    CS_B[13].addActiveElement(stator.ReluctanceElements[70+19+35]);
    CS_B[14].setTurns(6.5); // coil-side B-15
    CS_B[14].setPolarity(1);
    CS_B[14].addPassiveElement(stator.ReluctanceElements[0+20+35]);
    CS_B[14].addActiveElement(stator.ReluctanceElements[350+20+35]);
    CS_B[14].addActiveElement(stator.ReluctanceElements[280+20+35]);
    CS_B[14].addActiveElement(stator.ReluctanceElements[210+20+35]);
    CS_B[14].addActiveElement(stator.ReluctanceElements[140+20+35]);
    CS_B[14].addActiveElement(stator.ReluctanceElements[70+20+35]);
    CS_B[15].setTurns(6.5); // coil-side B-16
    CS_B[15].setPolarity(1);
    CS_B[15].addPassiveElement(stator.ReluctanceElements[0+21+35]);
    CS_B[15].addActiveElement(stator.ReluctanceElements[350+21+35]);
    CS_B[15].addActiveElement(stator.ReluctanceElements[280+21+35]);
    CS_B[15].addActiveElement(stator.ReluctanceElements[210+21+35]);
    CS_B[15].addActiveElement(stator.ReluctanceElements[140+21+35]);
    CS_B[15].addActiveElement(stator.ReluctanceElements[70+21+35]);


    std::vector<MD_CoilSide> CS_C(16);
    CS_C[0].setTurns(6.5); // coil-side C-1
    CS_C[0].setPolarity(1);
    CS_C[0].addPassiveElement(stator.ReluctanceElements[0+24]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[350+24]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[280+24]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[210+24]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[140+24]);
    CS_C[0].addActiveElement(stator.ReluctanceElements[70+24]);
    CS_C[1].setTurns(6.5); // coil-side C-2
    CS_C[1].setPolarity(1);
    CS_C[1].addPassiveElement(stator.ReluctanceElements[0+25]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[350+25]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[280+25]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[210+25]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[140+25]);
    CS_C[1].addActiveElement(stator.ReluctanceElements[70+25]);
    CS_C[2].setTurns(6.5); // coil-side C-3
    CS_C[2].setPolarity(1);
    CS_C[2].addPassiveElement(stator.ReluctanceElements[0+26]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[350+26]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[280+26]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[210+26]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[140+26]);
    CS_C[2].addActiveElement(stator.ReluctanceElements[70+26]);
    CS_C[3].setTurns(6.5); // coil-side C-4
    CS_C[3].setPolarity(1);
    CS_C[3].addPassiveElement(stator.ReluctanceElements[0+27]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[350+27]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[280+27]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[210+27]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[140+27]);
    CS_C[3].addActiveElement(stator.ReluctanceElements[70+27]);
    CS_C[4].setTurns(6.5); // coil-side C-5
    CS_C[4].setPolarity(1);
    CS_C[4].addPassiveElement(stator.ReluctanceElements[0+30]);
    CS_C[4].addActiveElement(stator.ReluctanceElements[350+30]);
    CS_C[4].addActiveElement(stator.ReluctanceElements[280+30]);
    CS_C[4].addActiveElement(stator.ReluctanceElements[210+30]);
    CS_C[4].addActiveElement(stator.ReluctanceElements[140+30]);
    CS_C[4].addActiveElement(stator.ReluctanceElements[70+30]);
    CS_C[5].setTurns(6.5); // coil-side C-6
    CS_C[5].setPolarity(1);
    CS_C[5].addPassiveElement(stator.ReluctanceElements[0+31]);
    CS_C[5].addActiveElement(stator.ReluctanceElements[350+31]);
    CS_C[5].addActiveElement(stator.ReluctanceElements[280+31]);
    CS_C[5].addActiveElement(stator.ReluctanceElements[210+31]);
    CS_C[5].addActiveElement(stator.ReluctanceElements[140+31]);
    CS_C[5].addActiveElement(stator.ReluctanceElements[70+31]);
    CS_C[6].setTurns(6.5); // coil-side C-7
    CS_C[6].setPolarity(1);
    CS_C[6].addPassiveElement(stator.ReluctanceElements[0+32]);
    CS_C[6].addActiveElement(stator.ReluctanceElements[350+32]);
    CS_C[6].addActiveElement(stator.ReluctanceElements[280+32]);
    CS_C[6].addActiveElement(stator.ReluctanceElements[210+32]);
    CS_C[6].addActiveElement(stator.ReluctanceElements[140+32]);
    CS_C[6].addActiveElement(stator.ReluctanceElements[70+32]);
    CS_C[7].setTurns(6.5); // coil-side C-8
    CS_C[7].setPolarity(1);
    CS_C[7].addPassiveElement(stator.ReluctanceElements[0+33]);
    CS_C[7].addActiveElement(stator.ReluctanceElements[350+33]);
    CS_C[7].addActiveElement(stator.ReluctanceElements[280+33]);
    CS_C[7].addActiveElement(stator.ReluctanceElements[210+33]);
    CS_C[7].addActiveElement(stator.ReluctanceElements[140+33]);
    CS_C[7].addActiveElement(stator.ReluctanceElements[70+33]);
    CS_C[8].setTurns(6.5); // coil-side C-9
    CS_C[8].setPolarity(-1);
    CS_C[8].addPassiveElement(stator.ReluctanceElements[0+24+35]);
    CS_C[8].addActiveElement(stator.ReluctanceElements[350+24+35]);
    CS_C[8].addActiveElement(stator.ReluctanceElements[280+24+35]);
    CS_C[8].addActiveElement(stator.ReluctanceElements[210+24+35]);
    CS_C[8].addActiveElement(stator.ReluctanceElements[140+24+35]);
    CS_C[8].addActiveElement(stator.ReluctanceElements[70+24+35]);
    CS_C[9].setTurns(6.5); // coil-side C-10
    CS_C[9].setPolarity(-1);
    CS_C[9].addPassiveElement(stator.ReluctanceElements[0+25+35]);
    CS_C[9].addActiveElement(stator.ReluctanceElements[350+25+35]);
    CS_C[9].addActiveElement(stator.ReluctanceElements[280+25+35]);
    CS_C[9].addActiveElement(stator.ReluctanceElements[210+25+35]);
    CS_C[9].addActiveElement(stator.ReluctanceElements[140+25+35]);
    CS_C[9].addActiveElement(stator.ReluctanceElements[70+25+35]);
    CS_C[10].setTurns(6.5); // coil-side C-11
    CS_C[10].setPolarity(-1);
    CS_C[10].addPassiveElement(stator.ReluctanceElements[0+26+35]);
    CS_C[10].addActiveElement(stator.ReluctanceElements[350+26+35]);
    CS_C[10].addActiveElement(stator.ReluctanceElements[280+26+35]);
    CS_C[10].addActiveElement(stator.ReluctanceElements[210+26+35]);
    CS_C[10].addActiveElement(stator.ReluctanceElements[140+26+35]);
    CS_C[10].addActiveElement(stator.ReluctanceElements[70+26+35]);
    CS_C[11].setTurns(6.5); // coil-side C-12
    CS_C[11].setPolarity(-1);
    CS_C[11].addPassiveElement(stator.ReluctanceElements[0+27+35]);
    CS_C[11].addActiveElement(stator.ReluctanceElements[350+27+35]);
    CS_C[11].addActiveElement(stator.ReluctanceElements[280+27+35]);
    CS_C[11].addActiveElement(stator.ReluctanceElements[210+27+35]);
    CS_C[11].addActiveElement(stator.ReluctanceElements[140+27+35]);
    CS_C[11].addActiveElement(stator.ReluctanceElements[70+27+35]);
    CS_C[12].setTurns(6.5); // coil-side C-13
    CS_C[12].setPolarity(-1);
    CS_C[12].addPassiveElement(stator.ReluctanceElements[0+30+35]);
    CS_C[12].addActiveElement(stator.ReluctanceElements[350+30+35]);
    CS_C[12].addActiveElement(stator.ReluctanceElements[280+30+35]);
    CS_C[12].addActiveElement(stator.ReluctanceElements[210+30+35]);
    CS_C[12].addActiveElement(stator.ReluctanceElements[140+30+35]);
    CS_C[12].addActiveElement(stator.ReluctanceElements[70+30+35]);
    CS_C[13].setTurns(6.5); // coil-side C-14
    CS_C[13].setPolarity(-1);
    CS_C[13].addPassiveElement(stator.ReluctanceElements[0+31+35]);
    CS_C[13].addActiveElement(stator.ReluctanceElements[350+31+35]);
    CS_C[13].addActiveElement(stator.ReluctanceElements[280+31+35]);
    CS_C[13].addActiveElement(stator.ReluctanceElements[210+31+35]);
    CS_C[13].addActiveElement(stator.ReluctanceElements[140+31+35]);
    CS_C[13].addActiveElement(stator.ReluctanceElements[70+31+35]);
    CS_C[14].setTurns(6.5); // coil-side C-15
    CS_C[14].setPolarity(-1);
    CS_C[14].addPassiveElement(stator.ReluctanceElements[0+32+35]);
    CS_C[14].addActiveElement(stator.ReluctanceElements[350+32+35]);
    CS_C[14].addActiveElement(stator.ReluctanceElements[280+32+35]);
    CS_C[14].addActiveElement(stator.ReluctanceElements[210+32+35]);
    CS_C[14].addActiveElement(stator.ReluctanceElements[140+32+35]);
    CS_C[14].addActiveElement(stator.ReluctanceElements[70+32+35]);
    CS_C[15].setTurns(6.5); // coil-side C-16
    CS_C[15].setPolarity(-1);
    CS_C[15].addPassiveElement(stator.ReluctanceElements[0+33+35]);
    CS_C[15].addActiveElement(stator.ReluctanceElements[350+33+35]);
    CS_C[15].addActiveElement(stator.ReluctanceElements[280+33+35]);
    CS_C[15].addActiveElement(stator.ReluctanceElements[210+33+35]);
    CS_C[15].addActiveElement(stator.ReluctanceElements[140+33+35]);
    CS_C[15].addActiveElement(stator.ReluctanceElements[70+33+35]);


    MD_ElectricCircuitComponent* Coil_A = new MD_ElectricCircuitComponent;
    Coil_A->setCoil(CS_A,"COIL_PHASE_A", 0.1, 0.00005);
    stator.CoilComponents.push_back(Coil_A);

    MD_ElectricCircuitComponent* Coil_B = new MD_ElectricCircuitComponent;
    Coil_B->setCoil(CS_B,"COIL_PHASE_B", 0.1, 0.00005);
    stator.CoilComponents.push_back(Coil_B);

    MD_ElectricCircuitComponent* Coil_C = new MD_ElectricCircuitComponent;
    Coil_C->setCoil(CS_C,"COIL_PHASE_C", 0.1, 0.00005);
    stator.CoilComponents.push_back(Coil_C);

    // Stator electric circuits definition
    MD_ElectricCircuit mesh1;
    mesh1.addComponent(currentSource_A, -1);
    mesh1.addComponent(Coil_A, 1);
    mesh1.addComponent(Coil_B, -1);
    mesh1.addComponent(currentSource_B, 1);
    stator.ElectricCircuits.push_back(mesh1);
    MD_ElectricCircuit mesh2;
    mesh2.addComponent(currentSource_B, -1);
    mesh2.addComponent(Coil_B, 1);
    mesh2.addComponent(Coil_C, -1);
    mesh2.addComponent(currentSource_C, 1);
    stator.ElectricCircuits.push_back(mesh2);

    // Solve the project
    project.loadProject(stator, rotor, 2);
//    std::vector<ReportDataSolver> report = project.solve(timeStep, steps, 0.05, 200, 0.01);
//    std::vector<std::vector<double>> data;
//    data.resize(report.size());
//    for (unsigned i = 0; i < report.size(); ++i)
//        data[i] = report[i].data;
//    DataToCsv(data);

    return project;
}
