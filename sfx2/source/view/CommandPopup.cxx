/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "CommandPopup.hxx"

#include <vcl/layout.hxx>
#include <vcl/fixed.hxx>
#include <workwin.hxx>
#include <sfx2/msgpool.hxx>

#include <comphelper/processfactory.hxx>
#include <comphelper/dispatchcommand.hxx>

#include <com/sun/star/frame/XDispatchInformationProvider.hpp>
#include <com/sun/star/frame/theUICommandDescription.hpp>
#include <com/sun/star/ui/theUICategoryDescription.hpp>

#include <com/sun/star/ui/theModuleUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/ui/XUIConfigurationManagerSupplier.hpp>

#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>

#include <vcl/commandinfoprovider.hxx>

using namespace css;

CommandListBox::CommandListBox(vcl::Window* pParent, CommandPopup& rPopUp,
                               uno::Reference<frame::XFrame> xFrame)
    : InterimItemWindow(pParent, "sfx/ui/commandpopup.ui", "CommandBox")
    , m_rPopUp(rPopUp)
    , m_xFrame(xFrame)
    , m_pEntry(m_xBuilder->weld_entry("entry"))
    , m_pListBox(m_xBuilder->weld_tree_view("listBox"))
{
    m_pEntry->connect_changed(LINK(this, CommandListBox, ModifyHdl));
}

void CommandListBox::recurse(uno::Reference<container::XIndexAccess> xIndexAccess,
                             MenuContent& rMenuContent)
{
    for (sal_Int32 n = 0; n < xIndexAccess->getCount(); n++)
    {
        MenuContent aNewContent;
        OUString aModuleIdentifier;
        uno::Sequence<beans::PropertyValue> aProperty;
        uno::Reference<container::XIndexAccess> xIndexContainer;
        try
        {
            if (xIndexAccess->getByIndex(n) >>= aProperty)
            {
                bool bShow = true;
                bool bEnabled = true;

                for (int i = 0; i < aProperty.getLength(); i++)
                {
                    OUString aPropName = aProperty[i].Name;
                    if (aPropName == "CommandURL")
                        aProperty[i].Value >>= aNewContent.m_aCommandURL;
                    else if (aPropName == "ItemDescriptorContainer")
                        aProperty[i].Value >>= xIndexContainer;
                    else if (aPropName == "ModuleIdentifier")
                        aProperty[i].Value >>= aModuleIdentifier;
                    else if (aPropName == "IsVisible")
                        aProperty[i].Value >>= bShow;
                    else if (aPropName == "Enabled")
                        aProperty[i].Value >>= bEnabled;
                }

                OUString aModuleName(vcl::CommandInfoProvider::GetModuleIdentifier(m_xFrame));
                auto aProperties = vcl::CommandInfoProvider::GetCommandProperties(
                    aNewContent.m_aCommandURL, aModuleName);
                aNewContent.m_aMenuLabel
                    = vcl::CommandInfoProvider::GetMenuLabelForCommand(aProperties)
                          .replaceAll("~", "");
                aNewContent.m_aLabel = rMenuContent.m_aLabel + " -> " + aNewContent.m_aMenuLabel;
                //OUString aTooltip(vcl::CommandInfoProvider::GetTooltipForCommand(aNewContent.m_aCommandURL, aProperties, m_xFrame));
                if (xIndexContainer.is())
                    recurse(xIndexContainer, aNewContent);

                rMenuContent.m_aSubMenuContent.push_back(aNewContent);
            }
        }
        catch (const lang::IndexOutOfBoundsException&)
        {
            return;
        }
    }
}

