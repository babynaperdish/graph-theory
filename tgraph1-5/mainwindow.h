#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTabWidget>
#include <QMessageBox>
#include <QSlider>

#include "randomgenerator.h"
#include "logdistribution.h"
#include "graph.h"
#include "graphanalyzer.h"
#include "weightedgraph.h"
#include "pathfinder.h"
#include "maxflow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGenerateGraphClicked();
    void onShowCharacteristicsClicked();
    void onGenerateWeightMatrixClicked();
    void onApplyShimbelClicked();
    void onFindPathsClicked();
    void onPathLengthChanged(int value);

    void onPerformDFS();
    void onPerformDijkstra();
    void onCompareAlgorithms();

    void onGenerateCapacitiesClicked();
    void onFindMaxFlowClicked();
    void onFindMinCostFlowClicked();

    void onCountSpanningTreesClicked();

    void onCheckEulerian();
    void onModifyToEulerian();

    void onSolveCutsClicked();
    void onSymDiffClicked();

    void onBuildHistogramClicked();

private:
    Ui::MainWindow *ui;

    QTabWidget* tabWidget;

    // Tab 1: Graph & Characteristics
    QWidget* graphTab;
    QSpinBox* vertexCountSpin;
    QPushButton* generateGraphBtn;
    QTextEdit* graphOutput;
    QComboBox* graphTypeCombo;
    QPushButton* showCharBtn;
    QTextEdit* charOutput;


    // Tab 2: Weight Matrix
    QWidget* weightTab;
    QComboBox* signTypeCombo;
    QPushButton* generateWeightMatrixBtn;
    QTableWidget* weightMatrixTable;

    // Tab 3: Shimbel Method
    QWidget* shimbelTab;
    QSlider* pathLengthSlider;
    QSpinBox* pathLengthSpin;
    QLabel* pathLengthLabel;
    QPushButton* applyShimbelBtn;
    QTableWidget* minMatrixTable;
    QTableWidget* maxMatrixTable;

    // Tab 4: Find Routes
    QWidget* pathTab;
    QSpinBox* startVertexSpin;
    QSpinBox* endVertexSpin;
    QPushButton* findPathsBtn;
    QLabel* pathResultLabel;

    //----------------------Lab 2-------------
    // Tab 5: Algorithms & Comparison
    QWidget* algorithmsTab;
    QSpinBox* startDFSSpin;
    QPushButton* performDFSButton;
    QSpinBox* startDijkstraSpin;
    QSpinBox* endDijkstraSpin;
    QPushButton* performDijkstraButton;
    QPushButton* compareAlgorithmsBtn;
    QTextEdit* algorithmsOutput;
//----------------------------------------
    RandomGenerator rng;
    Graph* currentGraph;
    WeightedGraph* currentWeightedGraph;
    double currentP;

    void setupUI();
    void displayMatrix(QTableWidget* table, const std::vector<std::vector<double>>& matrix);
    void showMessage(const QString& title, const QString& message);
    void updateShimbelMatrices();
    void updateAlgorithmsOutput();
    void displayIntMatrix(QTableWidget* table, const std::vector<std::vector<int>>& matrix);
//--------------------------------------------
    // Tab 6: Max Flow & Min Cost Flow
    QWidget* flowTab;
    QSpinBox* sourceVertexSpin;
    QSpinBox* sinkVertexSpin;
    QPushButton* generateCapacitiesBtn;
    QPushButton* findMaxFlowBtn;
    QPushButton* findMinCostFlowBtn;
    QTableWidget* capacityMatrixTable;
    QTableWidget* costMatrixTable;
    QTextEdit* flowOutput;
    QTableWidget* flowMatrixTable;
    QLabel* flowMatrixLabel;
    MaxFlowGraph* currentFlowGraph;
//----------------------------------------------
    // Tab 7: Spanning Trees
    QWidget* spanningTreeTab;
    QPushButton* countSpanningTreesBtn;
    QTextEdit* spanningTreeOutput;
    void onCountSpanningTrees();
    QTableWidget* kirchhoffMatrixTable;
//------------------------------------------------
    // Tab 8: Eulerian Cycle
    QWidget* eulerianTab;
    QPushButton* checkEulerianBtn;
    QPushButton* modifyToEulerianBtn;
    QTextEdit* eulerianOutput;
//-------------------------------------------
    // Tab 9: Cuts
    QWidget* cutsTab;
    QSpinBox* cut1Spin;
    QSpinBox* cut2Spin;
    QPushButton* solveCutsBtn;
    QPushButton* symDiffBtn;
    QTextEdit* cutsOutput;

//-----------------------------------------
    //Tab 10:
    QWidget* histogramTab;
    QPushButton* buildHistogramBtn;
    QTextEdit* histogramOutput;
};

#endif
