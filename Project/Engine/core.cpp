/*
 * Implementation file for core functions in the library.
 *
 * Part of the Cyclone physics system.
 *
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

#include "core.h"

using namespace cyclone;


const PhysXVec3 PhysXVec3::GRAVITY = PhysXVec3(0, -9.81, 0);
const PhysXVec3 PhysXVec3::HIGH_GRAVITY = PhysXVec3(0, -19.62, 0);
const PhysXVec3 PhysXVec3::UP = PhysXVec3(0, 1, 0);
const PhysXVec3 PhysXVec3::RIGHT = PhysXVec3(1, 0, 0);
const PhysXVec3 PhysXVec3::OUT_OF_SCREEN = PhysXVec3(0, 0, 1);
const PhysXVec3 PhysXVec3::X = PhysXVec3(0, 1, 0);
const PhysXVec3 PhysXVec3::Y = PhysXVec3(1, 0, 0);
const PhysXVec3 PhysXVec3::Z = PhysXVec3(0, 0, 1);

/*
 * Definition of the sleep epsilon extern.
 */
cyclone::PhysXReal cyclone::sleepEpsilon = ((cyclone::PhysXReal)0.3);

/*
 * Functions to change sleepEpsilon.
 */
void cyclone::setSleepEpsilon(cyclone::PhysXReal value)
{
    cyclone::sleepEpsilon = value;
}

cyclone::PhysXReal cyclone::getSleepEpsilon()
{
    return cyclone::sleepEpsilon;
}

cyclone::PhysXReal cyclone::PhyXMat4::getDeterminant() const
{
    return -data[8] * data[5] * data[2] +
        data[4] * data[9] * data[2] +
        data[8] * data[1] * data[6] -
        data[0] * data[9] * data[6] -
        data[4] * data[1] * data[10] +
        data[0] * data[5] * data[10];
}

void cyclone::PhyXMat4::setInverse(const cyclone::PhyXMat4& m)
{
    // Make sure the determinant is non-zero.
    cyclone::PhysXReal det = getDeterminant();
    if (det == 0) return;
    det = ((cyclone::PhysXReal)1.0) / det;

    data[0] = (-m.data[9] * m.data[6] + m.data[5] * m.data[10]) * det;
    data[4] = (m.data[8] * m.data[6] - m.data[4] * m.data[10]) * det;
    data[8] = (-m.data[8] * m.data[5] + m.data[4] * m.data[9]) * det;

    data[1] = (m.data[9] * m.data[2] - m.data[1] * m.data[10]) * det;
    data[5] = (-m.data[8] * m.data[2] + m.data[0] * m.data[10]) * det;
    data[9] = (m.data[8] * m.data[1] - m.data[0] * m.data[9]) * det;

    data[2] = (-m.data[5] * m.data[2] + m.data[1] * m.data[6]) * det;
    data[6] = (+m.data[4] * m.data[2] - m.data[0] * m.data[6]) * det;
    data[10] = (-m.data[4] * m.data[1] + m.data[0] * m.data[5]) * det;

    data[3] = (m.data[9] * m.data[6] * m.data[3]
        - m.data[5] * m.data[10] * m.data[3]
        - m.data[9] * m.data[2] * m.data[7]
        + m.data[1] * m.data[10] * m.data[7]
        + m.data[5] * m.data[2] * m.data[11]
        - m.data[1] * m.data[6] * m.data[11]) * det;
    data[7] = (-m.data[8] * m.data[6] * m.data[3]
        + m.data[4] * m.data[10] * m.data[3]
        + m.data[8] * m.data[2] * m.data[7]
        - m.data[0] * m.data[10] * m.data[7]
        - m.data[4] * m.data[2] * m.data[11]
        + m.data[0] * m.data[6] * m.data[11]) * det;
    data[11] = (m.data[8] * m.data[5] * m.data[3]
        - m.data[4] * m.data[9] * m.data[3]
        - m.data[8] * m.data[1] * m.data[7]
        + m.data[0] * m.data[9] * m.data[7]
        + m.data[4] * m.data[1] * m.data[11]
        - m.data[0] * m.data[5] * m.data[11]) * det;
}

cyclone::PhysXMat3 cyclone::PhysXMat3::linearInterpolate(const cyclone::PhysXMat3& a, const cyclone::PhysXMat3& b, cyclone::PhysXReal prop)
{
    cyclone::PhysXMat3 result;
    for (unsigned i = 0; i < 9; i++) {
        result.data[i] = a.data[i] * (1 - prop) + b.data[i] * prop;
    }
    return result;
}