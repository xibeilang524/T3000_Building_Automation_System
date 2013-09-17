﻿// DialogCM5_BacNet.cpp : implementation file
// DialogCM5 Bacnet programming by Fance 2013 05 01

#include "stdafx.h"
#include "T3000.h"
#include "DialogCM5_BacNet.h"
#include "CM5\MyOwnListCtrl.h"
#include "BacnetInput.h"
#include "BacnetOutput.h"
#include "BacnetProgram.h"
#include "BacnetVariable.h"
#include "globle_function.h"

#include "gloab_define.h"
#include "datalink.h"
#include "BacnetWait.h"
#include "Bacnet_Include.h"
#include "CM5\ud_str.h"
//bool CM5ProcessPTA(	BACNET_PRIVATE_TRANSFER_DATA * data);
CString temp_device_id,temp_mac,temp_vendor_id;
HANDLE hwait_thread;
BacnetWait *WaitDlg=NULL;
// CDialogCM5_BacNet
HWND BacNet_hwd;
_Refresh_Info Bacnet_Refresh_Info;
#define WM_FRESH_CM_LIST WM_USER + 975
//#define WM_SEND_OVER     WM_USER + 1287
// int m_Input_data_length;


IMPLEMENT_DYNCREATE(CDialogCM5_BacNet, CFormView)

CDialogCM5_BacNet::CDialogCM5_BacNet()
	: CFormView(CDialogCM5_BacNet::IDD)
{
	m_MSTP_THREAD = true;
	//CM5_hThread = NULL;
}

CDialogCM5_BacNet::~CDialogCM5_BacNet()
{
}

void CDialogCM5_BacNet::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, m_device_list_info);
}

BEGIN_MESSAGE_MAP(CDialogCM5_BacNet, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CDialogCM5_BacNet::OnBnClickedButtonTest)
	ON_MESSAGE(WM_FRESH_CM_LIST,Fresh_UI)
	ON_BN_CLICKED(IDC_BUTTON_CM5_ADVANCE, &CDialogCM5_BacNet::OnBnClickedButtonCm5Input)
//	ON_BN_CLICKED(IDC_BUTTON1, &CDialogCM5_BacNet::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_CM5_PROGRAMING, &CDialogCM5_BacNet::OnBnClickedButtonCm5Programing)
	ON_BN_CLICKED(IDC_BUTTON_CM5_OUTPUT, &CDialogCM5_BacNet::OnBnClickedButtonCm5Output)
	ON_BN_CLICKED(IDC_BUTTON_CM5_VARIABLE, &CDialogCM5_BacNet::OnBnClickedButtonCm5Variable)
	ON_BN_CLICKED(IDC_BUTTON_BAC_READ_TOTAL, &CDialogCM5_BacNet::OnBnClickedButtonBacReadTotal)
	ON_MESSAGE(MY_RESUME_DATA, AllMessageCallBack)
	ON_MESSAGE(WM_DELETE_WAIT_DLG,Delete_Wait_Dlg)	
END_MESSAGE_MAP()


// CDialogCM5_BacNet diagnostics

#ifdef _DEBUG
void CDialogCM5_BacNet::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDialogCM5_BacNet::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

