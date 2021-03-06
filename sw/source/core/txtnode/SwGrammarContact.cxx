/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <vcl/timer.hxx>
#include <hints.hxx>
#include <IGrammarContact.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <SwGrammarMarkUp.hxx>
#include <txtfrm.hxx>

namespace {

/*
 * This class is responsible for the delayed display of grammar checks when a paragraph is edited
 * It's a client of the paragraph the cursor points to.
 * If the cursor position changes, updateCursorPosition has to be called
 * If the grammar checker wants to set a grammar marker at a paragraph, he has to request
 * the grammar list from this class. If the requested paragraph is not edited, it returns
 * the normal grammar list. But if the paragraph is the active one, a proxy list will be returned and
 * all changes are set in this proxy list. If the cursor leaves the paragraph the proxy list
 * will replace the old list. If the grammar checker has completed the paragraph ('setChecked')
 * then a timer is setup which replaces the old list as well.
 */
class SwGrammarContact : public IGrammarContact, public SwClient
{
    Timer aTimer;
    std::unique_ptr<SwGrammarMarkUp> mpProxyList;
    bool mbFinished;
    SwTextNode* getMyTextNode() { return static_cast<SwTextNode*>(GetRegisteredIn()); }
      DECL_LINK( TimerRepaint, Timer *, void );

public:
    SwGrammarContact();
    virtual ~SwGrammarContact() override { aTimer.Stop(); }

    // (pure) virtual functions of IGrammarContact
    virtual void updateCursorPosition( const SwPosition& rNewPos ) override;
    virtual SwGrammarMarkUp* getGrammarCheck( SwTextNode& rTextNode, bool bCreate ) override;
    virtual void finishGrammarCheck( SwTextNode& rTextNode ) override;
protected:
    // virtual function of SwClient
    virtual void SwClientNotify( const SwModify&, const SfxHint& rHint) override;
};

}

SwGrammarContact::SwGrammarContact() : mbFinished( false )
{
    aTimer.SetTimeout( 2000 );  // Repaint of grammar check after 'setChecked'
    aTimer.SetInvokeHandler( LINK(this, SwGrammarContact, TimerRepaint) );
    aTimer.SetDebugName( "sw::SwGrammarContact TimerRepaint" );
}

IMPL_LINK( SwGrammarContact, TimerRepaint, Timer *, pTimer, void )
{
    if( pTimer )
    {
        pTimer->Stop();
        if( GetRegisteredIn() )
        {   //Replace the old wrong list by the proxy list and repaint all frames
            getMyTextNode()->SetGrammarCheck( mpProxyList.release() );
            SwTextFrame::repaintTextFrames( *getMyTextNode() );
        }
    }
}

/* I'm always a client of the current paragraph */
void SwGrammarContact::updateCursorPosition( const SwPosition& rNewPos )
{
    SwTextNode* pTextNode = rNewPos.nNode.GetNode().GetTextNode();
    if( pTextNode == GetRegisteredIn() ) // paragraph has been changed
        return;

    aTimer.Stop();
    if( GetRegisteredIn() ) // My last paragraph has been left
    {
        if( mpProxyList )
        {   // replace old list by the proxy list and repaint
            getMyTextNode()->SetGrammarCheck( mpProxyList.release() );
            SwTextFrame::repaintTextFrames( *getMyTextNode() );
        }
        EndListeningAll();
    }
    if( pTextNode )
        pTextNode->Add( this ); // welcome new paragraph
}

/* deliver a grammar check list for the given text node */
SwGrammarMarkUp* SwGrammarContact::getGrammarCheck( SwTextNode& rTextNode, bool bCreate )
{
    SwGrammarMarkUp *pRet = nullptr;
    if( GetRegisteredIn() == &rTextNode ) // hey, that's my current paragraph!
    {   // so you will get a proxy list...
        if( bCreate )
        {
            if( mbFinished )
            {
                mpProxyList.reset();
            }
            if( !mpProxyList )
            {
                if( rTextNode.GetGrammarCheck() )
                    mpProxyList.reset( static_cast<SwGrammarMarkUp*>(rTextNode.GetGrammarCheck()->Clone()) );
                else
                {
                    mpProxyList.reset( new SwGrammarMarkUp() );
                    mpProxyList->SetInvalid( 0, COMPLETE_STRING );
                }
            }
            mbFinished = false;
        }
        pRet = mpProxyList.get();
    }
    else
    {
        pRet = rTextNode.GetGrammarCheck(); // do you have already a list?
        if( bCreate && !pRet ) // do you want to create a list?
        {
            pRet = new SwGrammarMarkUp();
            pRet->SetInvalid( 0, COMPLETE_STRING );
            rTextNode.SetGrammarCheck( pRet );
            rTextNode.SetGrammarCheckDirty( true );
        }
    }
    return pRet;
}

void SwGrammarContact::SwClientNotify(const SwModify&, const SfxHint& rHint)
{
    auto pLegacy = dynamic_cast<const sw::LegacyModifyHint*>(&rHint);
    if(!pLegacy || pLegacy->GetWhich() != RES_OBJECTDYING)
        return;
    aTimer.Stop();
    EndListeningAll();
    mpProxyList.reset();
}

void SwGrammarContact::finishGrammarCheck( SwTextNode& rTextNode )
{
    if( &rTextNode != GetRegisteredIn() ) // not my paragraph
        SwTextFrame::repaintTextFrames( rTextNode ); // can be repainted directly
    else
    {
        if( mpProxyList )
        {
            mbFinished = true;
            aTimer.Start(); // will replace old list and repaint with delay
        }
        else if( getMyTextNode()->GetGrammarCheck() )
        {   // all grammar problems seems to be gone, no delay needed
            getMyTextNode()->SetGrammarCheck( nullptr );
            SwTextFrame::repaintTextFrames( *getMyTextNode() );
        }
    }
}

IGrammarContact* createGrammarContact()
{
    return new SwGrammarContact();
}

void finishGrammarCheck( SwTextNode& rTextNode )
{
    IGrammarContact* pGrammarContact = getGrammarContact( rTextNode );
    if( pGrammarContact )
        pGrammarContact->finishGrammarCheck( rTextNode );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
