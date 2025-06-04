#include "MainWindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
    , m_reportGenerator(nullptr)
    , m_compressionInProgress(false)
    , m_updateTimer(new QTimer(this))
{
    setWindowTitle("IFC Compressor - Makyar");
    setMinimumSize(800, 600);
    resize(1000, 700);
    
    // Initialisation des composants
    m_settings = new QSettings(this);
    m_reportGenerator = new ReportGenerator(this);
    
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupConnections();
    
    loadSettings();
    loadDefaultLogo();
    loadDefaultIcon();
    setApplicationIcon();
    
    updateUI();
}

MainWindow::~MainWindow()
{
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait(3000);
    }
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget();
    setCentralWidget(m_centralWidget);
    
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *scrollContent = new QWidget();
    m_mainLayout = new QVBoxLayout(scrollContent);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Header avec logo
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    m_logoLabel = new QLabel();
    m_logoLabel->setFixedSize(120, 80);
    m_logoLabel->setScaledContents(true);
    m_logoLabel->setStyleSheet("QLabel { border: 2px solid #3498db; border-radius: 10px; "
                               "background-color: white; padding: 5px; }");
    
    m_titleLabel = new QLabel("IFC Compressor");
    m_titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; "
                               "color: #3498db; margin-left: 20px; }");
    
    QVBoxLayout *logoButtonLayout = new QVBoxLayout();
    m_changeLogoButton = new QPushButton("Changer Logo");
    m_changeLogoButton->setMaximumWidth(120);
    m_changeIconButton = new QPushButton("Changer Icône");
    m_changeIconButton->setMaximumWidth(120);
    
    logoButtonLayout->addWidget(m_changeLogoButton);
    logoButtonLayout->addWidget(m_changeIconButton);
    
    headerLayout->addWidget(m_logoLabel);
    headerLayout->addWidget(m_titleLabel, 1);
    headerLayout->addLayout(logoButtonLayout);
    
    m_mainLayout->addLayout(headerLayout);
    
    // Ligne de séparation
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("QFrame { color: #3498db; }");
    m_mainLayout->addWidget(line);
    
    // Groupe fichier d'entrée
    m_inputGroup = new QGroupBox("Fichier IFC d'entrée");
    m_inputGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    QHBoxLayout *inputLayout = new QHBoxLayout(m_inputGroup);
    
    m_inputFileEdit = new QLineEdit();
    m_inputFileEdit->setPlaceholderText("Sélectionnez un fichier IFC...");
    m_browseInputButton = new QPushButton("Parcourir");
    m_browseInputButton->setMinimumWidth(100);
    
    inputLayout->addWidget(m_inputFileEdit);
    inputLayout->addWidget(m_browseInputButton);
    
    m_mainLayout->addWidget(m_inputGroup);
    
    // Groupe fichier de sortie
    m_outputGroup = new QGroupBox("Fichier IFC de sortie");
    m_outputGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    QGridLayout *outputLayout = new QGridLayout(m_outputGroup);
    
    outputLayout->addWidget(new QLabel("Fichier:"), 0, 0);
    m_outputFileEdit = new QLineEdit();
    m_outputFileEdit->setPlaceholderText("Nom du fichier de sortie...");
    outputLayout->addWidget(m_outputFileEdit, 0, 1);
    
    outputLayout->addWidget(new QLabel("Dossier:"), 1, 0);
    m_outputDirEdit = new QLineEdit();
    m_outputDirEdit->setPlaceholderText("Dossier de destination...");
    outputLayout->addWidget(m_outputDirEdit, 1, 1);
    
    m_browseOutputButton = new QPushButton("Parcourir");
    m_browseOutputButton->setMinimumWidth(100);
    outputLayout->addWidget(m_browseOutputButton, 1, 2);
    
    m_mainLayout->addWidget(m_outputGroup);
    
    // Groupe paramètres de compression
    m_compressionGroup = new QGroupBox("Paramètres de compression");
    m_compressionGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    QGridLayout *compressionLayout = new QGridLayout(m_compressionGroup);
    
    compressionLayout->addWidget(new QLabel("Niveau FPR:"), 0, 0);
    m_fprLevelCombo = new QComboBox();
    m_fprLevelCombo->addItems({"Faible (1)", "Moyen (2)", "Élevé (3)", "Maximum (4)"});
    m_fprLevelCombo->setCurrentIndex(1);
    compressionLayout->addWidget(m_fprLevelCombo, 0, 1);
    
    m_removeObjectsCheck = new QCheckBox("Supprimer les objets inutiles");
    m_removeObjectsCheck->setChecked(true);
    compressionLayout->addWidget(m_removeObjectsCheck, 1, 0, 1, 2);
    
    m_generateReportCheck = new QCheckBox("Générer un rapport");
    m_generateReportCheck->setChecked(true);
    compressionLayout->addWidget(m_generateReportCheck, 2, 0);
    
    m_reportFormatCombo = new QComboBox();
    m_reportFormatCombo->addItems({"Excel (.xlsx)", "CSV (.csv)"});
    compressionLayout->addWidget(m_reportFormatCombo, 2, 1);
    
    m_mainLayout->addWidget(m_compressionGroup);
    
    // Groupe progression
    m_progressGroup = new QGroupBox("Progression");
    m_progressGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    QVBoxLayout *progressLayout = new QVBoxLayout(m_progressGroup);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    progressLayout->addWidget(m_progressBar);
    
    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setMaximumHeight(100);
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setStyleSheet("QTextEdit { background-color: #2c3e50; color: white; "
                                "font-family: 'Courier New', monospace; }");
    progressLayout->addWidget(m_logTextEdit);
    
    m_mainLayout->addWidget(m_progressGroup);
    
    // Boutons d'action
    m_buttonLayout = new QHBoxLayout();
    
    m_startButton = new QPushButton("Démarrer la compression");
    m_startButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; "
                                "font-weight: bold; padding: 10px; border-radius: 5px; } "
                                "QPushButton:hover { background-color: #2ecc71; }");
    
    m_cancelButton = new QPushButton("Annuler");
    m_cancelButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; "
                                 "font-weight: bold; padding: 10px; border-radius: 5px; } "
                                 "QPushButton:hover { background-color: #c0392b; }");
    m_cancelButton->setVisible(false);
    
    m_resetButton = new QPushButton("Reset");
    m_resetButton->setStyleSheet("QPushButton { background-color: #f39c12; color: white; "
                                "font-weight: bold; padding: 10px; border-radius: 5px; } "
                                "QPushButton:hover { background-color: #e67e22; }");
    
    m_openOutputButton = new QPushButton("Ouvrir dossier");
    m_openOutputButton->setEnabled(false);
    
    m_openReportButton = new QPushButton("Ouvrir rapport");
    m_openReportButton->setEnabled(false);
    
    m_buttonLayout->addWidget(m_startButton);
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_resetButton);
    m_buttonLayout->addWidget(m_openOutputButton);
    m_buttonLayout->addWidget(m_openReportButton);
    
    m_mainLayout->addLayout(m_buttonLayout);
    m_mainLayout->addStretch();
    
    m_scrollArea->setWidget(scrollContent);
    
    QVBoxLayout *centralLayout = new QVBoxLayout(m_centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(m_scrollArea);
}

