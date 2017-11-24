#include "StdAfx.h"
#include "TableFrameHook.h"


//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CPersonalTableFrameHook::m_wPlayerCount=MAX_CHAIR;			//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//���캯��
CPersonalTableFrameHook::CPersonalTableFrameHook()
{
	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pPersonalRoomEventSink=NULL;

	return;
}

//��������
CPersonalTableFrameHook::~CPersonalTableFrameHook(void)
{
}

//�ӿڲ�ѯ
void *  CPersonalTableFrameHook::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPersonalTableFrameHook,Guid,dwQueryVer);
	QUERYINTERFACE(IPersonalTableFrameHook,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPersonalTableFrameHook,Guid,dwQueryVer);
	return NULL;
}

//
bool CPersonalTableFrameHook::SetPersonalRoomEventSink(IUnknownEx * pIUnknownEx)
{
	m_pPersonalRoomEventSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IPersonalRoomEventSink);

	return true;
}

//��ʼ��
bool  CPersonalTableFrameHook::InitTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//��Ϸ��ʼ
bool CPersonalTableFrameHook::OnEventGameStart(WORD wChairCount)
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnEventGameStart(m_pITableFrame, wChairCount);
	}

	return false;
}

//��Ϸ��ʼ
void CPersonalTableFrameHook::PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID,  WORD wChairID,   DWORD dwKindID, TCHAR * szRoomID,  TCHAR * szPersonalRoomGUID)
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->PersonalRoomWriteJoinInfo(dwUserID, wTableID,  wChairID,  dwKindID, szRoomID,szPersonalRoomGUID);
	}
}

//��Ϸ����
bool  CPersonalTableFrameHook::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnEventGameEnd(m_pITableFrame,wChairID,pIServerUserItem,cbReason);
	}
	
	return false;
}

//��Ϸ����
bool  CPersonalTableFrameHook::OnEventGameEnd(WORD wTableID,  WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen,  byte * cbSpecialInfo, SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo )
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnEventGameEnd(wTableID,  wChairCount, dwDrawCountLimit, dwPersonalPlayCount, nSpecialInfoLen, cbSpecialInfo, sysStartTime , PersonalUserScoreInfo);
	}

	return false;
}


//�û�����
bool CPersonalTableFrameHook::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pPersonalRoomEventSink!=NULL)  
	{
		return m_pPersonalRoomEventSink->OnActionUserSitDown(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//�û�����
bool CPersonalTableFrameHook::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pPersonalRoomEventSink!=NULL) 
	{
		return m_pPersonalRoomEventSink->OnActionUserStandUp(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//�û�ͬ��
bool CPersonalTableFrameHook::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnActionUserOnReady(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem ,pData ,wDataSize);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
