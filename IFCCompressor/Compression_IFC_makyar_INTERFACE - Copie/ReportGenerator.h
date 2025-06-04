#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include "CompressionWorker.h"

class ReportGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ReportGenerator(QObject *parent = nullptr);
    ~ReportGenerator();
    
    // Génération des rapports
    QString generateReport(const CompressionResult &result, const QString &format = "xlsx");
    
    // Configuration
    void setReportDirectory(const QString &directory);
    QString getReportDirectory() const;
    
    // Formats supportés
    static QStringList supportedFormats();
    
private:
    // Génération spécifique par format
    QString generateExcelReport(const CompressionResult &result);
    QString generateCsvReport(const CompressionResult &result);
    
    // Utilitaires
    QString generateReportFilename(const CompressionResult &result, const QString &format);
    QString formatFileSize(qint64 bytes);
    QString formatPercentage(double value);
    QString escapeCSV(const QString &text);
    
    // Données membres
    QString m_reportDirectory;
    QString m_templateDirectory;
};

// Structure pour les données du rapport Excel (si on utilise une bibliothèque Excel)
struct ExcelReportData {
    QString title;
    QString subtitle;
    QDateTime timestamp;
    
    QString inputFile;
    QString outputFile;
    QString originalSize;
    QString compressedSize;
    QString compressionRatio;
    QString fprLevel;
    QString objectsRemoved;
    QString processingTime;
    QString version;
};

#endif // REPORTGENERATOR_H