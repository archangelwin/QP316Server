#include "StdAfx.h"
#include "PersonalRoomGame.h"
#include  "..\��Ϸ������\TableFrame.h"
#include  "..\��Ϸ������\AttemperEngineSink.h"


#define ZZMJ_KIND_ID  386
#define HZMJ_KIND_ID  389
#define ZJH_KIND_ID	  6
#define ZJH_MAX_PLAYER 5
#define NN_KIND_ID	  27
#define NN_MAX_PLAYER 4
#define TBZ_KIND_ID	  47
#define TBZ_MAX_PLAYER 4
#define SET_RULE			1
////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��������
#define INVALID_VALUE				0xFFFF								//��Чֵ

//ʱ�Ӷ���
#define IDI_SWITCH_STATUS			(IDI_MATCH_MODULE_START+1)					//�л�״̬
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+2)					//�����û�
#define IDI_CHECK_START_SIGNUP		(IDI_MATCH_MODULE_START+3)					//��ʼ����
#define IDI_CHECK_END_SIGNUP		(IDI_MATCH_MODULE_START+4)					//��ʼ��ֹ
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+5)					//��ʼʱ��
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+6)					//����ʱ��

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CPersonalRoomGame::CPersonalRoomGame()
{

	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//�ں˽ӿ�
	m_ppITableFrame=NULL;
	m_pITimerEngine=NULL;
	m_pIDBCorrespondManager=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//����ӿ�
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
}

CPersonalRoomGame::~CPersonalRoomGame(void)
{
	//�ͷ���Դ
	SafeDeleteArray(m_ppITableFrame);

	//�رն�ʱ��
	m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);
	m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);
	m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);			

}

//�ӿڲ�ѯ
VOID* CPersonalRoomGame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IPersonalRoomItem,Guid,dwQueryVer);
	QUERYINTERFACE(IPersonalRoomEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IPersonalRoomItem,Guid,dwQueryVer);
	return NULL;
}

//����֪ͨ
void CPersonalRoomGame::OnStartService()
{

}

//������
bool CPersonalRoomGame::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//��������
	CPersonalTableFrameHook * pTableFrameHook=new CPersonalTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetPersonalRoomEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//���ýӿ�
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//��ʼ���ӿ�
bool CPersonalRoomGame::InitPersonalRooomInterface(tagPersonalRoomManagerParameter & PersonalRoomManagerParameter)
{
	//��������
	m_pPersonalRoomOption=PersonalRoomManagerParameter.pPersonalRoomOption;
	m_pGameServiceOption=PersonalRoomManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=PersonalRoomManagerParameter.pGameServiceAttrib;

	//�ں����
	m_pITimerEngine=PersonalRoomManagerParameter.pITimerEngine;
	m_pIDBCorrespondManager=PersonalRoomManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=PersonalRoomManagerParameter.pTCPNetworkEngine;
	m_pITCPNetworkEngine = PersonalRoomManagerParameter.pITCPNetworkEngine;
	m_pITCPSocketService = PersonalRoomManagerParameter.pITCPSocketService;

	//�������		
	m_pIGameServiceFrame=PersonalRoomManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=PersonalRoomManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=PersonalRoomManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=PersonalRoomManagerParameter.pIServerUserItemSink;

	//��������
	//m_DistributeManage.SetDistributeRule(m_pMatchOption->cbDistributeRule);

	//��������
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}	

	return true;
}

//ʱ���¼�
bool CPersonalRoomGame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	return true;
}

