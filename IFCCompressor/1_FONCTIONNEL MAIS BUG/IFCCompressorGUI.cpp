#include "IFCCompressorGUI.h"
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Globalization;
using namespace msclr::interop;

namespace IFCCompressorGUI {

	// Gestionnaire pour le bouton de selection de fichier
	System::Void MainForm::buttonBrowseInput_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			textBoxInputFile->Text = openFileDialog->FileName;
			
			// Generer automatiquement le nom du fichier de sortie
			String^ outputFile = GenerateOutputFileName(openFileDialog->FileName);
			textBoxOutputFile->Text = outputFile;
			
			// Activer le bouton de compression
			buttonCompress->Enabled = true;
			
			LogMessage("Fichier IFC selectionne : " + openFileDialog->FileName);
			LogMessage("Fichier de sortie : " + outputFile);
		}
	}

	// Gestionnaire pour le bouton de compression
	System::Void MainForm::buttonCompress_Click(System::Object^ sender, System::EventArgs^ e)
	{
		// Valider l'entree FPR
		if (!ValidateFPRInput())
		{
			MessageBox::Show("Veuillez entrer une valeur FPR valide (exemple: 0.01)\n\n" +
							"Formats acceptes :\n" +
							"- 0.001 (haute precision)\n" +
							"- 0.01 (equilibre)\n" +
							"- 0.1 (compression maximale)", 
							"Erreur de validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return;
		}

		// Verifier que les fichiers sont specifies
		if (String::IsNullOrEmpty(textBoxInputFile->Text) || String::IsNullOrEmpty(textBoxOutputFile->Text))
		{
			MessageBox::Show("Veuillez selectionner un fichier IFC d'entree.", 
							"Fichier requis", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return;
		}

		// Verifier que le fichier d'entree existe
		if (!File::Exists(textBoxInputFile->Text))
		{
			MessageBox::Show("Le fichier d'entree n'existe pas.", 
							"Fichier introuvable", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		// Verifier que IFCCompressor.exe existe
		String^ compressorPath = GetIFCCompressorPath();
		if (String::IsNullOrEmpty(compressorPath))
		{
			MessageBox::Show("Impossible de trouver IFCCompressor.exe dans le dossier :\n" +
							"..\\Compression_IFC_makyar_CODE\\x64\\Release\\", 
							"Executable introuvable", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		// Demarrer la compression en arriere-plan
		UpdateUI(true);
		LogMessage("=== DEBUT DE LA COMPRESSION ===");
		LogMessage("Executable utilise : " + compressorPath);
		LogMessage("Fichier d'entree : " + textBoxInputFile->Text);
		LogMessage("Fichier de sortie : " + textBoxOutputFile->Text);
		LogMessage("Parametre FPR : " + textBoxFPR->Text);
		
		backgroundWorker->RunWorkerAsync();
	}

	// Gestionnaire pour ouvrir le dossier de sortie
	System::Void MainForm::buttonOpenFolder_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (!String::IsNullOrEmpty(outputFolder) && Directory::Exists(outputFolder))
		{
			Process::Start("explorer.exe", outputFolder);
		}
	}

	// Travail en arriere-plan (compression)
	System::Void MainForm::backgroundWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		BackgroundWorker^ worker = dynamic_cast<BackgroundWorker^>(sender);
		
		try
		{
			// Construire la ligne de commande avec formatage international pour FPR
			String^ compressorPath = GetIFCCompressorPath();
			
			// Convertir FPR en utilisant la culture invariante
			double fprValue = Double::Parse(textBoxFPR->Text, CultureInfo::InvariantCulture);
			String^ fprFormatted = fprValue.ToString("0.########", CultureInfo::InvariantCulture);
			
			String^ arguments = String::Format("\"{0}\" \"{1}\" -fpr {2}", 
											textBoxInputFile->Text,
											textBoxOutputFile->Text,
											fprFormatted);

			// Creer le processus
			ProcessStartInfo^ startInfo = gcnew ProcessStartInfo();
			startInfo->FileName = compressorPath;
			startInfo->Arguments = arguments;
			startInfo->UseShellExecute = false;
			startInfo->RedirectStandardOutput = true;
			startInfo->RedirectStandardError = true;
			startInfo->CreateNoWindow = true;
			startInfo->WindowStyle = ProcessWindowStyle::Hidden;

			Process^ process = gcnew Process();
			process->StartInfo = startInfo;
			
			// Demarrer le processus
			process->Start();
			
			// Lire la sortie en temps reel
			String^ output = "";
			String^ error = "";
			
			while (!process->HasExited)
			{
				if (!process->StandardOutput->EndOfStream)
				{
					String^ line = process->StandardOutput->ReadLine();
					if (!String::IsNullOrEmpty(line))
					{
						output += line + "\n";
						worker->ReportProgress(0, line);
					}
				}
				
				Threading::Thread::Sleep(100);
			}
			
			// Lire le reste de la sortie
			output += process->StandardOutput->ReadToEnd();
			error = process->StandardError->ReadToEnd();
			
			process->WaitForExit();
			int exitCode = process->ExitCode;
			
			// Preparer le resultat
			array<Object^>^ result = gcnew array<Object^>(3);
			result[0] = exitCode;
			result[1] = output;
			result[2] = error;
			
			e->Result = result;
		}
		catch (Exception^ ex)
		{
			e->Result = gcnew Exception("Erreur lors de l'execution : " + ex->Message);
		}
	}

	// Progression de la compression
	System::Void MainForm::backgroundWorker_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e)
	{
		if (e->UserState != nullptr)
		{
			String^ message = dynamic_cast<String^>(e->UserState);
			if (!String::IsNullOrEmpty(message))
			{
				LogMessage(message);
			}
		}
	}

	// Fin de la compression
	System::Void MainForm::backgroundWorker_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		UpdateUI(false);
		
		if (e->Error != nullptr)
		{
			LogMessage("ERREUR : " + e->Error->Message);
			MessageBox::Show("Erreur pendant la compression :\n" + e->Error->Message, 
							"Erreur", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
		else if (dynamic_cast<Exception^>(e->Result) != nullptr)
		{
			Exception^ ex = dynamic_cast<Exception^>(e->Result);
			LogMessage("ERREUR : " + ex->Message);
			MessageBox::Show("Erreur pendant la compression :\n" + ex->Message, 
							"Erreur", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
		else
		{
			array<Object^>^ result = dynamic_cast<array<Object^>^>(e->Result);
			int exitCode = (int)result[0];
			String^ output = dynamic_cast<String^>(result[1]);
			String^ error = dynamic_cast<String^>(result[2]);
			
			if (exitCode == 0)
			{
				LogMessage("=== COMPRESSION TERMINEE AVEC SUCCES ===");
				if (!String::IsNullOrEmpty(output))
				{
					LogMessage("Sortie du programme :");
					LogMessage(output);
				}
				
				// Verifier que le fichier de sortie existe
				if (File::Exists(textBoxOutputFile->Text))
				{
					FileInfo^ inputInfo = gcnew FileInfo(textBoxInputFile->Text);
					FileInfo^ outputInfo = gcnew FileInfo(textBoxOutputFile->Text);
					
					double compressionRatio = (double)outputInfo->Length / (double)inputInfo->Length;
					double compressionPercent = (1.0 - compressionRatio) * 100.0;
					
					// Utiliser la culture locale pour l'affichage des nombres
					LogMessage(String::Format("Taille originale : {0:N0} octets", inputInfo->Length));
					LogMessage(String::Format("Taille compressee : {0:N0} octets", outputInfo->Length));
					LogMessage(String::Format("Taux de compression : {0:F1}%", compressionPercent));
					
					outputFolder = Path::GetDirectoryName(textBoxOutputFile->Text);
					buttonOpenFolder->Visible = true;
					
					MessageBox::Show(String::Format("Compression terminee avec succes !\n\nTaux de compression : {0:F1}%\n\nFichier sauvegarde :\n{1}", 
													compressionPercent, textBoxOutputFile->Text),
									"Compression reussie", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
				else
				{
					LogMessage("ATTENTION : Le fichier de sortie n'a pas ete cree.");
					MessageBox::Show("La compression semble avoir reussi mais le fichier de sortie n'a pas ete trouve.", 
									"Avertissement", MessageBoxButtons::OK, MessageBoxIcon::Warning);
				}
			}
			else
			{
				LogMessage("=== ECHEC DE LA COMPRESSION ===");
				LogMessage("Code de sortie : " + exitCode.ToString());
				if (!String::IsNullOrEmpty(error))
				{
					LogMessage("Erreur : " + error);
				}
				if (!String::IsNullOrEmpty(output))
				{
					LogMessage("Sortie : " + output);
				}
				
				MessageBox::Show("La compression a echoue. Consultez le journal pour plus de details.", 
								"Echec de la compression", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}
	}

	// Obtenir le chemin vers IFCCompressor.exe
	String^ MainForm::GetIFCCompressorPath()
	{
		try
		{
			// Obtenir le dossier de l'executable actuel
			String^ currentPath = Application::StartupPath;
			
			// Remonter de deux niveaux et aller dans Compression_IFC_makyar_CODE
			String^ parentPath = Directory::GetParent(currentPath)->Parent->FullName;
			String^ compressorDir = Path::Combine(parentPath, "Compression_IFC_makyar_CODE");
			String^ compressorPath = Path::Combine(compressorDir, "x64\\Release\\IFCCompressor.exe");
			
			// Verifier que le fichier existe
			if (File::Exists(compressorPath))
			{
				return compressorPath;
			}
			
			// Essayer d'autres emplacements possibles
			compressorPath = Path::Combine(compressorDir, "IFCCompressor.exe");
			if (File::Exists(compressorPath))
			{
				return compressorPath;
			}
			
			compressorPath = Path::Combine(compressorDir, "Release\\IFCCompressor.exe");
			if (File::Exists(compressorPath))
			{
				return compressorPath;
			}
			
			return nullptr;
		}
		catch (Exception^)
		{
			return nullptr;
		}
	}

	// Ajouter un message au journal
	void MainForm::LogMessage(String^ message)
	{
		if (this->InvokeRequired)
		{
			this->Invoke(gcnew Action<String^>(this, &MainForm::LogMessage), message);
			return;
		}
		
		String^ timestamp = DateTime::Now.ToString("HH:mm:ss");
		textBoxLog->AppendText("[" + timestamp + "] " + message + "\n");
		textBoxLog->SelectionStart = textBoxLog->Text->Length;
		textBoxLog->ScrollToCaret();
	}

	// Mettre a jour l'interface utilisateur
	void MainForm::UpdateUI(bool compressing)
	{
		if (this->InvokeRequired)
		{
			this->Invoke(gcnew Action<bool>(this, &MainForm::UpdateUI), compressing);
			return;
		}
		
		buttonCompress->Enabled = !compressing && !String::IsNullOrEmpty(textBoxInputFile->Text);
		buttonBrowseInput->Enabled = !compressing;
		textBoxFPR->Enabled = !compressing;
		progressBar->Visible = compressing;
		labelProgress->Visible = compressing;
		
		if (compressing)
		{
			buttonOpenFolder->Visible = false;
			progressBar->Style = ProgressBarStyle::Marquee;
		}
	}

	// Valider l'entree FPR - VERSION CORRIGEE
	bool MainForm::ValidateFPRInput()
	{
		try
		{
			// CORRECTION : Utiliser CultureInfo::InvariantCulture pour accepter
			// le format international "0.001" au lieu du format local "0,001"
			double fprValue = Double::Parse(textBoxFPR->Text, CultureInfo::InvariantCulture);
			
			// Verifier que la valeur est dans la plage acceptable
			bool isValid = fprValue > 0.0 && fprValue <= 1.0;
			
			if (isValid)
			{
				// Optionnel : reformater la valeur pour uniformiser l'affichage
				textBoxFPR->Text = fprValue.ToString("0.########", CultureInfo::InvariantCulture);
			}
			
			return isValid;
		}
		catch (Exception^ ex)
		{
			// Log de debug pour identifier les problemes
			LogMessage("Erreur de validation FPR : " + ex->Message);
			LogMessage("Valeur saisie : '" + textBoxFPR->Text + "'");
			return false;
		}
	}

	// Generer le nom du fichier de sortie
	String^ MainForm::GenerateOutputFileName(String^ inputFile)
	{
		try
		{
			String^ directory = Path::GetDirectoryName(inputFile);
			String^ fileNameWithoutExt = Path::GetFileNameWithoutExtension(inputFile);
			String^ extension = Path::GetExtension(inputFile);
			
			return Path::Combine(directory, fileNameWithoutExt + "_COMPRESSED" + extension);
		}
		catch (Exception^)
		{
			return inputFile + "_COMPRESSED.ifc";
		}
	}
}