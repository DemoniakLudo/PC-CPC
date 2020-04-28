#include  <afxwin.h>

#include  "Resource.h"
#include  "BreakPoints.h"
#include  "EditBreakPoint.h"



IMPLEMENT_DYNAMIC(EditBreakPoint, CDialog)

EditBreakPoint::EditBreakPoint(CWnd* pParent /*=NULL*/)
    : CDialog(EditBreakPoint::IDD, pParent)
{

}


void EditBreakPoint::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ENABLE, m_BpEnable);
    DDX_Control(pDX, IDC_COND_ENABLE, m_BpEnableCondition);
    DDX_Control(pDX, IDC_LIST_BREAK, m_ListConditions);
    DDX_Control(pDX, IDC_ADR, m_Adr);
    DDX_Control(pDX, IDC_CONDITION, m_Condition);
}


BEGIN_MESSAGE_MAP(EditBreakPoint, CDialog)
    ON_EN_CHANGE(IDC_CONDITION, OnEnChangeCondition)
    ON_BN_CLICKED(IDC_ENABLE, OnBnClickedEnable)
    ON_BN_CLICKED(IDC_COND_ENABLE, OnBnClickedCondEnable)
    ON_LBN_SELCHANGE(IDC_LIST_BREAK, OnLbnSelchangeListBreak)
END_MESSAGE_MAP()


// Gestionnaires de messages de EditBreakPoint

void EditBreakPoint::OnEnChangeCondition()
{
    // TODO:  S'il s'agit d'un contrôle RICHEDIT, le contrôle ne
    // envoyez cette notification sauf si vous substituez CDialog::OnInitDialog()
    // fonction et appelle CRichEditCtrl().SetEventMask()
    // avec l'indicateur ENM_CHANGE ajouté au masque grâce à l'opérateur OR.

    // TODO:  Ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void EditBreakPoint::OnBnClickedEnable()
{
    // TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void EditBreakPoint::OnBnClickedCondEnable()
{
    // TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}

void EditBreakPoint::OnLbnSelchangeListBreak()
{
    // TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}
