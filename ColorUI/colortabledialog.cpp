#include "colortabledialog.h"

#include<QFileDialog>
#include<QIntValidator>
#include<QKeyEvent>
#include<QFile>
#include<QMessageBox>
#include<QSettings>
#include<QVariant>
#include<QSpacerItem>
#include<QScrollArea>
#include "xctwriter.h"
#include "xctreader.h"

#include "colortableimportdialog.h"

#include <QColorDialog>

#include<iostream>

ColorTableDialog::ColorTableDialog(const QVector<QRgb>& startColors, QWidget* parent) :
    QDialog(parent), m_startColors(startColors), m_colors( startColors), m_colorTableDir( QDir::homePath())
{

    scrollArea=new QScrollArea(this);
    QWidget* widget=new QWidget;
    colorButtonsLayout=new QGridLayout;
    widget->setLayout(colorButtonsLayout);
    scrollArea->setWidget(widget);

    QVBoxLayout* mainLayout=new QVBoxLayout;
    mainLayout->addWidget(scrollArea);
    createLayoutAndConnectColorButtons();


    QLayout* colorControlLayout=    createAndLayoutColorControls();
    QLayout* buttonLayout=          createAndLayoutButtons();

    //mainLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));

    //mainLayout->addItem(colorButtonLayout);
    mainLayout->addItem(colorControlLayout);

    //mainLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));

    mainLayout->addItem(buttonLayout);

    setLayout(mainLayout);

    makeConnections();

    loadSettings();
}

void ColorTableDialog::setColor( int idx, const QRgb& color ){

    Q_ASSERT( idx>=0 && idx<m_colors.size());

    if( m_colors[idx]==color ) return;

    m_colors[idx]=color;

    colorToControls( color );

    setButtonColor( colorButtons[idx], color);

    if( idx==currentColorIndex ){
        colorToControls(color);
    }

    emit colorChanged( idx, color);
    emit colorsChanged(m_colors);
}


void ColorTableDialog::setColors( const QVector<QRgb>& colors ){

    if( m_colors==colors ) return;

    if( colors.empty() ) return;

    m_colors=colors;

    createLayoutAndConnectColorButtons();

    //currentColorIndex=0;
    //colorToControls(m_colors[currentColorIndex]);

    emit colorsChanged(m_colors);
}

void ColorTableDialog::interpolateColors( int i, int j){

    Q_ASSERT( i>=0 && i< m_colors.size());
    Q_ASSERT( j>=0 && j< m_colors.size());


    if(i==j ) return;

    if( j<i) std::swap(i,j);

    int red_i=qRed(m_colors[i]);
    int green_i=qGreen(m_colors[i]);
    int blue_i=qBlue(m_colors[i]);
    int alpha_i=qAlpha(m_colors[i]);

    int red_j=qRed(m_colors[j]);
    int green_j=qGreen(m_colors[j]);
    int blue_j=qBlue(m_colors[j]);
    int alpha_j=qAlpha(m_colors[j]);

    QVector<QRgb> tmp(m_colors);

    for( int k=i; k<=j; k++){

        int red=red_i + (k-i)*(red_j-red_i)/(j-i);
        int green=green_i + (k-i)*(green_j-green_i)/(j-i);
        int blue=blue_i + (k-i)*(blue_j-blue_i)/(j-i);
        int alpha=alpha_i + (k-i)*(alpha_j-alpha_i)/(j-i);
        tmp[k]=qRgba( red, green, blue, alpha );
    }

    setColors(tmp);
}


