#include "IFCCompressorGUI.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Activer les styles visuels Windows
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// Creer et afficher la fenetre principale
	IFCCompressorGUI::MainForm^ form = gcnew IFCCompressorGUI::MainForm();
	Application::Run(form);

	return 0;
}