//���ݿ��¼�
bool CPersonalRoomGame::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	switch(wRequestID)
	{
	case DBO_GR_CREATE_SUCCESS:			//�����ɹ�
		{
			return OnDBCreateSucess(dwContextID,pData,wDataSize, pIServerUserItem);
		}
	case DBO_GR_CREATE_FAILURE:			//����ʧ��
		{
			return OnDBCreateFailure(dwContextID,pData,wDataSize, pIServerUserItem);
		}
	case DBO_GR_CANCEL_CREATE_RESULT:		//ȡ������
		{
			return OnDBCancelCreateTable(dwContextID,pData,wDataSize, pIServerUserItem);
		}
	case DBO_GR_LOAD_PERSONAL_ROOM_OPTION:
		{
			//ASSERT(wDataSize == sizeof(tagPersonalRoomOption));
			//if (sizeof(tagPersonalRoomOption)!=wDataSize) return false;
			//tagPersonalRoomOption * pPersonalRoomOption = (tagPersonalRoomOption *)pData;
			//memcpy(&m_PersonalRoomOption, pPersonalRoomOption, sizeof(tagPersonalRoomOption) );
			return true;
		}
	case DBO_GR_LOAD_PERSONAL_PARAMETER:
		{
			return OnDBLoadPersonalParameter(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	case DBO_GR_DISSUME_TABLE_RESULTE:
		{
			return OnDBDissumeTableResult(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	case DBO_GR_CURRENCE_ROOMCARD_AND_BEAN:
		{
			return OnDBCurrenceRoomCardAndBeant(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	}
	return true;
}

//Լս���¼�
bool CPersonalRoomGame::OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_GR_CREATE_TABLE:
		{
			return OnTCPNetworkSubCreateTable(pData, wDataSize, dwSocketID, pIServerUserItem);
		}
	case SUB_GR_CANCEL_REQUEST:
		{
			return OnTCPNetworkSubCancelRequest(pData, wDataSize, dwSocketID,  pIServerUserItem);
		}
	case SUB_GR_REQUEST_REPLY:
		{
			return OnTCPNetworkSubRequestReply(pData, wDataSize, dwSocketID,  pIServerUserItem);
		}
	case SUB_GR_HOSTL_DISSUME_TABLE://����ǿ�ƽ�ɢ����
		{
			return OnTCPNetworkSubHostDissumeTable(pData, wDataSize, dwSocketID,  pIServerUserItem);
		}
	}

	return true;
}

//�û�����
bool CPersonalRoomGame::OnEventUserItemScore(IServerUserItem * pIServerUserItem,BYTE cbReason)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	//�������
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//��������
	m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//��������
	CMD_GR_MobileUserScore MobileUserScore;
	ZeroMemory(&MobileUserScore,sizeof(MobileUserScore));

	//��������
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//�������
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//��������
	m_pIGameServiceFrame->SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));


	//��ʱд��
	if (
		(CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)
		&&(pIServerUserItem->IsVariation()==true))
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//��ȡ����
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//˽�˷���������ݽṹ
		DBR_GR_WritePersonalGameScore  WritePersonalScore;
		memcpy(&WritePersonalScore.VariationInfo, &WriteGameScore.VariationInfo, sizeof(WritePersonalScore.VariationInfo));

		//��������
		if(pIServerUserItem->IsAndroidUser()==true)
		{
			WriteGameScore.VariationInfo.lScore=0;
			WriteGameScore.VariationInfo.lGrade=0;
			WriteGameScore.VariationInfo.lInsure=0;
			WriteGameScore.VariationInfo.lRevenue=0;
		}


		if ((lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) != 0))
		{
			WriteGameScore.VariationInfo.lScore=0;
			WriteGameScore.VariationInfo.lGrade=0;
			WriteGameScore.VariationInfo.lInsure=0;
			WriteGameScore.VariationInfo.lRevenue=0;
		}


		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);


		//Լս����ID
		DWORD dwRoomHostID = 0;
		TCHAR szPersonalRoomID[ROOM_ID_LEN] = {0};
		//INT_PTR nSize = m_TableFrameArray.GetCount();
		for(INT_PTR i = 0; i < m_pGameServiceOption->wTableCount; ++i)
		{
			CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[i];
			if (pTableFrame)
			{
				if (pTableFrame->GetTableID() == pIServerUserItem->GetTableID())
				{
					lstrcpyn(szPersonalRoomID, pTableFrame->GetPersonalTableID(), sizeof(szPersonalRoomID));
					dwRoomHostID = pTableFrame->GetRecordTableOwner();
					wsprintf(WritePersonalScore.szPersonalRoomGUID, TEXT("%s"), pTableFrame->GetPersonalRoomGUID());
					WritePersonalScore.nGamesNum = pTableFrame->GetDrawCount();
				}
			}
		}

		//Լս����������ݽṹ
		WritePersonalScore.dwRoomHostID = dwRoomHostID;
		WritePersonalScore.bTaskForward = WriteGameScore.bTaskForward;
		WritePersonalScore.dwClientAddr = WriteGameScore.dwClientAddr;
		WritePersonalScore.dwDBQuestID  = WriteGameScore.dwDBQuestID;
		WritePersonalScore.dwInoutIndex = WriteGameScore.dwInoutIndex;

		WritePersonalScore.dwUserID		= WriteGameScore.dwUserID;
		WritePersonalScore.dwPersonalRoomTax =static_cast<DWORD>(m_pPersonalRoomOption->lPersonalRoomTax);
		lstrcpyn(WritePersonalScore.szRoomID, szPersonalRoomID, sizeof(WritePersonalScore.szRoomID));	


		//�������ݿ�д�����
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			m_pIDBCorrespondManager->PostDataBaseRequest(WritePersonalScore.dwUserID,DBR_GR_WRITE_PERSONAL_GAME_SCORE,0L,&WritePersonalScore,sizeof(WritePersonalScore), TRUE);
		}
	}

	//֪ͨ����
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		((CTableFrame*)m_ppITableFrame[pIServerUserItem->GetTableID()])->OnUserScroeNotify(pIServerUserItem->GetChairID(),pIServerUserItem,cbReason);
	}

	return true;
}

//�û�����
bool CPersonalRoomGame::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//�û�״̬
bool CPersonalRoomGame::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//�������
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//�û�Ȩ��
bool CPersonalRoomGame::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//�û���¼
bool CPersonalRoomGame::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	

	return true;
}

//�û��ǳ�
bool CPersonalRoomGame::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{

	return true;
}

//��¼���
bool CPersonalRoomGame::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	
	return true;
}



//��Ϸ��ʼ
bool CPersonalRoomGame::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	return true;
}

//��Ϸ��ʼ
void CPersonalRoomGame::PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID,  WORD wChairID, DWORD dwKindID,  TCHAR * szRoomID,  TCHAR * szPersonalRoomGUID)
{
	//д������Ϣ
	DBR_GR_WriteJoinInfo JoinInfo;
	ZeroMemory(&JoinInfo, sizeof(DBR_GR_WriteJoinInfo));

	JoinInfo.dwUserID = dwUserID;
	JoinInfo.wTableID = wTableID;
	JoinInfo.wChairID = wChairID;
	JoinInfo.wKindID = dwKindID;
	lstrcpyn(JoinInfo.szRoomID,  szRoomID, sizeof(JoinInfo.szRoomID) );
	
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[wTableID];
	if(pTableFrame)
	{
		wsprintf(JoinInfo.szPeronalRoomGUID,  TEXT("%s"), pTableFrame->GetPersonalRoomGUID() );
	}

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(0, DBR_GR_WRITE_JOIN_INFO, 0, &JoinInfo, sizeof(JoinInfo));
	// 			//�����ƽ�
	// 			CAttemperEngineSink *pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;						
	// 			WORD wBindIndex = pUserItem->GetBindIndex();
	// 			tagBindParameter * pBind=pAttemperEngineSink->GetBindParameter(wBindIndex);
	// 			DBR_GR_PerformRoomTaskProgress PerformRoom;
	// 			ZeroMemory(&PerformRoom, sizeof(PerformRoom));
	// 			PerformRoom.dwUserID = pUserItem->GetUserID();
	// 			PerformRoom.wKindID=pTableFrame->GetGameServiceOption()->wKindID;
	// 			PerformRoom.nDrawCount =1;			
	// 			m_pIDBCorrespondManager->PostDataBaseRequest(PerformRoom.dwUserID, DBR_GR_WRITE_ROOM_TASK_PROGRESS,pBind->dwSocketID, &PerformRoom, sizeof(PerformRoom));
}