//The window which created by the button ,will delete when the wait dialog send this message,to this window.
//It means ,it has done .we don't needed.
LRESULT CDialogCM5_BacNet::Delete_Wait_Dlg(WPARAM wParam,LPARAM lParam)
{
	if(WaitDlg!=NULL)
	{
		Sleep(1000);
		delete WaitDlg;
		WaitDlg = NULL;
		((CButton *)GetDlgItem(IDC_BUTTON_BAC_READ_TOTAL))->EnableWindow(TRUE);

		if(bac_read_which_list == BAC_READ_INPUT_LIST)
		{
			if(bac_input_read_results)
			{
			 CBacnetInput DLG;
			 DLG.DoModal();
			}
			else
				MessageBox(_T("Inputs list read time out!"));	
			return 0;
		}

		if(bac_read_which_list == BAC_READ_OUTPUT_LIST)
		{
			if(bac_output_read_results)
			{
				CBacnetOutput DLG;
				DLG.DoModal();
			}
			else
				MessageBox(_T("Outputs list read time out!"));
			return 0;
		}

		if(bac_read_which_list == BAC_READ_VARIABLE_LIST)
		{
			if(bac_variable_read_results)
			{
				CBacnetVariable DLG;
				DLG.DoModal();
			}
			else
				MessageBox(_T("Variable list read time out!"));
			return 0;
		}

		if(bac_read_which_list == BAC_READ_PROGRAM_LIST)
		{
			if(bac_program_read_results)
			{
				CBacnetProgram DLG;
				DLG.DoModal();
			}
			else
				MessageBox(_T("Program list read time out!"));
			return 0;
		}

	}
	return 0;
}
// CDialogCM5_BacNet message handlers
LRESULT  CDialogCM5_BacNet::AllMessageCallBack(WPARAM wParam, LPARAM lParam)
{
	_MessageInvokeIDInfo *pInvoke =(_MessageInvokeIDInfo *)lParam;
	bool msg_result=WRITE_FAIL;
	msg_result = MKBOOL(wParam);
	if(msg_result)
	{
		for (int i=0;i<BAC_READ_INPUT_GROUP_NUMBER;i++)
		{
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Input_InvokeID[i])
				Bacnet_Refresh_Info.Input_result[i] = true;
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Output_InvokeID[i])
				Bacnet_Refresh_Info.Output_result[i] = true;
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Variable_InvokeID[i])
				Bacnet_Refresh_Info.Variable_result[i] = true;
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Program_InvokeID[i])
				Bacnet_Refresh_Info.Program_result[i] = true;
		}

		SetPaneString(BAC_SHOW_MISSION_RESULTS,_T("Bacnet operation success!"));
		//MessageBox(_T("Bacnet operation success!"));
	}
	else
	{
		for (int i=0;i<BAC_READ_INPUT_GROUP_NUMBER;i++)
		{
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Input_InvokeID[i])
				Bacnet_Refresh_Info.Input_result[i] = false;
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Output_InvokeID[i])
				Bacnet_Refresh_Info.Output_result[i] = false;
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Variable_InvokeID[i])
				Bacnet_Refresh_Info.Variable_result[i] = false;
			if(pInvoke->Invoke_ID==Bacnet_Refresh_Info.Program_InvokeID[i])
				Bacnet_Refresh_Info.Program_result[i] = false;
		}
		SetPaneString(BAC_SHOW_MISSION_RESULTS,_T("Bacnet operation fail!"));
		//MessageBox(_T("Bacnet operation fail!"));
	}
	if(pInvoke)
		delete pInvoke;
	return 0;
}

void CDialogCM5_BacNet::OnBnClickedButtonTest()
{
	// TODO: Add your control notification handler code here
	Send_WhoIs_Global(-1,-1);
}