void MainWindow::setupMenuBar()
{
    // Menu Fichier
    QMenu *fileMenu = menuBar()->addMenu("&Fichier");
    
    m_openAction = new QAction("&Ouvrir IFC...", this);
    m_openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(m_openAction);
    
    fileMenu->addSeparator();
    
    m_exitAction = new QAction("&Quitter", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(m_exitAction);
    
    // Menu Outils
    QMenu *toolsMenu = menuBar()->addMenu("&Outils");
    
    m_changeLogoAction = new QAction("Changer le &logo...", this);
    toolsMenu->addAction(m_changeLogoAction);
    
    m_changeIconAction = new QAction("Changer l'&icône...", this);
    toolsMenu->addAction(m_changeIconAction);
    
    // Menu Aide
    QMenu *helpMenu = menuBar()->addMenu("&Aide");
    
    m_aboutAction = new QAction("À &propos...", this);
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("Principal");
    toolBar->addAction(m_openAction);
    toolBar->addSeparator();
    toolBar->addAction(m_changeLogoAction);
    toolBar->addAction(m_changeIconAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Prêt");
}

void MainWindow::setupConnections()
{
    // Boutons
    connect(m_browseInputButton, &QPushButton::clicked, this, &MainWindow::browseInputFile);
    connect(m_browseOutputButton, &QPushButton::clicked, this, &MainWindow::browseOutputDirectory);
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::startCompression);
    connect(m_cancelButton, &QPushButton::clicked, this, &MainWindow::cancelCompression);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::resetForm);
    connect(m_openOutputButton, &QPushButton::clicked, this, &MainWindow::openOutputDirectory);
    connect(m_openReportButton, &QPushButton::clicked, this, &MainWindow::openReportFile);
    connect(m_changeLogoButton, &QPushButton::clicked, this, &MainWindow::changeLogo);
    connect(m_changeIconButton, &QPushButton::clicked, this, &MainWindow::changeIcon);
    
    // Champs de texte
    connect(m_inputFileEdit, &QLineEdit::textChanged, this, &MainWindow::updateOutputPath);
    connect(m_outputFileEdit, &QLineEdit::textChanged, this, &MainWindow::updateUI);
    
    // Actions du menu
    connect(m_openAction, &QAction::triggered, this, &MainWindow::browseInputFile);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    connect(m_changeLogoAction, &QAction::triggered, this, &MainWindow::changeLogo);
    connect(m_changeIconAction, &QAction::triggered, this, &MainWindow::changeIcon);
    connect(m_aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "À propos", 
                          "IFC Compressor v1.0\n\n"
                          "Développé par Makyar\n"
                          "Outil de compression des fichiers IFC avec optimisation FPR");
    });
}

