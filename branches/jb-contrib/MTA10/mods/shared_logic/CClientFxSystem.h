/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CClientFxSystem
{
public:
    CClientFxSystem(CFxSystemSA* pFxSystem) { m_pFxSystem = pFxSystem; }
private:
public:
    CFxSystemSA* m_pFxSystem;
};