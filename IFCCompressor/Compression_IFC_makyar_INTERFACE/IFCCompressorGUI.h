#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <process.h>
#include <fstream>
#include <filesystem>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Globalization;

namespace IFCCompressorGUI {

	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			// Initialiser les valeurs par defaut
			textBoxFPR->Text = "0,01";
			textBoxInputFile->Text = "";
			textBoxOutputFile->Text = "";
			progressBar->Visible = false;
			labelProgress->Visible = false;
			buttonOpenFolder->Visible = false;
			
			// Desactiver le bouton de compression au debut
			buttonCompress->Enabled = false;
		}

	protected:
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: 
		// Controles de l'interface
		System::Windows::Forms::Label^ labelTitle;
		System::Windows::Forms::Label^ labelInputFile;
		System::Windows::Forms::TextBox^ textBoxInputFile;
		System::Windows::Forms::Button^ buttonBrowseInput;
		System::Windows::Forms::Label^ labelOutputFile;
		System::Windows::Forms::TextBox^ textBoxOutputFile;
		System::Windows::Forms::Label^ labelFPR;
		System::Windows::Forms::TextBox^ textBoxFPR;
		System::Windows::Forms::Label^ labelFPRInfo;
		System::Windows::Forms::Button^ buttonCompress;
		System::Windows::Forms::ProgressBar^ progressBar;
		System::Windows::Forms::Label^ labelProgress;
		System::Windows::Forms::RichTextBox^ textBoxLog;
		System::Windows::Forms::Button^ buttonOpenFolder;
		System::Windows::Forms::OpenFileDialog^ openFileDialog;
		System::ComponentModel::Container ^components;

	private:
		// Variables pour la compression
		String^ outputFolder;
		Process^ compressionProcess;
		BackgroundWorker^ backgroundWorker;

	private:
		/// <summary>
		/// Designer variable necessaire
		/// </summary>

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->labelTitle = (gcnew System::Windows::Forms::Label());
			this->labelInputFile = (gcnew System::Windows::Forms::Label());
			this->textBoxInputFile = (gcnew System::Windows::Forms::TextBox());
			this->buttonBrowseInput = (gcnew System::Windows::Forms::Button());
			this->labelOutputFile = (gcnew System::Windows::Forms::Label());
			this->textBoxOutputFile = (gcnew System::Windows::Forms::TextBox());
			this->labelFPR = (gcnew System::Windows::Forms::Label());
			this->textBoxFPR = (gcnew System::Windows::Forms::TextBox());
			this->labelFPRInfo = (gcnew System::Windows::Forms::Label());
			this->buttonCompress = (gcnew System::Windows::Forms::Button());
			this->progressBar = (gcnew System::Windows::Forms::ProgressBar());
			this->labelProgress = (gcnew System::Windows::Forms::Label());
			this->textBoxLog = (gcnew System::Windows::Forms::RichTextBox());
			this->buttonOpenFolder = (gcnew System::Windows::Forms::Button());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->backgroundWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->SuspendLayout();
			
			// 
			// labelTitle
			// 
			this->labelTitle->AutoSize = true;
			this->labelTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelTitle->ForeColor = System::Drawing::Color::DarkBlue;
			this->labelTitle->Location = System::Drawing::Point(12, 9);
			this->labelTitle->Name = L"labelTitle";
			this->labelTitle->Size = System::Drawing::Size(521, 24);
			this->labelTitle->TabIndex = 0;
			this->labelTitle->Text = L"IFCCompressor - Interface Graphique de Compression IFC";
			
			// 
			// labelInputFile
			// 
			this->labelInputFile->AutoSize = true;
			this->labelInputFile->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelInputFile->Location = System::Drawing::Point(13, 50);
			this->labelInputFile->Name = L"labelInputFile";
			this->labelInputFile->Size = System::Drawing::Size(126, 16);
			this->labelInputFile->TabIndex = 1;
			this->labelInputFile->Text = L"Fichier IFC d'entree :";
			
			// 
			// textBoxInputFile
			// 
			this->textBoxInputFile->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxInputFile->Location = System::Drawing::Point(16, 69);
			this->textBoxInputFile->Name = L"textBoxInputFile";
			this->textBoxInputFile->ReadOnly = true;
			this->textBoxInputFile->Size = System::Drawing::Size(450, 22);
			this->textBoxInputFile->TabIndex = 2;
			
			// 
			// buttonBrowseInput
			// 
			this->buttonBrowseInput->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buttonBrowseInput->Location = System::Drawing::Point(472, 69);
			this->buttonBrowseInput->Name = L"buttonBrowseInput";
			this->buttonBrowseInput->Size = System::Drawing::Size(100, 23);
			this->buttonBrowseInput->TabIndex = 3;
			this->buttonBrowseInput->Text = L"Parcourir...";
			this->buttonBrowseInput->UseVisualStyleBackColor = true;
			this->buttonBrowseInput->Click += gcnew System::EventHandler(this, &MainForm::buttonBrowseInput_Click);
			
			// 
			// labelOutputFile
			// 
			this->labelOutputFile->AutoSize = true;
			this->labelOutputFile->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelOutputFile->Location = System::Drawing::Point(13, 105);
			this->labelOutputFile->Name = L"labelOutputFile";
			this->labelOutputFile->Size = System::Drawing::Size(125, 16);
			this->labelOutputFile->TabIndex = 4;
			this->labelOutputFile->Text = L"Fichier de sortie :";
			
			// 
			// textBoxOutputFile
			// 
			this->textBoxOutputFile->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOutputFile->Location = System::Drawing::Point(16, 124);
			this->textBoxOutputFile->Name = L"textBoxOutputFile";
			this->textBoxOutputFile->ReadOnly = true;
			this->textBoxOutputFile->Size = System::Drawing::Size(556, 22);
			this->textBoxOutputFile->TabIndex = 5;
			
			// 
			// labelFPR
			// 
			this->labelFPR->AutoSize = true;
			this->labelFPR->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelFPR->Location = System::Drawing::Point(13, 160);
			this->labelFPR->Name = L"labelFPR";
			this->labelFPR->Size = System::Drawing::Size(102, 16);
			this->labelFPR->TabIndex = 6;
			this->labelFPR->Text = L"Parametre FPR :";
			
			// 
			// textBoxFPR
			// 
			this->textBoxFPR->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxFPR->Location = System::Drawing::Point(121, 157);
			this->textBoxFPR->Name = L"textBoxFPR";
			this->textBoxFPR->Size = System::Drawing::Size(100, 22);
			this->textBoxFPR->TabIndex = 7;
			this->textBoxFPR->Text = L"0,01";
			
			// 
			// labelFPRInfo
			// 
			this->labelFPRInfo->AutoSize = true;
			this->labelFPRInfo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelFPRInfo->ForeColor = System::Drawing::Color::Gray;
			this->labelFPRInfo->Location = System::Drawing::Point(227, 160);
			this->labelFPRInfo->Name = L"labelFPRInfo";
			this->labelFPRInfo->Size = System::Drawing::Size(345, 13);
			this->labelFPRInfo->TabIndex = 8;
			this->labelFPRInfo->Text = L"(Precision d'arrondi : 0,001=haute precision, 0,01=equilibre, 0,1=compression max)";
			
			// 
			// buttonCompress
			// 
			this->buttonCompress->BackColor = System::Drawing::Color::Green;
			this->buttonCompress->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buttonCompress->ForeColor = System::Drawing::Color::White;
			this->buttonCompress->Location = System::Drawing::Point(200, 195);
			this->buttonCompress->Name = L"buttonCompress";
			this->buttonCompress->Size = System::Drawing::Size(200, 40);
			this->buttonCompress->TabIndex = 9;
			this->buttonCompress->Text = L"DEMARRER LA COMPRESSION";
			this->buttonCompress->UseVisualStyleBackColor = false;
			this->buttonCompress->Click += gcnew System::EventHandler(this, &MainForm::buttonCompress_Click);
			
			// 
			// progressBar
			// 
			this->progressBar->Location = System::Drawing::Point(16, 250);
			this->progressBar->Name = L"progressBar";
			this->progressBar->Size = System::Drawing::Size(556, 23);
			this->progressBar->Style = System::Windows::Forms::ProgressBarStyle::Marquee;
			this->progressBar->TabIndex = 10;
			this->progressBar->Visible = false;
			
			// 
			// labelProgress
			// 
			this->labelProgress->AutoSize = true;
			this->labelProgress->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelProgress->Location = System::Drawing::Point(13, 231);
			this->labelProgress->Name = L"labelProgress";
			this->labelProgress->Size = System::Drawing::Size(134, 16);
			this->labelProgress->TabIndex = 11;
			this->labelProgress->Text = L"Compression en cours...";
			this->labelProgress->Visible = false;
			
			// 
			// textBoxLog
			// 
			this->textBoxLog->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxLog->Location = System::Drawing::Point(16, 285);
			this->textBoxLog->Name = L"textBoxLog";
			this->textBoxLog->ReadOnly = true;
			this->textBoxLog->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::Vertical;
			this->textBoxLog->Size = System::Drawing::Size(556, 200);
			this->textBoxLog->TabIndex = 12;
			this->textBoxLog->Text = L"Pret pour la compression. Selectionnez un fichier IFC pour commencer.";
			
			// 
			// buttonOpenFolder
			// 
			this->buttonOpenFolder->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buttonOpenFolder->Location = System::Drawing::Point(420, 195);
			this->buttonOpenFolder->Name = L"buttonOpenFolder";
			this->buttonOpenFolder->Size = System::Drawing::Size(152, 40);
			this->buttonOpenFolder->TabIndex = 13;
			this->buttonOpenFolder->Text = L"Ouvrir le dossier";
			this->buttonOpenFolder->UseVisualStyleBackColor = true;
			this->buttonOpenFolder->Visible = false;
			this->buttonOpenFolder->Click += gcnew System::EventHandler(this, &MainForm::buttonOpenFolder_Click);
			
			// 
			// openFileDialog
			// 
			this->openFileDialog->Filter = L"Fichiers IFC (*.ifc)|*.ifc|Tous les fichiers (*.*)|*.*";
			this->openFileDialog->Title = L"Selectionner un fichier IFC";
			
			// 
			// backgroundWorker
			// 
			this->backgroundWorker->WorkerReportsProgress = true;
			this->backgroundWorker->WorkerSupportsCancellation = true;
			this->backgroundWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainForm::backgroundWorker_DoWork);
			this->backgroundWorker->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &MainForm::backgroundWorker_ProgressChanged);
			this->backgroundWorker->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainForm::backgroundWorker_RunWorkerCompleted);
			
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(584, 497);
			this->Controls->Add(this->buttonOpenFolder);
			this->Controls->Add(this->textBoxLog);
			this->Controls->Add(this->labelProgress);
			this->Controls->Add(this->progressBar);
			this->Controls->Add(this->buttonCompress);
			this->Controls->Add(this->labelFPRInfo);
			this->Controls->Add(this->textBoxFPR);
			this->Controls->Add(this->labelFPR);
			this->Controls->Add(this->textBoxOutputFile);
			this->Controls->Add(this->labelOutputFile);
			this->Controls->Add(this->buttonBrowseInput);
			this->Controls->Add(this->textBoxInputFile);
			this->Controls->Add(this->labelInputFile);
			this->Controls->Add(this->labelTitle);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"IFCCompressor - Interface Graphique";
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

	// Gestionnaires d'evenements
	private: System::Void buttonBrowseInput_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void buttonCompress_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void buttonOpenFolder_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void backgroundWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
	private: System::Void backgroundWorker_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e);
	private: System::Void backgroundWorker_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);

	// Methodes utilitaires
	private: String^ GetIFCCompressorPath();
	private: void LogMessage(String^ message);
	private: void UpdateUI(bool compressing);
	private: bool ValidateFPRInput();
	private: String^ GenerateOutputFileName(String^ inputFile);
	};
}