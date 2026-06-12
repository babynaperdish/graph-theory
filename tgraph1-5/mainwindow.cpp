#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graph.h"
#include "Skeleton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QSplitter>
#include <QMessageBox>
#include <QInputDialog>
#include <climits>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentGraph(nullptr)
    , currentWeightedGraph(nullptr)
    , currentP(0.7)
    , currentFlowGraph(nullptr)
{
    ui->setupUi(this);
    setupUI();

    setWindowTitle("Laboratory Work 1-5 - Graph Algorithms");
    resize(1400, 900);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (currentGraph) delete currentGraph;
    if (currentWeightedGraph) delete currentWeightedGraph;
    if (currentFlowGraph) delete currentFlowGraph;
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);

    // ==================== TAB 1: GRAPH & CHARACTERISTICS ====================
    graphTab = new QWidget();
    QVBoxLayout* graphLayout = new QVBoxLayout(graphTab);

    QSplitter* splitter = new QSplitter(Qt::Vertical);

    QWidget* topWidget = new QWidget();
    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);

    QGroupBox* inputGroup1 = new QGroupBox("Graph Generation Parameters");
    QHBoxLayout* inputLayout1 = new QHBoxLayout(inputGroup1);

    inputLayout1->addWidget(new QLabel("Number of vertices:"));
    vertexCountSpin = new QSpinBox();
    vertexCountSpin->setRange(2, 50);
    vertexCountSpin->setValue(10);
    inputLayout1->addWidget(vertexCountSpin);

    generateGraphBtn = new QPushButton("Generate Graph");
    inputLayout1->addWidget(generateGraphBtn);

    inputLayout1->addWidget(new QLabel("Graph type:"));
    graphTypeCombo = new QComboBox();
    graphTypeCombo->addItem("Undirected");
    graphTypeCombo->addItem("Directed");
    inputLayout1->addWidget(graphTypeCombo);

    topLayout->addWidget(inputGroup1);

    graphOutput = new QTextEdit();
    graphOutput->setReadOnly(true);
    graphOutput->setFontFamily("Courier New");
    graphOutput->setMaximumHeight(300);
    topLayout->addWidget(new QLabel("Graph Structure:"));
    topLayout->addWidget(graphOutput);

    splitter->addWidget(topWidget);

    QWidget* bottomWidget = new QWidget();
    QVBoxLayout* bottomLayout = new QVBoxLayout(bottomWidget);

    QHBoxLayout* charButtonLayout = new QHBoxLayout();
    showCharBtn = new QPushButton("Refresh Graph Characteristics");
    charButtonLayout->addWidget(showCharBtn);
    charButtonLayout->addStretch();
    bottomLayout->addLayout(charButtonLayout);

    charOutput = new QTextEdit();
    charOutput->setReadOnly(true);
    charOutput->setFontFamily("Courier New");
    charOutput->setMaximumHeight(250);
    bottomLayout->addWidget(new QLabel("Graph Characteristics:"));
    bottomLayout->addWidget(charOutput);

    splitter->addWidget(bottomWidget);

    graphLayout->addWidget(splitter);
    tabWidget->addTab(graphTab, "1. Graph & Characteristics");

    // ==================== TAB 2: WEIGHT MATRIX ====================
    weightTab = new QWidget();
    QVBoxLayout* weightLayout = new QVBoxLayout(weightTab);

    QGroupBox* weightGroup = new QGroupBox("Weight Matrix Generation");
    QVBoxLayout* weightGroupLayout = new QVBoxLayout(weightGroup);

    QHBoxLayout* signLayout = new QHBoxLayout();
    signLayout->addWidget(new QLabel("Sign type:"));
    signTypeCombo = new QComboBox();
    signTypeCombo->addItem("Only Positive");
    signTypeCombo->addItem("Only Negative");
    signTypeCombo->addItem("Mixed");
    signLayout->addWidget(signTypeCombo);
    signLayout->addStretch();
    weightGroupLayout->addLayout(signLayout);

    generateWeightMatrixBtn = new QPushButton("Generate Weight Matrix");
    weightGroupLayout->addWidget(generateWeightMatrixBtn);

    weightLayout->addWidget(weightGroup);

    weightMatrixTable = new QTableWidget();
    weightMatrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    weightLayout->addWidget(new QLabel("Edge Weights Matrix:"));
    weightLayout->addWidget(weightMatrixTable);

    tabWidget->addTab(weightTab, "2. Weight Matrix");

    // ==================== TAB 3: SHIMBEL METHOD ====================
    shimbelTab = new QWidget();
    QVBoxLayout* shimbelLayout = new QVBoxLayout(shimbelTab);

    QGroupBox* shimbelGroup = new QGroupBox("Shimbel Method Parameters");
    QVBoxLayout* shimbelGroupLayout = new QVBoxLayout(shimbelGroup);

    QHBoxLayout* lengthLayout = new QHBoxLayout();
    lengthLayout->addWidget(new QLabel("Path length (number of edges):"));
    pathLengthSpin = new QSpinBox();
    pathLengthSpin->setRange(0, 0);
    pathLengthSpin->setValue(1);
    pathLengthSpin->setToolTip("0 - only diagonal\n1 - paths of 1 edge (same as weight matrix)\n"
                               "Maximum length is limited to (vertices - 1)");
    lengthLayout->addWidget(pathLengthSpin);

    pathLengthSlider = new QSlider(Qt::Horizontal);
    pathLengthSlider->setRange(0, 0);
    pathLengthSlider->setValue(1);
    lengthLayout->addWidget(pathLengthSlider);

    pathLengthLabel = new QLabel("Showing paths of length: 1 edge");
    lengthLayout->addWidget(pathLengthLabel);
    lengthLayout->addStretch();
    shimbelGroupLayout->addLayout(lengthLayout);

    applyShimbelBtn = new QPushButton("Apply Shimbel Method");
    shimbelGroupLayout->addWidget(applyShimbelBtn);

    shimbelLayout->addWidget(shimbelGroup);

    QHBoxLayout* shimbelTablesLayout = new QHBoxLayout();

    QVBoxLayout* minTableLayout = new QVBoxLayout();
    minTableLayout->addWidget(new QLabel("Minimum Paths Matrix:"));
    minMatrixTable = new QTableWidget();
    minMatrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    minTableLayout->addWidget(minMatrixTable);
    shimbelTablesLayout->addLayout(minTableLayout);

    QVBoxLayout* maxTableLayout = new QVBoxLayout();
    maxTableLayout->addWidget(new QLabel("Maximum Paths Matrix:"));
    maxMatrixTable = new QTableWidget();
    maxMatrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    maxTableLayout->addWidget(maxMatrixTable);
    shimbelTablesLayout->addLayout(maxTableLayout);

    shimbelLayout->addLayout(shimbelTablesLayout);

    tabWidget->addTab(shimbelTab, "3. Shimbel Method");

    // ==================== TAB 4: FIND ROUTES ====================
    pathTab = new QWidget();
    QVBoxLayout* pathLayout = new QVBoxLayout(pathTab);

    QGroupBox* inputGroup3 = new QGroupBox("Route Search Parameters");
    QHBoxLayout* inputLayout3 = new QHBoxLayout(inputGroup3);

    inputLayout3->addWidget(new QLabel("Start vertex:"));
    startVertexSpin = new QSpinBox();
    startVertexSpin->setRange(0, 49);
    startVertexSpin->setValue(0);
    inputLayout3->addWidget(startVertexSpin);

    inputLayout3->addWidget(new QLabel("End vertex:"));
    endVertexSpin = new QSpinBox();
    endVertexSpin->setRange(0, 49);
    endVertexSpin->setValue(5);
    inputLayout3->addWidget(endVertexSpin);

    findPathsBtn = new QPushButton("Find Routes");
    inputLayout3->addWidget(findPathsBtn);

    pathLayout->addWidget(inputGroup3);

    pathResultLabel = new QLabel();
    pathResultLabel->setAlignment(Qt::AlignLeft);
    pathResultLabel->setWordWrap(true);
    pathResultLabel->setStyleSheet("QLabel { font-size: 10pt; font-family: monospace; }");
    pathLayout->addWidget(pathResultLabel);

    tabWidget->addTab(pathTab, "4. Find Routes");

    // ==================== TAB 5: ALGORITHMS & COMPARISON ====================
    algorithmsTab = new QWidget();
    QVBoxLayout* algorithmsLayout = new QVBoxLayout(algorithmsTab);

    QGroupBox* dfsGroup = new QGroupBox("Depth-First Search (DFS) Traversal");
    QVBoxLayout* dfsLayout = new QVBoxLayout(dfsGroup);

    QHBoxLayout* dfsControlLayout = new QHBoxLayout();
    dfsControlLayout->addWidget(new QLabel("Start vertex:"));
    startDFSSpin = new QSpinBox();
    startDFSSpin->setRange(0, 49);
    startDFSSpin->setValue(0);
    dfsControlLayout->addWidget(startDFSSpin);

    performDFSButton = new QPushButton("Perform DFS Traversal");
    dfsControlLayout->addWidget(performDFSButton);
    dfsControlLayout->addStretch();
    dfsLayout->addLayout(dfsControlLayout);

    algorithmsLayout->addWidget(dfsGroup);

    QGroupBox* dijkstraGroup = new QGroupBox("Dijkstra Algorithm - Shortest Path");
    QVBoxLayout* dijkstraLayout = new QVBoxLayout(dijkstraGroup);

    QHBoxLayout* dijkstraControlLayout = new QHBoxLayout();
    dijkstraControlLayout->addWidget(new QLabel("Start vertex:"));
    startDijkstraSpin = new QSpinBox();
    startDijkstraSpin->setRange(0, 49);
    startDijkstraSpin->setValue(0);
    dijkstraControlLayout->addWidget(startDijkstraSpin);

    dijkstraControlLayout->addWidget(new QLabel("End vertex:"));
    endDijkstraSpin = new QSpinBox();
    endDijkstraSpin->setRange(0, 49);
    endDijkstraSpin->setValue(5);
    dijkstraControlLayout->addWidget(endDijkstraSpin);

    performDijkstraButton = new QPushButton("Find Shortest Path");
    dijkstraControlLayout->addWidget(performDijkstraButton);
    dijkstraControlLayout->addStretch();
    dijkstraLayout->addLayout(dijkstraControlLayout);

    algorithmsLayout->addWidget(dijkstraGroup);

    QGroupBox* comparisonGroup = new QGroupBox("Algorithm Performance Comparison");
    QVBoxLayout* comparisonLayout = new QVBoxLayout(comparisonGroup);

    compareAlgorithmsBtn = new QPushButton("Compare Algorithms");
    comparisonLayout->addWidget(compareAlgorithmsBtn);

    algorithmsLayout->addWidget(comparisonGroup);

    algorithmsOutput = new QTextEdit();
    algorithmsOutput->setReadOnly(true);
    algorithmsOutput->setFontFamily("Courier New");
    algorithmsOutput->setMinimumHeight(400);
    algorithmsLayout->addWidget(algorithmsOutput);

    tabWidget->addTab(algorithmsTab, "5. Algorithms & Comparison");

    // ==================== TAB 6: MAX FLOW & MIN COST FLOW ====================
    flowTab = new QWidget();
    QVBoxLayout* flowLayout = new QVBoxLayout(flowTab);

    // Группа для генерации данных
    QGroupBox* flowGenerationGroup = new QGroupBox("Flow Network Generation");
    QVBoxLayout* flowGenLayout = new QVBoxLayout(flowGenerationGroup);

    QHBoxLayout* flowSourceLayout = new QHBoxLayout();
    flowSourceLayout->addWidget(new QLabel("Source vertex:"));
    sourceVertexSpin = new QSpinBox();
    sourceVertexSpin->setRange(0, 49);
    sourceVertexSpin->setValue(0);
    flowSourceLayout->addWidget(sourceVertexSpin);

    flowSourceLayout->addWidget(new QLabel("Sink vertex:"));
    sinkVertexSpin = new QSpinBox();
    sinkVertexSpin->setRange(0, 49);
    sinkVertexSpin->setValue(5);
    flowSourceLayout->addWidget(sinkVertexSpin);
    flowSourceLayout->addStretch();
    flowGenLayout->addLayout(flowSourceLayout);

    generateCapacitiesBtn = new QPushButton("Generate Capacities and Costs");
    flowGenLayout->addWidget(generateCapacitiesBtn);

    flowLayout->addWidget(flowGenerationGroup);

    // Группа для алгоритмов
    QGroupBox* flowAlgorithmsGroup = new QGroupBox("Flow Algorithms");
    QHBoxLayout* flowAlgoLayout = new QHBoxLayout(flowAlgorithmsGroup);

    findMaxFlowBtn = new QPushButton("Find Maximum Flow (Ford-Fulkerson)");
    findMinCostFlowBtn = new QPushButton("Find Minimum Cost Flow (2/3 of max)");
    flowAlgoLayout->addWidget(findMaxFlowBtn);
    flowAlgoLayout->addWidget(findMinCostFlowBtn);
    flowAlgoLayout->addStretch();

    flowLayout->addWidget(flowAlgorithmsGroup);

    // Таблицы для матриц
    QHBoxLayout* matricesLayout = new QHBoxLayout();

    QVBoxLayout* capLayout = new QVBoxLayout();
    capLayout->addWidget(new QLabel("Capacity Matrix:"));
    capacityMatrixTable = new QTableWidget();
    capacityMatrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    capLayout->addWidget(capacityMatrixTable);
    matricesLayout->addLayout(capLayout);

    QVBoxLayout* costLayout = new QVBoxLayout();
    costLayout->addWidget(new QLabel("Cost Matrix:"));
    costMatrixTable = new QTableWidget();
    costMatrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    costLayout->addWidget(costMatrixTable);
    matricesLayout->addLayout(costLayout);

    QVBoxLayout* flowLayout2 = new QVBoxLayout();
    flowMatrixLabel = new QLabel("Flow Matrix:");
    flowMatrixLabel->hide();  // изначально скрыта
    flowLayout2->addWidget(flowMatrixLabel);
    flowMatrixTable = new QTableWidget();
    flowMatrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    flowMatrixTable->hide();  // изначально скрыта
    flowLayout2->addWidget(flowMatrixTable);
    matricesLayout->addLayout(flowLayout2);

    flowLayout->addLayout(matricesLayout);

    // Вывод результатов
    flowOutput = new QTextEdit();
    flowOutput->setReadOnly(true);
    flowOutput->setFontFamily("Courier New");
    flowOutput->setMinimumHeight(300);
    flowLayout->addWidget(new QLabel("Algorithm Results:"));
    flowLayout->addWidget(flowOutput);

    tabWidget->addTab(flowTab, "6. Max Flow & Min Cost Flow");

    // ==================== TAB 7: SPANNING TREES ====================
    spanningTreeTab = new QWidget();
    QVBoxLayout* spanningTreeLayout = new QVBoxLayout(spanningTreeTab);

    QGroupBox* spanningGroup = new QGroupBox("Spanning Tree Count (Kirchhoff's Theorem)");
    QVBoxLayout* spanningGroupLayout = new QVBoxLayout(spanningGroup);

    QLabel* spanningInfo = new QLabel(
        "Count the number of spanning trees in the generated undirected graph\n"
        "using the Kirchhoff matrix tree theorem: τ(G) = det(L')"
    );
    spanningInfo->setWordWrap(true);
    spanningGroupLayout->addWidget(spanningInfo);

    countSpanningTreesBtn = new QPushButton("Count Spanning Trees");
    spanningGroupLayout->addWidget(countSpanningTreesBtn);

    spanningTreeLayout->addWidget(spanningGroup);

    // Таблица для матрицы Кирхгофа
    spanningTreeLayout->addWidget(new QLabel("Kirchhoff Matrix L = D - A:"));
    kirchhoffMatrixTable = new QTableWidget();
    kirchhoffMatrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    kirchhoffMatrixTable->setMinimumHeight(200);
    spanningTreeLayout->addWidget(kirchhoffMatrixTable);

    // Вывод результатов
    spanningTreeOutput = new QTextEdit();
    spanningTreeOutput->setReadOnly(true);
    spanningTreeOutput->setFontFamily("Courier New");
    spanningTreeOutput->setMinimumHeight(250);
    spanningTreeLayout->addWidget(new QLabel("Result:"));
    spanningTreeLayout->addWidget(spanningTreeOutput);

    tabWidget->addTab(spanningTreeTab, "7. Spanning Trees");

    // ==================== TAB 8: EULERIAN CYCLE ====================
    eulerianTab = new QWidget();
    QVBoxLayout* eulerianLayout = new QVBoxLayout(eulerianTab);

    QGroupBox* eulerianGroup = new QGroupBox("Eulerian Cycle Analysis");
    QVBoxLayout* eulerianGroupLayout = new QVBoxLayout(eulerianGroup);

    QLabel* eulerianInfo = new QLabel(
        "Check if the graph is Eulerian and find the Eulerian cycle.\n"
        "Conditions: All vertex degrees must be even and graph connected."
    );
    eulerianInfo->setWordWrap(true);
    eulerianGroupLayout->addWidget(eulerianInfo);

    QHBoxLayout* eulerianBtnLayout = new QHBoxLayout();
    checkEulerianBtn = new QPushButton("Check & Find Eulerian Cycle");
    modifyToEulerianBtn = new QPushButton("Modify Graph to Eulerian");
    eulerianBtnLayout->addWidget(checkEulerianBtn);
    eulerianBtnLayout->addWidget(modifyToEulerianBtn);
    eulerianGroupLayout->addLayout(eulerianBtnLayout);

    eulerianLayout->addWidget(eulerianGroup);

    eulerianOutput = new QTextEdit();
    eulerianOutput->setReadOnly(true);
    eulerianOutput->setFontFamily("Courier New");
    eulerianOutput->setMinimumHeight(500);
    eulerianLayout->addWidget(new QLabel("Result:"));
    eulerianLayout->addWidget(eulerianOutput);

    tabWidget->addTab(eulerianTab, "8. Eulerian Cycle");

    // ==================== TAB 9: CUTS ====================
    cutsTab = new QWidget();
    QVBoxLayout* cutsLayout = new QVBoxLayout(cutsTab);

    QGroupBox* cutsGroup = new QGroupBox("Fundamental Cuts & Symmetric Difference");
    QVBoxLayout* cutsGroupLayout = new QVBoxLayout(cutsGroup);

    QLabel* cutsInfo = new QLabel(
        "Build fundamental cut system from MST.\n"
        "Perform symmetric difference of selected cuts."
    );
    cutsInfo->setWordWrap(true);
    cutsGroupLayout->addWidget(cutsInfo);

    QHBoxLayout* cutsSelectLayout = new QHBoxLayout();
    cutsSelectLayout->addWidget(new QLabel("Cut 1:"));
    cut1Spin = new QSpinBox();
    cut1Spin->setRange(1, 1);
    cut1Spin->setValue(1);
    cutsSelectLayout->addWidget(cut1Spin);

    cutsSelectLayout->addWidget(new QLabel("Cut 2:"));
    cut2Spin = new QSpinBox();
    cut2Spin->setRange(1, 1);
    cut2Spin->setValue(2);
    cutsSelectLayout->addWidget(cut2Spin);
    cutsSelectLayout->addStretch();
    cutsGroupLayout->addLayout(cutsSelectLayout);

    QHBoxLayout* cutsBtnLayout = new QHBoxLayout();
    solveCutsBtn = new QPushButton("Show Fundamental Cuts");
    symDiffBtn = new QPushButton("Symmetric Difference");
    cutsBtnLayout->addWidget(solveCutsBtn);
    cutsBtnLayout->addWidget(symDiffBtn);
    cutsGroupLayout->addLayout(cutsBtnLayout);

    cutsLayout->addWidget(cutsGroup);

    cutsOutput = new QTextEdit();
    cutsOutput->setReadOnly(true);
    cutsOutput->setFontFamily("Courier New");
    cutsOutput->setMinimumHeight(400);
    cutsLayout->addWidget(new QLabel("Result:"));
    cutsLayout->addWidget(cutsOutput);

    tabWidget->addTab(cutsTab, "9. Cuts");

    // ==================== TAB 10: DEGREE HISTOGRAM ====================
    histogramTab = new QWidget();
    QVBoxLayout* histogramLayout = new QVBoxLayout(histogramTab);

    QGroupBox* histogramGroup = new QGroupBox("Degree Distribution Histogram");
    QVBoxLayout* histogramGroupLayout = new QVBoxLayout(histogramGroup);

    QLabel* histogramInfo = new QLabel(
        "Generate 1000 numbers each\n"
        "and build a histogram of distribution.\n\n"
        "X-axis: vertex degree\n"
        "Y-axis: number of vertices with this degree"
    );
    histogramInfo->setWordWrap(true);
    histogramGroupLayout->addWidget(histogramInfo);

    buildHistogramBtn = new QPushButton("1000 random values");
    histogramGroupLayout->addWidget(buildHistogramBtn);

    histogramLayout->addWidget(histogramGroup);

    histogramOutput = new QTextEdit();
    histogramOutput->setReadOnly(true);
    histogramOutput->setFontFamily("Courier New");
    histogramOutput->setMinimumHeight(500);
    histogramLayout->addWidget(new QLabel("Result:"));
    histogramLayout->addWidget(histogramOutput);

    tabWidget->addTab(histogramTab, "10. Histogram");

    connect(buildHistogramBtn, &QPushButton::clicked, this, &MainWindow::onBuildHistogramClicked);


    // ==================== CONNECTIONS ====================
    connect(generateGraphBtn, &QPushButton::clicked, this, &MainWindow::onGenerateGraphClicked);
    connect(showCharBtn, &QPushButton::clicked, this, &MainWindow::onShowCharacteristicsClicked);
    connect(generateWeightMatrixBtn, &QPushButton::clicked, this, &MainWindow::onGenerateWeightMatrixClicked);
    connect(applyShimbelBtn, &QPushButton::clicked, this, &MainWindow::onApplyShimbelClicked);
    connect(findPathsBtn, &QPushButton::clicked, this, &MainWindow::onFindPathsClicked);
    connect(pathLengthSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onPathLengthChanged);
    connect(pathLengthSlider, &QSlider::valueChanged, this, &MainWindow::onPathLengthChanged);
    connect(performDFSButton, &QPushButton::clicked, this, &MainWindow::onPerformDFS);
    connect(performDijkstraButton, &QPushButton::clicked, this, &MainWindow::onPerformDijkstra);
    connect(compareAlgorithmsBtn, &QPushButton::clicked, this, &MainWindow::onCompareAlgorithms);
    connect(generateCapacitiesBtn, &QPushButton::clicked, this, &MainWindow::onGenerateCapacitiesClicked);
    connect(findMaxFlowBtn, &QPushButton::clicked, this, &MainWindow::onFindMaxFlowClicked);
    connect(findMinCostFlowBtn, &QPushButton::clicked, this, &MainWindow::onFindMinCostFlowClicked);
    connect(countSpanningTreesBtn, &QPushButton::clicked, this, &MainWindow::onCountSpanningTreesClicked);
    connect(checkEulerianBtn, &QPushButton::clicked, this, &MainWindow::onCheckEulerian);
    connect(modifyToEulerianBtn, &QPushButton::clicked, this, &MainWindow::onModifyToEulerian);
    connect(solveCutsBtn, &QPushButton::clicked, this, &MainWindow::onSolveCutsClicked);
    connect(symDiffBtn, &QPushButton::clicked, this, &MainWindow::onSymDiffClicked);
}

