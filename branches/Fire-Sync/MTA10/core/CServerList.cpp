/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerList.cpp
*  PURPOSE:     Master server list querying
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

extern CCore* g_pCore;

CServerList::CServerList ( void )
{
    m_bUpdated = false;
    m_iPass = 0;
    m_strStatus = "Idle";
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision = 1;
}


CServerList::~CServerList ( void )
{
    Clear ();
}


void CServerList::Clear ( void )
{
    // Clear all entries
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
        delete *i;
    m_Servers.clear ();
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision++;
    m_llLastTickCount = 0;
}


void CServerList::Pulse ( void )
{
    // Get QueriesPerSecond setting
    int iQueriesPerSecond = 100;
    int iVar;
    CVARS_GET ( "browser_speed", iVar );
    if ( iVar == 0 ) iQueriesPerSecond = 4;
    else if ( iVar == 1 ) iQueriesPerSecond = 10;
    else if ( iVar == 2 ) iQueriesPerSecond = 100;

    long long llTickCount = GetTickCount64_ ();
    // Ensure m_llLastTickCount is initialized
    m_llLastTickCount = m_llLastTickCount ? m_llLastTickCount : llTickCount;
    // Ensure m_llLastTickCount is in range
    m_llLastTickCount = Clamp ( llTickCount - 1000, m_llLastTickCount, llTickCount );

    // Calc number of queries this pulse
    int iTicksPerQuery = 1000 / Max ( 1, iQueriesPerSecond );
    int iDeltaTicks = llTickCount - m_llLastTickCount;
    int iNumQueries = iDeltaTicks / Max ( 1, iTicksPerQuery );
    iNumQueries = Clamp ( 0, iNumQueries, SERVER_LIST_QUERIES_PER_PULSE );
    int iNumTicksUsed = iNumQueries * iTicksPerQuery;
    m_llLastTickCount += iNumTicksUsed;
#if MTA_DEBUG
    OutputDebugLine ( SString ( "%08x  Size: %d  iNumQueries: %d", this, m_Servers.size(), iNumQueries ) );
#endif

    unsigned int n = m_Servers.size ();
    unsigned int uiQueriesSent = 0;
    unsigned int uiQueriesResent = 0;
    unsigned int uiRepliesParsed = 0;
    unsigned int uiNoReplies = 0;
    unsigned int uiActiveServers = 0;

    // Scan all servers in our list, and keep the value of scanned servers
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
    {
        CServerListItem * pServer = *i;
        uint uiPrevRevision = pServer->uiRevision;
        std::string strResult = pServer->Pulse ( (int)( uiQueriesSent /*+ uiQueriesResent*/ ) < iNumQueries );
        if ( uiPrevRevision != pServer->uiRevision )
            m_bUpdated |= true;         // Flag GUI update
        if ( strResult == "SentQuery" )
            uiQueriesSent++;
        else
        if ( strResult == "ResentQuery" )
            uiQueriesResent++;
        else
        if ( strResult == "ParsedQuery" )
            uiRepliesParsed++;
        else
        if ( strResult == "NoReply" )
            uiNoReplies++;

        if ( pServer->nMaxPlayers )
            uiActiveServers++;
    }

    // Check whether we are done scanning
    std::stringstream ss;

    // Store the new number of scanned servers
    m_nScanned += uiRepliesParsed;
    m_nSkipped += uiNoReplies;
    if ( m_nScanned + m_nSkipped == n ) {
        ss << "   " << uiActiveServers << " servers";
        // We are no longer refreshing
        m_iPass = 0;
    } else {
        ss << "   " << uiActiveServers << " servers...";
    }

    // Update our status message
    m_strStatus = ss.str () + m_strStatus2;
}


bool CServerList::Exists ( CServerListItem Server )
{
    // Look for a duplicate entry
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
        if ( **i == Server ) return true;
    return false;
}

void CServerList::Add ( CServerListItem Server, bool addAtFront )
{
    if ( addAtFront )
        m_Servers.push_front ( new CServerListItem ( Server ) );
    else
        m_Servers.push_back ( new CServerListItem ( Server ) );
}

