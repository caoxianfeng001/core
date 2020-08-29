/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <comphelper/processfactory.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/ucb/UniversalContentBroker.hpp>

#include <vcl/virdev.hxx>
#include <vcl/vclmain.hxx>
#include <vcl/bitmapaccess.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/svapp.hxx>

#include <chrono>
#include <iostream>
#include <tools/ScopedNanoTimer.hxx>

using namespace css;

class Benchmark
{
    ScopedVclPtr<VirtualDevice> mpVDev;
    int mnRepeats;

    Size maSource;
    Size maTarget;

public:
    Benchmark()
        : mpVDev(VclPtr<VirtualDevice>::Create())
        , mnRepeats(2)
    {
        mpVDev->SetAntialiasing(AntialiasingFlags::Enable | AntialiasingFlags::PixelSnapHairline);
        mpVDev->SetOutputSizePixel(Size(4000, 4000));
        mpVDev->SetBackground(Wallpaper(COL_LIGHTGRAY));
    }

    void run()
    {
        std::vector<sal_Int64> aCompleteTimes(20, 0);

        mnRepeats = 5;
        for (long iSize : { 4000, 2000, 1000 })
        {
            std::vector<std::pair<OUString, sal_Int64>> aTotalTimes(20);

            maSource = Size(iSize, iSize);
            maTarget = Size(200, 200);

            for (int i = 0; i < mnRepeats; i++)
            {
                size_t a = 0;
                //benchBitmapScale2(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                //benchBitmapScale4(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                //benchBitmapScale8X(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                //benchBitmapScale8(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                benchBitmapScale24(aTotalTimes[a].first, aTotalTimes[a].second);
                a++;
                benchBitmapScale24Combo(aTotalTimes[a].first, aTotalTimes[a].second);
                a++;
                benchBitmapScale32(aTotalTimes[a].first, aTotalTimes[a].second);
                a++;
                benchBitmapScale32Combo(aTotalTimes[a].first, aTotalTimes[a].second);
                a++;

                /*benchBitmap2(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                benchBitmap4(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                benchBitmap8X(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                benchBitmap8(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                benchBitmap24(aTotalTimes[a].first, aTotalTimes[a].second); a++;
                benchBitmap32(aTotalTimes[a].first, aTotalTimes[a].second); a++;*/
            }

            int i = 0;
            for (auto& rPair : aTotalTimes)
            {
                if (!rPair.first.isEmpty())
                {
                    aCompleteTimes[i] += rPair.second / double(mnRepeats);
                    printf("TIME %d: %s - %f\n", i, rPair.first.toUtf8().getStr(),
                           rPair.second / double(mnRepeats));
                }
                i++;
            }
            printf("\n");
        }
        int i = 0;
        for (auto& rTime : aCompleteTimes)
        {
            if (rTime > 0)
            {
                printf("TIME %d: %f\n", i, double(rTime));
                i++;
            }
        }
    }