void CDialogCM5_BacNet::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	#ifdef Fance_Enable_Test
	Fresh();//Fance
	Initial_All_Point();
	#endif
	// TODO: Add your specialized code here and/or call the base class
}
void CDialogCM5_BacNet::Initial_All_Point()
{
	m_Input_data.clear();
	m_Variable_data.clear();
	m_Output_data.clear();
	m_Program_data.clear();
	for(int i=0;i<5*BAC_READ_INPUT_GROUP_NUMBER;i++)
	{
			Str_in_point temp_in;
			//temp_in.auto_manual = 0;
			//temp_in.calibration = 0;
			//temp_in.calibration_increment = 0;
			//temp_in.calibration_sign = 0;
			//temp_in.control = 0;
			//temp_in.decom = 0;
			////temp_in.description = "";
			//temp_in.digital_analog = 0;
			//temp_in.filter = 0;
			////temp_in.label = 0;
			//temp_in.range =0;
			//temp_in.sen_off = 0;
			//temp_in.sen_on = 0;
			//temp_in.unused = 0;
			//temp_in.value = 0;
			m_Input_data.push_back(temp_in);

			Str_out_point temp_out;
			m_Output_data.push_back(temp_out);
			Str_variable_point temp_variable;
			m_Variable_data.push_back(temp_variable);
			Str_program_point temp_program;
			m_Program_data.push_back(temp_program);
		}
}
//INPUT int test_function_return_value();
void CDialogCM5_BacNet::Fresh()
{


	BACNET_ADDRESS src = {
		0
	};  /* address where message came from */
	uint16_t pdu_len = 0;
	unsigned timeout = 100;     /* milliseconds */
	BACNET_ADDRESS my_address, broadcast_address;
	char my_port[50];
	static bool has_run_once=false;
	if(!has_run_once)
	{
		has_run_once =true;
		bac_program_pool_size = 26624;
		bac_program_size = 0;
		bac_free_memory = 26624;
		hwait_thread = NULL;
		close_com();
	//	int test111=test_function_return_value();
		Device_Set_Object_Instance_Number(4194300);
		address_init();
		Init_Service_Handlers();
		dlmstp_set_baud_rate(19200);
		dlmstp_set_mac_address(0);
		dlmstp_set_max_info_frames(DEFAULT_MAX_INFO_FRAMES);
		dlmstp_set_max_master(DEFAULT_MAX_MASTER);
		memset(my_port,0,50);
		sprintf(my_port,"COM5");
		dlmstp_init(my_port);


		//Initial_List();
//		BacNet_hwd = this->m_hWnd;
		//    dlenv_init();
		datalink_get_broadcast_address(&broadcast_address);
		//    print_address("Broadcast", &broadcast_address);
		datalink_get_my_address(&my_address);
	//		print_address("Address", &my_address);

		CM5_hThread =CreateThread(NULL,NULL,MSTP_Receive,this,NULL, &nThreadID);
		BacNet_hwd = this->m_hWnd;
		Send_WhoIs_Global(-1,-1);

		HICON hIcon = NULL; 
		//hIcon   = AfxGetApp()->LoadIcon(IDI_ICON_INPUT);
		//((CButton *)GetDlgItem(IDC_BUTTON_CM5_ADVANCE))->SetIcon(hIcon);
		HINSTANCE hInstResource    = NULL; 
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_INPUT), RT_GROUP_ICON); 
		hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_ICON_INPUT), IMAGE_ICON, 32, 32, 0); 
		((CButton *)GetDlgItem(IDC_BUTTON_CM5_ADVANCE))->SetIcon(hIcon);

		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_PROGRAMING), RT_GROUP_ICON); 
		hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_ICON_PROGRAMING), IMAGE_ICON, 32, 32, 0); 
		((CButton *)GetDlgItem(IDC_BUTTON_CM5_PROGRAMING))->SetIcon(hIcon);

		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_OUTPUT), RT_GROUP_ICON); 
		hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_ICON_OUTPUT), IMAGE_ICON, 32, 32, 0); 
		((CButton *)GetDlgItem(IDC_BUTTON_CM5_OUTPUT))->SetIcon(hIcon);

		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_VARIABLE), RT_GROUP_ICON); 
		hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_ICON_VARIABLE), IMAGE_ICON, 32, 32, 0); 
		((CButton *)GetDlgItem(IDC_BUTTON_CM5_VARIABLE))->SetIcon(hIcon);


		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_WEEKLY), RT_GROUP_ICON); 
		hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_ICON_WEEKLY), IMAGE_ICON, 32, 32, 0); 
		((CButton *)GetDlgItem(IDC_BUTTON_CM5_WEEKLY))->SetIcon(hIcon);

		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_ANNUAL), RT_GROUP_ICON); 
		hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_ICON_ANNUAL), IMAGE_ICON, 32, 32, 0); 
		((CButton *)GetDlgItem(IDC_BUTTON_CM5_ANNUAL))->SetIcon(hIcon);
		
	}
}









typedef struct BACnet_Object_Property_Value_Own {
	BACNET_OBJECT_TYPE object_type;
	uint32_t object_instance;
	BACNET_PROPERTY_ID object_property;
	uint32_t array_index;
	BACNET_APPLICATION_DATA_VALUE value;
	HTREEITEM t_PropertyChild;
} BACNET_OBJECT_PROPERTY_VALUE_Own;

typedef struct _DEVICE_INFO
{
	uint32_t i_device_id;
	uint32_t i_vendor_id;
	uint32_t i_mac;
	HTREEITEM t_DeviceChild;
	vector	<BACnet_Object_Property_Value_Own> my_Property_value;

}DEVICE_INFO;



vector <_DEVICE_INFO> g_device_info;

//Str_in_point Private_data[100];
//int Private_data_length;
//HWND      m_input_dlg_hwnd;
//HWND      m_pragram_dlg_hwnd;


