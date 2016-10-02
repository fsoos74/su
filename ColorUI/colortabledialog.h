#ifndef COLORTABLEDIALOG_H
#define COLORTABLEDIALOG_H

#include<QDialog>
#include<QVector>
#include<QColor>
#include<QLabel>
#include<QPushButton>
#include<QBoxLayout>
#include<QGridLayout>
#include<QSlider>
#include<QLabel>
#include<QLineEdit>
#include<QSpinBox>
#include<QButtonGroup>
#include<QRadioButton>


class ColorTableDialog : public QDialog
{
    Q_OBJECT

public:

    const int N_COLORS=256;
    const int COLORS_PER_ROW=32;
    const int COLOR_BUTTON_WIDTH=32;
    const int COLOR_BUTTON_HEIGHT=32;

    ColorTableDialog(const QVector<QRgb>& startColors, QWidget* parent=nullptr);

    const QVector<QRgb>& colors()const{
        return m_colors;
    }

    QString colortablesPath()const{
        return m_colorTableDir;
    }

signals:

    void colorChanged(int, const QRgb&);
    void colorsChanged( const QVector<QRgb>&);

public slots:

    void setColor( int idx, const QRgb& color );
    void setColors( const QVector<QRgb>& colors );
    void interpolateColors( int i, int j);

protected:

    void keyPressEvent(QKeyEvent *ev);
    void accept();

private slots:

    void onLoad();
    void onSave();
    void onReset();
    void onFlip();
    void onColorButtonClicked();
    void sliderValueChanged();
    void lineEditReturnPressed();

private:

    bool eventFilter(QObject *obj, QEvent *event);
    void colorToControls( const QRgb& color );
    void setCurrentColorIndex( int );
    void setButtonColor( QPushButton* button, const QRgb& rgb );

    void saveSettings();
    void loadSettings();


    QLayout* createAndLayoutColorButtons();
    QLayout* createAndLayoutColorControls();
    QLayout* createAndLayoutButtons();
    void makeConnections();

    QVector<QRgb> m_startColors;
    QVector<QRgb> m_colors;
    QSlider* slRed;
    QSlider* slGreen;
    QSlider* slBlue;
    QSlider* slAlpha;
    QLineEdit* leRed;
    QLineEdit* leGreen;
    QLineEdit* leBlue;
    QLineEdit* leAlpha;
    QPushButton* btOk;
    QPushButton* btReset;
    QPushButton* btFlip;
    QPushButton* btCancel;
    QPushButton* btLoad;
    QPushButton* btSave;
    QVector<QPushButton*> colorButtons;

    QString m_colorTableDir;

    int currentColorIndex=0;

    bool ignoreSliderValueChange=false;        // flag if value changes for sliders should be ignored
};

#endif // COLORTABLEDIALOG_H