    void benchBitmapScale2(OUString& rName, sal_Int64& rTotal)
    {
        rName = "2 Scale";
        Bitmap aBitmap(maSource, 2, &Bitmap::GetGreyPalette(2));
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Super);
        rTotal += aTimer.stop();
    }

    void benchBitmapScale4(OUString& rName, sal_Int64& rTotal)
    {
        rName = "4 Scale";
        Bitmap aBitmap(maSource, 4, &Bitmap::GetGreyPalette(16));
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Super);
        rTotal += aTimer.stop();
    }

    void benchBitmapScale8X(OUString& rName, sal_Int64& rTotal)
    {
        rName = "8X Scale";
        Bitmap aBitmap(maSource, 8);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Super);
        rTotal += aTimer.stop();
    }

    void benchBitmapScale8(OUString& rName, sal_Int64& rTotal)
    {
        rName = "8 Scale";
        Bitmap aBitmap(maSource, 8, &Bitmap::GetGreyPalette(256));
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Super);
        rTotal += aTimer.stop();
    }

    void benchBitmapScale24(OUString& rName, sal_Int64& rTotal)
    {
        rName = "24 Scale Super";
        Bitmap aBitmap(maSource, 24);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Super);
        rTotal += aTimer.stop();
    }

    void benchBitmapScale24Combo(OUString& rName, sal_Int64& rTotal)
    {
        rName = "24 Scale Combo";
        Bitmap aBitmap(maSource, 24);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Combo);
        rTotal += aTimer.stop();
    }

    void benchBitmapScale32(OUString& rName, sal_Int64& rTotal)
    {
        rName = "32 Scale Super";
        Bitmap aBitmap(maSource, 32);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Super);
        rTotal += aTimer.stop();
    }

    void benchBitmapScale32Combo(OUString& rName, sal_Int64& rTotal)
    {
        rName = "32 Scale Combo";
        Bitmap aBitmap(maSource, 32);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        aBitmap.Scale(maTarget, BmpScaleFlag::Combo);
        rTotal += aTimer.stop();
    }

    void benchBitmap2(OUString& rName, sal_Int64& rTotal)
    {
        rName = "2";
        mpVDev->Erase();

        Bitmap aBitmap(Size(4000, 4000), 2, &Bitmap::GetGreyPalette(2));
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        mpVDev->DrawBitmap(Point(), aBitmap);
        rTotal += aTimer.stop();
    }

    void benchBitmap4(OUString& rName, sal_Int64& rTotal)
    {
        rName = "4";
        mpVDev->Erase();

        Bitmap aBitmap(Size(4000, 4000), 4, &Bitmap::GetGreyPalette(16));
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        mpVDev->DrawBitmap(Point(), aBitmap);
        rTotal += aTimer.stop();
    }

    void benchBitmap8X(OUString& rName, sal_Int64& rTotal)
    {
        rName = "8X";
        mpVDev->Erase();
        Bitmap aBitmap(Size(4000, 4000), 8);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        mpVDev->DrawBitmap(Point(), aBitmap);
        rTotal += aTimer.stop();
    }

    void benchBitmap8(OUString& rName, sal_Int64& rTotal)
    {
        rName = "8";
        mpVDev->Erase();
        Bitmap aBitmap(Size(4000, 4000), 8, &Bitmap::GetGreyPalette(256));
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        mpVDev->DrawBitmap(Point(), aBitmap);
        rTotal += aTimer.stop();
    }

    void benchBitmap24(OUString& rName, sal_Int64& rTotal)
    {
        rName = "24";
        mpVDev->Erase();
        Bitmap aBitmap(Size(4000, 4000), 24);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer(rName.toUtf8(), 1000);
        mpVDev->DrawBitmap(Point(), aBitmap);
        rTotal += aTimer.stop();
    }

    void benchBitmap32(OUString& rName, sal_Int64& rTotal)
    {
        rName = "32";
        mpVDev->Erase();
        Bitmap aBitmap(Size(4000, 4000), 32);
        aBitmap.Erase(COL_YELLOW);
        ScopedNanoTimer aTimer("32", 1000);
        mpVDev->DrawBitmap(Point(), aBitmap);
        rTotal += aTimer.stop();
    }
};

class BenchmarkApp : public Application
{
public:
    BenchmarkApp() {}

    virtual int Main() override
    {
        Benchmark aBenchmark;
        aBenchmark.run();
        return 0;
    }

protected:
    void Init() override
    {
        try
        {
            uno::Reference<uno::XComponentContext> xComponentContext
                = ::cppu::defaultBootstrap_InitialComponentContext();
            uno::Reference<lang::XMultiServiceFactory> xMSF
                = uno::Reference<lang::XMultiServiceFactory>(xComponentContext->getServiceManager(),
                                                             uno::UNO_QUERY);

            if (!xMSF.is())
                Application::Abort("Bootstrap failure - no service manager");

            comphelper::setProcessServiceFactory(xMSF);
        }
        catch (const uno::Exception& e)
        {
            Application::Abort("Bootstrap exception " + e.Message);
        }
    }

    void DeInit() override
    {
        uno::Reference<lang::XComponent> xComponent(comphelper::getProcessComponentContext(),
                                                    uno::UNO_QUERY_THROW);
        xComponent->dispose();
        comphelper::setProcessServiceFactory(nullptr);
    }
};

void vclmain::createApplication() { static BenchmarkApp aApplication; }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