void CommandListBox::initialize()
{
    m_xGlobalCategoryInfo
        = ui::theUICategoryDescription::get(comphelper::getProcessComponentContext());
    m_sModuleLongName = vcl::CommandInfoProvider::GetModuleIdentifier(m_xFrame);
    m_xModuleCategoryInfo.set(m_xGlobalCategoryInfo->getByName(m_sModuleLongName),
                              uno::UNO_QUERY_THROW);
    m_xUICmdDescription
        = frame::theUICommandDescription::get(comphelper::getProcessComponentContext());

    uno::Reference<ui::XModuleUIConfigurationManagerSupplier> xModuleCfgSupplier
        = ui::theModuleUIConfigurationManagerSupplier::get(
            comphelper::getProcessComponentContext());

    uno::Reference<ui::XUIConfigurationManager> xCfgMgr
        = xModuleCfgSupplier->getUIConfigurationManager(m_sModuleLongName);
    uno::Reference<container::XIndexAccess> xConfigData
        = xCfgMgr->getSettings("private:resource/menubar/menubar", false);

    recurse(xConfigData, m_aMenuContent);

    Size aSize(400, 400);
    SetSizePixel(aSize);
    m_rPopUp.SetOutputSizePixel(aSize);
    m_rPopUp.StartPopupMode(tools::Rectangle(Point(10, 10), aSize),
                            FloatWinPopupFlags::Down | FloatWinPopupFlags::GrabFocus
                                | FloatWinPopupFlags::AllMouseButtonClose
                                | FloatWinPopupFlags::NoMouseUpClose);

    Show();

    GrabFocus();
    m_pEntry->grab_focus();
}

void CommandListBox::dispose()
{
    m_pEntry.reset();
    m_pListBox.reset();
    InterimItemWindow::dispose();
}

IMPL_LINK_NOARG(CommandListBox, ModifyHdl, weld::Entry&, void)
{
    m_pListBox->clear();

    OUString sText = m_pEntry->get_text();
    if (sText.isEmpty())
        return;

    m_aCommandList.clear();

    m_pListBox->freeze();
    findInMenu(m_aMenuContent, sText.toAsciiLowerCase());
    m_pListBox->thaw();

    if (m_pListBox->n_children() > 0)
    {
        m_pListBox->set_cursor(0);
        m_pListBox->select(0);
    }

    m_pEntry->grab_focus();
}

void CommandListBox::findInMenu(MenuContent& aMenuContent, OUString const& rText)
{
    for (MenuContent& aSubContent : aMenuContent.m_aSubMenuContent)
    {
        if (aSubContent.m_aMenuLabel.toAsciiLowerCase().startsWith(rText))
        {
            OUString sCommandURL = aSubContent.m_aCommandURL;
            //Image aImage = vcl::CommandInfoProvider::GetImageForCommand(sCommandURL, m_xFrame);
            m_pListBox->append_text(aSubContent.m_aLabel);
            m_aCommandList.push_back(sCommandURL);
        }
        findInMenu(aSubContent, rText);
    }
}

bool CommandListBox::EventNotify(NotifyEvent& rNotifyEvent)
{
    const KeyEvent* pKeyEvent = rNotifyEvent.GetKeyEvent();

    if (pKeyEvent)
    {
        if (pKeyEvent->GetKeyCode().GetCode() == KEY_DOWN
            || pKeyEvent->GetKeyCode().GetCode() == KEY_UP)
        {
            m_pListBox->grab_focus();
        }
        else if (pKeyEvent->GetKeyCode().GetCode() == KEY_RETURN)
        {
            size_t nSelected = m_pListBox->get_selected_index();
            if (nSelected < m_aCommandList.size())
            {
                OUString sCommand = m_aCommandList[nSelected];
                dispatchCommandAndClose(sCommand);
            }
        }
        else
        {
            m_pEntry->grab_focus();
        }
    }

    return InterimItemWindow::EventNotify(rNotifyEvent);
}

void CommandListBox::dispatchCommandAndClose(OUString const& rCommand)
{
    m_rPopUp.EndPopupMode(FloatWinPopupEndFlags::CloseAll);
    comphelper::dispatchCommand(rCommand, uno::Sequence<beans::PropertyValue>());
}

CommandPopup::CommandPopup(vcl::Window* pParent)
    : FloatingWindow(pParent, WB_BORDER | WB_SYSTEMWINDOW)
{
}

CommandPopup::~CommandPopup() { disposeOnce(); }

void CommandPopup::dispose() { FloatingWindow::dispose(); }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
