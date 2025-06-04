#include "ReportGenerator.h"
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

// Pour Excel, vous devrez ajouter une bibliothèque comme QXlsx ou similaire
// #include "xlsxdocument.h"
// #include "xlsxworksheet.h"

ReportGenerator::ReportGenerator(QObject *parent)
    : QObject(parent)
    , m_reportDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/IFC_Compression_Reports")
{
    // Créer le dossier de rapport s'il n'existe pas
    QDir().mkpath(m_reportDirectory);
    QDir().mkpath("ressources/reports");
}

ReportGenerator::~ReportGenerator()
{
}

QString ReportGenerator::generateReport(const CompressionResult &result, const QString &format)
{
    QString reportPath;
    
    try {
        if (format.toLower() == "xlsx") {
            reportPath = generateExcelReport(result);
        } else if (format.toLower() == "csv") {
            reportPath = generateCsvReport(result);
        } else {
            qWarning() << "Format de rapport non supporté:" << format;
            return QString();
        }
        
        if (!reportPath.isEmpty() && QFileInfo::exists(reportPath)) {
            qDebug() << "Rapport généré avec succès:" << reportPath;
            return reportPath;
        } else {
            qWarning() << "Échec de la génération du rapport";
            return QString();
        }
        
    } catch (const std::exception &e