void MainWindow::displayMatrix(QTableWidget* table, const std::vector<std::vector<double>>& matrix)
{
    if (matrix.empty()) return;

    int n = matrix.size();
    table->setRowCount(n);
    table->setColumnCount(n);

    QStringList headers;
    for (int i = 0; i < n; i++) {
        headers << QString::number(i);
    }
    table->setVerticalHeaderLabels(headers);
    table->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QString value;
            if (matrix[i][j] >= WeightedGraph::getInfinity() - 1)
                value = "—";
            else if (matrix[i][j] <= -WeightedGraph::getInfinity() + 1)
                value = "—";
            else {
                value = QString::number(static_cast<int>(matrix[i][j]));
            }

            QTableWidgetItem* item = new QTableWidgetItem(value);
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, j, item);
        }
    }

    table->resizeColumnsToContents();
}

void MainWindow::updateShimbelMatrices()
{
    if (!currentWeightedGraph) return;

    int length = pathLengthSpin->value();
    int n = currentWeightedGraph->getWeightMatrix().size();

    std::vector<std::vector<double>> minPaths;
    std::vector<std::vector<double>> maxPaths;

    if (length == 0) {
        minPaths = std::vector<std::vector<double>>(n, std::vector<double>(n, WeightedGraph::getInfinity()));
        maxPaths = std::vector<std::vector<double>>(n, std::vector<double>(n, -WeightedGraph::getInfinity()));
        for (int i = 0; i < n; i++) {
            minPaths[i][i] = 0;
            maxPaths[i][i] = 0;
        }
    } else if (length == 1) {
        minPaths = currentWeightedGraph->getWeightMatrix();
        maxPaths = currentWeightedGraph->getWeightMatrix();
    } else {
        minPaths = currentWeightedGraph->findSimpleMinPaths(length);
        maxPaths = currentWeightedGraph->findSimpleMaxPaths(length);
    }

    displayMatrix(minMatrixTable, minPaths);
    displayMatrix(maxMatrixTable, maxPaths);
}

