/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <basegfx/units/LengthUnit.hxx>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class LengthUnitTest : public CppUnit::TestFixture
{
public:
    void test();

    CPPUNIT_TEST_SUITE(LengthUnitTest);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST_SUITE_END();
};

void LengthUnitTest::test()
{
    gfx::LengthUnit cm = 1_cm + 5_cm - 2_cm;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, cm._cm(), 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.04, cm._m(), 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(40.0, cm._mm(), 1e-4);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(1440000), cm.value());

    gfx::LengthUnit cm2 = 5_cm * 2;
    CPPUNIT_ASSERT_EQUAL(sal_Int64(3600000), cm2.value());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, cm2._cm(), 1e-4);

    // 1 km - 50 m = 950 m = 95000 cm
    gfx::LengthUnit cm3 = 100000_cm - 5000_cm;
    CPPUNIT_ASSERT_EQUAL(sal_Int64(34200000000), cm3.value());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(95000.0, cm3._cm(), 1e-4);

    // (635 * 20) + 3 * (635 * 15) = 41275EMU
    gfx::LengthUnit pt = 1_pt + 3_px;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.25, pt._pt(), 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(65.0, pt._twip(), 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0451, pt._in(), 1e-4);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(41275), pt.value());

    gfx::LengthUnit inch = 1_in; // 1440 * 635
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1440.0, inch._twip(), 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(96.0, inch._px(), 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, inch._in(), 1e-4);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(914400), inch.value());

    // Conversion
    sal_Int64 asNumber(17_pt);
    asNumber += sal_Int64(1_pt);
    gfx::LengthUnit asLength(asNumber);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(18 * 635 * 20), asLength.value());

    gfx::LengthUnit maximum(SAL_MAX_INT64);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(256204778801.5, maximum._m(), 1e-1);
    // 256204778 km
    CPPUNIT_ASSERT_EQUAL(sal_Int64(SAL_MAX_INT64), maximum.value());

    gfx::LengthUnit minimum(SAL_MIN_INT64);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-256204778801.5, minimum._m(), 1e-1);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(SAL_MIN_INT64), minimum.value());

    // 27 emu + 33 emu + 360 emu = 420
    gfx::LengthUnit emus = 27_emu + 33_emu + 1_hmm;
    CPPUNIT_ASSERT_EQUAL(sal_Int64(420), emus.value());

    //  Creation from number
    int number = 10;
    auto asCm = gfx::LengthUnit::cm(number);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, asCm._cm(), 1e-4);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(3600000), asCm.value());

    auto asMm = gfx::LengthUnit::mm(number);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, asMm._mm(), 1e-4);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(360000), asMm.value());

    auto asInch = gfx::LengthUnit::in(number);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, asInch._in(), 1e-4);
    CPPUNIT_ASSERT_EQUAL(sal_Int64(9144000), asInch.value());
}

CPPUNIT_TEST_SUITE_REGISTRATION(LengthUnitTest);