//��Ϸ����
bool CPersonalRoomGame::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return true;
}

//��Ϸ����
bool CPersonalRoomGame::OnEventGameEnd(WORD wTableID,  WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen ,byte * cbSpecialInfo,SYSTEMTIME sysStartTime,  tagPersonalUserScoreInfo * PersonalUserScoreInfo)
{
	DWORD dwTimeNow = (DWORD)time(NULL);
	if((dwDrawCountLimit!=0 && dwDrawCountLimit <= dwPersonalPlayCount) )
	{

		CMD_GR_PersonalTableEnd PersonalTableEnd;
		ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

		lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("Լս������"), CountArray(PersonalTableEnd.szDescribeString));
		TCHAR szInfo[260] = {0};
		for(int i = 0; i < wChairCount; ++i)
		{
			PersonalTableEnd.lScore[i] = PersonalUserScoreInfo[i].lScore;
		}
		//���������Ϣ
		memcpy( PersonalTableEnd.cbSpecialInfo, cbSpecialInfo, nSpecialInfoLen);
		PersonalTableEnd.nSpecialInfoLen = nSpecialInfoLen;
		CopyMemory(&PersonalTableEnd.sysStartTime,&sysStartTime,sizeof(SYSTEMTIME));
		GetLocalTime(&PersonalTableEnd.sysEndTime);
		WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * wChairCount;

		CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[wTableID];
		for(int i = 0; i < wChairCount; ++i)
		{
			IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
			if(pUserItem == NULL) continue;
			
			

			PersonalTableEnd.lScore[i] =PersonalUserScoreInfo[i].lScore;
			m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd) );
		}


		//��ֹ�ͻ�����Ϊ״̬Ϊfree��ȡ�����û�ָ��
		for(int i = 0; i < wChairCount; ++i)
		{
			IServerUserItem* pTableUserItem = pTableFrame->GetTableUserItem(i);
			if(pTableUserItem == NULL) continue;

			pTableFrame->PerformStandUpAction(pTableUserItem);
		}

		//��ɢ����
		m_pIGameServiceFrame->DismissPersonalTable(m_pGameServiceOption->wServerID, wTableID);
		return true;
	}

	return false;
}

//�û�����
bool CPersonalRoomGame::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//�û�����
bool CPersonalRoomGame::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	return true;
}

 //�û�ͬ��
bool CPersonalRoomGame::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	return true; 
}


