#include "CompressionWorker.h"
#include <QThread>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QDebug>

// Inclure ici l'interface vers le code Makyar
// #include "MakyarCompressionCore.h"

CompressionWorker::CompressionWorker(QObject *parent)
    : QObject(parent)
    , m_progressTimer(new QTimer(this))
    , m_cancelled(false)
    , m_running(false)
    , m_currentStep(0)
    , m_totalSteps(5)
{
    m_progressTimer->setInterval(100); // Mise à jour toutes les 100ms
    connect(m_progressTimer, &QTimer::timeout, this, &CompressionWorker::updateProgress);
}

CompressionWorker::~CompressionWorker()
{
    if (m_running) {
        cancelCompression();
    }
}

void CompressionWorker::setParameters(const CompressionParameters &params)
{
    QMutexLocker locker(&m_mutex);
    m_parameters = params;
}

void CompressionWorker::cancelCompression()
{
    QMutexLocker locker(&m_mutex);
    m_cancelled = true;
    
    if (m_progressTimer->isActive()) {
        m_progressTimer->stop();
    }
}

void CompressionWorker::startCompression()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_running) {
        return;
    }
    
    m_running = true;
    m_cancelled = false;
    m_currentStep = 0;
    m_startTime = QDateTime::currentDateTime();
    
    // Initialisation du résultat
    m_result = CompressionResult();
    m_result.inputFile = m_parameters.inputFile;
    m_result.outputFile = m_parameters.outputFile;
    m_result.fprLevel = m_parameters.fprLevel;
    m_result.objectsRemoved = m_parameters.removeObjects;
    m_result.timestamp = m_startTime;
    
    locker.unlock();
    
    // Démarrer la minuterie de progression
    m_progressTimer->start();
    
    // Exécuter la compression
    bool success = false;
    QString message;
    
    try {
        // Vérifications préliminaires
        if (!QFileInfo::exists(m_parameters.inputFile)) {
            throw QString("Le fichier d'entrée n'existe pas: %1").arg(m_parameters.inputFile);
        }
        
        m_result.originalSize = getFileSize(m_parameters.inputFile);
        if (m_result.originalSize <= 0) {
            throw QString("Impossible de lire la taille du fichier d'entrée");
        }
        
        // Créer le dossier de sortie si nécessaire
        QDir outputDir = QFileInfo(m_parameters.outputFile).absoluteDir();
        if (!outputDir.exists()) {
            if (!outputDir.mkpath(".")) {
                throw QString("Impossible de créer le dossier de sortie: %1").arg(outputDir.absolutePath());
            }
        }
        
        // Étapes de compression
        success = callMakyarCompression();
        
        if (success && !m_cancelled) {
            m_result.compressedSize = getFileSize(m_parameters.outputFile);
            m_result.compressionRatio = calculateCompressionRatio(m_result.originalSize, m_result.compressedSize);
            
            QDateTime endTime = QDateTime::currentDateTime();
            m_result.processingTime = formatDuration(m_startTime.msecsTo(endTime));
            
            message = QString("Compression terminée avec succès (ratio: %1%)")
                     .arg(m_result.compressionRatio, 0, 'f', 1);
        } else if (m_cancelled) {
            message = "Compression annulée par l'utilisateur";
            success = false;
        } else {
            message = "Échec de la compression";
        }
        
    } catch (const QString &error) {
        success = false;
        message = error;
        emit compressionError(error);
    }
    
    // Arrêter la minuterie
    m_progressTimer->stop();
    
    // État final
    QMutexLocker finalLocker(&m_mutex);
    m_running = false;
    finalLocker.unlock();
    
    // Émettre le résultat
    if (success) {
        emit compressionFinished(true, message, m_result);
    } else {
        emit compressionFinished(false, message, m_result);
    }
}

void CompressionWorker::updateProgress()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_cancelled || !m_running) {
        return;
    }
    
    int percentage = (m_currentStep * 100) / m_totalSteps;
    percentage = qMin(percentage, 95); // Ne jamais atteindre 100% avant la fin réelle
    
    emit progressUpdated(percentage, m_currentOperation);
}