void CServerList::Remove ( CServerListItem Server )
{
    // Look for a duplicate entry
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
    {
        if ( **i == Server )
        {
            m_Servers.remove ( *i );
            return;
        }
    }
}


void CServerList::Refresh ( void )
{   // Assumes we already have a (saved) list of servers, so we just need to refresh

    // Reinitialize each server list item
    for ( std::list<CServerListItem*>::iterator iter = m_Servers.begin (); iter != m_Servers.end (); iter++ )
    {
        CServerListItem* pOldItem = *iter;
        *iter = new CServerListItem( *pOldItem );
        delete pOldItem;
    }

    m_iPass = 1;
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision++;
}

void CServerListInternet::Refresh ( void )
{   // Gets the server list from the master server and refreshes
    m_ulStartTime = CClientTime::GetTime ();
    m_HTTP.Get ( SERVER_LIST_MASTER_URL );
    m_iPass = 1;
    m_bUpdated = true;

    // Clear the previous server list
    Clear ();
}


void CServerListInternet::Pulse ( void )
{   // We also need to take care of the master server list here
    unsigned long ulTime = CClientTime::GetTime () - m_ulStartTime;

    if ( m_iPass == 1 ) {
        // We are polling for the master server list (first pass)
        stringstream ss;
        ss << "Requesting master server list (" << ulTime << "ms elapsed)";
        m_strStatus = ss.str ();
        m_strStatus2 = "";
        m_bUpdated = true;
        
        // Attempt to get the HTTP data
        CHTTPBuffer buffer;
        if ( m_HTTP.GetData ( buffer ) ) {
            // We got the data, parse it and switch pass
            if ( ParseList ( buffer.GetData (), buffer.GetSize () ) ) {
                m_iPass++;
                GetServerCache ()->GetServerListCachedInfo ( this );
            } else {
                // Abort
                m_strStatus = "Master server list could not be parsed.";
                m_iPass = 0;
            }
        } else {
            // Take care of timeouts
            if ( ulTime > SERVER_LIST_MASTER_TIMEOUT ) {
                // Abort
                m_strStatus = "Master server list could not be retrieved.";
                m_iPass = 0;
            }
        }
        if ( m_iPass == 0 )
        {
            // If query failed, load from backup
            GetServerCache ()->GenerateServerList ( this );
            GetServerCache ()->GetServerListCachedInfo ( this );
            m_strStatus2 = string ( "  (Backup server list)" );
            m_iPass = 2;
        }
    } else if ( m_iPass == 2 ) {
        // We are scanning our known servers (second pass)
        CServerList::Pulse ();
    }
}


bool CServerListInternet::ParseList ( const char *szBuffer, unsigned int nLength )
{
    unsigned int i = 0, j = 0;

    // Read out the server count
    if ( nLength < 2 ) return false;
    unsigned int uiCount = ntohs ( *((unsigned short*)&szBuffer[0]) );
    i = 2;

    // Add all servers until we hit the count or nLength
    while ( i < ( nLength - 6 ) && uiCount-- ) {

        // Read the IPv4-address
        in_addr _Address;
        _Address.S_un.S_un_b.s_b1 = szBuffer[i];
        _Address.S_un.S_un_b.s_b2 = szBuffer[i+1];
        _Address.S_un.S_un_b.s_b3 = szBuffer[i+2];
        _Address.S_un.S_un_b.s_b4 = szBuffer[i+3];

        // Read the query port
        unsigned short _usQueryPort = ntohs ( *((unsigned short*)(&szBuffer[i+4])) );
        CServerListItem& item = *new CServerListItem ( _Address, _usQueryPort );

        // Add the server
        m_Servers.push_back ( &item );
        i += 6;
    }
    return true;
}


