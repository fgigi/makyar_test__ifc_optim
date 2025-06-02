
// IFCCompressorDlg.h : ͷ�ļ�
//

#pragma once
#include <string>

// CIFCCompressorDlg �Ի���
class CIFCCompressorDlg : public CDialogEx
{
// ����
public:
	CIFCCompressorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IFCCOMPRESSOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP hDropInfo); 
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelectbtn();
	afx_msg void OnBnClickedStartbtn();
	afx_msg void OnEnChangeFileedt();
	HANDLE hSemaphore;
	int process;
	CDialog *digResult;
	std::string processInfo;
	static DWORD WINAPI CreateCompressor(LPVOID lpParam);
	static DWORD WINAPI CreateProcessUpdate(LPVOID lpParam);
	afx_msg void OnBnClickedNofpr();
	afx_msg void OnBnClickedFpr();
	afx_msg void OnEnChangeFpreb();
};