//extern  MSTP_Port;
volatile struct mstp_port_struct_t MSTP_Port;
static void Read_Properties(
    void)
{
    uint32_t device_id = 0;
    bool status = false;
    unsigned max_apdu = 0;
    BACNET_ADDRESS src;
    bool next_device = false;
    static unsigned index = 0;
    static unsigned property = 0;
    /* list of required (and some optional) properties in the
       Device Object
       note: you could just loop through
       all the properties in all the objects. */
	 const int object_props[] = {
	PROP_OBJECT_LIST//PROP_MODEL_NAME//PROP_OBJECT_LIST
	 };
    //const int object_props[] = {
    //    PROP_OBJECT_IDENTIFIER,
    //    PROP_OBJECT_NAME,
    //    PROP_OBJECT_TYPE,
    //    PROP_SYSTEM_STATUS,
    //    PROP_VENDOR_NAME,
    //    PROP_VENDOR_IDENTIFIER,
    //    PROP_MODEL_NAME,
    //    PROP_FIRMWARE_REVISION,
    //    PROP_APPLICATION_SOFTWARE_VERSION,
    //    PROP_PROTOCOL_VERSION,
    //    PROP_PROTOCOL_SERVICES_SUPPORTED,
    //    PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED,
    //    PROP_MAX_APDU_LENGTH_ACCEPTED,
    //    PROP_SEGMENTATION_SUPPORTED,
    //    PROP_LOCAL_TIME,
    //    PROP_LOCAL_DATE,
    //    PROP_UTC_OFFSET,
    //    PROP_DAYLIGHT_SAVINGS_STATUS,
    //    PROP_APDU_SEGMENT_TIMEOUT,
    //    PROP_APDU_TIMEOUT,
    //    PROP_NUMBER_OF_APDU_RETRIES,
    //    PROP_TIME_SYNCHRONIZATION_RECIPIENTS,
    //    PROP_MAX_MASTER,
    //    PROP_MAX_INFO_FRAMES,
    //    PROP_DEVICE_ADDRESS_BINDING,
    //    /* note: PROP_OBJECT_LIST is missing cause
    //       we need to get it with an index method since
    //       the list could be very large */
    //    /* some proprietary properties */
    //    514, 515,
    //    /* end of list */
    //    -1
    //};

    if (address_count()) {
        if (address_get_by_index(index, &device_id, &max_apdu, &src)) {
            if (object_props[property] < 0)
                next_device = true;
            else {
                /*status*/ g_invoke_id= Send_Read_Property_Request(device_id,  /* destination device */
                    OBJECT_DEVICE, device_id, (BACNET_PROPERTY_ID)object_props[property],
                    BACNET_ARRAY_ALL);
              //  if (status)	Fance
              //      property++;
            }
        } else
            next_device = true;
        if (next_device) {
            next_device = false;
           // index++;Fance
            if (index >= MAX_ADDRESS_CACHE)
                index = 0;
            property = 0;
        }
    }

    return;
}
uint32_t gloab_device_id;
uint32_t g_vendor_id;
uint32_t g_mac;
HTREEITEM FirstChild ;
HTREEITEM SecondChild ;
HTREEITEM DeviceChild;