void MainWindow::showMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}

void MainWindow::onGenerateGraphClicked()
{
    int n = vertexCountSpin->value();
    Graph::GraphType graphType = (graphTypeCombo->currentIndex() == 0)
                                  ? Graph::UNDIRECTED : Graph::DIRECTED;

    if (currentGraph != nullptr)
    {
        delete currentGraph;
        currentGraph = nullptr;
    }

    if (currentWeightedGraph != nullptr)
    {
        delete currentWeightedGraph;
        currentWeightedGraph = nullptr;
    }

    currentGraph = new Graph(n, graphType);

    QString output;

    if (graphType == Graph::UNDIRECTED) {
        std::vector<int> desiredDegrees;
        LogDistribution logDist(currentP, rng);

        for (int i = 0; i < n; i++)
        {
            int deg = logDist.generate();
            if (deg < 1) deg = 1;
            if (deg > n - 1) deg = n - 1;
            desiredDegrees.push_back(deg);
        }

        currentGraph->buildGraphFromDesiredDegrees(desiredDegrees);

        std::vector<int> actualDegrees(n, 0);
        for (int i = 0; i < n; i++) {
            actualDegrees[i] = currentGraph->getNeighbors(i).size();
        }

        int actualSum = 0;
        for (int d : actualDegrees) actualSum += d;
        int edgeCount = actualSum / 2;

        output += "=== DEGREES ===\n";
        for (int d : actualDegrees) output += QString::number(d) + " ";
        output += "\nSum of degrees: " + QString::number(actualSum)+ "\n";
        output += "Number of edges: " + QString::number(edgeCount) + "\n\n";
    } else {
        std::vector<int> desiredOutDegrees;
        std::vector<int> desiredInDegrees;

        LogDistribution logDist(currentP, rng);

        for (int i = 0; i < n; i++)
        {
            int outDeg = logDist.generate();
            int inDeg = logDist.generate();
            desiredOutDegrees.push_back(outDeg);
            desiredInDegrees.push_back(inDeg);
        }

        currentGraph->buildDirectedGraphFromDesiredDegrees(desiredOutDegrees, desiredInDegrees);

        output += "=== DEGREES ===\n";
        output += "Out-degrees: ";
        for (int i = 0; i < n; i++) {
            output += QString::number(currentGraph->getOutDegree(i)) + " ";
        }
        output += "\nIn-degrees:  ";
        for (int i = 0; i < n; i++) {
            output += QString::number(currentGraph->getInDegree(i)) + " ";
        }
        output += "\n\n";
    }

    output += "=== GRAPH STRUCTURE ===\n";
    output += "Adjacency list:\n";
    for (int i = 0; i < n; i++)
    {
        output += QString("  %1: ").arg(i);
        for (int v : currentGraph->getNeighbors(i))
        {
            output += QString::number(v) + " ";
        }
        output += "\n";
    }

    int edgeCount = 0;
    for (int i = 0; i < n; i++) {
        edgeCount += currentGraph->getNeighbors(i).size();
    }
    if (graphType == Graph::UNDIRECTED) edgeCount /= 2;

    output += "\n=== VALIDATION ===\n";
    output += "Number of edges: " + QString::number(edgeCount) + "\n";

    startVertexSpin->setRange(0, n-1);
    endVertexSpin->setRange(0, n-1);
    startDFSSpin->setRange(0, n-1);
    startDijkstraSpin->setRange(0, n-1);
    endDijkstraSpin->setRange(0, n-1);

    graphOutput->setText(output);

    if (currentWeightedGraph) {
        delete currentWeightedGraph;
        currentWeightedGraph = nullptr;
    }

    weightMatrixTable->setRowCount(0);
    minMatrixTable->setRowCount(0);
    maxMatrixTable->setRowCount(0);

    int maxPathLength = n - 1;
    pathLengthSpin->setRange(0, maxPathLength);
    pathLengthSlider->setRange(0, maxPathLength);
    pathLengthSpin->setToolTip(QString("0 - only diagonal\n1 - paths of 1 edge (same as weight matrix)\n"
                                       "Maximum length: %1 edges (vertices - 1)").arg(maxPathLength));

    onShowCharacteristicsClicked();
    updateAlgorithmsOutput();
}

