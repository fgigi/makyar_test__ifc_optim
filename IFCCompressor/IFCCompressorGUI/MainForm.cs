using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Globalization;

namespace IFCCompressorGUI
{
    public partial class MainForm : Form
    {
        private const string IFC_COMPRESSOR_EXE = "IFCCompressor.exe"; // Name of your C++ executable

        public MainForm()
        {
            InitializeComponent();
            InitializeCustomComponents();
        }

        private void InitializeCustomComponents()
        {
            // Load Logo
            string logoPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ressources", "images", "fond.png");
            if (File.Exists(logoPath))
            {
                try
                {
                    pictureBoxLogo.Image = Image.FromFile(logoPath);
                    pictureBoxLogo.SizeMode = PictureBoxSizeMode.Zoom; // Or StretchImage, CenterImage
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error loading logo: {ex.Message}", "Logo Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            else
            {
                // Optional: provide a default visual or text if logo is missing
                pictureBoxLogo.BackColor = Color.LightGray;
                // Add a label to picturebox if you want text
                Label noLogoLabel = new Label { Text = "Logo not found", AutoSize = true, TextAlign = ContentAlignment.MiddleCenter, Dock = DockStyle.Fill };
                pictureBoxLogo.Controls.Add(noLogoLabel);
            }


            // Load Icon
            string iconPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ressources", "icons", "app.ico");
            if (File.Exists(iconPath))
            {
                try
                {
                    this.Icon = new Icon(iconPath);
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error loading icon: {ex.Message}", "Icon Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }

            // Populate FPR ComboBox
            cmbFPR.Items.AddRange(new object[] { "0.1", "0.01", "0.001", "0.0001" }); // Add more as needed
            cmbFPR.SelectedIndex = 1; // Default to 0,01

            // Set initial state for output textbox
            txtOutputFile.Enabled = false;
            btnBrowseOutput.Enabled = false;
        }

        private void btnBrowseInput_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openFileDialog = new OpenFileDialog())
            {
                openFileDialog.Filter = "IFC files (*.ifc)|*.ifc|All files (*.*)|*.*";
                openFileDialog.RestoreDirectory = true;

                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    txtInputFile.Text = openFileDialog.FileName;
                }
            }
        }

        private void txtInputFile_TextChanged(object sender, EventArgs e)
        {
            if (!string.IsNullOrWhiteSpace(txtInputFile.Text) && File.Exists(txtInputFile.Text))
            {
                string dir = Path.GetDirectoryName(txtInputFile.Text);
                string fileNameWithoutExt = Path.GetFileNameWithoutExtension(txtInputFile.Text);
                string ext = Path.GetExtension(txtInputFile.Text);
                txtOutputFile.Text = Path.Combine(dir, $"{fileNameWithoutExt}_OPTIMISE{ext}");
                txtOutputFile.Enabled = true;
                btnBrowseOutput.Enabled = true;
            }
            else
            {
                txtOutputFile.Text = "";
                txtOutputFile.Enabled = false;
                btnBrowseOutput.Enabled = false;
            }
        }

