
// IFCCompressorDlg.h : 头文件
//

#pragma once
#include <string>

// CIFCCompressorDlg 对话框
class CIFCCompressorDlg : public CDialogEx
{
// 构造
public:
	CIFCCompressorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IFCCOMPRESSOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