void MainWindow::onShowCharacteristicsClicked()
{
    if (!currentGraph)
    {
        showMessage("Error", "First generate a graph!");
        return;
    }

    GraphAnalyzer analyzer(*currentGraph);

    QString output;

    auto ecc = analyzer.computeEccentricities();
    output += "Vertex eccentricities:\n";
    for (size_t i = 0; i < ecc.size(); i++)
    {
        if (ecc[i] < 0)
            output += QString("  e(%1) = — (unreachable)\n").arg(i);
        else
            output += QString("  e(%1) = %2\n").arg(i).arg(ecc[i]);
    }

    auto [radius, center] = analyzer.findCenter();
    output += "\nGraph radius: " + QString::number(radius) + "\n";
    output += "Center vertices: ";
    for (int v : center) output += QString::number(v) + " ";
    output += "\n";

    auto [diameter, diamVertices] = analyzer.findDiameter();
    output += "\nGraph diameter: " + QString::number(diameter) + "\n";
    output += "Diametral vertices: ";
    for (int v : diamVertices) output += QString::number(v) + " ";
    output += "\n";

    std::vector<int> diamPath = analyzer.findDiameterPath();
    if (!diamPath.empty()) {
       output += "Diameter path: ";
        for (size_t i = 0; i < diamPath.size(); ++i) {
            output += QString::number(diamPath[i]);
            if (i < diamPath.size() - 1)
                output += " → ";
    }
    output += "\n";
}

    charOutput->setText(output);
}

