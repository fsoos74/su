#ifndef SELECTHEADERWORDSDIALOG_H
#define SELECTHEADERWORDSDIALOG_H

#include <QDialog>
#include<QComboBox>
#include<QPushButton>
#include<QVector>
#include<QStringList>


class SelectHeaderWordsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectHeaderWordsDialog( int n=1, QWidget *parent = 0);
    ~SelectHeaderWordsDialog();

    int headerWordCount()const{
        return comboBoxes.size();
    }

    QString selectedHeaderWord(int )const;

public slots:

    void setHeaderWords( const QStringList& list);

private:

    QLayout* createAndLayoutComboBoxes();
    QLayout* createAndLayoutButtons();
    void createConnections();

    QPushButton* btOk;
    QPushButton* btCancel;
    QVector<QComboBox*> comboBoxes;

};

#endif // SELECTHEADERWORDSDIALOG_H