void MainWindow::browseInputFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Sélectionner un fichier IFC", m_lastInputDir,
        "Fichiers IFC (*.ifc);;Tous les fichiers (*.*)");
    
    if (!fileName.isEmpty()) {
        m_inputFileEdit->setText(fileName);
        m_lastInputDir = QFileInfo(fileName).absolutePath();
        updateOutputPath();
    }
}

void MainWindow::browseOutputDirectory()
{
    QString directory = QFileDialog::getExistingDirectory(this,
        "Sélectionner le dossier de destination", m_lastOutputDir);
    
    if (!directory.isEmpty()) {
        m_outputDirEdit->setText(directory);
        m_lastOutputDir = directory;
    }
}

void MainWindow::updateOutputPath()
{
    QString inputFile = m_inputFileEdit->text();
    if (!inputFile.isEmpty()) {
        QFileInfo fileInfo(inputFile);
        QString baseName = fileInfo.completeBaseName();
        QString outputFileName = baseName + "_OPTIMISE.ifc";
        
        m_outputFileEdit->setText(outputFileName);
        
        if (m_outputDirEdit->text().isEmpty()) {
            m_outputDirEdit->setText(fileInfo.absolutePath());
        }
    }
}

void MainWindow::startCompression()
{
    // Validation des entrées
    if (m_inputFileEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un fichier IFC d'entrée.");
        return;
    }
    
    if (m_outputFileEdit->text().isEmpty() || m_outputDirEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez spécifier le fichier et le dossier de sortie.");
        return;
    }
    
    // Configuration du worker
    if (!m_workerThread) {
        m_workerThread = new QThread(this);
        m_worker = new CompressionWorker();
        m_worker->moveToThread(m_workerThread);
        
        connect(m_workerThread, &QThread::started, m_worker, &CompressionWorker::startCompression);
        connect(m_worker, &CompressionWorker::progressUpdated, this, &MainWindow::onCompressionProgress);
        connect(m_worker, &CompressionWorker::compressionFinished, this, &MainWindow::onCompressionFinished);
        connect(m_worker, &CompressionWorker::compressionError, this, &MainWindow::onCompressionError);
    }
    
    // Configuration des paramètres
    CompressionParameters params;
    params.inputFile = m_inputFileEdit->text();
    params.outputFile = QDir(m_outputDirEdit->text()).filePath(m_outputFileEdit->text());
    params.fprLevel = m_fprLevelCombo->currentIndex() + 1;
    params.removeObjects = m_removeObjectsCheck->isChecked();
    params.generateReport = m_generateReportCheck->isChecked();
    params.reportFormat = m_reportFormatCombo->currentIndex() == 0 ? "xlsx" : "csv";
    
    m_worker->setParameters(params);
    
    // Interface
    m_compressionInProgress = true;
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_startButton->setVisible(false);
    m_cancelButton->setVisible(true);
    m_logTextEdit->clear();
    m_logTextEdit->append(QString("[%1] Démarrage de la compression...")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    
    statusBar()->showMessage("Compression en cours...");
    
    m_workerThread->start();
}

void MainWindow::cancelCompression()
{
    if (m_worker) {
        m_worker->cancelCompression();
    }
    
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait(3000);
    }
    
    m_compressionInProgress = false;
    m_progressBar->setVisible(false);
    m_startButton->setVisible(true);
    m_cancelButton->setVisible(false);
    
    m_logTextEdit->append(QString("[%1] Compression annulée par l'utilisateur")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    
    statusBar()->showMessage("Compression annulée");
}

void MainWindow::onCompressionProgress(int percentage, const QString &message)
{
    m_progressBar->setValue(percentage);
    m_logTextEdit->append(QString("[%1] %2")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                         .arg(message));
}

void MainWindow::onCompressionError(const QString &error)
{
    m_compressionInProgress = false;
    m_progressBar->setVisible(false);
    m_startButton->setVisible(true);
    m_cancelButton->setVisible(false);
    
    m_logTextEdit->append(QString("[%1] ERREUR: %2")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                         .arg(error));
    
    statusBar()->showMessage("Erreur lors de la compression");
    QMessageBox::critical(this, "Erreur", "Erreur lors de la compression:\n" + error);
}

void MainWindow::resetForm()
{
    m_inputFileEdit->clear();
    m_outputFileEdit->clear();
    m_outputDirEdit->clear();
    m_fprLevelCombo->setCurrentIndex(1);
    m_removeObjectsCheck->setChecked(true);
    m_generateReportCheck->setChecked(true);
    m_reportFormatCombo->setCurrentIndex(0);
    m_logTextEdit->clear();
    m_progressBar->setVisible(false);
    m_openOutputButton->setEnabled(false);
    m_openReportButton->setEnabled(false);
    
    statusBar()->showMessage("Formulaire réinitialisé");
}

void MainWindow::openOutputDirectory()
{
    if (!m_outputDirEdit->text().isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_outputDirEdit->text()));
    }
}