void ColorTableDialog::onLoad(){

    QString fileName=QFileDialog::getOpenFileName(this, "Open Color Table", m_colorTableDir, tr("XML Color Table Files (*.xct)") );

    if( fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Load Color Table"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    // update coloor table directory
    m_colorTableDir=QFileInfo(fileName).absolutePath();
    QVector<QRgb> colors;

    XCTReader reader(colors);
    if (!reader.read(&file)) {
        QMessageBox::warning(this, tr("Load Color Table"),
                             tr("Parse error in file %1:\n\n%2")
                             .arg(fileName)
                             .arg(reader.errorString()));

    } else {

        setColors( colors );
    }

}

void ColorTableDialog::onImport(){

    ColortableImportDialog dlg(this);
    dlg.setWindowTitle(tr("import Colortable"));

    if( dlg.exec()==QDialog::Accepted){

        setColors(dlg.colors());
    }
}

void ColorTableDialog::onSave(){


    QString fileName=QFileDialog::getSaveFileName(this, "Save Color Table", m_colorTableDir, tr("XML Color Table Files (*.xct)" ));
    if( fileName.isEmpty()) return;

    // make files with no suffix end with xct
    QFileInfo info(fileName);
    if( info.suffix().isEmpty() ){
        fileName=fileName+QString(".xct");
    }


    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Save Color Table"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    XCTWriter writer(m_colors);
    writer.writeFile(&file);
}

void ColorTableDialog::onReset(){
    setColors( m_startColors);
}


void ColorTableDialog::onFlip(){

    QVector<QRgb> colors(m_colors.size());

    for( int i=0; i<m_colors.size(); i++){
        colors[i]=m_colors[ m_colors.size() - i - 1];
    }
    setColors(colors);
}

void ColorTableDialog::onColorButtonClicked(){

    QObject* sender=QObject::sender();

    for( int i=0; i<colorButtons.size(); i++){
        if( colorButtons[i]==sender){

            ignoreSliderValueChange=true;
            setCurrentColorIndex(i);
            ignoreSliderValueChange=false;
            break;
        }
    }

}

void ColorTableDialog::sliderValueChanged(){

    if( ignoreSliderValueChange ) return;

    int red=slRed->value();
    int green=slGreen->value();
    int blue=slBlue->value();
    int alpha=slAlpha->value();
    setColor(currentColorIndex, qRgba( red, green, blue, alpha) );

}

void ColorTableDialog::lineEditReturnPressed(){

    int red=leRed->text().toInt();
    int green=leGreen->text().toInt();
    int blue=leBlue->text().toInt();
    int alpha=leAlpha->text().toInt();
    setColor(currentColorIndex, qRgba( red, green, blue, alpha) );

}


bool ColorTableDialog::eventFilter(QObject *obj, QEvent *event)
{
    for( int i=0; i<colorButtons.size(); i++){

        if( obj == colorButtons[i] && event->type() == QEvent::MouseButtonDblClick ){

            chooseColor(i);
            return true;
        }

        if (obj == colorButtons[i] && event->type() == QEvent::MouseButtonPress) {

            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

            // interpolate
            if (mouseEvent->modifiers() == Qt::ShiftModifier) {

                if( i!=currentColorIndex ){

                    interpolateColors( currentColorIndex, i);
                }

                return true;
            }

            // copy
            if (mouseEvent->modifiers() == Qt::ControlModifier) {

                if( i!=currentColorIndex ){

                    setColor( i, m_colors[currentColorIndex] );
                }

                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}


void ColorTableDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
        //std::cout<<"ATE IT"<<std::endl;
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}




void ColorTableDialog::setCurrentColorIndex( int idx ){

    Q_ASSERT( idx>=0 && idx<m_colors.size());

    currentColorIndex=idx;
    colorToControls(m_colors[idx]);

    for( int i=0; i<colorButtons.size(); i++){
        colorButtons[i]->setChecked(i==currentColorIndex);
    }

}

void ColorTableDialog::colorToControls( const QRgb& color ){

    slRed->setValue(qRed(color));
    leRed->setText(QString::number(qRed(color)));

    slGreen->setValue(qGreen(color));
    leGreen->setText(QString::number(qGreen(color)));

    slBlue->setValue(qBlue(color));
    leBlue->setText(QString::number(qBlue(color)));

    slAlpha->setValue(qAlpha(color));
    leAlpha->setText(QString::number(qAlpha(color)));
}

void ColorTableDialog::setButtonColor( QPushButton* button, const QRgb& rgb ){

    Q_ASSERT( button );

    QPixmap pixmap(COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
    pixmap.fill(rgb);
    QIcon icon(pixmap);
    button->setIcon(icon);
}

void ColorTableDialog::chooseColor(int idx){

    QColor c = QColorDialog::getColor( m_colors.at(idx), this, QString("Select Color %1").arg(idx) );
    if( !c.isValid() ) return;

    setColor( idx, qRgb(c.red(), c.green(), c.blue()) );
}

void ColorTableDialog::accept()
{
    QDialog::accept();
    saveSettings();
}



void ColorTableDialog::createLayoutAndConnectColorButtons(){

    const int GAP=3;

    for( QPushButton* button : colorButtons ){

        if( !button ) continue;
        colorButtonsLayout->removeWidget(button);
        delete button;
    }
    colorButtons.clear();

    if( m_colors.empty()) return;

    colorButtons.reserve(m_colors.size() );

    QWidget* widget=scrollArea->widget();

    for( int i=0; i<m_colors.size(); i++){

        int row=i / COLORS_PER_ROW;
        int column= i % COLORS_PER_ROW;

        QPushButton* button=new QPushButton(widget);
        //button->setStyleSheet("border:2px solid #000000;");
        button->setCheckable(true);
        button->setFixedSize(COLOR_BUTTON_WIDTH+2*GAP, COLOR_BUTTON_HEIGHT+2*GAP);
        setButtonColor(button, m_colors[i] );
        colorButtonsLayout->addWidget(button, row, column);
        colorButtons.push_back(button);

        button->installEventFilter(this);
        connect( button, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));
    }

    colorButtonsLayout->setSpacing(GAP);
    colorButtonsLayout->setVerticalSpacing(GAP);

    for( int i=0; i<COLORS_PER_ROW; i++){
        colorButtonsLayout->setColumnMinimumWidth( i, COLOR_BUTTON_WIDTH);
    }
    for( int i=0; i<=m_colors.size()/COLORS_PER_ROW; i++ ){
        colorButtonsLayout->setRowMinimumHeight( i, COLOR_BUTTON_HEIGHT );
    }

    colorButtonsLayout->update();

    widget->setMinimumSize(colorButtonsLayout->sizeHint() );
}

QLayout* ColorTableDialog::createAndLayoutColorControls(){

    const int LINE_EDIT_WIDTH=50;

    QGridLayout* colorControlLayout=new QGridLayout;
    colorControlLayout->setSpacing(5);

    QLabel* rlabel=new QLabel( "Red:", this);
    rlabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    colorControlLayout->addWidget(rlabel, 0, 0);

    leRed=new QLineEdit( this );
    leRed->setAlignment(Qt::AlignRight);
    leRed->setFixedWidth(LINE_EDIT_WIDTH);
    colorControlLayout->addWidget(leRed, 0, 1);

    slRed=new QSlider(Qt::Orientation::Horizontal, this);
    slRed->setRange(0,255);
    colorControlLayout->addWidget(slRed, 0, 2);


    QLabel* glabel=new QLabel( "Green:", this);
    glabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter );
    colorControlLayout->addWidget(glabel, 1, 0);

    leGreen=new QLineEdit( this );
    leGreen->setAlignment(Qt::AlignRight);
    leGreen->setFixedWidth(LINE_EDIT_WIDTH);
    colorControlLayout->addWidget(leGreen, 1, 1);

    slGreen=new QSlider( Qt::Orientation::Horizontal, this);
    slGreen->setRange(0,255);
    colorControlLayout->addWidget(slGreen, 1, 2);


    QLabel* blabel=new QLabel( "Blue:", this);
    blabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter );
    colorControlLayout->addWidget(blabel, 2, 0);

    leBlue=new QLineEdit( this );
    leBlue->setAlignment(Qt::AlignRight);
    leBlue->setFixedWidth(LINE_EDIT_WIDTH);
    colorControlLayout->addWidget(leBlue, 2, 1);

    slBlue=new QSlider( Qt::Orientation::Horizontal, this);
    slBlue->setRange(0,255);
    colorControlLayout->addWidget(slBlue, 2, 2);


    QLabel* alabel=new QLabel( "Alpha:", this);
    alabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter );
    colorControlLayout->addWidget(alabel, 3, 0);

    leAlpha=new QLineEdit( this );
    leAlpha->setAlignment(Qt::AlignRight);
    leAlpha->setFixedWidth(LINE_EDIT_WIDTH);
    colorControlLayout->addWidget(leAlpha, 3, 1);

    slAlpha=new QSlider( Qt::Orientation::Horizontal, this);
    slAlpha->setRange(0,255);
    colorControlLayout->addWidget(slAlpha, 3, 2);
    return colorControlLayout;
}

