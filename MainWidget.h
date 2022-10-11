/**
 ** This file is part of the SilkCodeDemo project.
 ** Copyright 2022 kevinlq <kevinlq0912@163.com>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#pragma once

#include <QWidget>

#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QAbstractItemModel>

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

private:
    QTableView  *m_pTableView = nullptr;
    QComboBox   *m_pExportPathCbx = nullptr;
    QPushButton *m_pChangeExportPathButton = nullptr;

    QPushButton *m_pConvertButton = nullptr;
    QPushButton *m_pImportFileButton = nullptr;
};

