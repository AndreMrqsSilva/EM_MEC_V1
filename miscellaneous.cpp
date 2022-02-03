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
    if (height < width)
        return (1.5 * permeability * height * length / width);
    else
        return (1.5 * permeability * width * length / height);
}

MEC_ElementGeometry::MEC_ElementGeometry()
{
    shape = REG_notDefined;
    position_tX = 0.0;
    position_rY = 0.0;
    height = 0.0;
    width = 0.0;
    widthBottom = 0.0;
    widthTop = 0.0;
    radiusBottom = 0.0;
    radiusTop = 0.0;
    arcBottom = 0.0;
    arcTop = 0.0;
    length = 0.0;
    crossSectionalArea = 0.0;
    crossSectionalAreaTangential = 0.0;
    crossSectionalAreaRadial = 0.0;
    volume = 0.0;
}

void MEC_ElementGeometry::setGeometryRectangle(double h, double w, double l)
{
    shape = REG_rectangle;
    height = h;
    width = w;
    widthTop = w;
    widthBottom = w;
    length = l;

    crossSectionalArea = height * width;
    crossSectionalAreaTangential = length * height;
    crossSectionalAreaRadial = length * width;
    volume = crossSectionalArea * length;
}

void MEC_ElementGeometry::setGeometryTrapezoid(double h, double bW, double tW, double l)
{
    shape = REG_trapezoid;
    height = h;
    width = (tW + bW) / 2.0;
    widthTop = tW;
    widthBottom = bW;
    length = l;

    crossSectionalArea = height * width;
    crossSectionalAreaTangential = length * height;
    crossSectionalAreaRadial = length * width;
    volume = crossSectionalArea * length;
}

void MEC_ElementGeometry::setGeometrySector(double rT, double rB, double aT, double aB, double l)
{
    shape = REG_sector;
    height = std::abs(rT - rB);
    width = (aT*rT + aB*rB) / 2.0;
    widthTop = aT * rT;
    widthBottom = aB * rB;
    radiusTop = rT;
    radiusBottom = rB;
    arcTop = aT;
    arcBottom = aB;
    length = l;

    crossSectionalArea = ((arcTop - arcBottom) / (radiusTop - radiusBottom))*(((std::pow(radiusTop,3)-std::pow(radiusBottom, 3.0)) / 3.0) - radiusBottom*((std::pow(radiusTop,2)-std::pow(radiusBottom, 2.0)) / 2.0));
    crossSectionalArea += arcBottom * (0.5 * (std::pow(radiusTop,2)-std::pow(radiusBottom,2)));
    crossSectionalAreaTangential = length * height;
    crossSectionalAreaRadial = length * width;
    volume = crossSectionalArea * length;
}

void MEC_ElementGeometry::setPosition(double tX, double rY)
{
    position_tX = tX;
    position_rY = rY;
}

void MEC_ElementGeometry::setTangentialPosition(double theta)
{
    position_tX = theta;
    position_rY = (radiusBottom + radiusTop) / 2.0;
}

void MEC_ElementGeometry::setTangentialPositionWithOffset(double offset)
{
    position_tX = offset + ((arcTop + arcBottom) / 4.0);
    position_rY = (radiusBottom + radiusTop) / 2.0;
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
        density = 7600.0;
    }
}