QLayout* ColorTableDialog::createAndLayoutButtons(){

    btOk=new QPushButton("Ok", this);
    btReset=new QPushButton("Reset", this);
    btFlip=new QPushButton("Flip", this);
    btCancel=new QPushButton("Cancel", this);
    btLoad=new QPushButton("Load", this);
    btSave=new QPushButton("Save", this);
    btImport=new QPushButton("Import", this);

    QHBoxLayout* buttonLayout=new QHBoxLayout;

    buttonLayout->addWidget(btLoad);
    buttonLayout->addWidget(btImport);
    buttonLayout->addWidget(btSave);
    buttonLayout->addWidget(btReset);
    buttonLayout->addWidget(btFlip);
    buttonLayout->addItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    buttonLayout->addWidget(btCancel);
    buttonLayout->addWidget(btOk);

    return buttonLayout;
}

void ColorTableDialog::makeConnections(){


    /*
     * for( int i=0; i<N_COLORS; i++){

        QPushButton* button=colorButtons[i];

        button->installEventFilter(this);
        connect( button, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));
    }
    */

    connect( slRed, SIGNAL( valueChanged(int)), this, SLOT( sliderValueChanged()));
    connect( leRed, SIGNAL( returnPressed()), this, SLOT( lineEditReturnPressed()));
    connect( slGreen, SIGNAL( valueChanged(int)), this, SLOT( sliderValueChanged()));
    connect( leGreen, SIGNAL( returnPressed()), this, SLOT( lineEditReturnPressed()));
    connect( slBlue, SIGNAL( valueChanged(int)), this, SLOT( sliderValueChanged()));
    connect( leBlue, SIGNAL( returnPressed()), this, SLOT( lineEditReturnPressed()));
    connect( slAlpha, SIGNAL( valueChanged(int)), this, SLOT( sliderValueChanged()));
    connect( leAlpha, SIGNAL( returnPressed()), this, SLOT( lineEditReturnPressed()));

    connect( btOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect( btCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect( btLoad, SIGNAL(clicked()), this, SLOT(onLoad()));
    connect( btImport, SIGNAL(clicked()), this, SLOT(onImport()));
    connect( btSave, SIGNAL(clicked()), this, SLOT(onSave()));
    connect( btReset, SIGNAL(clicked()), this, SLOT(onReset()));
    connect( btFlip, SIGNAL(clicked()), this, SLOT(onFlip()));
}

void ColorTableDialog::saveSettings(){

     QSettings settings(COMPANY, "ColorTableDialog");

     //std::cout<<"writing settings to "<<settings.fileName().toStdString()<<std::endl;

     settings.beginGroup("Dialog");
       settings.setValue("size", size());
       settings.setValue("position", pos() );
     settings.endGroup();

     settings.beginGroup("Paths");
       settings.setValue("colortables-path", m_colorTableDir);
    settings.endGroup();

     settings.sync();
}


void ColorTableDialog::loadSettings(){

    QSettings settings(COMPANY, "ColorTableDialog");

    settings.beginGroup("Dialog");
        resize(settings.value("size", QSize(800, 500)).toSize());
        move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

    settings.beginGroup("Paths");
        m_colorTableDir=settings.value("colortables-path", QDir::homePath()).toString();
    settings.endGroup();
}