void MainWindow::openReportFile()
{
    if (!m_lastReportPath.isEmpty() && QFileInfo::exists(m_lastReportPath)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_lastReportPath));
    }
}

void MainWindow::changeLogo()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Sélectionner un logo", m_currentLogoPath,
        "Images (*.png *.jpg *.jpeg *.bmp *.gif);;Tous les fichiers (*.*)");
    
    if (!fileName.isEmpty()) {
        // Copier le fichier dans le dossier ressources
        QString targetPath = "ressources/images/fond/" + QFileInfo(fileName).fileName();
        
        if (QFile::exists(targetPath)) {
            QFile::remove(targetPath);
        }
        
        if (QFile::copy(fileName, targetPath)) {
            m_currentLogoPath = targetPath;
            loadDefaultLogo();
            saveSettings();
            statusBar()->showMessage("Logo mis à jour", 2000);
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de copier le logo.");
        }
    }
}

void MainWindow::changeIcon()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Sélectionner une icône", m_currentIconPath,
        "Images (*.png *.jpg *.jpeg *.bmp *.gif *.ico);;Tous les fichiers (*.*)");
    
    if (!fileName.isEmpty()) {
        // Copier le fichier dans le dossier ressources
        QString targetPath = "ressources/images/icons/app/" + QFileInfo(fileName).fileName();
        
        if (QFile::exists(targetPath)) {
            QFile::remove(targetPath);
        }
        
        if (QFile::copy(fileName, targetPath)) {
            m_currentIconPath = targetPath;
            setApplicationIcon();
            saveSettings();
            statusBar()->showMessage("Icône mise à jour", 2000);
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de copier l'icône.");
        }
    }
}