void MainWindow::onGenerateWeightMatrixClicked()
{
    if (!currentGraph)
    {
        showMessage("Error", "First generate a graph!");
        return;
    }

    int n = currentGraph->getVerticesCount();

    WeightedGraph::SignType signType;
    switch (signTypeCombo->currentIndex())
    {
        case 0: signType = WeightedGraph::POSITIVE; break;
        case 1: signType = WeightedGraph::NEGATIVE; break;
        case 2: signType = WeightedGraph::MIXED; break;
        default: signType = WeightedGraph::POSITIVE;
    }

    if (currentWeightedGraph) {
        delete currentWeightedGraph;
    }

    LogDistribution logDist(0.85, rng);
    currentWeightedGraph = new WeightedGraph(n);

    currentWeightedGraph->generateWeightsFromGraph(*currentGraph, logDist, signType, rng);

    auto weightMatrix = currentWeightedGraph->getWeightMatrix();
    displayMatrix(weightMatrixTable, weightMatrix);

    int maxLength = n - 1;
    pathLengthSpin->setRange(0, maxLength);
    pathLengthSlider->setRange(0, maxLength);

    updateShimbelMatrices();
    updateAlgorithmsOutput();
}

void MainWindow::onApplyShimbelClicked()
{
    if (!currentWeightedGraph)
    {
        showMessage("Error", "First generate a weight matrix in tab 2!");
        return;
    }

    updateShimbelMatrices();
}

void MainWindow::onFindPathsClicked()
{
    if (!currentGraph)
    {
        showMessage("Error", "First generate a graph!");
        return;
    }

    int start = startVertexSpin->value();
    int end = endVertexSpin->value();

    PathFinder pf(*currentGraph);

    QString result = QString::fromStdString(pf.formatPaths(start, end));
    pathResultLabel->setText(result);
}

void MainWindow::onPerformDFS()
{
    if (!currentGraph)
    {
        showMessage("Error", "First generate a graph!");
        return;
    }

    int start = startDFSSpin->value();
    std::vector<int> traversal;
    if (currentGraph->getType() == Graph::UNDIRECTED) traversal = currentGraph->performDFS(start);
    else traversal = currentGraph->performDFSDirected(start);
    int iterations = currentGraph->getLastDFSIterations();

    QString output;
    output += "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    output += "║                    DEPTH-FIRST SEARCH TRAVERSAL                               ║\n";
    output += "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
    output += "Starting vertex: " + QString::number(start) + "\n";
    output += "Graph type: " + QString(graphTypeCombo->currentIndex() == 0 ? "Undirected" : "Directed") + "\n";
    output += "Total vertices: " + QString::number(currentGraph->getVerticesCount()) + "\n\n";
    output += "Traversal order:\n";
    output += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

    for (size_t i = 0; i < traversal.size(); i++) {
        output += QString::number(traversal[i]);
        if (i < traversal.size() - 1) output += " → ";
        if ((i + 1) % 15 == 0) output += "\n";
    }
    output += "\n\n";
    output += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    output += "Statistics:\n";
    output += "  • Vertices visited: " + QString::number(traversal.size()) + "\n";
    output += "  • Algorithm iterations: " + QString::number(iterations) + "\n";
    output += "  • Complexity: O(V + E)\n";

    algorithmsOutput->setText(output);
}