//��������
bool CPersonalRoomGame::OnTCPNetworkSubCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_CreateTable));
	if (wDataSize!=sizeof(CMD_GR_CreateTable)) return false;

	//��ȡ����
	CMD_GR_CreateTable * pCreateTable = (CMD_GR_CreateTable*)pData;
	ASSERT(pCreateTable!=NULL);
	
	int nStatus = pIServerUserItem->GetUserStatus() ;
	//������״̬���ж�����Ƿ�����Ϸ�У�����ڣ����ܴ�������
	if ((pIServerUserItem->GetUserStatus() == US_OFFLINE) || (nStatus <= US_PLAYING   &&  nStatus  >= US_SIT))
	{
		//��������
		CMD_GR_CreateFailure CreateFailure;
		ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

		CreateFailure.lErrorCode = 1;
		lstrcpyn(CreateFailure.szDescribeString, TEXT("�������Լս����Ϸ�У����ܴ������䣡"), CountArray(CreateFailure.szDescribeString));

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));
		return true;
	}

	//IServerUserItem * pIServerUserItem=m_pIServerUserManager->SetServerUserItemSink();


	//Ѱ�ҿ�������
	//INT_PTR nSize = m_ppITableFrame.GetCount();
	TCHAR szInfo[260] = {0};
	for(INT_PTR i = 0; i < m_pGameServiceOption->wTableCount; ++i)
	{
		CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[i];

		if(pTableFrame->GetNullChairCount() == pTableFrame->GetChairCount() && pTableFrame->IsPersonalTableLocked() == false)
		{
			//��������
			pTableFrame->SetPersonalTableLlocked(true);

			//���Ӻ�
			DWORD dwTableID = pTableFrame->GetTableID();

			//��������
			DBR_GR_CreateTable CreateTable;
			ZeroMemory(&CreateTable, sizeof(DBR_GR_CreateTable));

			CreateTable.dwUserID = pIServerUserItem->GetUserID();
			CreateTable.dwClientAddr = pIServerUserItem->GetClientAddr();
			CreateTable.dwServerID = m_pGameServiceOption->wServerID;
			CreateTable.dwTableID = dwTableID;
			CreateTable.dwDrawCountLimit = pCreateTable->dwDrawCountLimit;
			CreateTable.dwDrawTimeLimit = pCreateTable->dwDrawTimeLimit;
			CreateTable.lCellScore = static_cast<LONG>(pCreateTable->lCellScore);
			CreateTable.dwRoomTax = pCreateTable->dwRoomTax;
			CreateTable.wJoinGamePeopleCount = pCreateTable->wJoinGamePeopleCount;
			lstrcpyn(CreateTable.szPassword, pCreateTable->szPassword, CountArray(CreateTable.szPassword));
			memcpy(CreateTable.cbGameRule, pCreateTable->cbGameRule,  CountArray(CreateTable.cbGameRule));

			//Ͷ������
			m_pIDBCorrespondManager->PostDataBaseRequest(CreateTable.dwUserID, DBR_GR_CREATE_TABLE, dwSocketID, &CreateTable, sizeof(DBR_GR_CreateTable));

			return true;
		}
	}

	//��������
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = 1;
	lstrcpyn(CreateFailure.szDescribeString, TEXT("Ŀǰ����ϷԼս�������������Ժ����ԣ�"), CountArray(CreateFailure.szDescribeString));

	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	return true;
}
//ȡ������
bool CPersonalRoomGame::OnTCPNetworkSubCancelRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_GR_CancelRequest));
	if(wDataSize != sizeof(CMD_GR_CancelRequest)) return false;

	//��ȡ����
	CMD_GR_CancelRequest * pCancelRequest = (CMD_GR_CancelRequest*)pData;
	ASSERT(pCancelRequest!=NULL);
	//��ȡ�û�
	//WORD wBindIndex=LOWORD(dwSocketID);
	//IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCancelRequest->dwTableID];
	ASSERT(pTableFrame != NULL);

	//��������ɢ�ķ��䲻�ǵ�ǰ�����򷵻�
	//if (lstrcmp(pTableFrame->GetPersonalTableID(),  pCancelRequest->szRoomID) != 0)
	//{

	//	return true;
	//}

	if(pTableFrame->CancelTableRequest(pCancelRequest->dwUserID, static_cast<WORD>(pCancelRequest->dwChairID)) == false) return false;

	//ת������
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//�����û�

		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL || pUserItem == pIServerUserItem) continue;

		m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_REQUEST, pData, wDataSize);
	}

	return true;
}
//ȡ����
bool CPersonalRoomGame::OnTCPNetworkSubRequestReply(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_GR_RequestReply));
	if(wDataSize != sizeof(CMD_GR_RequestReply)) return false;

	//��ȡ����
	CMD_GR_RequestReply * pRequestReply = (CMD_GR_RequestReply*)pData;
	ASSERT(pRequestReply!=NULL);

	//��ȡ�û�
	//WORD wBindIndex=LOWORD(dwSocketID);
	//IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pRequestReply->dwTableID];
	ASSERT(pTableFrame != NULL);
	//if(pRequestReply->dwUserID == pTableFrame->GetTableOwner()->GetUserID()) return false;

	//ת������
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//�����û�
		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL || pUserItem == pIServerUserItem) continue;

		m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_REPLY, pData, wDataSize);
	}

	if(pTableFrame->CancelTableRequestReply(pRequestReply->dwUserID, pRequestReply->cbAgree) == false) return false;

	return true;
}

//ȡ������
bool CPersonalRoomGame::OnTCPNetworkSubHostDissumeTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_GR_HostDissumeGame));
	if(wDataSize != sizeof(CMD_GR_HostDissumeGame)) return false;

	//��ȡ����
	CMD_GR_HostDissumeGame * pCancelRequest = (CMD_GR_HostDissumeGame*)pData;
	ASSERT(pCancelRequest!=NULL);
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	//IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCancelRequest->dwTableID];

	ASSERT(pTableFrame != NULL);

	//if (lstrcmp(pTableFrame->GetPersonalTableID(),  pCancelRequest->szRoomID) != 0)
	//{
	//	SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GF_ALREADY_CANCLE, NULL, 0);
	//	return true;
	//}


	//Ͷ������
	DBR_GR_CancelCreateTable CancelCreateTable;
	ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

	CancelCreateTable.dwUserID = pTableFrame->GetTableOwner();
	CancelCreateTable.dwReason = pTableFrame->GetDrawCount();
	CancelCreateTable.dwDrawCountLimit = pTableFrame->GetPersonalTableParameter().dwPlayTurnCount;
	CancelCreateTable.dwDrawTimeLimit = pTableFrame->GetPersonalTableParameter().dwPlayTimeLimit;
	CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;
	lstrcpyn(CancelCreateTable.szRoomID,  pTableFrame->GetPersonalTableID(), sizeof(CancelCreateTable.szRoomID) );



	m_pIDBCorrespondManager->PostDataBaseRequest(CancelCreateTable.dwUserID, DBR_GR_HOST_CANCEL_CREATE_TABLE, dwSocketID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));


	pTableFrame->HostDissumeGame(true);

	if(pTableFrame)
	{
		pTableFrame->SetPersonalTableLlocked(false);
	}


	//��������
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));
	DismissTable.dwSocketID = dwSocketID;
	DismissTable.dwServerID = m_pGameServiceOption->wServerID;
	DismissTable.dwTableID = pTableFrame->GetTableID();

	//������Ϣ
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));


	return true;
}

//Լս����������󷿼���Ϣ
bool CPersonalRoomGame::OnDBCurrenceRoomCardAndBeant(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBO_GR_CurrenceRoomCardAndBeans));
	if (sizeof(DBO_GR_CurrenceRoomCardAndBeans)!=wDataSize) return false;
	DBO_GR_CurrenceRoomCardAndBeans * pCardAndBeans = (DBO_GR_CurrenceRoomCardAndBeans *)pData;
	CMD_GR_CurrenceRoomCardAndBeans  CurrenceRoomCardAndBeans;
	CurrenceRoomCardAndBeans.dbBeans = pCardAndBeans->dbBeans;
	CurrenceRoomCardAndBeans.lRoomCard = pCardAndBeans->lRoomCard;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CURRECE_ROOMCARD_AND_BEAN, &CurrenceRoomCardAndBeans, sizeof(CMD_GR_CurrenceRoomCardAndBeans));

	return true;

}

