#ifndef CMD_OX_HEAD_FILE
#define CMD_OX_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID							36									//��Ϸ I D
#define GAME_PLAYER						6									//��Ϸ����
#define GAME_NAME						TEXT("ͨ��ţţ")					//��Ϸ����
#define MAX_COUNT						5									//�����Ŀ
#define MAX_JETTON_AREA					6									//��ע����
#define MAX_TIMES						5									//�������

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//����汾

//��Ϸ״̬
#define GS_TK_FREE						GAME_STATUS_FREE					//�ȴ���ʼ
#define GS_TK_PLAYING					GAME_STATUS_PLAY					//��Ϸ����

//������Ϣ
#define IDM_ADMIN_UPDATE_STORAGE		WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE		WM_USER+1011
#define IDM_REQUEST_QUERY_USER			WM_USER+1012
#define IDM_USER_CONTROL				WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO		WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER		WM_USER+1015

//������¼
#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

//////////////////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START				100									//��Ϸ��ʼ
#define SUB_S_PLAYER_EXIT				101									//�û�ǿ��
#define SUB_S_SEND_CARD					102									//������Ϣ
#define SUB_S_GAME_END					103									//��Ϸ����
#define SUB_S_OPEN_CARD					104									//�û�̯��
#define SUB_S_ANDROID_BANKOPERATOR		105									//���������в���
#define SUB_S_ADMIN_STORAGE_INFO		112									//ˢ�¿��Ʒ����
#define SUB_S_REQUEST_QUERY_RESULT		113									//��ѯ�û����
#define SUB_S_USER_CONTROL				114									//�û�����
#define SUB_S_USER_CONTROL_COMPLETE		115									//�û��������
#define SUB_S_OPERATION_RECORD		    116									//������¼
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 117

//�����˴��ȡ��
struct tagCustomAndroid
{
	SCORE									lRobotScoreMin;	
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet; 
	SCORE									lRobotBankGetBanker; 
	SCORE									lRobotBankStoMul; 
};

//��������
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}CONTROL_TYPE;

//���ƽ��      ���Ƴɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID}CONTROL_RESULT;

//�û���Ϊ
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//������Ϣ
typedef struct
{
	CONTROL_TYPE						control_type;					  //��������
	BYTE								cbControlCount;					  //���ƾ���
	bool							    bCancelControl;					  //ȡ����ʶ
}USERCONTROL;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//�����˱�ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬
}ROOMUSERINFO;

//�����û�����
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//�����û���Ϣ
	USERCONTROL							userControl;						//�û�����
}ROOMUSERCONTROL;

//��Ϸ״̬
struct CMD_S_StatusFree
{
	//��Ϸ����
	LONGLONG							lCellScore;							//��������
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//����������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//״̬��Ϣ
	BYTE                                cbDynamicJoin;                      //��̬����
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	LONGLONG							lCellScore;							//��������
	WORD								wBankerUser;						//ׯ���û�

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;						//����������

	//�˿���Ϣ
	BYTE								bOxCard[GAME_PLAYER];				//ţţ����
	BYTE								cbOxCardData[GAME_PLAYER][MAX_COUNT];//ţţ�˿�
	BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD								wBankerUser;						//ׯ���û�
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
	LONGLONG							lCellScore;							//��Ϸ�׷�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��
	LONGLONG							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
	BYTE								cbDelayOverGame;
};

//�������ݰ�
struct CMD_S_SendCard
{
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
};

//�û��˳�
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//�˳��û�
};

//�û�̯��
struct CMD_S_Open_Card
{
	WORD								wPlayerID;							//̯���û�
	BYTE								bOpen;								//̯�Ʊ�־
};

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//�û���Ϣ
	bool								bFind;								//�ҵ���ʶ
};

//�û�����
struct CMD_S_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	CONTROL_RESULT							controlResult;						//���ƽ��
	CONTROL_TYPE							controlType;						//��������
	BYTE									cbControlCount;						//���ƾ���
};

//�û�����
struct CMD_S_UserControlComplete
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	CONTROL_TYPE							controlType;						//��������
	BYTE									cbRemainControlCount;				//ʣ����ƾ���
};

//���Ʒ���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG	lRoomStorageStart;						//������ʼ���
	LONGLONG	lRoomStorageCurrent;
	LONGLONG	lRoomStorageDeduct;
	LONGLONG	lMaxRoomStorage[2];
	WORD		wRoomStorageMul[2];
};

//������¼
struct CMD_S_Operation_Record
{
	TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];					//��¼���²�����20����¼
};


//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	ROOMUSERINFO						currentqueryuserinfo;				//��ǰ��ѯ�û���Ϣ
	bool								bExistControl;						//��ѯ�û����ڿ��Ʊ�ʶ
	USERCONTROL							currentusercontrol;
};

//////////////////////////////////////////////////////////////////////////

//�ͻ�������ṹ
#define SUB_C_OPEN_CARD					1									//�û�̯��
#define SUB_C_STORAGE					6									//���¿��
#define	SUB_C_STORAGEMAXMUL				7									//��������
#define SUB_C_REQUEST_QUERY_USER		8									//�����ѯ�û�
#define SUB_C_USER_CONTROL				9									//�û�����

//�����������
#define SUB_C_REQUEST_UDPATE_ROOMINFO	10									//������·�����Ϣ
#define SUB_C_CLEAR_CURRENT_QUERYUSER	11

//�û�̯��
struct CMD_C_OxCard
{
	BYTE								bOX;								//ţţ��־
	BYTE								cbOxCardData[MAX_COUNT];			//ţţ�˿�
};

struct CMD_C_UpdateStorage
{
	LONGLONG						lRoomStorageCurrent;					//�����ֵ
	LONGLONG						lRoomStorageDeduct;						//�����ֵ
};

struct CMD_C_ModifyStorage
{
	LONGLONG						lMaxRoomStorage[2];							//�������
	WORD							wRoomStorageMul[2];							//Ӯ�ָ���
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//��ѯ�û�GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			    //��ѯ�û��ǳ�
};

//�û�����
struct CMD_C_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	USERCONTROL								userControlInfo;					//
};

#pragma pack(pop)

#endif