        private void btnBrowseOutput_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog saveFileDialog = new SaveFileDialog())
            {
                saveFileDialog.Filter = "IFC files (*.ifc)|*.ifc|All files (*.*)|*.*";
                if (!string.IsNullOrWhiteSpace(txtOutputFile.Text))
                {
                    saveFileDialog.InitialDirectory = Path.GetDirectoryName(txtOutputFile.Text);
                    saveFileDialog.FileName = Path.GetFileName(txtOutputFile.Text);
                }
                else if (!string.IsNullOrWhiteSpace(txtInputFile.Text))
                {
                     string dir = Path.GetDirectoryName(txtInputFile.Text);
                     string fileNameWithoutExt = Path.GetFileNameWithoutExtension(txtInputFile.Text);
                     string ext = Path.GetExtension(txtInputFile.Text);
                     saveFileDialog.FileName = Path.Combine(dir, $"{fileNameWithoutExt}_OPTIMISE{ext}");
                }


                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    txtOutputFile.Text = saveFileDialog.FileName;
                }
            }
        }

        private async void btnCompress_Click(object sender, EventArgs e)
        {
            string inputFile = txtInputFile.Text;
            string outputFile = txtOutputFile.Text;
            string fpr = cmbFPR.SelectedItem.ToString();

            // Validations
            if (string.IsNullOrWhiteSpace(inputFile) || !File.Exists(inputFile))
            {
                MessageBox.Show("Please select a valid input IFC file.", "Input Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (string.IsNullOrWhiteSpace(outputFile))
            {
                MessageBox.Show("Please specify a valid output file path.", "Output Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (string.IsNullOrWhiteSpace(fpr))
            {
                MessageBox.Show("Please select an FPR value.", "FPR Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string compressorPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, IFC_COMPRESSOR_EXE);
            if (!File.Exists(compressorPath))
            {
                MessageBox.Show($"Compressor executable '{IFC_COMPRESSOR_EXE}' not found in the application directory: {AppDomain.CurrentDomain.BaseDirectory}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }


            lblStatus.Text = "Processing... Please wait.";
            this.Enabled = false; // Disable form during processing
            Application.DoEvents(); // Refresh UI

            long originalSize = new FileInfo(inputFile).Length;
            long compressedSize = 0;
            int entitiesBefore = 0;
            int entitiesAfter = 0;
            int representationsRemoved = 0; // New variable to store this
            StringBuilder processOutput = new StringBuilder();
            StringBuilder processError = new StringBuilder();

            Stopwatch stopwatch = Stopwatch.StartNew();

            try
            {
                ProcessStartInfo startInfo = new ProcessStartInfo
                {
                    FileName = compressorPath,
                    Arguments = $"\"{inputFile}\" \"{outputFile}\" {fpr}",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    StandardOutputEncoding = Encoding.UTF8, // Match C++ output encoding if necessary
                    StandardErrorEncoding = Encoding.UTF8
                };

                using (Process process = new Process { StartInfo = startInfo })
                {
                    process.OutputDataReceived += (s, data) => { if (data.Data != null) processOutput.AppendLine(data.Data); };
                    process.ErrorDataReceived += (s, data) => { if (data.Data != null) processError.AppendLine(data.Data); };

                    process.Start();
                    process.BeginOutputReadLine();
                    process.BeginErrorReadLine();
                    
                    await Task.Run(() => process.WaitForExit()); // Asynchronously wait

                    if (process.ExitCode != 0)
                    {
                        MessageBox.Show($"Error during compression (Exit Code: {process.ExitCode}):\n{processOutput}\n{processError}", "Compression Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        lblStatus.Text = "Compression failed.";
                        this.Enabled = true;
                        return;
                    }
                }

                if (File.Exists(outputFile))
                {
                    compressedSize = new FileInfo(outputFile).Length;
                }
                else
                {
                     MessageBox.Show($"Output file was not created: {outputFile}\nLog:\n{processOutput}\n{processError}", "Output Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                     lblStatus.Text = "Compression failed (output not found).";
                     this.Enabled = true;
                     return;
                }


                // Parse output from C++ console
                // Adjust these regex patterns based on the EXACT output of your C++ program
                var entitiesBeforeMatch = Regex.Match(processOutput.ToString(), @"Number of entities before compression:\s*(\d+)");
                if (entitiesBeforeMatch.Success) int.TryParse(entitiesBeforeMatch.Groups[1].Value, out entitiesBefore);

                var entitiesAfterMatch = Regex.Match(processOutput.ToString(), @"Number of entities after compression:\s*(\d+)");
                if (entitiesAfterMatch.Success) int.TryParse(entitiesAfterMatch.Groups[1].Value, out entitiesAfter);
                
                // Example from your C++ code: "Successfully removed X representations based on FPR Y."
                var representationsRemovedMatch = Regex.Match(processOutput.ToString(), @"Successfully removed (\d+) representations based on FPR");
                if (representationsRemovedMatch.Success) int.TryParse(representationsRemovedMatch.Groups[1].Value, out representationsRemoved);


                stopwatch.Stop();
                lblStatus.Text = $"Compression successful! Time: {stopwatch.Elapsed.TotalSeconds:F2}s. Output: {outputFile}";
                MessageBox.Show("Compression successful!", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);

                // Generate summary report
                GenerateSummaryReport(inputFile, outputFile, originalSize, compressedSize, fpr, entitiesBefore, entitiesAfter, representationsRemoved, processOutput.ToString());

            }
            catch (Exception ex)
            {
                stopwatch.Stop();
                MessageBox.Show($"An unexpected error occurred: {ex.Message}\n\nDetails:\n{processOutput}\n{processError}", "Runtime Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                lblStatus.Text = "An error occurred.";
            }
            finally
            {
                this.Enabled = true; // Re-enable form
            }
        }

        private void GenerateSummaryReport(string inputFile, string outputFile, long originalSize, long compressedSize, string fpr, int entitiesBefore, int entitiesAfter, int representationsRemoved, string consoleLog)
        {
            double compressionRatio = 0;
            if (originalSize > 0)
            {
                compressionRatio = (1.0 - (double)compressedSize / originalSize) * 100.0;
            }

            string deletedInfo = $"Entities removed: {entitiesBefore - entitiesAfter}. Representations removed (based on FPR): {representationsRemoved}.";
            if (representationsRemoved == 0 && (entitiesBefore - entitiesAfter > 0)) {
                deletedInfo = $"Entities removed: {entitiesBefore - entitiesAfter}. (Detailed representation removal count not found in log)";
            } else if (representationsRemoved == 0 && (entitiesBefore - entitiesAfter == 0)) {
                 deletedInfo = "No entities or representations reported as removed.";
            }


            List<string> reportLines = new List<string>
            {
                "IFC Compression Summary",
                $"Timestamp: {DateTime.Now:yyyy-MM-dd HH:mm:ss}",
                "--- Details ---",
                $"Original File: {inputFile}",
                $"Original Size (bytes): {originalSize}",
                $"Compressed File: {outputFile}",
                $"Compressed Size (bytes): {compressedSize}",
                $"Compression Ratio: {compressionRatio:F2}%",
                $"FPR Level Chosen: {fpr}",
                $"Entities Before: {entitiesBefore}",
                $"Entities After: {entitiesAfter}",
                $"What was deleted: {deletedInfo}",
                "--- C++ Compressor Log ---",
                $"{consoleLog.Trim()}"
            };

            using (SaveFileDialog saveFileDialog = new SaveFileDialog())
            {
                saveFileDialog.Filter = "CSV files (*.csv)|*.csv|Text files (*.txt)|*.txt";
                saveFileDialog.Title = "Save Compression Summary Report";
                saveFileDialog.FileName = $"CompressionReport_{Path.GetFileNameWithoutExtension(inputFile)}_{DateTime.Now:yyyyMMddHHmmss}.csv";

                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        // For CSV, we'll make it simple: one key-value pair per line for readability in CSV.
                        // Or a more structured CSV with headers. Let's do structured.
                        StringBuilder csvContent = new StringBuilder();
                        csvContent.AppendLine("Category,Value");
                        csvContent.AppendLine($"Timestamp,\"{DateTime.Now:yyyy-MM-dd HH:mm:ss}\"");
                        csvContent.AppendLine($"Original File,\"{inputFile}\"");
                        csvContent.AppendLine($"Original Size (bytes),{originalSize}");
                        csvContent.AppendLine($"Compressed File,\"{outputFile}\"");
                        csvContent.AppendLine($"Compressed Size (bytes),{compressedSize}");
                        csvContent.AppendLine($"Compression Ratio (%),\"{compressionRatio.ToString("F2", CultureInfo.InvariantCulture)}\"");
                        csvContent.AppendLine($"FPR Level Chosen,\"{fpr}\"");
                        csvContent.AppendLine($"Entities Before,{entitiesBefore}");
                        csvContent.AppendLine($"Entities After,{entitiesAfter}");
                        csvContent.AppendLine($"Entities Removed,{entitiesBefore - entitiesAfter}");
                        csvContent.AppendLine($"Representations Removed (from log),{representationsRemoved}");
                        // For multi-line log, it's tricky in simple CSV. Let's put it in one cell, escaping quotes.
                        string cleanedLog = consoleLog.Trim().Replace("\"", "\"\""); // Escape quotes for CSV
                        csvContent.AppendLine($"C++ Compressor Log,\"{cleanedLog}\"");


                        File.WriteAllText(saveFileDialog.FileName, csvContent.ToString());
                        MessageBox.Show($"Summary report saved to: {saveFileDialog.FileName}", "Report Saved", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show($"Error saving report: {ex.Message}", "Report Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }
    }
}