//˽������
bool CPersonalRoomGame::OnDBDissumeTableResult(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBO_GR_DissumeResult));
	if (sizeof(DBO_GR_DissumeResult)!=wDataSize) return false;
	DBO_GR_DissumeResult * pDissumeResult = (DBO_GR_DissumeResult *)pData;
	CMD_GR_DissumeTable  DissumeTable;
	DissumeTable.cbIsDissumSuccess = pDissumeResult->cbIsDissumSuccess;
	lstrcpyn(DissumeTable.szRoomID , pDissumeResult->szRoomID, sizeof(DissumeTable.szRoomID));
	DissumeTable.sysDissumeTime = pDissumeResult->sysDissumeTime;

	//��ȡ������ҵ���Ϣ
	for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
	{
		memcpy(&DissumeTable.PersonalUserScoreInfo[i],  &pDissumeResult->PersonalUserScoreInfo[i],  sizeof(tagPersonalUserScoreInfo));
	}

	m_pITCPNetworkEngine->SendData(pDissumeResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_HOST_DISSUME_TABLE_RESULT, &DissumeTable, sizeof(CMD_GR_DissumeTable));

	return true;

}


//˽������
bool CPersonalRoomGame::OnDBLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagPersonalTableParameter)==0);
	if (wDataSize%sizeof(tagPersonalTableParameter)!=0) return false;

	//�������
	ASSERT(m_PersonalTableParameterArray.GetCount() == 0);
	if(m_PersonalTableParameterArray.GetCount() != 0)
	{
		INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
		for(INT_PTR i = 0; i < nSize; ++i)
		{
			tagPersonalTableParameter* pPersonalTableParameter = m_PersonalTableParameterArray.GetAt(i);
			SafeDelete(pPersonalTableParameter);
		}
		m_PersonalTableParameterArray.RemoveAll();
	}

	//����ת��
	WORD wCount = wDataSize/sizeof(tagPersonalTableParameter);
	tagPersonalTableParameter* pParameterData = (tagPersonalTableParameter*)pData;

	//��������
	for(int i = 0; i < wCount; ++i)
	{
		tagPersonalTableParameter* pPersonalTableParameter = new tagPersonalTableParameter;
		CopyMemory(pPersonalTableParameter,&pParameterData[i],sizeof(tagPersonalTableParameter));
		m_PersonalTableParameterArray.Add(pPersonalTableParameter);
	}

	return true;
}

