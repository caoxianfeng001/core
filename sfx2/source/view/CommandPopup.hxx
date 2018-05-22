/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#pragma once

#include <vcl/layout.hxx>

#include <sfx2/dllapi.h>
#include <sfx2/viewfrm.hxx>
#include <vcl/floatwin.hxx>

#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/container/XNameAccess.hpp>

#include <vcl/InterimItemWindow.hxx>
#include <vcl/weld.hxx>
#include <vcl/window.hxx>

struct MenuContent
{
    OUString m_aCommandURL;
    OUString m_aLabel;
    OUString m_aMenuLabel;
    std::vector<MenuContent> m_aSubMenuContent;
};

class SFX2_DLLPUBLIC CommandPopup : public FloatingWindow
{
public:
    explicit CommandPopup(vcl::Window* pWorkWindow);

    ~CommandPopup() override;

    void dispose() override;
};

class SFX2_DLLPUBLIC CommandListBox final : public InterimItemWindow
{
private:
    CommandPopup& m_rPopUp;

    std::vector<OUString> m_aCommandList;
    css::uno::Reference<css::frame::XFrame> m_xFrame;
    css::uno::Reference<css::container::XNameAccess> m_xGlobalCategoryInfo;
    css::uno::Reference<css::container::XNameAccess> m_xModuleCategoryInfo;
    css::uno::Reference<css::container::XNameAccess> m_xUICmdDescription;
    OUString m_sModuleLongName;

    MenuContent m_aMenuContent;
    OUString m_PreviousText;

    std::unique_ptr<weld::Entry> m_pEntry;
    std::unique_ptr<weld::TreeView> m_pListBox;

    DECL_LINK(ModifyHdl, weld::Entry&, void);

    void recurse(css::uno::Reference<css::container::XIndexAccess> xIndexAccess,
                 MenuContent& rMenuContent);
    void findInMenu(MenuContent& aMenuContent, OUString const& rText);

    void dispatchCommandAndClose(OUString const& rCommand);
    bool EventNotify(NotifyEvent& rNotifyEvent) override;

public:
    CommandListBox(vcl::Window* pParent, CommandPopup& rPopUp,
                   css::uno::Reference<css::frame::XFrame> xFrame);

    void initialize();

    void dispose() override;
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
