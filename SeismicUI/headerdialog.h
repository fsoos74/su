#ifndef HEADERDIALOG_H
#define HEADERDIALOG_H

#include <QDialog>
#include <QItemSelection>
#include <QIntValidator>
#include <QStandardItemModel>
#include <header.h>
#include <segy_header_def.h>

class NavigationWidget;

namespace Ui {
class HeaderDialog;
}

class HeaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HeaderDialog(QWidget *parent = 0, bool navigate=false);
    ~HeaderDialog();

    NavigationWidget* navigationWidget();

signals:
    void navigateFirst();
    void navigateLast();
    void navigatePrevious();
    void navigateNext();
    void navigateTo( int );
    void finished();

public slots:
    void setData( const seismic::Header& hdr, const std::vector<seismic::SEGYHeaderWordDef>& hdef);

private slots:
    void tableSelectionChanged( const QItemSelection& new_item, const QItemSelection& old_item);

    void on_pbClose_clicked();

private:

    Ui::HeaderDialog *ui;

    QStandardItemModel *model=nullptr;

    seismic::Header m_hdr;
    std::vector<seismic::SEGYHeaderWordDef> m_hdef;
};

#endif // HEADERDIALOG_H