double NonlinearMaterial::getReluctivity(double B)
{
    double c1, c2, c3, c4; // CUBIC SPLINE: f(x) = c1 * x^3 + c2 * x^2 + c3 * x + c4

    if ( B  < 1.5697354 ) {
        if ( B  < 1.3540317 ) {
            if ( B  < 1.1379473 ) {
                if ( B  < 0.7517234 ) {
                    if ( B  < 0.5154116 ) {
                        c1 = -3.18056101243931;
                        c2 = 90.3222632659487;
                        c3 = -45.708228999686;
                        c4 = 49.9314571100000;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = -3.18056101243949;
                    c2 = 85.4043691449919;
                    c3 = 44.8633157738485;
                    c4 = 49.9314571100000;
                    B -= 0.5154116;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                } else {
                    if ( B  < 1.0067174 ) {
                        c1 = 313.465984739134;
                        c2 = 83.1495568514139;
                        c3 = 84.6945974231260;
                        c4 = 65.2604749600000;
                        B -= 0.7517234;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 1179.88101734833;
                    c2 = 322.945392789125;
                    c3 = 188.246373011766;
                    c4 = 97.4609647900000;
                    B -= 1.0067174;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                }
            } else {
                if ( B  < 1.3011592 ) {
                    if ( B  < 1.2317553 ) {
                        c1 = 3416.37363807719;
                        c2 = 787.452396544681;
                        c3 = 333.963763866262;
                        c4 = 130.392522100000;
                        B -= 1.1379473;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 9265.64615188475;
                    c2 = 1748.90193126692;
                    c3 = 571.894090649613;
                    c4 = 171.470762400000;
                    B -= 1.2317553;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );

                } else {
                    c1 = 22825.8134950655;
                    c2 = 3678.11786814929;
                    c3 = 948.550430106316;
                    c4 = 222.684345000000;
                    B -= 1.3011592;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                }
            }
        } else {
            if ( B  < 1.4811189 ) {
                if ( B  < 1.4288814 ) {
                    if ( B  < 1.3954514 ) {
                        c1 = 50475.4384794818;
                        c2 = 7298.69134020287;
                        c3 = 1528.92177497491;
                        c4 = 286.492533300000;
                        B -= 1.3540317;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 96793.5869993480;
                    c2 = 13570.7238977686;
                    c3 = 2393.32669330712;
                    c4 = 365.928336400000;
                    B -= 1.3954514;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                } else {
                    if ( B  < 1.4568093 ) {
                        c1 = 151919.314966801;
                        c2 = 23278.1527379332;
                        c3 = 3625.18463923864;
                        c4 = 464.719639700000;
                        B -= 1.4288814;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 175870.400677739;
                    c2 = 36006.5150473170;
                    c3 = 5280.88091267832;
                    c4 = 587.428875400000;
                    B -= 1.4568093;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                }
            } else {
                if ( B  < 1.5247078 ) {
                    if ( B  < 1.5033213 ) {
                        c1 = 114850.695397411;
                        c2 = 48832.5323242637;
                        c3 = 7343.28421866251;
                        c4 = 739.609807700000;
                        B -= 1.4811189;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = -29187.7262626889;
                    c2 = 56482.4155627381;
                    c3 = 9681.52881762888;
                    c4 = 927.977164900000;
                    B -= 1.5033213;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                } else {
                    if ( B  < 1.546460 ) {
                        c1 = -168411.991879394;
                        c2 = 54609.7456395871;
                        c3 = 12057.4013231824;
                        c4 = 1160.57973400000;
                        B -= 1.5247078;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = -213185.896795551;
                    c2 = 43619.7516503103;
                    c3 = 14194.1089941317;
                    c4 = 1446.96045300000;
                    B -= 1.546460;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                }
            }
        }
    } else {
        if ( B  < 1.8514709 ) {
            if ( B  < 1.6614493 ) {
                if ( B  < 1.5957413 ) {
                    if ( B  < 1.5697354 ) {
                        c1 = -213185.896795551;
                        c2 = 43619.7516503103;
                        c3 = 14194.1089941317;
                        c4 = 1446.96045300000;
                        B -= 1.546460;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = -187379.836467504;
                    c2 = 28733.7905834856;
                    c3 = 15878.1666310402;
                    c4 = 1798.27663900000;
                    B -= 1.5697354;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                } else {
                    if ( B  < 1.6258058 ) {
                        c1 = -90136.2103776513;
                        c2 = 14114.8467159151;
                        c3 = 16992.4840077847;
                        c4 = 2227.33988100000;
                        B -= 1.5957413;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = -130316.924086920;
                    c2 = 5985.14642521938;
                    c3 = 17596.7802515763;
                    c4 = 2748.51905300000;
                    B -= 1.6258058;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                }
            } else {
                if ( B  < 1.7523791 ) {
                    if ( B  < 1.7044355 ) {
                        c1 = 179573.668298103;
                        c2 = -7949.70742585714;
                        c3 = 17526.7564215500;
                        c4 = 3377.43255300000;
                        B -= 1.6614493;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 143580.186654784;
                    c2 = 15207.8614347303;
                    c3 = 17838.7568814063;
                    c4 = 4130.41525600000;
                    B -= 1.7044355;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                } else {
                    if ( B  < 1.8018636 ) {
                        c1 = 72581.4044230628;
                        c2 = 35859.1145454375;
                        c3 = 20287.0915510090;
                        c4 = 5036.44901300000;
                        B -= 1.7523791;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 155205.149029106;
                    c2 = 46634.0780669564;
                    c3 = 24369.2259408371;
                    c4 = 6136.94931400000;
                    B -= 1.8018636;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                }
            }
        } else {
            if ( B  < 2.0199321 ) {
                if ( B  < 1.9447560 ) {
                    if ( B  < 1.8996345 ) {
                        c1 = 294367.437690382;
                        c2 = 69732.0032352504;
                        c3 = 30141.8330458201;
                        c4 = 7479.54898900000;
                        B -= 1.8514709;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 499823.956584293;
                    c2 = 112265.389801084;
                    c3 = 38907.4826850648;
                    c4 = 9125.93645500000;
                    B -= 1.8996345;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                } else {
                    if ( B  < 1.9853123 ) {
                        c1 = 1626631.77235495;
                        c2 = 179923.809772139;
                        c3 = 52091.4976536080;
                        c4 = 11155.9835100000;
                        B -= 1.9447560;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 2379074.86983649;
                    c2 = 377834.308219616;
                    c3 = 74712.1032143170;
                    c4 = 13673.0716400000;
                    B -= 1.9853123;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                }
            } else {
                if ( B  < 2.0667479 ) {
                    if ( B  < 2.0474229 ) {
                        c1 = 21788294.0437909;
                        c2 = 624923.596755912;
                        c3 = 109427.381332989;
                        c4 = 16811.1503600000;
                        B -= 2.0199321;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    c1 = 44104949.7788923;
                    c2 = 2421856.49845304;
                    c3 = 193185.803574359;
                    c4 = 20744.3527300000;
                    B -= 2.0474229;
                    return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                } else {
                    if ( B  < 2.0734227 ) {
                        c1 = 44104949.7788936;
                        c2 = 4978840.96188438;
                        c3 = 336204.281995381;
                        c4 = 25700.4310600000;
                        B -= 2.0667479;
                        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
                    }
                    B -= 2.0734227;
                    return (2.8179E4 + (4.8867E5 * std::atan(0.8361 * B)));
                }
            }
        }
    }










//    if ( B  < 0.5154116 ) {
//        c1 = -3.18056101243931;
//        c2 = 90.3222632659487;
//        c3 = -45.708228999686;
//        c4 = 49.9314571100000;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 0.7517234 ) {
//        c1 = -3.18056101243949;
//        c2 = 85.4043691449919;
//        c3 = 44.8633157738485;
//        c4 = 49.9314571100000;
//        B -= 0.5154116;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.0067174 ) {
//        c1 = 313.465984739134;
//        c2 = 83.1495568514139;
//        c3 = 84.6945974231260;
//        c4 = 65.2604749600000;
//        B -= 0.7517234;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.1379473 ) {
//        c1 = 1179.88101734833;
//        c2 = 322.945392789125;
//        c3 = 188.246373011766;
//        c4 = 97.4609647900000;
//        B -= 1.0067174;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.2317553 ) {
//        c1 = 3416.37363807719;
//        c2 = 787.452396544681;
//        c3 = 333.963763866262;
//        c4 = 130.392522100000;
//        B -= 1.1379473;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.3011592 ) {
//        c1 = 9265.64615188475;
//        c2 = 1748.90193126692;
//        c3 = 571.894090649613;
//        c4 = 171.470762400000;
//        B -= 1.2317553;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.3540317 ) {
//        c1 = 22825.8134950655;
//        c2 = 3678.11786814929;
//        c3 = 948.550430106316;
//        c4 = 222.684345000000;
//        B -= 1.3011592;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.3954514 ) {
//        c1 = 50475.4384794818;
//        c2 = 7298.69134020287;
//        c3 = 1528.92177497491;
//        c4 = 286.492533300000;
//        B -= 1.3540317;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.4288814 ) {
//        c1 = 96793.5869993480;
//        c2 = 13570.7238977686;
//        c3 = 2393.32669330712;
//        c4 = 365.928336400000;
//        B -= 1.3954514;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.4568093 ) {
//        c1 = 151919.314966801;
//        c2 = 23278.1527379332;
//        c3 = 3625.18463923864;
//        c4 = 464.719639700000;
//        B -= 1.4288814;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.4811189 ) {
//        c1 = 175870.400677739;
//        c2 = 36006.5150473170;
//        c3 = 5280.88091267832;
//        c4 = 587.428875400000;
//        B -= 1.4568093;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.5033213 ) {
//        c1 = 114850.695397411;
//        c2 = 48832.5323242637;
//        c3 = 7343.28421866251;
//        c4 = 739.609807700000;
//        B -= 1.4811189;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.5247078 ) {
//        c1 = -29187.7262626889;
//        c2 = 56482.4155627381;
//        c3 = 9681.52881762888;
//        c4 = 927.977164900000;
//        B -= 1.5033213;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.546460 ) {
//        c1 = -168411.991879394;
//        c2 = 54609.7456395871;
//        c3 = 12057.4013231824;
//        c4 = 1160.57973400000;
//        B -= 1.5247078;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.5697354 ) {
//        c1 = -213185.896795551;
//        c2 = 43619.7516503103;
//        c3 = 14194.1089941317;
//        c4 = 1446.96045300000;
//        B -= 1.546460;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.5957413 ) {
//        c1 = -187379.836467504;
//        c2 = 28733.7905834856;
//        c3 = 15878.1666310402;
//        c4 = 1798.27663900000;
//        B -= 1.5697354;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.6258058 ) {
//        c1 = -90136.2103776513;
//        c2 = 14114.8467159151;
//        c3 = 16992.4840077847;
//        c4 = 2227.33988100000;
//        B -= 1.5957413;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.6614493 ) {
//        c1 = -130316.924086920;
//        c2 = 5985.14642521938;
//        c3 = 17596.7802515763;
//        c4 = 2748.51905300000;
//        B -= 1.6258058;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.7044355 ) {
//        c1 = 179573.668298103;
//        c2 = -7949.70742585714;
//        c3 = 17526.7564215500;
//        c4 = 3377.43255300000;
//        B -= 1.6614493;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.7523791 ) {
//        c1 = 143580.186654784;
//        c2 = 15207.8614347303;
//        c3 = 17838.7568814063;
//        c4 = 4130.41525600000;
//        B -= 1.7044355;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.8018636 ) {
//        c1 = 72581.4044230628;
//        c2 = 35859.1145454375;
//        c3 = 20287.0915510090;
//        c4 = 5036.44901300000;
//        B -= 1.7523791;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.8514709 ) {
//        c1 = 155205.149029106;
//        c2 = 46634.0780669564;
//        c3 = 24369.2259408371;
//        c4 = 6136.94931400000;
//        B -= 1.8018636;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.8996345 ) {
//        c1 = 294367.437690382;
//        c2 = 69732.0032352504;
//        c3 = 30141.8330458201;
//        c4 = 7479.54898900000;
//        B -= 1.8514709;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.9447560 ) {
//        c1 = 499823.956584293;
//        c2 = 112265.389801084;
//        c3 = 38907.4826850648;
//        c4 = 9125.93645500000;
//        B -= 1.8996345;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 1.9853123 ) {
//        c1 = 1626631.77235495;
//        c2 = 179923.809772139;
//        c3 = 52091.4976536080;
//        c4 = 11155.9835100000;
//        B -= 1.9447560;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 2.0199321 ) {
//        c1 = 2379074.86983649;
//        c2 = 377834.308219616;
//        c3 = 74712.1032143170;
//        c4 = 13673.0716400000;
//        B -= 1.9853123;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 2.0474229 ) {
//        c1 = 21788294.0437909;
//        c2 = 624923.596755912;
//        c3 = 109427.381332989;
//        c4 = 16811.1503600000;
//        B -= 2.0199321;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 2.0667479 ) {
//        c1 = 44104949.7788923;
//        c2 = 2421856.49845304;
//        c3 = 193185.803574359;
//        c4 = 20744.3527300000;
//        B -= 2.0474229;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    if ( B  < 2.0734227 ) {
//        c1 = 44104949.7788936;
//        c2 = 4978840.96188438;
//        c3 = 336204.281995381;
//        c4 = 25700.4310600000;
//        B -= 2.0667479;
//        return ( (c1 * (B*B*B)) + (c2 * (B*B)) + (c3 * B) + c4 );
//    }
//    B -= 2.0734227;
//    return (2.8179E4 + (4.8867E5 * std::atan(0.8361 * B)));
}

double NonlinearMaterial::getReluctivityDerivative(double B)
{
    double c1, c2, c3; // CUBIC SPLINE: f(x) = c1 * x^3 + c2 * x^2 + c3 * x + c4

    if ( B  < 0.5154116 ) {
        c1 = -3.18056101243931;
        c2 = 90.3222632659487;
        c3 = -45.708228999686;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 0.7517234 ) {
        c1 = -3.18056101243949;
        c2 = 85.4043691449919;
        c3 = 44.8633157738485;
        B -= 0.5154116;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.0067174 ) {
        c1 = 313.465984739134;
        c2 = 83.1495568514139;
        c3 = 84.6945974231260;
        B -= 0.7517234;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.1379473 ) {
        c1 = 1179.88101734833;
        c2 = 322.945392789125;
        c3 = 188.246373011766;
        B -= 1.0067174;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.2317553 ) {
        c1 = 3416.37363807719;
        c2 = 787.452396544681;
        c3 = 333.963763866262;
        B -= 1.1379473;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.3011592 ) {
        c1 = 9265.64615188475;
        c2 = 1748.90193126692;
        c3 = 571.894090649613;
        B -= 1.2317553;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.3540317 ) {
        c1 = 22825.8134950655;
        c2 = 3678.11786814929;
        c3 = 948.550430106316;
        B -= 1.3011592;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.3954514 ) {
        c1 = 50475.4384794818;
        c2 = 7298.69134020287;
        c3 = 1528.92177497491;
        B -= 1.3540317;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.4288814 ) {
        c1 = 96793.5869993480;
        c2 = 13570.7238977686;
        c3 = 2393.32669330712;
        B -= 1.3954514;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.4568093 ) {
        c1 = 151919.314966801;
        c2 = 23278.1527379332;
        c3 = 3625.18463923864;
        B -= 1.4288814;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.4811189 ) {
        c1 = 175870.400677739;
        c2 = 36006.5150473170;
        c3 = 5280.88091267832;
        B -= 1.4568093;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.5033213 ) {
        c1 = 114850.695397411;
        c2 = 48832.5323242637;
        c3 = 7343.28421866251;
        B -= 1.4811189;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.5247078 ) {
        c1 = -29187.7262626889;
        c2 = 56482.4155627381;
        c3 = 9681.52881762888;
        B -= 1.5033213;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.546460 ) {
        c1 = -168411.991879394;
        c2 = 54609.7456395871;
        c3 = 12057.4013231824;
        B -= 1.5247078;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.5697354 ) {
        c1 = -213185.896795551;
        c2 = 43619.7516503103;
        c3 = 14194.1089941317;
        B -= 1.546460;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.5957413 ) {
        c1 = -187379.836467504;
        c2 = 28733.7905834856;
        c3 = 15878.1666310402;
        B -= 1.5697354;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.6258058 ) {
        c1 = -90136.2103776513;
        c2 = 14114.8467159151;
        c3 = 16992.4840077847;
        B -= 1.5957413;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.6614493 ) {
        c1 = -130316.924086920;
        c2 = 5985.14642521938;
        c3 = 17596.7802515763;
        B -= 1.6258058;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.7044355 ) {
        c1 = 179573.668298103;
        c2 = -7949.70742585714;
        c3 = 17526.7564215500;
        B -= 1.6614493;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.7523791 ) {
        c1 = 143580.186654784;
        c2 = 15207.8614347303;
        c3 = 17838.7568814063;
        B -= 1.7044355;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.8018636 ) {
        c1 = 72581.4044230628;
        c2 = 35859.1145454375;
        c3 = 20287.0915510090;
        B -= 1.7523791;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.8514709 ) {
        c1 = 155205.149029106;
        c2 = 46634.0780669564;
        c3 = 24369.2259408371;
        B -= 1.8018636;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.8996345 ) {
        c1 = 294367.437690382;
        c2 = 69732.0032352504;
        c3 = 30141.8330458201;
        B -= 1.8514709;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.9447560 ) {
        c1 = 499823.956584293;
        c2 = 112265.389801084;
        c3 = 38907.4826850648;
        B -= 1.8996345;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 1.9853123 ) {
        c1 = 1626631.77235495;
        c2 = 179923.809772139;
        c3 = 52091.4976536080;
        B -= 1.9447560;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 2.0199321 ) {
        c1 = 2379074.86983649;
        c2 = 377834.308219616;
        c3 = 74712.1032143170;
        B -= 1.9853123;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 2.0474229 ) {
        c1 = 21788294.0437909;
        c2 = 624923.596755912;
        c3 = 109427.381332989;
        B -= 2.0199321;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 2.0667479 ) {
        c1 = 44104949.7788923;
        c2 = 2421856.49845304;
        c3 = 193185.803574359;
        B -= 2.0474229;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }
    if ( B  < 2.0734227 ) {
        c1 = 44104949.7788936;
        c2 = 4978840.96188438;
        c3 = 336204.281995381;
        B -= 2.0667479;
        return ( (3.0 * c1 * (B*B)) + (2.0 * c2 * B) + c3);
    }

    B -= 2.0734227;
    return (4.8867E5 * (0.8361 / (std::pow(0.8361 * B, 2.0) + 1.0)));
}

double NonlinearMaterial::getIronLosses(double B, double f, double volume)
{
    double losses_h, losses_c, losses_e;

    losses_h = k_h * f * (B*B); // hysteresis
    losses_c = k_c * (f*f) * (B*B); // eddy current
    losses_e = k_e * std::pow(f*B, 1.5); // additional

    return volume * (losses_h + losses_c + losses_e);
}

double NonlinearMaterial::getIronLosses(double B_prev, double B_curr, double timeStep, double volume)
{
    double losses_h, losses_c, losses_e;

    double dB = std::abs(B_curr - B_prev);

    losses_h = k_h * std::pow(dB / 2.0, 2.0) / timeStep;
    losses_c = k_c * std::pow(dB / (2.0 * timeStep), 2.0);
    losses_e = k_e * std::pow(dB / (2.0 * timeStep), 1.5);

    return 0.5 * volume * (losses_h + losses_c + losses_e);
}

double round_SuperLUPrecision(double v)
{
    QString n = QString::number(v);
    return ( n.toDouble() );
}

void linsysToCsv(std::vector<std::vector<double> > A, std::vector<double> B)
{
    unsigned size = A.size();
    QString text;
    for (unsigned i = 0; i < size; ++i) {
        for (unsigned j = 0; j < size; ++j)
            text.append(QString("%1,").arg(A[i][j]));
        text.append(QString(",%1\n").arg(B[i]));
    }
    QString fileName = "C:/Users/AMSilva/Documents/superlu.csv";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    QTextStream data(&file);
    data << text;
    file.close();
}

void DataToCsv(std::vector<std::vector<double>> data)
{
    unsigned m = data.size();
    unsigned n = data.front().size();
    QString text;
    for (unsigned i = 0; i < m; ++i) {
        for (unsigned j = 0; j < n; ++j)
            text.append(QString("%1,").arg(data[i][j]));
        text.append(QString("\n"));
    }
    QString fileName = "C:/Users/AMSilva/Documents/MEC_RESULTS.csv";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    QTextStream textStream(&file);
    textStream << text;
    file.close();
}