#define  PRINT_ENABLED 1
void local_rp_ack_print_data(
	BACNET_READ_PROPERTY_DATA * data)
{

	int len = 0;
	uint8_t *application_data;
	int application_data_len;
	bool first_value = true;
	bool print_brace = false;

	

	if (data) 
	{
		application_data = data->application_data;
		application_data_len = data->application_data_len;
		/* FIXME: what if application_data_len is bigger than 255? */
		/* value? need to loop until all of the len is gone... */
		for (;;) 
		{
			BACnet_Object_Property_Value_Own object_value;  /* for bacapp printing */
			BACNET_APPLICATION_DATA_VALUE value;        /* for decode value data */
			len = bacapp_decode_application_data(application_data,(uint8_t) application_data_len, &value);
			if (first_value && (len < application_data_len)) 
			{
				first_value = false;
#if PRINT_ENABLED
				fprintf(stdout, "{");
#endif
				print_brace = true;
			}
			object_value.object_type = data->object_type;
			object_value.object_instance = data->object_instance;
			object_value.object_property = data->object_property;
			object_value.array_index = data->array_index;
			object_value.value = value;


			if(g_device_info.at(0).my_Property_value.size()==0)
			{
				g_device_info.at(0).my_Property_value.push_back(object_value);
			}
			else
			{
				bool find_id=false;
				for (int i=0;i<g_device_info.at(0).my_Property_value.size();i++)
				{
					if((g_device_info.at(0).my_Property_value.at(i).value.type.Object_Id.instance!=object_value.value.type.Object_Id.instance)
						||(g_device_info.at(0).my_Property_value.at(i).value.type.Object_Id.type!=object_value.value.type.Object_Id.type))
						continue;
					else
					{
						find_id = true;
						break;
					}
				}
				if(!find_id)
					g_device_info.at(0).my_Property_value.push_back(object_value);
			}


		//	g_device_info.at(0).my_Property_value.push_back(object_value);

		//	g_device_info.at(0).my_Property_value.push_back(object_value);

				//TRACE("%d",object_value);
		//	bacapp_print_value(stdout, &object_value);
			if (len > 0) 
			{
				if (len < application_data_len) 
				{
					application_data += len;
					application_data_len -= len;
					/* there's more! */

#if PRINT_ENABLED
					fprintf(stdout, ",");
#endif
				} 
				else 
				{
					break;
				}
			} 
			else 
			{
				break;
			}
		}
#if PRINT_ENABLED
		if (print_brace)
			fprintf(stdout, "}");
		fprintf(stdout, "\r\n");
#endif
	}
}

/*
 * This is called when we receive a private transfer packet.
 * We parse the data, send the private part for processing and then send the
 * response which the application generates.
 * If there are any BACnet level errors we send an error response from here.
 * If there are any application level errors they will be packeged up in the
 * response block which we send back to the originator of the request.
 *
 */



 







void Localhandler_read_property_ack(
	uint8_t * service_request,
	uint16_t service_len,
	BACNET_ADDRESS * src,
	BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data)
{
	int len = 0;
	BACNET_READ_PROPERTY_DATA data;

	(void) src;
	(void) service_data;        /* we could use these... */
	len = rp_ack_decode_service_request(service_request, service_len, &data);
#if 0
	fprintf(stderr, "Received Read-Property Ack!\n");
#endif
	if (len > 0)
	{
		local_rp_ack_print_data(&data);
		::PostMessage(BacNet_hwd,WM_FRESH_CM_LIST,NULL,NULL);
	}
}

//static void Read_Property_feed_back( uint8_t * service_request,
//	uint16_t service_len,
//	BACNET_ADDRESS * src,
//	BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data)
//{
//	AfxMessageBox("Receive");
//}



static void LocalIAmHandler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src)
{
	
    int len = 0;
    uint32_t device_id = 0;
    unsigned max_apdu = 0;
    int segmentation = 0;
    uint16_t vendor_id = 0;

    (void) src;
    (void) service_len;
    len =  iam_decode_service_request(service_request, &device_id, &max_apdu,
        &segmentation, &vendor_id);




    fprintf(stderr, "Received I-Am Request");
    if (len != -1) 
	{
        fprintf(stderr, " from %u!\n", device_id);
        address_add(device_id, max_apdu, src);
    } else
        fprintf(stderr, "!\n");

	if(src->mac_len==1)
		temp_mac.Format(_T("%d"),src->mac[0]);

	temp_device_id.Format(_T("%d"),device_id);
	temp_vendor_id.Format(_T("%d"),vendor_id);

	::PostMessage(BacNet_hwd,WM_FRESH_CM_LIST,NULL,NULL);
	return;

	//gloab_device_id = device_id;
	//g_vendor_id = vendor_id;
	//if(src->mac_len==1)
	//	g_mac=src->mac[0];
	
	////_DEVICE_INFO temp_info;
	////temp_info.i_device_id = device_id;
	////temp_info.i_vendor_id = vendor_id;
	////if(src->mac_len==1)
	////	temp_info.i_mac=src->mac[0];
	////if(g_device_info.size()==0)
	////{
	////	g_device_info.push_back(temp_info);
	////}
	////else
	////{
	////	bool find_id=false;
	////	for (int i=0;i<g_device_info.size();i++)
	////	{
	////		if(g_device_info.at(i).i_device_id!=device_id)
	////			continue;
	////		else
	////			find_id = true;
	////	}
	////	if(!find_id)
	////		g_device_info.push_back(temp_info);
	////	else
	////		return ;
	////}
	////
	////::PostMessage(BacNet_hwd,WM_ADD_LIST,NULL,NULL);
    return;
}