//�����ɹ�
bool CPersonalRoomGame::OnDBCreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_GR_CreateSuccess));
	if(wDataSize != sizeof(DBO_GR_CreateSuccess)) return false;

	//�ж�����
	CAttemperEngineSink * pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;
	tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	if (pIServerUserItem==NULL) return false;

	DBO_GR_CreateSuccess* pCreateSuccess = (DBO_GR_CreateSuccess*)pData;

	//��ȡ��Ϣ
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCreateSuccess->dwTableID];

	if(pTableFrame == NULL)
	{
		return true;
	}

	WORD wChairID = pTableFrame->GetNullChairID();

	//��ʼ����
	LONGLONG lIniScore = 0;
	byte cbIsJoinGame = 0;
	INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagPersonalTableParameter* pTableParameter = m_PersonalTableParameterArray.GetAt(i);
		if(pTableParameter->dwPlayTurnCount == pCreateSuccess->dwDrawCountLimit && pTableParameter->dwPlayTimeLimit == pCreateSuccess->dwDrawTimeLimit)
		{
			lIniScore = pTableParameter->lIniScore;

			//��������Ϣ��Ϊ�ƶ���������Ϣ
			pTableParameter->wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;	//�μ���Ϸ���������
			pTableParameter->lCellScore = pCreateSuccess->lCellScore;													//Լս�������׷�

			pTableParameter->dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//˽�˷Ž�����Ϸ��������
			pTableParameter->dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;							//Լս��������Ϸ�����ʱ�� ��λ��

			cbIsJoinGame = pTableParameter->cbIsJoinGame ;

			break;
		}
	}

	cbIsJoinGame = m_pPersonalRoomOption->cbIsJoinGame;
	if (pCreateSuccess->lCellScore != 0)
	{
			pTableFrame->SetCellScore(pCreateSuccess->lCellScore);
	}

	pTableFrame->SetGameRule(pCreateSuccess->cbGameRule, RULE_LEN);
	//���ɷ���Ψһ��ʶ
	TCHAR szInfo[32] = {0};
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	wsprintf(szInfo, TEXT("%d%d%d%d%d%d%d%d"), pIServerUserItem->GetUserID(),  tm.wYear,  tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	pTableFrame->SetCreatePersonalTime(tm);
	//����˽�˷���Ψһ��ʶ
	pTableFrame->SetPersonalRoomGUID(szInfo, lstrlen(szInfo));


	//���ʹ�õ��ǽ�����ݿ�
	if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
	{
		pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, 0);
		pTableFrame->SetDataBaseMode(0);
	}
	else
	{
		pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, lIniScore);
		pTableFrame->SetDataBaseMode(1);
	}


	//��������������Ϣ
	tagPersonalTableParameter PersonalTableParameter;
	PersonalTableParameter.lIniScore = lIniScore;
	PersonalTableParameter.wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;		//�μ���Ϸ���������
	PersonalTableParameter.lCellScore = pCreateSuccess->lCellScore;													//Լս�������׷�
	PersonalTableParameter.dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//˽�˷Ž�����Ϸ��������
	PersonalTableParameter.dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;								//Լս��������Ϸ�����ʱ�� ��λ��
	PersonalTableParameter.cbIsJoinGame = cbIsJoinGame;

	PersonalTableParameter.dwTimeAfterBeginCount = m_pPersonalRoomOption->dwTimeAfterBeginCount;
	PersonalTableParameter.dwTimeAfterCreateRoom =m_pPersonalRoomOption->dwTimeAfterCreateRoom;
	PersonalTableParameter.dwTimeNotBeginGame = m_pPersonalRoomOption->dwTimeNotBeginGame;
	PersonalTableParameter.dwTimeOffLineCount = m_pPersonalRoomOption->dwTimeOffLineCount;

	pTableFrame->SetPersonalTableParameter(PersonalTableParameter, *m_pPersonalRoomOption);

	//cbGameRule[1] Ϊ  2 ��3 ��4 ��5, 0�ֱ��Ӧ 2�� �� 3�� �� 4�� �� 5�� �� 2-5�� �⼸������
	if (pCreateSuccess->cbGameRule[0] == SET_RULE ) 
	{
		if( pCreateSuccess->cbGameRule[1] != 0)
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
			CMD_GR_ChangeChairCount  ChangeChairCount;
			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

			//��ͻ��˷������������������ı����Ϣ
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
		}
		else
		{
			pTableFrame->SetTableChairCount( pCreateSuccess->cbGameRule[2]);
			CMD_GR_ChangeChairCount  ChangeChairCount;
			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[2];

			//��ͻ��˷������������������ı����Ϣ
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
		}

	}


	//תת�齫����һ�����������ӵĸ���
	if (m_pGameServiceAttrib->wKindID == ZZMJ_KIND_ID || m_pGameServiceAttrib->wKindID  == HZMJ_KIND_ID)
	{
		if (pCreateSuccess->cbGameRule[0] == SET_RULE) 
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);

			CMD_GR_ChangeChairCount  ChangeChairCount;
			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

			//��ͻ��˷������������������ı����Ϣ
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
		}
	}

	if ( ((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0 )
	{
		//թ�𻨷����������Ӹ���
		if (m_pGameServiceAttrib->wKindID == ZJH_KIND_ID)
		{
			//cbGameRule[1] Ϊ  2 ��3 ��4 ��5, 0�ֱ��Ӧ 2�� �� 3�� �� 4�� �� 5�� �� 2-5�� �⼸������
			if (pCreateSuccess->cbGameRule[0] == SET_RULE && pCreateSuccess->cbGameRule[1] != 0) 
			{
				pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
				CMD_GR_ChangeChairCount  ChangeChairCount;
				ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

				//��ͻ��˷������������������ı����Ϣ
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
			else
			{

				pTableFrame->SetTableChairCount(ZJH_MAX_PLAYER);
				CMD_GR_ChangeChairCount  ChangeChairCount;
				ChangeChairCount.dwChairCount = ZJH_MAX_PLAYER;

				//��ͻ��˷������������������ı����Ϣ
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
		}

		//թ�𻨷����������Ӹ���
		if (m_pGameServiceAttrib->wKindID == NN_KIND_ID)
		{
			//cbGameRule[1] Ϊ  2 ��3 ��4 ��5, 0�ֱ��Ӧ 2�� �� 3�� �� 4�� �� 5�� �� 2-5�� �⼸������
			if (pCreateSuccess->cbGameRule[0] == SET_RULE && pCreateSuccess->cbGameRule[1] != 0) 
			{
				pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
				CMD_GR_ChangeChairCount  ChangeChairCount;
				ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

				//��ͻ��˷������������������ı����Ϣ
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
			else
			{

				pTableFrame->SetTableChairCount(NN_MAX_PLAYER);
				CMD_GR_ChangeChairCount  ChangeChairCount;
				ChangeChairCount.dwChairCount = NN_MAX_PLAYER;

				//��ͻ��˷������������������ı����Ϣ
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
		}

		//թ�𻨷����������Ӹ���
		if (m_pGameServiceAttrib->wKindID == TBZ_KIND_ID)
		{
			//cbGameRule[1] Ϊ  2 ��3 ��4 ��5, 0�ֱ��Ӧ 2�� �� 3�� �� 4�� �� 5�� �� 2-5�� �⼸������
			if (pCreateSuccess->cbGameRule[0] == SET_RULE && pCreateSuccess->cbGameRule[1] != 0) 
			{
				pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
				CMD_GR_ChangeChairCount  ChangeChairCount;
				ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

				//��ͻ��˷������������������ı����Ϣ
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
			else
			{

				pTableFrame->SetTableChairCount(TBZ_MAX_PLAYER);
				CMD_GR_ChangeChairCount  ChangeChairCount;
				ChangeChairCount.dwChairCount = TBZ_MAX_PLAYER;

				//��ͻ��˷������������������ı����Ϣ
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
		}
	}


	//��������	
	tagUserInfo* pUserInfo = pIServerUserItem->GetUserInfo();
	pUserInfo->dBeans = pCreateSuccess->dCurBeans;
	pUserInfo->lRoomCard = pCreateSuccess->lRoomCard;
	pTableFrame->SetTableOwner(pUserInfo->dwUserID);
	pTableFrame->SetTimerNotBeginAfterCreate();
	pTableFrame->SetRoomCardFee(pCreateSuccess->iRoomCardFee);
	tagUserRule* pUserRule = pIServerUserItem->GetUserRule();
	lstrcpyn(pUserRule->szPassword, pCreateSuccess->szPassword, CountArray(pUserRule->szPassword));

	//�������������Ϸ
	if (pCreateSuccess->cbIsJoinGame)
	{

		//ִ������
		if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, pCreateSuccess->szPassword)== FALSE)
		{
			CTraceService::TraceString(TEXT("������������ʧ��"), TraceLevel_Info);
			//��������
			pTableFrame->SetPersonalTableLlocked(false);
			pTableFrame->SetPersonalTable(0, 0, 0);
			pTableFrame->SetCellScore(m_pGameServiceOption->lCellScore);
			pTableFrame->SetTableOwner(0);
			pTableFrame->SetRoomCardFee(0);
			//�˻�����
			DBR_GR_CancelCreateTable CancelCreateTable;
			ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			CancelCreateTable.dwUserID = pCreateSuccess->dwUserID;
			CancelCreateTable.dwClientAddr = pBindParameter->dwClientAddr;
			CancelCreateTable.dwTableID = pCreateSuccess->dwTableID;
			CancelCreateTable.dwReason = CANCELTABLE_REASON_SYSTEM;
			CancelCreateTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
			CancelCreateTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
			CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;

			//Ͷ������
			m_pIDBCorrespondManager->PostDataBaseRequest(pCreateSuccess->dwUserID, DBR_GR_CANCEL_CREATE_TABLE, dwContextID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			return true;
		}
	}




	//���ݻ���
	CMD_CS_C_CreateTable CS_CreateTable;
	ZeroMemory(&CS_CreateTable, sizeof(CMD_CS_C_CreateTable));

	CS_CreateTable.dwSocketID = dwContextID;
	CS_CreateTable.dwClientAddr = pBindParameter->dwClientAddr;
	CS_CreateTable.PersonalTable.dwServerID	= m_pGameServiceOption->wServerID;
	CS_CreateTable.PersonalTable.dwKindID = m_pGameServiceOption->wKindID;
	CS_CreateTable.PersonalTable.dwTableID = pCreateSuccess->dwTableID;
	CS_CreateTable.PersonalTable.dwUserID = pIServerUserItem->GetUserID();
	CS_CreateTable.PersonalTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
	CS_CreateTable.PersonalTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
	CS_CreateTable.PersonalTable.lCellScore = pCreateSuccess->lCellScore;
	CS_CreateTable.PersonalTable.dwRoomTax = pCreateSuccess->dwRoomTax;
	CS_CreateTable.PersonalTable.wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;
	lstrcpyn(CS_CreateTable.PersonalTable.szPassword, pCreateSuccess->szPassword, CountArray(CS_CreateTable.PersonalTable.szPassword));
	lstrcpyn(CS_CreateTable.szClientAddr, pCreateSuccess->szClientAddr, CountArray(CS_CreateTable.szClientAddr));

	//��������
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_CREATE_TABLE, &CS_CreateTable, sizeof(CMD_CS_C_CreateTable));


	return true;
}

//����ʧ��
bool CPersonalRoomGame::OnDBCreateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_GR_CreateFailure));
	if(wDataSize != sizeof(DBO_GR_CreateFailure)) return false;

	//�ж�����
	CAttemperEngineSink * pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;
	tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	if (pIServerUserItem==NULL) return false;

	DBO_GR_CreateFailure* pCreateFailure = (DBO_GR_CreateFailure*)pData;

	//��������
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = pCreateFailure->lErrorCode;
	lstrcpyn(CreateFailure.szDescribeString, pCreateFailure->szDescribeString, CountArray(CreateFailure.szDescribeString));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	return true;
}

//ȡ������
bool CPersonalRoomGame::OnDBCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_GR_CancelCreateResult));
	if(wDataSize != sizeof(DBO_GR_CancelCreateResult)) return false;

	////�ж�����
	CAttemperEngineSink * pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;
	//tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(LOWORD(dwContextID));
	//if(pBindParameter == NULL) return true;
	//if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	////��ȡ�û�
	//if (pIServerUserItem==NULL) return false;

	//��ȡ����
	DBO_GR_CancelCreateResult* pCancelCreateResult = (DBO_GR_CancelCreateResult*)pData;
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCancelCreateResult->dwTableID];
	ASSERT(pTableFrame != NULL);


	pTableFrame->SetPersonalTableLlocked(false);
	pTableFrame->SetPersonalTable(0, 0, 0);
	pTableFrame->SetTableOwner(0);
	pTableFrame->SetRoomCardFee(0);
	WORD wChairCount = pTableFrame->GetChairCount();

	
	for(int i = 0; i < wChairCount; ++i)
	{
		IServerUserItem* pUserItem =pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL) continue;
		if(pUserItem->GetUserStatus() == US_OFFLINE) 
		{
			pTableFrame->PerformStandUpAction(pUserItem);
			continue;
		}
		//�󶨲���
		WORD wBindIndex = pUserItem->GetBindIndex();
		tagBindParameter * pBind=pAttemperEngineSink->GetBindParameter(wBindIndex);

		//��������
		CMD_GR_CancelTable CancelTable;
		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = pCancelCreateResult->dwReason;
		if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_SYSTEM)
			lstrcpyn(CancelTable.szDescribeString, TEXT("��Ϸ�Զ���ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_PLAYER)
			lstrcpyn(CancelTable.szDescribeString, TEXT("��Ϸδ��ʼ����Ϸ�Զ���ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_ENFOCE)
			lstrcpyn(CancelTable.szDescribeString, TEXT("�����˳���Ϸ����Ϸ��ʱ����Ϸ��ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_OVER_TIME)
			lstrcpyn(CancelTable.szDescribeString, TEXT("��Ϸ��ʱ����Ϸ��ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_NOT_START)
			lstrcpyn(CancelTable.szDescribeString, TEXT("Լս�涨��ʼʱ�䵽δ��ʼ��Ϸ����Ϸ��ɢ��"), CountArray(CancelTable.szDescribeString));

		if (CANCELTABLE_REASON_NOT_START == pCancelCreateResult->dwReason || CANCELTABLE_REASON_OVER_TIME == pCancelCreateResult->dwReason)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		}
		//��ɢ��Ϣ
		m_pITCPNetworkEngine->SendData(pBind->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//�û�״̬
		//pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		pTableFrame->PerformStandUpAction(pUserItem);
	}

	////��������
	//CMD_GR_CancelTable CancelTable;
	//ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
	//CancelTable.dwReason = pCancelCreateResult->dwReason;
	//lstrcpyn(CancelTable.szDescribeString, pCancelCreateResult->szDescribeString, CountArray(CancelTable.szDescribeString));

	////��������
	//m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

	////�˳��û�
	//pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);

	return true;
}


//Լս�������¼�
bool CPersonalRoomGame::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_CREATE_TABLE_RESULT:		//�������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_CreateTableResult));
			if (wDataSize!=sizeof(CMD_CS_S_CreateTableResult)) return false;

			//��������
			CMD_CS_S_CreateTableResult * pCreateTableResult=(CMD_CS_S_CreateTableResult *)pData;

			//��ȡ�û�
			WORD wBindIndex=LOWORD(pCreateTableResult->dwSocketID);
			//�ж�����
			CAttemperEngineSink * pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;
			IServerUserItem * pIServerUserItem=pAttemperEngineSink->GetBindUserItem(wBindIndex);
			if (pIServerUserItem==NULL) return false;

			if(pCreateTableResult->PersonalTable.dwDrawCountLimit == 0 && pCreateTableResult->PersonalTable.dwDrawTimeLimit == 0)
			{
				//��������
				CMD_GR_CreateFailure CreateFailure;
				ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

				CreateFailure.lErrorCode = 1;
				lstrcpyn(CreateFailure.szDescribeString, TEXT("���������������ȷ��"), CountArray(CreateFailure.szDescribeString));

				//��������
				m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

				return false;
			}

			//��ȡ����
			CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCreateTableResult->PersonalTable.dwTableID];
			ASSERT(pTableFrame != NULL);

			tagPersonalTableParameter PersonalTableParameter = pTableFrame->GetPersonalTableParameter();
			pTableFrame->SetPersonalTableID(pCreateTableResult->PersonalTable.szRoomID);

			//��������
			CMD_GR_CreateSuccess CreateSuccess;
			ZeroMemory(&CreateSuccess, sizeof(CMD_GR_CreateSuccess));

			CreateSuccess.dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
			CreateSuccess.dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
			CreateSuccess.dBeans = pIServerUserItem->GetUserInfo()->dBeans;
			CreateSuccess.lRoomCard = pIServerUserItem->GetUserInfo()->lRoomCard;
			lstrcpyn(CreateSuccess.szServerID, pCreateTableResult->PersonalTable.szRoomID, CountArray(CreateSuccess.szServerID));

			m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_SUCCESS, &CreateSuccess, sizeof(CMD_GR_CreateSuccess));


			//�������Ӵ�����¼
			DBR_GR_InsertCreateRecord  CreateRecord;
			ZeroMemory(&CreateRecord, sizeof(DBR_GR_InsertCreateRecord));

			//��������
			CreateRecord.dwServerID	= pCreateTableResult->PersonalTable.dwServerID;
			CreateRecord.dwUserID = pCreateTableResult->PersonalTable.dwUserID;
			CreateRecord.lCellScore = pCreateTableResult->PersonalTable.lCellScore;
			CreateRecord.dwDrawCountLimit = pCreateTableResult->PersonalTable.dwDrawCountLimit;
			CreateRecord.dwDrawTimeLimit = pCreateTableResult->PersonalTable.dwDrawTimeLimit;
			lstrcpyn(CreateRecord.szPassword, pCreateTableResult->PersonalTable.szPassword, CountArray(CreateRecord.szPassword));
			lstrcpyn(CreateRecord.szRoomID, pCreateTableResult->PersonalTable.szRoomID, CountArray(CreateRecord.szRoomID));
			CreateRecord.wJoinGamePeopleCount = pCreateTableResult->PersonalTable.wJoinGamePeopleCount;
			CreateRecord.dwRoomTax =  pCreateTableResult->PersonalTable.dwRoomTax;
			lstrcpyn(CreateRecord.szClientAddr, pCreateTableResult->szClientAddr, CountArray(CreateRecord.szClientAddr));

			m_pIDBCorrespondManager->PostDataBaseRequest(0, DBR_GR_INSERT_CREATE_RECORD, 0, &CreateRecord, sizeof(CreateRecord));

			return true;
		}
	case SUB_CS_C_DISMISS_TABLE_RESULT:		//��ɢ���
		{
			ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTableResult));
			if(wDataSize != sizeof(CMD_CS_C_DismissTableResult)) return false;

			CMD_CS_C_DismissTableResult* pDismissTable = (CMD_CS_C_DismissTableResult*)pData;
			//��ȡ����
			CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pDismissTable->PersonalTableInfo.dwTableID];
			ASSERT(pTableFrame != NULL);

			for (int i = 0; i < pTableFrame->GetChairCount(); i++)
			{
				memcpy(&(pDismissTable->PersonalUserScoreInfo[i]), &(pTableFrame->m_PersonalUserScoreInfo[i]),  sizeof(tagPersonalUserScoreInfo));
			}
			for (int i = 0; i < pTableFrame->GetChairCount(); i++)
			{
				ZeroMemory(&(pTableFrame->m_PersonalUserScoreInfo[i]), sizeof(pTableFrame->m_PersonalUserScoreInfo[i]));
			}

			m_pIDBCorrespondManager->PostDataBaseRequest(0, DBR_GR_DISSUME_ROOM, 0, pDismissTable, sizeof(CMD_CS_C_DismissTableResult));
			return true;

		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