bool CompressionWorker::callMakyarCompression()
{
    // Cette méthode fait l'interface avec le code de compression Makyar
    // Pour l'instant, simulation des étapes
    
    try {
        // Étape 1: Préprocessing
        {
            QMutexLocker locker(&m_mutex);
            if (m_cancelled) return false;
            m_currentStep = 1;
            m_currentOperation = "Analyse du fichier IFC...";
        }
        
        if (!preprocessFile()) {
            return false;
        }
        QThread::msleep(500); // Simulation
        
        // Étape 2: Compression principale
        {
            QMutexLocker locker(&m_mutex);
            if (m_cancelled) return false;
            m_currentStep = 2;
            m_currentOperation = QString("Compression FPR niveau %1...").arg(m_parameters.fprLevel);
        }
        
        if (!performCompression()) {
            return false;
        }
        QThread::msleep(1000); // Simulation
        
        // Étape 3: Suppression d'objets (si activée)
        if (m_parameters.removeObjects) {
            QMutexLocker locker(&m_mutex);
            if (m_cancelled) return false;
            m_currentStep = 3;
            m_currentOperation = "Suppression des objets inutiles...";
            QThread::msleep(300); // Simulation
        }
        
        // Étape 4: Post-processing
        {
            QMutexLocker locker(&m_mutex);
            if (m_cancelled) return false;
            m_currentStep = 4;
            m_currentOperation = "Finalisation du fichier...";
        }
        
        if (!postprocessFile()) {
            return false;
        }
        QThread::msleep(200); // Simulation
        
        // Étape 5: Validation
        {
            QMutexLocker locker(&m_mutex);
            if (m_cancelled) return false;
            m_currentStep = 5;
            m_currentOperation = "Validation du fichier de sortie...";
        }
        
        if (!validateOutput()) {
            return false;
        }
        
        return true;
        
    } catch (...) {
        return false;
    }
}

bool CompressionWorker::preprocessFile()
{
    // Interface avec MakyarCompressionCore::preprocessIFC()
    // Pour l'instant, simulation
    
    QMutexLocker locker(&m_mutex);
    if (m_cancelled) return false;
    
    // TODO: Appel réel au code Makyar
    /*
    try {
        MakyarCompressionCore core;
        return core.preprocessIFC(m_parameters.inputFile);
    } catch (...) {
        return false;
    }
    */
    
    // Simulation pour le moment
    return QFileInfo::exists(m_parameters.inputFile);
}

bool CompressionWorker::performCompression()
{
    // Interface avec MakyarCompressionCore::compressIFC()
    
    QMutexLocker locker(&m_mutex);
    if (m_cancelled) return false;
    
    // TODO: Appel réel au code Makyar
    /*
    try {
        MakyarCompressionCore core;
        MakyarCompressionCore::Parameters coreParams;
        coreParams.inputFile = m_parameters.inputFile;
        coreParams.outputFile = m_parameters.outputFile;
        coreParams.fprLevel = m_parameters.fprLevel;
        coreParams.removeObjects = m_parameters.removeObjects;
        
        return core.compressIFC(coreParams);
    } catch (...) {
        return false;
    }
    */
    
    // Simulation pour le moment - copie du fichier avec un nom différent
    locker.unlock();
    
    return QFile::copy(m_parameters.inputFile, m_parameters.outputFile);
}

bool CompressionWorker::postprocessFile()
{
    // Interface avec MakyarCompressionCore::postprocessIFC()
    
    QMutexLocker locker(&m_mutex);
    if (m_cancelled) return false;
    
    // TODO: Appel réel au code Makyar
    /*
    try {
        MakyarCompressionCore core;
        return core.postprocessIFC(m_parameters.outputFile);
    } catch (...) {
        return false;
    }
    */
    
    // Simulation pour le moment
    return QFileInfo::exists(m_parameters.outputFile);
}

bool CompressionWorker::validateOutput()
{
    QMutexLocker locker(&m_mutex);
    if (m_cancelled) return false;
    
    // Vérifications de base
    if (!QFileInfo::exists(m_parameters.outputFile)) {
        return false;
    }
    
    qint64 outputSize = getFileSize(m_parameters.outputFile);
    if (outputSize <= 0) {
        return false;
    }
    
    // TODO: Validation plus approfondie avec le code Makyar
    /*
    try {
        MakyarCompressionCore core;
        return core.validateIFC(m_parameters.outputFile);
    } catch (...) {
        return false;
    }
    */
    
    return true;
}

qint64 CompressionWorker::getFileSize(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists() && fileInfo.isFile()) {
        return fileInfo.size();
    }
    return -1;
}

double CompressionWorker::calculateCompressionRatio(qint64 original, qint64 compressed)
{
    if (original <= 0) {
        return 0.0;
    }
    
    return ((double)(original - compressed) / (double)original) * 100.0;
}

QString CompressionWorker::formatDuration(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    
    if (minutes > 0) {
        return QString("%1m %2s").arg(minutes).arg(seconds);
    } else {
        return QString("%1s").arg(seconds);
    }
}