void MainWindow::onPerformDijkstra()
{
    if (!currentWeightedGraph)
    {
        QMessageBox::critical(this, "Error",
            "Weighted graph not generated!\n\n"
            "Please go to tab 2 and generate weight matrix first.");
        return;
    }

    // проверка весов
    bool hasNegative = false;
    auto& matrix = currentWeightedGraph->getWeightMatrix();
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            if (i != j && matrix[i][j] < -1e-9 && matrix[i][j] < WeightedGraph::getInfinity() - 1) {
                hasNegative = true;
                break;
            }
        }
    }

    if (hasNegative) {
        QMessageBox::critical(this, "Algorithm Error",
            "❌ DIJKSTRA ALGORITHM CANNOT WORK WITH NEGATIVE WEIGHTS!\n\n"
            "The weight matrix contains negative values.\n"
            "Dijkstra's algorithm only works with non-negative weights.\n\n"
            "Generating positive weights.\n\n");

        // автоматически генерируем положительные веса и повторяем попытку
        signTypeCombo->setCurrentIndex(0); // Only Positive
        onGenerateWeightMatrixClicked();
        onPerformDijkstra();
        return;
    }

    int start = startDijkstraSpin->value();
    int end = endDijkstraSpin->value();

    std::string result = currentWeightedGraph->formatDijkstraPath(start, end);
    algorithmsOutput->setText(QString::fromStdString(result));
}

void MainWindow::onCompareAlgorithms()
{
    if (!currentGraph || !currentWeightedGraph)
    {
        showMessage("Error", "First generate a graph and weight matrix!");
        return;
    }

    updateAlgorithmsOutput();
}

void MainWindow::updateAlgorithmsOutput()
{
    if (!currentGraph || !currentWeightedGraph) return;

    int n = currentGraph->getVerticesCount();
    int e = currentWeightedGraph->getEdgeCount();

    // проверка весов
    bool hasNegativeWeights = false;
    auto& matrix = currentWeightedGraph->getWeightMatrix();
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            if (i != j && matrix[i][j] < -1e-9 && matrix[i][j] < WeightedGraph::getInfinity() - 1) {
                hasNegativeWeights = true;
                break;
            }
        }
    }

    QString output;
    output += "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    output += "║                    ALGORITHM PERFORMANCE COMPARISON                          ║\n";
    output += "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";

    output += "Graph parameters:\n";
    output += "  • Vertices: " + QString::number(n) + "\n";
    output += "  • Type: " + QString(graphTypeCombo->currentIndex() == 0 ? "Undirected" : "Directed") + "\n";
    output += "  • Edges: " + QString::number(e) + "\n\n";

    output += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    output += "ALGORITHM COMPARISON (by number of iterations)\n";
    output += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    output += "1. DEPTH-FIRST SEARCH (DFS) TRAVERSAL\n";
    output += "   ┌─────────────────────────────────────────────────────────────────────┐\n";
    currentGraph->performDFS(0);
    int dfsIter = currentGraph->getLastDFSIterations() / 2;
    output += "   │ Iterations: " + QString::number(dfsIter) + "\n";
    output += "   │ Theoretical complexity: O(V + E)\n";
    output += "   │ For current graph: " + QString::number(n) + " + " + QString::number(e) + " = " + QString::number(n + e) + " operations\n";
    output += "   └─────────────────────────────────────────────────────────────────────┘\n\n";

    output += "2. DIJKSTRA ALGORITHM (shortest path from 0 to " + QString::number(n-1) + ")\n";
    output += "   ┌─────────────────────────────────────────────────────────────────────┐\n";

    int dijkstraIter = 0;
    if (hasNegativeWeights) {

        output += "   │ ERROR: Negative weights detected!                                   │\n";
        output += "   │                                                                     │\n";
        output += "   │ Dijkstra algorithm does NOT work with negative weights!             │\n";
        output += "   │                                                                     │\n";
        output += "   │ Status: NOT EXECUTED                                                │\n";

        int dijkstraIter = 0;
        output += "   │ Iterations: N/A (error)\n";
        output += "   │ Theoretical complexity: O(V²) = " + QString::number(n) + "² = " + QString::number(n*n) + " operations\n";
        output += "   └─────────────────────────────────────────────────────────────────────┘\n\n";
    } else {
        currentWeightedGraph->dijkstra(0, n-1);
        int dijkstraIter = currentWeightedGraph->getLastDijkstraIterations();
        output += "   │ Iterations: " + QString::number(dijkstraIter) + "\n";
        output += "   │ Theoretical complexity: O(V²)\n";
        output += "   │ For current graph: " + QString::number(n) + "² = " + QString::number(n*n) + " operations\n";
        output += "   └─────────────────────────────────────────────────────────────────────┘\n\n";
    }

    output += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    output += "SUMMARY TABLE\n";
    output += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    output += QString("%1 | %2 | %3\n")
              .arg("Algorithm", 35)
              .arg("Iterations", 12)
              .arg("Theoretical Complexity", 25);
    output += QString("%1-+-%2-+-%3\n")
              .arg(QString(35, '-'))
              .arg(QString(12, '-'))
              .arg(QString(25, '-'));

    output += QString("%1 | %2 | %3\n")
              .arg("DFS Traversal", 35)
              .arg(dfsIter, 12)
              .arg("O(V + E) = " + QString::number(n) + " + " + QString::number(e), 25);

    if (hasNegativeWeights) {
        output += QString("%1 | %2 | %3\n")
                  .arg("Dijkstra (ERROR)", 35)
                  .arg("N/A", 12)
                  .arg("Not executed - negative weights", 25);
    } else {
        output += QString("%1 | %2 | %3\n")
                  .arg("Dijkstra", 35)
                  .arg(dijkstraIter, 12)
                  .arg("O(V²) = " + QString::number(n) + "² = " + QString::number(n*n), 25);
    }

    output += "\n";
    output += "Legend:\n";
    output += "  V - number of vertices (" + QString::number(n) + ")\n";
    output += "  E - number of edges (" + QString::number(e) + ")\n";

    algorithmsOutput->setText(output);
}

