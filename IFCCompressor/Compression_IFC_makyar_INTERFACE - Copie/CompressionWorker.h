#ifndef COMPRESSIONWORKER_H
#define COMPRESSIONWORKER_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QFileInfo>
#include <QDateTime>

// Structure pour les paramètres de compression
struct CompressionParameters {
    QString inputFile;
    QString outputFile;
    int fprLevel;
    bool removeObjects;
    bool generateReport;
    QString reportFormat;
};

// Structure pour les résultats de compression
struct CompressionResult {
    QString inputFile;
    QString outputFile;
    qint64 originalSize;
    qint64 compressedSize;
    double compressionRatio;
    int fprLevel;
    bool objectsRemoved;
    QDateTime timestamp;
    QString processingTime;
    QString version = "1.0";
    
    CompressionResult() : originalSize(0), compressedSize(0), compressionRatio(0.0), 
                         fprLevel(1), objectsRemoved(false) {}
};

class CompressionWorker : public QObject
{
    Q_OBJECT

public:
    explicit CompressionWorker(QObject *parent = nullptr);
    ~CompressionWorker();
    
    void setParameters(const CompressionParameters &params);
    void cancelCompression();

public slots:
    void startCompression();

signals:
    void progressUpdated(int percentage, const QString &message);
    void compressionFinished(bool success, const QString &message, const CompressionResult &result);
    void compressionError(const QString &error);

private slots:
    void updateProgress();

private:
    // Interface avec le code de compression Makyar
    bool callMakyarCompression();
    bool preprocessFile();
    bool performCompression();
    bool postprocessFile();
    bool validateOutput();
    
    // Calculs et utilitaires
    qint64 getFileSize(const QString &filePath);
    double calculateCompressionRatio(qint64 original, qint64 compressed);
    QString formatDuration(qint64 milliseconds);
    
    // Données membres
    CompressionParameters m_parameters;
    CompressionResult m_result;
    QTimer *m_progressTimer;
    QMutex m_mutex;
    QDateTime m_startTime;
    
    // État
    bool m_cancelled;
    bool m_running;
    int m_currentStep;
    int m_totalSteps;
    QString m_currentOperation;
};

#endif // COMPRESSIONWORKER_H