void MainWindow::loadDefaultLogo()
{
    // Charger le logo personnalisé ou le logo par défaut
    QPixmap logo;
    
    if (!m_currentLogoPath.isEmpty() && QFile::exists(m_currentLogoPath)) {
        logo.load(m_currentLogoPath);
    } else {
        // Chercher un logo dans le dossier par défaut
        QDir logoDir("ressources/images/fond");
        QStringList logoFiles = logoDir.entryList(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif", 
                                                 QDir::Files);
        
        if (!logoFiles.isEmpty()) {
            QString logoPath = logoDir.filePath(logoFiles.first());
            logo.load(logoPath);
            m_currentLogoPath = logoPath;
        } else {
            // Logo par défaut (généré programmatiquement)
            logo = QPixmap(120, 80);
            logo.fill(QColor(52, 152, 219));
            
            QPainter painter(&logo);
            painter.setPen(QPen(Qt::white, 2));
            painter.setFont(QFont("Arial", 14, QFont::Bold));
            painter.drawText(logo.rect(), Qt::AlignCenter, "MAKYAR\nIFC");
        }
    }
    
    if (!logo.isNull()) {
        m_logoLabel->setPixmap(logo.scaled(m_logoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::loadDefaultIcon()
{
    // Charger l'icône personnalisée ou l'icône par défaut
    if (!m_currentIconPath.isEmpty() && QFile::exists(m_currentIconPath)) {
        return; // L'icône sera chargée par setApplicationIcon()
    }
    
    // Chercher une icône dans le dossier par défaut
    QDir iconDir("ressources/images/icons/app");
    QStringList iconFiles = iconDir.entryList(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif" << "*.ico", 
                                             QDir::Files);
    
    if (!iconFiles.isEmpty()) {
        m_currentIconPath = iconDir.filePath(iconFiles.first());
    }
}

void MainWindow::setApplicationIcon()
{
    QIcon appIcon;
    
    if (!m_currentIconPath.isEmpty() && QFile::exists(m_currentIconPath)) {
        appIcon = QIcon(m_currentIconPath);
    } else {
        // Icône par défaut (généré programmatiquement)
        QPixmap iconPixmap(64, 64);
        iconPixmap.fill(QColor(52, 152, 219));
        
        QPainter painter(&iconPixmap);
        painter.setPen(QPen(Qt::white, 2));
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(iconPixmap.rect(), Qt::AlignCenter, "IFC");
        
        appIcon = QIcon(iconPixmap);
    }
    
    setWindowIcon(appIcon);
    QApplication::setWindowIcon(appIcon);
}

void MainWindow::updateUI()
{
    bool hasInput = !m_inputFileEdit->text().isEmpty();
    bool hasOutput = !m_outputFileEdit->text().isEmpty() && !m_outputDirEdit->text().isEmpty();
    
    m_startButton->setEnabled(hasInput && hasOutput && !m_compressionInProgress);
    m_resetButton->setEnabled(!m_compressionInProgress);
    
    // Activer/désactiver le combo format de rapport selon la case à cocher
    m_reportFormatCombo->setEnabled(m_generateReportCheck->isChecked());
}

void MainWindow::loadSettings()
{
    m_settings->beginGroup("Paths");
    m_lastInputDir = m_settings->value("lastInputDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    m_lastOutputDir = m_settings->value("lastOutputDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    m_currentLogoPath = m_settings->value("logoPath", "").toString();
    m_currentIconPath = m_settings->value("iconPath", "").toString();
    m_settings->endGroup();
    
    m_settings->beginGroup("Settings");
    m_fprLevelCombo->setCurrentIndex(m_settings->value("fprLevel", 1).toInt());
    m_removeObjectsCheck->setChecked(m_settings->value("removeObjects", true).toBool());
    m_generateReportCheck->setChecked(m_settings->value("generateReport", true).toBool());
    m_reportFormatCombo->setCurrentIndex(m_settings->value("reportFormat", 0).toInt());
    m_settings->endGroup();
    
    // Géométrie de la fenêtre
    m_settings->beginGroup("Geometry");
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("state").toByteArray());
    m_settings->endGroup();
}

void MainWindow::saveSettings()
{
    m_settings->beginGroup("Paths");
    m_settings->setValue("lastInputDir", m_lastInputDir);
    m_settings->setValue("lastOutputDir", m_lastOutputDir);
    m_settings->setValue("logoPath", m_currentLogoPath);
    m_settings->setValue("iconPath", m_currentIconPath);
    m_settings->endGroup();
    
    m_settings->beginGroup("Settings");
    m_settings->setValue("fprLevel", m_fprLevelCombo->currentIndex());
    m_settings->setValue("removeObjects", m_removeObjectsCheck->isChecked());
    m_settings->setValue("generateReport", m_generateReportCheck->isChecked());
    m_settings->setValue("reportFormat", m_reportFormatCombo->currentIndex());
    m_settings->endGroup();
    
    // Géométrie de la fenêtre
    m_settings->beginGroup("Geometry");
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("state", saveState());
    m_settings->endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_compressionInProgress) {
        int ret = QMessageBox::question(this, "Compression en cours",
                                       "Une compression est en cours. Voulez-vous vraiment quitter?",
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        } else {
            cancelCompression();
        }
    }
    
    saveSettings();
    event->accept();
}

QString MainWindow::formatFileSize(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (bytes >= GB) {
        return QString("%1 GB").arg(double(bytes) / GB, 0, 'f', 2);
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(double(bytes) / MB, 0, 'f', 2);
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(double(bytes) / KB, 0, 'f', 2);
    } else {
        return QString("%1 bytes").arg(bytes);
    }
}currentDateTime().toString("hh:mm:ss"))
                         .arg(message));
    
    // Auto-scroll vers le bas
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logTextEdit->setTextCursor(cursor);
}

