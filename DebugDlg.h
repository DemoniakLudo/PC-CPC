#pragma once
#include  <afxwin.h>

#include  "Resource.h"


#define     ITEM_PC         1
#define     ITEM_BREAK      2


class CMyListBox : public CListBox
{
public :
    CMyListBox( void ) { maxLen = 256; }
    void SetMaxLen( int m ) { maxLen = m; }

protected:
    virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct  );
    virtual void MeasureItem(LPMEASUREITEMSTRUCT) {}
    virtual int CompareItem(LPCOMPAREITEMSTRUCT) { return( 0 ); }

private:
    char Text[ 256 ];
    int maxLen;
};

class DebugDlg : public CDialog
{
// Construction
public:
    DebugDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    //{{AFX_DATA(CSNAEditorDlg)
    enum { IDD = IDD_DEBUG_DIALOG };
    CEdit       m_Crtc_Index;
    CEdit       m_Crtc_Reg00;
    CEdit       m_Crtc_Reg01;
    CEdit       m_Crtc_Reg02;
    CEdit       m_Crtc_Reg03;
    CEdit       m_Crtc_Reg04;
    CEdit       m_Crtc_Reg05;
    CEdit       m_Crtc_Reg06;
    CEdit       m_Crtc_Reg07;
    CEdit       m_Crtc_Reg08;
    CEdit       m_Crtc_Reg09;
    CEdit       m_Crtc_Reg10;
    CEdit       m_Crtc_Reg11;
    CEdit       m_Crtc_Reg12;
    CEdit       m_Crtc_Reg13;
    CEdit       m_Crtc_Reg14;
    CEdit       m_Crtc_Reg15;
    CEdit       m_Crtc_Reg16;
    CEdit       m_Crtc_Reg17;
    CEdit       m_Crtc_Reg18;
    CEdit       m_Crtc_Reg19;
    CEdit       m_Crtc_Reg20;
    CEdit       m_Crtc_Reg21;
    CEdit       m_Crtc_Reg22;
    CEdit       m_Z80_FLAGS;
    CEdit       m_Z80_AF;
    CEdit       m_Z80_BC;
    CEdit       m_Z80_DE;
    CEdit       m_Z80_HL;
    CEdit       m_Z80_AF_;
    CEdit       m_Z80_BC_;
    CEdit       m_Z80_DE_;
    CEdit       m_Z80_HL_;
    CEdit       m_Z80_SP;
    CEdit       m_Z80_PC;
    CEdit       m_Z80_IX;
    CEdit       m_Z80_IY;
    CEdit       m_Z80_IR;
    CEdit       m_Z80_IM;
    CEdit       m_Z80_IFF1;
    CEdit       m_Z80_IFF2;
    CEdit       m_Z80_T;
    CEdit       m_VGA_Rom;
    CEdit       m_VGA_Ram;
    CEdit       m_VGA_Pen;
    CStatic     m_VGA_Col00;
    CStatic     m_VGA_Col01;
    CStatic     m_VGA_Col02;
    CStatic     m_VGA_Col03;
    CStatic     m_VGA_Col04;
    CStatic     m_VGA_Col05;
    CStatic     m_VGA_Col06;
    CStatic     m_VGA_Col07;
    CStatic     m_VGA_Col08;
    CStatic     m_VGA_Col09;
    CStatic     m_VGA_Col10;
    CStatic     m_VGA_Col11;
    CStatic     m_VGA_Col12;
    CStatic     m_VGA_Col13;
    CStatic     m_VGA_Col14;
    CStatic     m_VGA_Col15;
    CStatic     m_VGA_Col16;
    CEdit       m_NumRom;
    CEdit       m_CntHSync;
    CScrollBar  m_ScrollDesasm;
    CScrollBar  m_ScrollStack;
    CScrollBar  m_ScrollDump;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSNAEditorDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CSNAEditorDlg)
    virtual void OnOK( void ) {};
    virtual BOOL OnInitDialog( void );
    afx_msg void OnPaint( void );
    afx_msg HCURSOR OnQueryDragIcon( void );
    afx_msg BOOL PreTranslateMessage( MSG *pMsg );
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBpClearT( void );
    afx_msg void OnBpShowComp( void );
    afx_msg void OnBnClickedAscii( void );
    afx_msg void OnLbnDblclkDesasm( void );
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void UpdateDatas( void );
    void ExecOneCycle( BOOL );
    void SetHex( CEdit * Ctrl, int valeur, int digits = 4 );
    void SetColor( CStatic * s, int i );

    BOOL IsLoaded, IsValidated;
    CFont font9, font8;
    CButton m_BpShowComp;
    int AsmOffset, IncAsmOffset, StackOffset, DumpOffset;
    BOOL Drawing;
    CMyListBox m_Desasm;
    CListBox m_Stack;
    CListBox m_Dump;
    CButton m_Ascii;
};


void ShowDebug( HWND hWnd );