LRESULT CDialogCM5_BacNet::Fresh_UI(WPARAM wParam,LPARAM lParam)
{
	((CStatic *)GetDlgItem(IDC_STATIC_CM_DEVICE_ID))->SetWindowTextW(temp_device_id);
	((CStatic *)GetDlgItem(IDC_STATIC_CM5_MAC))->SetWindowTextW(temp_mac);
	((CStatic *)GetDlgItem(IDC_STATIC_CM5_VENDOR_ID))->SetWindowTextW(temp_vendor_id);
	
	return 0;
}

DWORD WINAPI   CDialogCM5_BacNet::MSTP_Receive(LPVOID lpVoid)
{
	BACNET_ADDRESS src = {0};
	uint16_t pdu_len;
	CDialogCM5_BacNet *mparent = (CDialogCM5_BacNet *)lpVoid;
	uint8_t Rx_Buf[MAX_MPDU] = { 0 };
	while(mparent->m_MSTP_THREAD)
	{
		pdu_len = dlmstp_receive(&src, &Rx_Buf[0], MAX_MPDU, INFINITE);
		if(pdu_len==0)
			continue;
		npdu_handler(&src, &Rx_Buf[0], pdu_len);
		//CString TEMP1;
		//TEMP1.Format("%s",Rx_Buf);
		//	AfxMessageBox(TEMP1);
	}
	return 0;
}

static void Init_Service_Handlers(
	void)
{
	Device_Init(NULL);

	/* we need to handle who-is to support dynamic device binding */
	apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS, handler_who_is);
	apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, LocalIAmHandler);



	apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_PRIVATE_TRANSFER,local_handler_conf_private_trans_ack);
	//apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_READ_PROPERTY,Read_Property_feed_back);

	apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_READ_PROPERTY,	Localhandler_read_property_ack);
	/* set the handler for all the services we don't implement */
	/* It is required to send the proper reject message... */
	apdu_set_unrecognized_service_handler_handler
		(handler_unrecognized_service);
	/* we must implement read property - it's required! */
	apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY,
		handler_read_property);
	apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROP_MULTIPLE,
		handler_read_property_multiple);
	/* handle the data coming back from confirmed requests */
	//   apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_READ_PROPERTY,handler_read_property_ack);
#if defined(BACFILE)
	apdu_set_confirmed_handler(SERVICE_CONFIRMED_ATOMIC_READ_FILE,
		handler_atomic_read_file);
#endif
	apdu_set_confirmed_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV,
		handler_cov_subscribe);

////#if 0
////	/* Adding these handlers require the project(s) to change. */
////#if defined(BACFILE)
////	apdu_set_confirmed_handler(SERVICE_CONFIRMED_ATOMIC_WRITE_FILE,
////		handler_atomic_write_file);
////#endif
////	apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_RANGE,
////		handler_read_range);
////	apdu_set_confirmed_handler(SERVICE_CONFIRMED_REINITIALIZE_DEVICE,
////		handler_reinitialize_device);
////	apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION,
////		handler_timesync_utc);
////	apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION,
////		handler_timesync);
////	apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_COV_NOTIFICATION,
////		handler_ucov_notification);
////	/* handle communication so we can shutup when asked */
////	apdu_set_confirmed_handler(SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL,
////		handler_device_communication_control);
////#endif
}

