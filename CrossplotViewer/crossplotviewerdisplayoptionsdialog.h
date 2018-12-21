#ifndef CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H
#define CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H

#include <QDialog>
#include <crossplotview.h> // Symbol


namespace Ui {
class CrossplotViewerDisplayOptionsDialog;
}

class CrossplotViewerDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrossplotViewerDisplayOptionsDialog(QWidget *parent = 0);
    ~CrossplotViewerDisplayOptionsDialog();

    int pointSize();
    QColor pointColor();
    CrossplotView::Symbol pointSymbol();
    CrossplotView::ColorStyle colorStyle();
    QColor trendlineColor();

public slots:
    void setPointSize(int);
    void setPointColor(QColor);
    void setPointSymbol(CrossplotView::Symbol);
    void setColorStyle(CrossplotView::ColorStyle);
    void setTrendlineColor(QColor);

signals:
    void pointSizeChanged(int);
    void pointSymbolChanged(CrossplotView::Symbol);
    void colorStyleChanged(CrossplotView::ColorStyle);
    void pointColorChanged(QColor);
    void trendlineColorChanged(QColor);

private slots:
    void on_cbFixColor_clicked();
    void on_cbTrendlineColor_clicked();
    void on_cbSymbol_currentIndexChanged(const QString &arg1);

    void on_cbColorStyle_currentIndexChanged(const QString &arg1);

private:

    void saveSettings();
    void loadSettings();

    Ui::CrossplotViewerDisplayOptionsDialog *ui;
};

#endif // CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H