void MainWindow::onCompressionFinished(bool success, const QString &message, 
                                      const CompressionResult &result)
{
    m_compressionInProgress = false;
    m_progressBar->setVisible(false);
    m_startButton->setVisible(true);
    m_cancelButton->setVisible(false);
    
    if (success) {
        m_lastResult = result;
        m_openOutputButton->setEnabled(true);
        
        // Génération du rapport si demandé
        if (m_generateReportCheck->isChecked()) {
            QString reportPath = m_reportGenerator->generateReport(result, 
                m_reportFormatCombo->currentIndex() == 0 ? "xlsx" : "csv");
            
            if (!reportPath.isEmpty()) {
                m_lastReportPath = reportPath;
                m_openReportButton->setEnabled(true);
                m_logTextEdit->append(QString("[%1] Rapport généré: %2")
                                     .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                                     .arg(QFileInfo(reportPath).fileName()));
            }
        }
        
        statusBar()->showMessage("Compression terminée avec succès");
        QMessageBox::information(this, "Succès", 
                               QString("Compression terminée!\n\n"
                                      "Taille originale: %1\n"
                                      "Taille compressée: %2\n"
                                      "Taux de compression: %3%")
                               .arg(formatFileSize(result.originalSize))
                               .arg(formatFileSize(result.compressedSize))
                               .arg(result.compressionRatio, 0, 'f', 1));
    } else {
        statusBar()->showMessage("Compression échouée");
        QMessageBox::critical(this, "Erreur", "La compression a échoué:\n" + message);
    }
    
    m_logTextEdit->append(QString("[%1] %2")
                         .arg(QDateTime::