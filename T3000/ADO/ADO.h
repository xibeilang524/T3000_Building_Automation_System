#pragma once
class  CADO
{
public:
	CADO(void);
	~CADO(void);
public:
	_ConnectionPtr m_pConnection;
	_RecordsetPtr  m_pRecordset;

	CString g_strOrigDatabaseFilePath;
	CString	g_strDatabasefilepath;
	CString g_strExePth;
	CString g_strImgeFolder;
public:
	void OnInitADOConn();						// �������ݿ�
	_RecordsetPtr& OpenRecordset(CString sql);  //�򿪼�¼��
	void CloseRecordset();						//�رռ�¼��
	void CloseConn();							//�ر����ݿ�����
	UINT GetRecordCount(_RecordsetPtr pRecordset);//��ü�¼��

	void Createtable(CString strSQL);//�Զ�������
	bool IsHaveTable(CADO ado, CString strTableName);//�жϱ��Ƿ����
	void DeleteDB();
};