void CServerListLAN::Pulse ( void )
{
    char szBuffer[32];

    // Broadcast the discover packet on a regular interval
    if ( (CClientTime::GetTime () - m_ulStartTime) > SERVER_LIST_BROADCAST_REFRESH )
        Discover ();

    // Poll our socket to discover any new servers
    timeval tm;
    tm.tv_sec = 0;
    tm.tv_usec = 0;
    fd_set readfds;
    readfds.fd_array[0] = m_Socket;
    readfds.fd_count = 1;
    int len = sizeof ( m_Remote );
    if ( select ( 1, &readfds, NULL, NULL, &tm ) > 0 )
        if ( recvfrom ( m_Socket, szBuffer, sizeof (szBuffer), 0, (sockaddr *) &m_Remote, &len ) > 10 )
            if ( strncmp ( szBuffer, SERVER_LIST_SERVER_BROADCAST_STR, strlen ( SERVER_LIST_SERVER_BROADCAST_STR ) ) == 0 ) {
                unsigned short usPort = ( unsigned short ) atoi ( &szBuffer[strlen ( SERVER_LIST_SERVER_BROADCAST_STR ) + 1] );
                CServerListItem Server ( m_Remote.sin_addr, usPort );
                // Add the server if doesn't already exist
                if ( !Exists ( Server ) ) Add ( Server );
            }

    // Scan our already known servers
    CServerList::Pulse ();
}


void CServerListLAN::Refresh ( void )
{   // Gets the server list from LAN-broadcasting servers
    m_iPass = 1;
    m_bUpdated = true;

    // Create the LAN-broadcast socket
    closesocket ( m_Socket );
    m_Socket = socket ( AF_INET, SOCK_DGRAM, 0 );
    const int Flags = 1;
    setsockopt ( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&Flags, sizeof ( Flags ) );
    if ( setsockopt ( m_Socket, SOL_SOCKET, SO_BROADCAST, (const char *)&Flags, sizeof ( Flags ) ) != 0 ) {
        m_strStatus = "Cannot bind LAN-broadcast socket";
        return;
    }

    // Prepare the structures
    memset ( &m_Remote, 0, sizeof (m_Remote) );
    m_Remote.sin_family         = AF_INET;
    m_Remote.sin_port           = htons ( SERVER_LIST_BROADCAST_PORT ); 
    m_Remote.sin_addr.s_addr    = INADDR_BROADCAST;

    // Clear the previous server list
    Clear ();

    // Discover other servers by sending out the broadcast packet
    Discover ();
}


void CServerListLAN::Discover ( void )
{
    m_strStatus = "Attempting to discover LAN servers";

    // Send out the broadcast packet
    std::string strQuery = std::string ( SERVER_LIST_CLIENT_BROADCAST_STR ) + " " + std::string ( MTA_DM_ASE_VERSION );
    sendto ( m_Socket, strQuery.c_str (), strQuery.length () + 1, 0, (sockaddr *)&m_Remote, sizeof (m_Remote) );
    
    // Keep the time
    m_ulStartTime = CClientTime::GetTime ();
}


std::string CServerListItem::Pulse ( bool bCanSendQuery )
{   // Queries the server on it's query port (ASE protocol)
    // and returns whether it is done scanning
    if ( bScanned || bSkipped ) return "Done";

    char szBuffer[SERVER_LIST_QUERY_BUFFER] = {0};

    if ( m_ulQueryStart == 0 ) {
        if ( !bCanSendQuery )
            return "NotSentQuery";
        Query ();
        return "SentQuery";
    } else {
        // Poll the socket
        sockaddr_in clntAddr;
        int addrLen = sizeof ( clntAddr );
        int len = recvfrom ( m_Socket, szBuffer, SERVER_LIST_QUERY_BUFFER, MSG_PARTIAL, (sockaddr *) &clntAddr, &addrLen );
        int error = WSAGetLastError();
        if ( len >= 0 ) {
            // Parse data
            ParseQuery ( szBuffer, len );
            CloseSocket ();
            bMaybeOffline = false;
            uiNoReplyCount = 0;
            uiRevision++;           // To flag browser gui update
            GetServerCache ()->SetServerCachedInfo ( this );    // Save parsed info in the cache
            return "ParsedQuery";
        }

        if ( CClientTime::GetTime () - m_ulQueryStart > SERVER_LIST_ITEM_TIMEOUT )
        {
            bMaybeOffline = true;       // Flag to help 'Include offline' browser option
            nPlayers = 0;               // We don't have player names, so zero this now
            uiRevision++;               // To flag browser gui update
            if ( uiQueryRetryCount == 0 )
                uiNoReplyCount++;       // Keep a persistant count of failures. (When uiNoReplyCount gets to 3, the server is removed from the Server Cache)

            if ( uiQueryRetryCount < 3 )
            {
                // Try again
                uiQueryRetryCount++;
                GetServerCache ()->SetServerCachedInfo ( this );
                Query ();
                return "ResentQuery";
            }
            else
            {
                // Give up
                bSkipped = true;
                CloseSocket ();
                GetServerCache ()->SetServerCachedInfo ( this );
                return "NoReply";
            }
        }

        return "WaitingReply";
    }
}


