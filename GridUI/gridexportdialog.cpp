#include "gridexportdialog.h"
#include "ui_gridexportdialog.h"

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include <QMessageBox>

#include<iostream>

GridExportDialog::GridExportDialog(std::shared_ptr<Grid2D<double>> grid, QWidget *parent) :
    QDialog(parent),
    m_grid(grid),
    ui(new Ui::GridExportDialog)
{
    ui->setupUi(this);


}

GridExportDialog::~GridExportDialog()
{
    delete ui;
}

void GridExportDialog::on_pushButton_clicked()
{
    QString fn=QFileDialog::getSaveFileName(this, "Export Grid", QDir::homePath() );
    if( fn.isEmpty()) return;
    ui->leFile->setText(fn);
}

void GridExportDialog::on_leFile_textChanged(const QString &arg1)
{
    ui->pbExport->setEnabled( !ui->leFile->text().isEmpty());
}


bool GridExportDialog::exportGrid(){

    bool useNulls=ui->cbNull->isChecked();
    QString nullValue=ui->leNull->text();

    QString filename=ui->leFile->text();

    QFile file( filename);

    std::cout<<"Filename="<<filename.toStdString()<<std::endl;

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Export Grid"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(filename)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);

    if( !m_grid ) return true;      // empty file for empty grid!

    Grid2DBounds bounds=m_grid->bounds();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            QString line=QString::asprintf("%6d %6d ", i, j);
            double value=(*m_grid)(i,j);
            if( value==m_grid->NULL_VALUE ){
                if( !useNulls ) continue;
                line.append(nullValue);
            }
            else{
                line.append(QString::number(value));
            }

            out<<line<<'\n';
            if( !out.status()==QTextStream::Ok){
                QMessageBox::warning(this, tr("Export Grid"),
                                     tr("Cannot write line \"%1\" to\n file %2:\n%3.")
                                     .arg(line)
                                     .arg(filename)
                                     .arg(file.errorString()));
                return false;
            }
        }

    }

    return true;
}

void GridExportDialog::on_pbExport_clicked()
{
    if( exportGrid()){
        accept();
    }
}
