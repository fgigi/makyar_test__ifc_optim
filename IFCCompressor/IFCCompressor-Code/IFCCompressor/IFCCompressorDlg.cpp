
// IFCCompressorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IFCCompressor.h"
#include "IFCCompressorDlg.h"
#include "afxdialogex.h"
#include "../IfcCompression/CIfcCompression.h"
#include "../IfcCompression/CIfcCompression.cpp"
#include <stdio.h>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_DROPFILES() 
END_MESSAGE_MAP()


// CIFCCompressorDlg �Ի���



CIFCCompressorDlg::CIFCCompressorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIFCCompressorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIFCCompressorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIFCCompressorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SelectBTN, &CIFCCompressorDlg::OnBnClickedSelectbtn)
	ON_BN_CLICKED(IDC_StartBTN, &CIFCCompressorDlg::OnBnClickedStartbtn)
	ON_EN_CHANGE(IDC_FileEDT, &CIFCCompressorDlg::OnEnChangeFileedt)
	ON_BN_CLICKED(IDC_NoFPR, &CIFCCompressorDlg::OnBnClickedNofpr)
	ON_BN_CLICKED(IDC_FPR, &CIFCCompressorDlg::OnBnClickedFpr)
	ON_EN_CHANGE(IDC_FPREB, &CIFCCompressorDlg::OnEnChangeFpreb)
END_MESSAGE_MAP()


// CIFCCompressorDlg ��Ϣ�������

BOOL CIFCCompressorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	((CButton*)GetDlgItem(IDC_NoFPR))->SetCheck(TRUE);
	((CProgressCtrl*)GetDlgItem(IDC_CPRO))->SetRange(0, 100);
	((CEdit*)GetDlgItem(IDC_FPREB))->SetWindowTextW(_T("2"));
	((CEdit*)GetDlgItem(IDC_FPREB))->EnableWindow(FALSE);
	
	CFont *f = new CFont();
	f->CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	GetDlgItem(IDC_ResultEDT)->SetFont(f);

	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	digResult = new CDialog();
	digResult->Create(IDD_Result, this);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CIFCCompressorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CIFCCompressorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CIFCCompressorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIFCCompressorDlg::OnBnClickedSelectbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString filePathName; 
	CFileDialog dlg(TRUE,_T("*.ifc"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("IFC File(*.ifc)|*.ifc|All files (*.*)|*.*||")); 
	if(dlg.DoModal()==IDOK) 
		filePathName=dlg.GetPathName(); 
	SetDlgItemText(IDC_FileEDT,filePathName);
}

void CIFCCompressorDlg::OnDropFiles(HDROP hDropInfo)
{
	HDROP m_hDropInfo = hDropInfo; 
	CString Filename; 
	WCHAR szFileName[MAX_PATH + 1];

	if (m_hDropInfo) { 
		
		DragQueryFile(hDropInfo, 0, szFileName, MAX_PATH);
		SetDlgItemText(IDC_FileEDT,szFileName);
	} 

	DragFinish(m_hDropInfo); 

	m_hDropInfo = 0; 

}

void CIFCCompressorDlg::OnBnClickedStartbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_StartBTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_SelectBTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_NoFPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_FPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_FPREB)->EnableWindow(FALSE);
	SetDlgItemText(IDC_ResultEDT,_T(""));
	((CProgressCtrl*)GetDlgItem(IDC_CPRO))->SetPos(0);

	CreateThread(NULL, 0, CreateCompressor, (LPVOID)this, 0, NULL);
	CreateThread(NULL, 0, CreateProcessUpdate, (LPVOID)this, 0, NULL);

}


void CIFCCompressorDlg::OnEnChangeFileedt()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString filePath;
	GetDlgItemText(IDC_FileEDT,filePath);
	if (filePath != _T(""))
	{
		if (GetCheckedRadioButton(IDC_NoFPR, IDC_FPR) == IDC_NoFPR)
			GetDlgItem(IDC_StartBTN)->EnableWindow(TRUE);
		else
		{
			CString str;
			GetDlgItemText(IDC_FPREB, str);
			if (str != _T(""))
				GetDlgItem(IDC_StartBTN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_StartBTN)->EnableWindow(FALSE);
		}
	}else
	{
		GetDlgItem(IDC_StartBTN)->EnableWindow(FALSE);
	}
}