//void CDialogCM5_BacNet::Initial_List()
//{
//	long style;
//	style=GetWindowLong(m_device_list_info.m_hWnd,GWL_STYLE);
//	style&=~LVS_TYPEMASK;
//	style|=LVS_REPORT;	
//	style|=LVS_EX_CHECKBOXES;
//	//	style|=RC_CHKBOX_SINGLE;
//
//	m_device_list_info.SetExtendedStyle(style);
//	SetWindowLong(m_device_list_info.m_hWnd,GWL_STYLE,style);//list_infomation.m_hWnd´
//	DWORD dwstyle=m_device_list_info.GetExtendedStyle();
//	dwstyle|=LVS_EX_FULLROWSELECT; //
//	dwstyle|=LVS_EX_GRIDLINES;     //
//	m_device_list_info.SetExtendedStyle(dwstyle);
//	m_device_list_info.InsertColumn(0,_T("Device ID"),LVCFMT_CENTER,100);  
//	m_device_list_info.InsertColumn(1,_T("MAC"),LVCFMT_CENTER,200);
//	m_device_list_info.InsertColumn(2,_T("Vendor ID"),LVCFMT_CENTER,200);
//	m_device_list_info.SetTextColor(RGB(0,0,255));
//	m_device_list_info.SetCheckboxeStyle(RC_CHKBOX_SINGLE);
//	//m_list_control.SortItems(0, FALSE); // sort the 1st column, ascending
//	m_device_list_info.SetSortable(FALSE);
//}


void CDialogCM5_BacNet::OnBnClickedButtonCm5Input()
{
	// TODO: Add your control notification handler code here
	//g_invoke_id = GetPrivateData();
	//Post_Invoke_ID_Monitor_Thread(MY_INVOKE_ID,g_invoke_id,this->m_hWnd);
	Show_Wait_Dialog_And_SendMessage(BAC_READ_INPUT_LIST);
	//bac_read_which_list = BAC_READ_INPUT_LIST;
	//CBacnetInput DLG;
	//DLG.DoModal();
}







//void CDialogCM5_BacNet::OnBnClickedButton1()
//{
//	// TODO: Add your control notification handler code here
//	//WritePrivateData();
//}


void CDialogCM5_BacNet::OnBnClickedButtonCm5Programing()
{
	// TODO: Add your control notification handler code here
	Show_Wait_Dialog_And_SendMessage(BAC_READ_PROGRAM_LIST);
	//CBacnetProgram DLG;
	//DLG.DoModal();
}


void CDialogCM5_BacNet::OnBnClickedButtonCm5Output()
{
	// TODO: Add your control notification handler code here
	Show_Wait_Dialog_And_SendMessage(BAC_READ_OUTPUT_LIST);

	//CBacnetOutput DLG;
	//DLG.DoModal();
}


void CDialogCM5_BacNet::OnBnClickedButtonCm5Variable()
{
	// TODO: Add your control notification handler code here
	Show_Wait_Dialog_And_SendMessage(BAC_READ_VARIABLE_LIST);
	//CBacnetVariable dlg;
	//dlg.DoModal();
}


void CDialogCM5_BacNet::Show_Wait_Dialog_And_SendMessage(int read_list_type)
{
	bac_read_which_list = read_list_type;
	if(WaitDlg==NULL)
	{
		WaitDlg = new BacnetWait;
		WaitDlg->Create(IDD_DIALOG_BACNET_WAIT,this);
		WaitDlg->ShowWindow(SW_SHOW);

		RECT RECT_SET1;
		GetWindowRect(&RECT_SET1);
		//	GetClientRect(&RECT_SET1);
		WaitDlg->MoveWindow(RECT_SET1.left+100,RECT_SET1.bottom-200,RECT_SET1.left+270/*RECT_SET1.right/2+20*/,100);
	}

	//::PostMessage(BacNet_hwd,WM_SEND_OVER,0,0);
	if(hwait_thread==NULL)
	{
		hwait_thread =CreateThread(NULL,NULL,Send_read_Command_Thread,this,NULL, NULL);
		((CButton *)GetDlgItem(IDC_BUTTON_BAC_READ_TOTAL))->EnableWindow(FALSE);
	}
}

//First,Get all information we needed.
void CDialogCM5_BacNet::OnBnClickedButtonBacReadTotal()
{
	//bac_read_which_list = BAC_READ_ALL_LIST;

	Show_Wait_Dialog_And_SendMessage(BAC_READ_ALL_LIST);
	
}