//------------------------------lab3-----------------------------------------------------
void MainWindow::onGenerateCapacitiesClicked()
{
    if (!currentGraph) {
        showMessage("Error", "First generate a graph in tab 1!");
        return;
    }

    int n = currentGraph->getVerticesCount();

    if (currentFlowGraph) {
        delete currentFlowGraph;
    }

    LogDistribution logDist(0.85, rng);
    currentFlowGraph = new MaxFlowGraph(n);
    currentFlowGraph->generateCapacitiesAndCosts(*currentGraph, logDist, rng);

    // Отображаем матрицы
    int size = n;
    capacityMatrixTable->setRowCount(size);
    capacityMatrixTable->setColumnCount(size);
    costMatrixTable->setRowCount(size);
    costMatrixTable->setColumnCount(size);

    QStringList headers;
    for (int i = 0; i < size; i++) {
        headers << QString::number(i);
    }
    capacityMatrixTable->setVerticalHeaderLabels(headers);
    capacityMatrixTable->setHorizontalHeaderLabels(headers);
    costMatrixTable->setVerticalHeaderLabels(headers);
    costMatrixTable->setHorizontalHeaderLabels(headers);

    auto& capMatrix = currentFlowGraph->getCapacityMatrix();
    auto& costMatrix = currentFlowGraph->getCostMatrix();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            QTableWidgetItem* capItem = new QTableWidgetItem(QString::number(capMatrix[i][j]));
            capItem->setTextAlignment(Qt::AlignCenter);
            capacityMatrixTable->setItem(i, j, capItem);

            QTableWidgetItem* costItem = new QTableWidgetItem(QString::number(costMatrix[i][j]));
            costItem->setTextAlignment(Qt::AlignCenter);
            costMatrixTable->setItem(i, j, costItem);
        }
    }

    capacityMatrixTable->resizeColumnsToContents();
    costMatrixTable->resizeColumnsToContents();

    sourceVertexSpin->setRange(0, n-1);
    sinkVertexSpin->setRange(0, n-1);

    flowMatrixTable->hide();
    flowMatrixLabel->hide();

    showMessage("Success", "Capacity and cost matrices generated!");
}

void MainWindow::onFindMaxFlowClicked()
{
    if (!currentFlowGraph) {
        showMessage("Error", "First generate capacities and costs!");
        return;
    }

    int source = sourceVertexSpin->value();
    int sink = sinkVertexSpin->value();

    if (source == sink) {
        showMessage("Error", "Source and sink must be different vertices!");
        return;
    }

    std::string result = currentFlowGraph->formatMaxFlowResult(source, sink);

    flowOutput->setText(QString::fromStdString(result));
    const auto& flowMatrix = currentFlowGraph->getFlowMatrix();
    displayIntMatrix(flowMatrixTable, flowMatrix);
    flowMatrixLabel->setText("Flow Matrix (Maximum Flow Result):");
    flowMatrixLabel->show();
}

void MainWindow::onFindMinCostFlowClicked()
{
    if (!currentFlowGraph) {
        showMessage("Error", "First generate capacities and costs!");
        return;
    }

    int source = sourceVertexSpin->value();
    int sink = sinkVertexSpin->value();

    if (source == sink) {
        showMessage("Error", "Source and sink must be different vertices!");
        return;
    }

    // сн ачала находим максимальный поток
    int maxFlow = currentFlowGraph->fordFulkerson(source, sink);
    int requiredFlow = (maxFlow * 2) / 3;

    if (requiredFlow == 0) {
        showMessage("Warning", "Required flow is zero! Try generating a different graph.");
        return;
    }

    std::string result = currentFlowGraph->formatMinCostFlowResult(source, sink);
    flowOutput->setText(QString::fromStdString(result));

    const auto& flowMatrix = currentFlowGraph->getFlowMatrix();
    displayIntMatrix(flowMatrixTable, flowMatrix);
    flowMatrixLabel->setText("Flow Matrix (Min Cost Flow Result):");
    flowMatrixLabel->show();
}

void MainWindow::displayIntMatrix(QTableWidget* table, const std::vector<std::vector<int>>& matrix)
{
    if (matrix.empty()) return;

    int n = matrix.size();
    table->setRowCount(n);
    table->setColumnCount(n);

    QStringList headers;
    for (int i = 0; i < n; i++) {
        headers << QString::number(i);
    }
    table->setVerticalHeaderLabels(headers);
    table->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QString value = QString::number(matrix[i][j]);
            QTableWidgetItem* item = new QTableWidgetItem(value);
            item->setTextAlignment(Qt::AlignCenter);

            if (matrix[i][j] > 0) {
                item->setBackground(QColor(200, 255, 200)); // светло-зелёный
            }

            table->setItem(i, j, item);
        }
    }

    table->resizeColumnsToContents();
    table->show();
}

//-----------------------------Lab4-------------------------------------
void MainWindow::onCountSpanningTreesClicked()
{
    if (!currentGraph) {
        showMessage("Error", "First generate a graph in Tab 1!");
        return;
    }

    if (currentGraph->getType() == Graph::DIRECTED) {
        showMessage("Error",
            "This operation is only available for undirected graphs!\n"
            "Please generate an undirected graph in Tab 1.");
        return;
    }

    if (!currentWeightedGraph) {
        showMessage("Error",
            "Weighted graph not generated!\n"
            "Please go to Tab 2 and generate a weight matrix first.");
        return;
    }

    int n = currentGraph->getVerticesCount();

    // отображение матрицы Кирхгофа
    auto kirchhoffMatrix = currentGraph->getKirchhoffMatrix();

    kirchhoffMatrixTable->setRowCount(n);
    kirchhoffMatrixTable->setColumnCount(n);

    QStringList headers;
    for (int i = 0; i < n; i++) headers << QString::number(i);
    kirchhoffMatrixTable->setVerticalHeaderLabels(headers);
    kirchhoffMatrixTable->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(kirchhoffMatrix[i][j]));
            item->setTextAlignment(Qt::AlignCenter);
            if (i == j) {
                item->setBackground(QColor(200, 230, 255));
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
            } else if (kirchhoffMatrix[i][j] != 0) {
                item->setBackground(QColor(255, 200, 200));
            }
            kirchhoffMatrixTable->setItem(i, j, item);
        }
    }
    kirchhoffMatrixTable->resizeColumnsToContents();

    Skeleton skeleton(*currentGraph, currentWeightedGraph);

    // подсчёт остовных деревьев
    long long treeCount = skeleton.countSpanningTrees();

    QString output;
    output = QString::fromStdString(skeleton.solveAll());

    spanningTreeOutput->setText(output);
}

//-----------------------------Lab 5-------------------------------------

void MainWindow::onCheckEulerian()
{
    if (!currentGraph) {
        showMessage("Error", "First generate a graph in Tab 1!");
        return;
    }

    if (currentGraph->getType() == Graph::DIRECTED) {
        showMessage("Error", "Eulerian cycle is only for undirected graphs!");
        return;
    }

    GraphAnalyzer analyzer(*currentGraph);
    eulerianOutput->setText(QString::fromStdString(analyzer.solveEulerian()));
}

