#ifndef AMPLITUDEOFFSETCURVEVIEWER_H
#define AMPLITUDEOFFSETCURVEVIEWER_H

#include <QMainWindow>
#include<QGraphicsScene>
#include<QVector>
#include<QPointF>
#include<avoproject.h>
#include "amplitudecurvedefinition.h"
#include "amplitudecurvedataselectiondialog.h"
#include "windowreductionfunction.h"
#include "curveviewerdisplayoptionsdialog.h"
#include<memory>
#include<QMap>

#include<baseviewer.h>


enum class PlotType{ AmplitudeOffset, AmplitudeAngle, AmplitudeSin2Angle };

static const QMap<PlotType, QString> PlotTypesAndNames{
    {PlotType::AmplitudeOffset,"Amplitude vs Offset"},
    {PlotType::AmplitudeAngle, "Amplitude vs Angle"},
    {PlotType::AmplitudeSin2Angle, "Amplitude vs sin^2(Angle)"}
};

static const QMap<PlotType, QString> PlotTypesAndXAxxisLabels{
    {PlotType::AmplitudeOffset,"Offset"},
    {PlotType::AmplitudeAngle, "Angle [Degrees]"},
    {PlotType::AmplitudeSin2Angle, "sin^2(Angle)"}
};

PlotType string2PlotType( const QString& );
QString plotType2String( PlotType );



namespace Ui {
class AmplitudeCurveViewer;
}

class AmplitudeCurveViewer : public BaseViewer
{
    Q_OBJECT

public:


    explicit AmplitudeCurveViewer(QWidget *parent = 0);
    ~AmplitudeCurveViewer();

    bool showRegressionLines()const{
        return m_showRegressionLines;
    }

    int datapointSize()const{
        return m_datapointSize;
    }

protected:

    void receivePoint( QPoint );
    void receivePoints( QVector<QPoint>, int code);
    void closeEvent(QCloseEvent*);

public slots:

    void setProject( std::shared_ptr<AVOProject>);
    void addCurve( AmplitudeCurveDefinition );

    void setShowRegressionLines(bool);
    void setDatapointSize(int);
    void setPlotTypeByName( QString );

private slots:

    void onMouseOver( QPointF);
    void sceneSelectionChanged();

    void on_action_Add_Curve_triggered();
    void runTableViewContextMenu(const QPoint&);

    void on_actionConfigure_Items_triggered();

    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionZoom_Fit_Window_triggered();

    void on_actionSelect_Plot_Type_triggered();

private:

    const qreal ZOOM_IN_FACTOR=1.1;
    const qreal ZOOM_OUT_FACTOR=0.9;
    const qreal X_PADDING_FACTOR=0.1;
    const qreal Y_PADDING_FACTOR=0.1;

    void showSelector();  // also creates it if necessary
    void changeCurveColor( int index );
    void removeCurve( int index );
    void adaptCurveToPlottype(QVector<QPointF>& curve, const double& depth);
    void updateRegressions();
    void updateScene();
    void updateTable();
    QVector<QPointF> buildCurve( AmplitudeCurveDefinition );

    void loadSettings();
    void saveSettings();

    Ui::AmplitudeCurveViewer *ui;

    QMap<int, QVector<QPointF> > m_curves;
    QMap<int, AmplitudeCurveDefinition> m_curveInfos;
    QMap<int, QColor> m_curveColors;
    QMap<int, QPointF> m_curveRegressions;
    QVector<int> m_tableRowCurveIndexMap;

    std::shared_ptr<AVOProject> m_project;
    AmplitudeCurveDataSelectionDialog* selector=nullptr;
    CurveViewerDisplayOptionsDialog* displayOptionsDialog=nullptr;
    int m_curveCounter=0;

    bool m_showRegressionLines=true;
    int m_datapointSize=11;             // make this mod 2 == 1

    PlotType m_plotType=PlotType::AmplitudeOffset;
};

#endif // AMPLITUDEOFFSETCURVEVIEWER_H
