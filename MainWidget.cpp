#include "MainWidget.h"
#include <SilkAudioCode.h>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    int nResult = -1;

    SilkAudioCode code;
    //nResult = code.encode("test01.wav", "test01.silk");
    //nResult = code.decode("test01.silk", "test01.pcm");

    qDebug() << "#nResult " << nResult;

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    pMainLayout->setContentsMargins(0,0,0,0);
    pMainLayout->setSpacing(1);

    // 输出路径
    m_pExportPathCbx = new QComboBox( this);
    m_pExportPathCbx->setMinimumSize(240,24);
    m_pExportPathCbx->addItem("D:\\SilkOutPut");
    m_pChangeExportPathButton = new QPushButton("改变", this);
    m_pChangeExportPathButton->setMaximumSize(80,30);
    auto pLabel = new QLabel(QString("输出文件夹"), this);
    auto pHLayout = new QHBoxLayout;
    pHLayout->setContentsMargins(2,2,2,2);
    pHLayout->setSpacing(8);
    pHLayout->addSpacing(4);
    pHLayout->addWidget(pLabel);
    pHLayout->addWidget(m_pExportPathCbx);
    pHLayout->addStretch();
    pHLayout->addWidget(m_pChangeExportPathButton);
    pHLayout->addSpacing(10);
    pMainLayout->addLayout(pHLayout);

    m_pConvertButton = new QPushButton("确认", this);
    m_pImportFileButton = new QPushButton("添加文件", this);

    auto pButtonLayout = new QHBoxLayout;
    pButtonLayout->setSpacing(1);
    pButtonLayout->setContentsMargins(0,0,0,0);
    pButtonLayout->addSpacing(4);
    pButtonLayout->addWidget(m_pImportFileButton);
    pButtonLayout->addStretch();
    pButtonLayout->addWidget(m_pConvertButton);
    pButtonLayout->addSpacing(10);
    pMainLayout->addLayout(pButtonLayout);

    m_pTableView = new QTableView(this);
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    pMainLayout->addWidget(m_pTableView);

    setLayout(pMainLayout);
}