DWORD WINAPI CIFCCompressorDlg::CreateCompressor(LPVOID lpParam)
{
	CIFCCompressorDlg *pDlg = (CIFCCompressorDlg *)lpParam;
	pDlg->process = 0;
	pDlg->processInfo = "";

	CString filePath;
	pDlg->GetDlgItemText(IDC_FileEDT, filePath);
	std::string fileName = (CStringA)(filePath);
	if (pDlg->GetCheckedRadioButton(IDC_NoFPR, IDC_FPR) == IDC_NoFPR)
		CIfcCompression::IfcCompressor(fileName, fileName + ".ifc", pDlg->hSemaphore, pDlg->process, pDlg->processInfo);
	else
	{
		CString percision;
		pDlg->GetDlgItemText(IDC_FPREB, percision);
		int p = atoi((CStringA)percision);
		CIfcCompression::IfcCompressor(fileName, fileName + ".ifc", pDlg->hSemaphore, pDlg->process, pDlg->processInfo,0,p,true);
	}
	
	pDlg->process = 100;
	ReleaseSemaphore(pDlg->hSemaphore, 1, NULL);
	return 1;
}

DWORD WINAPI CIFCCompressorDlg::CreateProcessUpdate(LPVOID lpParam)
{
	CIFCCompressorDlg *pDlg=(CIFCCompressorDlg *)lpParam;
	CString pInfo;
	CEdit *pEdit = (CEdit *)pDlg->GetDlgItem(IDC_ResultEDT);
	while (true)
	{
		WaitForSingleObject(pDlg->hSemaphore, INFINITE);
		pInfo = pDlg->processInfo.c_str();
		pDlg->SetDlgItemText(IDC_ResultEDT, pInfo);
		((CProgressCtrl*)(pDlg->GetDlgItem(IDC_CPRO)))->SetPos(pDlg->process);
		pEdit->SetSel(pInfo.GetLength(), pInfo.GetLength(), FALSE); //���ù��λ��
		if (pDlg->process == 100)
			break;
	}
	pDlg->GetDlgItem(IDC_StartBTN)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_SelectBTN)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_NoFPR)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_FPR)->EnableWindow(TRUE);
	if (pDlg->GetCheckedRadioButton(IDC_NoFPR, IDC_FPR) == IDC_FPR)
		pDlg->GetDlgItem(IDC_FPREB)->EnableWindow(TRUE);

	CString filePath;
	pDlg->GetDlgItemText(IDC_FileEDT, filePath);
	std::string fileName = (CStringA)(filePath);
	std::string compressedFileName = fileName + ".ifc";
	struct stat info;
	stat(fileName.c_str(), &info);
	int oriSize = info.st_size;
	stat(compressedFileName.c_str(), &info);
	int cprSize = info.st_size;
	CString oriS, cprS;
	oriS.Format(_T("%.2f"), (double)oriSize/(1024*1024));
	cprS.Format(_T("%.2f"), (double)cprSize / (1024 * 1024));
	//pDlg->MessageBox(_T("Done!"));
	pDlg->digResult->ShowWindow(SW_SHOW);
	CRect rc;
	(pDlg->digResult->GetDlgItem(IDC_Ori))->GetClientRect(&rc);

	(CStatic *)(pDlg->digResult->GetDlgItem(IDC_CPR))->SetWindowPos(NULL, 0, 0, 50, (rc.bottom - rc.top) * ((double)cprSize / oriSize), SWP_NOZORDER | SWP_NOMOVE);
	(pDlg->digResult->GetDlgItem(IDC_ORISIZE))->SetWindowText(oriS);
	(pDlg->digResult->GetDlgItem(IDC_CPRSIZE))->SetWindowText(cprS);
	return 1;
}

void CIFCCompressorDlg::OnBnClickedNofpr()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	((CEdit*)GetDlgItem(IDC_FPREB))->EnableWindow(FALSE);
}


void CIFCCompressorDlg::OnBnClickedFpr()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	((CEdit*)GetDlgItem(IDC_FPREB))->EnableWindow(TRUE);
	CString str, filePath;
	GetDlgItemText(IDC_FPREB, str);
	GetDlgItemText(IDC_FileEDT, filePath);
	if (str != _T("") && filePath != _T(""))
		GetDlgItem(IDC_StartBTN)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_StartBTN)->EnableWindow(FALSE);
}


void CIFCCompressorDlg::OnEnChangeFpreb()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str, filePath;
	GetDlgItemText(IDC_FPREB, str);
	GetDlgItemText(IDC_FileEDT, filePath);
	if (str == _T(""))
	{
		str = '0';
		SetDlgItemText(IDC_FPREB, str);
	}
	else
	{
		int p = atoi((CStringA)str);
		if (p < 0 )
		{
			str = '0';
			SetDlgItemText(IDC_FPREB, str);
		}
		else if (p>6)
		{
			str = '6';
			SetDlgItemText(IDC_FPREB, str);
		}
		else if (str.GetLength() > 1)
		{
			str.Format(_T("%d"),p);
			SetDlgItemText(IDC_FPREB, str);
		}
		else
		{
			if (filePath != _T(""))
				GetDlgItem(IDC_StartBTN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_StartBTN)->EnableWindow(FALSE);
		}
	}
}