static int resend_count=0;
DWORD WINAPI  CDialogCM5_BacNet::Send_read_Command_Thread(LPVOID lpVoid)
{
	CDialogCM5_BacNet *pParent = (CDialogCM5_BacNet *)lpVoid;
	
	

	for (int i=0;i<BAC_READ_INPUT_GROUP_NUMBER;i++)
	{
		if((bac_read_which_list == BAC_READ_INPUT_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
			Bacnet_Refresh_Info.Input_result[i] = BAC_RESULTS_UNKONW;
		if((bac_read_which_list == BAC_READ_OUTPUT_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
			Bacnet_Refresh_Info.Output_result[i] = BAC_RESULTS_UNKONW;
		if((bac_read_which_list == BAC_READ_PROGRAM_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
			Bacnet_Refresh_Info.Program_result[i] = BAC_RESULTS_UNKONW;
		if((bac_read_which_list == BAC_READ_VARIABLE_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
			Bacnet_Refresh_Info.Variable_result[i] = BAC_RESULTS_UNKONW;
	}
	// TODO: Add your control notification handler code here

	for (int i=0;i<BAC_READ_INPUT_GROUP_NUMBER;i++)
	{
		if((bac_read_which_list == BAC_READ_INPUT_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
		{
			resend_count = 0;
			do 
			{
				resend_count ++;
				if(resend_count>10)
					goto myend;
				g_invoke_id = GetPrivateData(1234,READINPUT_T3000,(BAC_READ_INPUT_GROUP_NUMBER+1)*i,4+(BAC_READ_INPUT_GROUP_NUMBER + 1)*i,sizeof(Str_in_point));
				Sleep(500);
			} while (g_invoke_id<0);

			Bacnet_Refresh_Info.Input_InvokeID[i] = g_invoke_id;
			Post_Invoke_ID_Monitor_Thread(MY_INVOKE_ID,g_invoke_id,BacNet_hwd);	
		}
		if((bac_read_which_list == BAC_READ_OUTPUT_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
		{
			resend_count = 0;
			do 
			{
				resend_count ++;
				if(resend_count>10)
					goto myend;
				g_invoke_id = GetPrivateData(1234,READOUTPUT_T3000,(BAC_READ_INPUT_GROUP_NUMBER+1)*i,4+(BAC_READ_INPUT_GROUP_NUMBER + 1)*i,sizeof(Str_out_point));
				Sleep(500);
			} while (g_invoke_id<0);
			Bacnet_Refresh_Info.Output_InvokeID[i] = g_invoke_id;
			Post_Invoke_ID_Monitor_Thread(MY_INVOKE_ID,g_invoke_id,BacNet_hwd);
		}
		if((bac_read_which_list == BAC_READ_VARIABLE_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
		{
			resend_count = 0;
			do 
			{
				resend_count ++;
				if(resend_count>10)
					goto myend;
				g_invoke_id = GetPrivateData(1234,READVARIABLE_T3000,(BAC_READ_INPUT_GROUP_NUMBER+1)*i,4+(BAC_READ_INPUT_GROUP_NUMBER + 1)*i,sizeof(Str_variable_point));
				Sleep(500);
			} while (g_invoke_id<0);

			Bacnet_Refresh_Info.Variable_InvokeID[i] = g_invoke_id;
			Post_Invoke_ID_Monitor_Thread(MY_INVOKE_ID,g_invoke_id,BacNet_hwd);
		}
		if((bac_read_which_list == BAC_READ_PROGRAM_LIST) || (bac_read_which_list ==BAC_READ_ALL_LIST))
		{
			resend_count = 0;
			do 
			{
				resend_count ++;
				if(resend_count>10)
					goto myend;
				g_invoke_id = GetPrivateData(1234,READPROGRAM_T3000,(BAC_READ_INPUT_GROUP_NUMBER+1)*i,4+(BAC_READ_INPUT_GROUP_NUMBER + 1)*i,sizeof(Str_program_point));
				Sleep(500);
			} while (g_invoke_id<0);

			Bacnet_Refresh_Info.Program_InvokeID[i] = g_invoke_id;
			Post_Invoke_ID_Monitor_Thread(MY_INVOKE_ID,g_invoke_id,BacNet_hwd);
		}
	}

	

		hwait_thread = NULL;
	return 0;
		//::PostMessage(BacNet_hwd,WM_SEND_OVER,0,0);
myend:	hwait_thread = NULL;
		((CButton *)(pParent->GetDlgItem(IDC_BUTTON_BAC_READ_TOTAL)))->EnableWindow(TRUE);
		::PostMessage(BacNet_hwd,WM_DELETE_WAIT_DLG,0,0);
	return 0;
}