void MainWindow::onModifyToEulerian()
{
    if (!currentGraph) {
        showMessage("Error", "First generate a graph in Tab 1!");
        return;
    }

    if (currentGraph->getType() == Graph::DIRECTED) {
        showMessage("Error", "This operation is only for undirected graphs!");
        return;
    }

    // модифицируем граф и получаем лог
    std::string log = currentGraph->makeEulerian();

    QString output = QString::fromStdString(log);

    output += "\n";

    // теперь проверяем эйлеровость
    GraphAnalyzer analyzer(*currentGraph);
    output += QString::fromStdString(analyzer.solveEulerian());

    eulerianOutput->setText(output);

}
void MainWindow::onPathLengthChanged(int value)
{
    pathLengthSpin->setValue(value);
    pathLengthSlider->setValue(value);

    if (value == 0) {
        pathLengthLabel->setText("Showing paths of length: 0 edges (only diagonal)");
    } else if (value == 1) {
        pathLengthLabel->setText("Showing paths of length: 1 edge (same as weight matrix)");
    } else {
        pathLengthLabel->setText(QString("Showing paths of length: %1 edges").arg(value));
    }

    if (currentWeightedGraph) {
        updateShimbelMatrices();
    }
}

void MainWindow::onSolveCutsClicked()
{
    if (!currentGraph) {
        showMessage("Error", "First generate a graph in Tab 1!");
        return;
    }

    if (currentGraph->getType() == Graph::DIRECTED) {
        showMessage("Error", "Cuts are only for undirected graphs!");
        return;
    }

    Skeleton* skeleton;

    if (currentWeightedGraph) {
        skeleton = new Skeleton(*currentGraph, currentWeightedGraph);
    } else {
        WeightedGraph* tempWG = new WeightedGraph(currentGraph->getVerticesCount());
        for (int i = 0; i < currentGraph->getVerticesCount(); i++) {
            for (int neighbor : currentGraph->getNeighbors(i)) {
                if (i < neighbor) {
                    tempWG->setWeight(i, neighbor, 1);
                    tempWG->setWeight(neighbor, i, 1);
                }
            }
        }
        skeleton = new Skeleton(*currentGraph, tempWG);
        delete tempWG;
    }

    cutsOutput->setText(QString::fromStdString(skeleton->solveCuts()));

    int numCuts = currentGraph->getVerticesCount() - 1;  // n-1 разрезов
    cut1Spin->setRange(1, numCuts);
    cut2Spin->setRange(1, numCuts);

    delete skeleton;
}

void MainWindow::onSymDiffClicked()
{
    if (!currentGraph) {
        showMessage("Error", "First generate a graph in Tab 1!");
        return;
    }

    if (currentGraph->getType() == Graph::DIRECTED) {
        showMessage("Error", "Cuts are only for undirected graphs!");
        return;
    }

    QString input = QInputDialog::getText(this, "Select Cuts",
        "Enter cut indices (comma-separated, starting from 0):\n"
        "Example: 0,2,4");

    QStringList parts = input.split(",");
    std::vector<int> indices;
    for (const QString& s : parts) {
        bool ok;
        int idx = s.trimmed().toInt(&ok);
        if (ok) indices.push_back(idx);
    }

    if (indices.size() < 2) {
        showMessage("Error", "Need at least 2 cuts for symmetric difference!");
        return;
    }

    Skeleton* skeleton;
    if (currentWeightedGraph) {
        skeleton = new Skeleton(*currentGraph, currentWeightedGraph);
    } else {
        WeightedGraph* tempWG = new WeightedGraph(currentGraph->getVerticesCount());
        for (int i = 0; i < currentGraph->getVerticesCount(); i++) {
            for (int neighbor : currentGraph->getNeighbors(i)) {
                if (i < neighbor) {
                    tempWG->setWeight(i, neighbor, 1);
                    tempWG->setWeight(neighbor, i, 1);
                }
            }
        }
        skeleton = new Skeleton(*currentGraph, tempWG);
        delete tempWG;
    }

    int maxCuts = currentGraph->getVerticesCount() - 1;
    for (int idx : indices) {
        if (idx < 0 || idx >= maxCuts) {
            showMessage("Error",
                "Invalid cut index " + QString::number(idx) +
                "! Valid range: 0 to " + QString::number(maxCuts - 1));
            delete skeleton;
            return;
        }
    }

    auto result = skeleton->symmetricDifferenceMultiple(indices);

    QString output;
    output += "╔══════════════════════════════════════════════════════════════════╗\n";
    output += "║        SYMMETRIC DIFFERENCE OF MULTIPLE CUTS                     ║\n";
    output += "╚══════════════════════════════════════════════════════════════════╝\n\n";
    output += "Cuts: ";
    for (size_t i = 0; i < indices.size(); i++) {
        if (i > 0) output += " ⊕ ";
        output += "S_" + QString::number(indices[i]);
    }
    output += "\n\n";
    output += QString::fromStdString(skeleton->formatCut(result));

    cutsOutput->setText(output);

    delete skeleton;
}

void MainWindow::onBuildHistogramClicked()
{
    const int NUM_SAMPLES = 1000;

    histogramOutput->clear();
    histogramOutput->append("Generating " + QString::number(NUM_SAMPLES) +
                            " random numbers from log-distribution...\n");
    QApplication::processEvents();

    // Генерируем случайные числа
    LogDistribution logDist(currentP, rng);
    std::map<int, int> histogram;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        int value = logDist.generate();
        if (value < 1) value = 1;
        histogram[value]++;
    }

    // Вывод результатов
    QString output;
    output += "╔══════════════════════════════════════════════════════════════════╗\n";
    output += "║        LOG-DISTRIBUTION HISTOGRAM                                ║\n";
    output += "║        " + QString::number(NUM_SAMPLES) + " random samples\n";
    output += "╚══════════════════════════════════════════════════════════════════╝\n\n";

    output += "p = " + QString::number(currentP, 'f', 2) + "\n";
    output += "Theoretical mean = " + QString::number(logDist.getMean(), 'f', 2) + "\n\n";

    // Находим максимум для масштабирования
    int maxCount = 0;
    int minValue = INT_MAX;
    int maxValue = 0;

    for (const auto& pair : histogram) {
        if (pair.second > maxCount) maxCount = pair.second;
        if (pair.first < minValue) minValue = pair.first;
        if (pair.first > maxValue) maxValue = pair.first;
    }

    const int BAR_WIDTH = 50;

    double sum = 0;
    for (const auto& pair : histogram) {
        sum += (double)pair.first * pair.second;
    }
    double avgValue = sum / NUM_SAMPLES;

    output += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    output += "Value  | Count  | Distribution\n";
    output += "-------+--------+";
    for (int i = 0; i < BAR_WIDTH + 2; i++) output += "─";
    output += "\n";

    for (const auto& pair : histogram) {
        int value = pair.first;
        int count = pair.second;

        int barLength = (int)((double)count / maxCount * BAR_WIDTH);

        output += QString("%1  | %2  | ")
                    .arg(value, -5)
                    .arg(count, 5);

        for (int i = 0; i < barLength; i++) {
            output += "*";
        }
        output += "\n";
    }

    output += "\n";
    output += "Value range: " + QString::number(minValue) + " to " +
              QString::number(maxValue) + "\n";
    output += "Average: " + QString::number(avgValue, 'f', 2) + "\n";

    histogramOutput->setText(output);
}
