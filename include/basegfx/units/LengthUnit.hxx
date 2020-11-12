/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <sal/types.h>

namespace gfx
{
constexpr sal_Int64 constFactor_hmm_to_EMU = 360ll;
constexpr sal_Int64 constFactor_mm_to_EMU = constFactor_hmm_to_EMU * 100ll;
constexpr sal_Int64 constFactor_cm_to_EMU = constFactor_hmm_to_EMU * 1000ll;
constexpr sal_Int64 constFactor_m_to_EMU = constFactor_hmm_to_EMU * 100000ll;

constexpr sal_Int64 constFactor_twip_to_EMU = 635ll;
constexpr sal_Int64 constFactor_in_to_EMU = constFactor_twip_to_EMU * 1440ll;
constexpr sal_Int64 constFactor_pt_to_EMU = constFactor_twip_to_EMU * 20ll;
constexpr sal_Int64 constFactor_px_to_EMU = constFactor_twip_to_EMU * 15ll;

template <typename T> class LengthUnitBase
{
private:
    // value in EMU units
    T m_nValue;

public:
    static constexpr LengthUnitBase cm(T nValue)
    {
        return LengthUnitBase(gfx::constFactor_cm_to_EMU * nValue);
    }

    static constexpr LengthUnitBase mm(T nValue)
    {
        return LengthUnitBase(gfx::constFactor_mm_to_EMU * nValue);
    }

    static constexpr LengthUnitBase hmm(T nValue)
    {
        return LengthUnitBase(gfx::constFactor_hmm_to_EMU * nValue);
    }

    static constexpr LengthUnitBase in(T nValue)
    {
        return LengthUnitBase(gfx::constFactor_in_to_EMU * nValue);
    }

    static constexpr LengthUnitBase twip(T nValue)
    {
        return LengthUnitBase(gfx::constFactor_twip_to_EMU * nValue);
    }

    static constexpr LengthUnitBase pt(T nValue)
    {
        return LengthUnitBase(gfx::constFactor_pt_to_EMU * nValue);
    }

    static constexpr LengthUnitBase px(T nValue)
    {
        return LengthUnitBase(gfx::constFactor_px_to_EMU * nValue);
    }

    constexpr explicit LengthUnitBase()
        : m_nValue(0)
    {
    }

    constexpr explicit LengthUnitBase(T nValue)
        : m_nValue(nValue)
    {
    }

    constexpr explicit operator T() const { return m_nValue; }

    constexpr LengthUnitBase& operator+=(LengthUnitBase const& rhs)
    {
        m_nValue += rhs.m_nValue;
        return *this;
    }

    constexpr LengthUnitBase& operator-=(LengthUnitBase const& rhs)
    {
        m_nValue -= rhs.m_nValue;
        return *this;
    }

    constexpr LengthUnitBase& operator*=(T const& rhs)
    {
        m_nValue *= rhs;
        return *this;
    }

    constexpr LengthUnitBase& operator/=(T const& rhs)
    {
        m_nValue /= rhs;
        return *this;
    }

    constexpr LengthUnitBase& operator-()
    {
        m_nValue = -m_nValue;
        return *this;
    }

    T value() const { return m_nValue; }

    double _hmm() const { return m_nValue / double(constFactor_hmm_to_EMU); }
    double _mm() const { return m_nValue / double(constFactor_mm_to_EMU); }
    double _cm() const { return m_nValue / double(constFactor_cm_to_EMU); }
    double _m() const { return m_nValue / double(constFactor_m_to_EMU); }
    double _twip() const { return m_nValue / double(constFactor_twip_to_EMU); }
    double _in() const { return m_nValue / double(constFactor_in_to_EMU); }
    double _pt() const { return m_nValue / double(constFactor_pt_to_EMU); }
    double _px() const { return m_nValue / double(constFactor_px_to_EMU); }
};

template <typename T>
inline LengthUnitBase<T> operator+(LengthUnitBase<T> lhs, const LengthUnitBase<T>& rhs)
{
    return lhs += rhs;
}

template <typename T>
inline LengthUnitBase<T> operator-(LengthUnitBase<T> lhs, const LengthUnitBase<T>& rhs)
{
    return lhs -= rhs;
}

template <typename T> inline LengthUnitBase<T> operator*(LengthUnitBase<T> lhs, const long rhs)
{
    return lhs *= rhs;
}

template <typename T> inline LengthUnitBase<T> operator/(LengthUnitBase<T> lhs, const long rhs)
{
    return lhs /= rhs;
}

typedef LengthUnitBase<sal_Int64> LengthUnit;
typedef LengthUnitBase<double> LengthUnitD;

} // end namespace gfx

constexpr gfx::LengthUnit operator"" _emu(unsigned long long value)
{
    return gfx::LengthUnit(value);
}

constexpr gfx::LengthUnit operator"" _in(unsigned long long value)
{
    return gfx::LengthUnit::in(value);
}

constexpr gfx::LengthUnit operator"" _cm(unsigned long long value)
{
    return gfx::LengthUnit::cm(value);
}

constexpr gfx::LengthUnit operator"" _mm(unsigned long long value)
{
    return gfx::LengthUnit::mm(value);
}

constexpr gfx::LengthUnit operator"" _hmm(unsigned long long value)
{
    return gfx::LengthUnit::hmm(value);
}

constexpr gfx::LengthUnit operator"" _twip(unsigned long long value)
{
    return gfx::LengthUnit::twip(value);
}

constexpr gfx::LengthUnit operator"" _pt(unsigned long long value)
{
    return gfx::LengthUnit::pt(value);
}

constexpr gfx::LengthUnit operator"" _px(unsigned long long value)
{
    return gfx::LengthUnit::px(value);
}
