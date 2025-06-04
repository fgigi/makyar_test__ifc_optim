#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QPixmap>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QSettings>
#include <QCloseEvent>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QScrollArea>
#include <QSplitter>

#include "CompressionWorker.h"
#include "ReportGenerator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void browseInputFile();
    void browseOutputDirectory();
    void startCompression();
    void cancelCompression();
    void onCompressionProgress(int percentage, const QString &message);
    void onCompressionFinished(bool success, const QString &message, 
                              const CompressionResult &result);
    void onCompressionError(const QString &error);
    void resetForm();
    void openOutputDirectory();
    void openReportFile();
    void changeLogo();
    void changeIcon();
    void loadSettings();
    void saveSettings();
    void updateOutputPath();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void updateUI();
    void loadDefaultLogo();
    void loadDefaultIcon();
    void setApplicationIcon();
    
    // Interface principale
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scrollArea;
    QSplitter *m_splitter;
    
    // Header avec logo
    QLabel *m_logoLabel;
    QLabel *m_titleLabel;
    QPushButton *m_changeLogoButton;
    QPushButton *m_changeIconButton;
    
    // Groupe fichier d'entrée
    QGroupBox *m_inputGroup;
    QLineEdit *m_inputFileEdit;
    QPushButton *m_browseInputButton;
    
    // Groupe fichier de sortie
    QGroupBox *m_outputGroup;
    QLineEdit *m_outputFileEdit;
    QLineEdit *m_outputDirEdit;
    QPushButton *m_browseOutputButton;
    
    // Groupe paramètres de compression
    QGroupBox *m_compressionGroup;
    QComboBox *m_fprLevelCombo;
    QCheckBox *m_removeObjectsCheck;
    QCheckBox *m_generateReportCheck;
    QComboBox *m_reportFormatCombo;
    
    // Groupe progression
    QGroupBox *m_progressGroup;
    QProgressBar *m_progressBar;
    QTextEdit *m_logTextEdit;
    
    // Boutons d'action
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_startButton;
    QPushButton *m_cancelButton;
    QPushButton *m_resetButton;
    QPushButton *m_openOutputButton;
    QPushButton *m_openReportButton;
    
    // Menu et toolbar
    QAction *m_openAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_changeLogoAction;
    QAction *m_changeIconAction;
    
    // Worker thread pour la compression
    QThread *m_workerThread;
    CompressionWorker *m_worker;
    
    // Générateur de rapport
    ReportGenerator *m_reportGenerator;
    
    // Données
    QString m_currentLogoPath;
    QString m_currentIconPath;
    QString m_lastInputDir;
    QString m_lastOutputDir;
    QString m_lastReportPath;
    CompressionResult m_lastResult;
    
    // État
    bool m_compressionInProgress;
    QTimer *m_updateTimer;
    
    // Configuration
    QSettings *m_settings;
};

#endif // MAINWINDOW_H