void CServerListItem::Query ( void )
{   // Performs a query according to ASE protocol
    sockaddr_in addr;
    memset ( &addr, 0, sizeof(addr) );
    addr.sin_family = AF_INET;
    addr.sin_addr = Address;
    addr.sin_port = htons ( usQueryPort );

    // Initialize socket on demand
    if ( m_Socket == INVALID_SOCKET )
    {
        m_Socket = socket ( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
        u_long flag = 1;
        ioctlsocket ( m_Socket, FIONBIO, &flag );
    }

    sendto ( m_Socket, "r", 1, 0, (sockaddr *) &addr, sizeof(addr) );
    m_ulQueryStart = CClientTime::GetTime ();
}


bool ReadString ( std::string &strRead, const char * szBuffer, unsigned int &i, unsigned int nLength )
{
    if ( i <= nLength )
    {
        unsigned char len = szBuffer[i];
        if ( i + len <= nLength )
        {
            const char *ptr = &szBuffer[i + 1];
            i += len;
            strRead = std::string ( ptr, len - 1 );
            return true;
        }
        i++;
    }
    return false;
}


bool CServerListItem::ParseQuery ( const char * szBuffer, unsigned int nLength )
{
    // Check length
    if ( nLength < 15 )
        return false;

    // Check header
    if ( strncmp ( szBuffer, "EYE2", 4 ) != 0 )
        return false;
    
    // Get IP as string
    const char* szIP = inet_ntoa ( Address );

    // Calculate the ping/latency
    nPing = ( CClientTime::GetTime () - m_ulQueryStart );

    // Parse relevant data
    std::string strTemp;
    unsigned int i = 4;

    // IP
    strHost = szIP;

    // Game
    if ( !ReadString ( strGame, szBuffer, i, nLength ) )
        return false;

    // Port
    if ( !ReadString ( strTemp, szBuffer, i, nLength ) )
        return false;
    usGamePort      = atoi ( strTemp.c_str () );

    // Server name
    if ( !ReadString ( strName, szBuffer, i, nLength ) )
        return false;

    // Game type
    if ( !ReadString ( strType, szBuffer, i, nLength ) )
        return false;

    // Map name
    if ( !ReadString ( strMap, szBuffer, i, nLength ) )
        return false;

    // Version
    if ( !ReadString ( strVersion, szBuffer, i, nLength ) )
        return false;

    if ( strVersion != MTA_DM_ASE_VERSION )
        return false;

    // Got space for password, serial verification, player count, players max?
    if ( i + 4 > nLength )
    {
        return false;
    }

    bPassworded = ( szBuffer[i++] == 1 );
    bSerials = ( szBuffer[i++] == 1 );
    nPlayers = (unsigned char)szBuffer[i++];
    nMaxPlayers = (unsigned char)szBuffer[i++];

    // Get player nicks
    vecPlayers.clear ();
    while ( i < nLength )
    {
        std::string strPlayer;

        if ( ReadString ( strPlayer, szBuffer, i, nLength ) )
        {
            // Remove color code, unless that results in an empty string
            std::string strResult = RemoveColorCode ( strPlayer.c_str () );
            if ( strResult.length () == 0 )
                strResult = strPlayer;
            vecPlayers.push_back ( strResult );
        }
    }

    bScanned = true;
    return true;
}
