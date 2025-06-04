namespace IFCCompressorGUI
{
    partial class MainForm
    {
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.PictureBox pictureBoxLogo;
        private System.Windows.Forms.Label lblInputFile;
        private System.Windows.Forms.TextBox txtInputFile;
        private System.Windows.Forms.Button btnBrowseInput;
        private System.Windows.Forms.Label lblOutputFile;
        private System.Windows.Forms.TextBox txtOutputFile;
        private System.Windows.Forms.Button btnBrowseOutput;
        private System.Windows.Forms.Label lblFPR;
        private System.Windows.Forms.ComboBox cmbFPR;
        private System.Windows.Forms.Button btnCompress;
        private System.Windows.Forms.Label lblStatus;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.pictureBoxLogo = new System.Windows.Forms.PictureBox();
            this.lblInputFile = new System.Windows.Forms.Label();
            this.txtInputFile = new System.Windows.Forms.TextBox();
            this.btnBrowseInput = new System.Windows.Forms.Button();
            this.lblOutputFile = new System.Windows.Forms.Label();
            this.txtOutputFile = new System.Windows.Forms.TextBox();
            this.btnBrowseOutput = new System.Windows.Forms.Button();
            this.lblFPR = new System.Windows.Forms.Label();
            this.cmbFPR = new System.Windows.Forms.ComboBox();
            this.btnCompress = new System.Windows.Forms.Button();
            this.lblStatus = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBoxLogo
            // 
            this.pictureBoxLogo.Location = new System.Drawing.Point(12, 12);
            this.pictureBoxLogo.Name = "pictureBoxLogo";
            this.pictureBoxLogo.Size = new System.Drawing.Size(460, 80); // Adjust size as needed
            this.pictureBoxLogo.TabIndex = 0;
            this.pictureBoxLogo.TabStop = false;
            // 
            // lblInputFile
            // 
            this.lblInputFile.AutoSize = true;
            this.lblInputFile.Location = new System.Drawing.Point(12, 105);
            this.lblInputFile.Name = "lblInputFile";
            this.lblInputFile.Size = new System.Drawing.Size(76, 13);
            this.lblInputFile.TabIndex = 1;
            this.lblInputFile.Text = "Input IFC File:";
            // 
            // txtInputFile
            // 
            this.txtInputFile.Location = new System.Drawing.Point(15, 121);
            this.txtInputFile.Name = "txtInputFile";
            this.txtInputFile.ReadOnly = true; // Or allow typing
            this.txtInputFile.Size = new System.Drawing.Size(376, 20);
            this.txtInputFile.TabIndex = 2;
            this.txtInputFile.TextChanged += new System.EventHandler(this.txtInputFile_TextChanged);
            // 
            // btnBrowseInput
            // 
            this.btnBrowseInput.Location = new System.Drawing.Point(397, 119);
            this.btnBrowseInput.Name = "btnBrowseInput";
            this.btnBrowseInput.Size = new System.Drawing.Size(75, 23);
            this.btnBrowseInput.TabIndex = 3;
            this.btnBrowseInput.Text = "Browse...";
            this.btnBrowseInput.UseVisualStyleBackColor = true;
            this.btnBrowseInput.Click += new System.EventHandler(this.btnBrowseInput_Click);
            // 
            // lblOutputFile
            // 
            this.lblOutputFile.AutoSize = true;
            this.lblOutputFile.Location = new System.Drawing.Point(12, 154);
            this.lblOutputFile.Name = "lblOutputFile";
            this.lblOutputFile.Size = new System.Drawing.Size(84, 13);
            this.lblOutputFile.TabIndex = 4;
            this.lblOutputFile.Text = "Output IFC File:";
            // 
            // txtOutputFile
            // 
            this.txtOutputFile.Location = new System.Drawing.Point(15, 170);
            this.txtOutputFile.Name = "txtOutputFile";
            this.txtOutputFile.Size = new System.Drawing.Size(376, 20);
            this.txtOutputFile.TabIndex = 5;
            // 
            // btnBrowseOutput
            // 
            this.btnBrowseOutput.Location = new System.Drawing.Point(397, 168);
            this.btnBrowseOutput.Name = "btnBrowseOutput";
            this.btnBrowseOutput.Size = new System.Drawing.Size(75, 23);
            this.btnBrowseOutput.TabIndex = 6;
            this.btnBrowseOutput.Text = "Browse...";
            this.btnBrowseOutput.UseVisualStyleBackColor = true;
            this.btnBrowseOutput.Click += new System.EventHandler(this.btnBrowseOutput_Click);
            // 
            // lblFPR
            // 
            this.lblFPR.AutoSize = true;
            this.lblFPR.Location = new System.Drawing.Point(12, 203);
            this.lblFPR.Name = "lblFPR";
            this.lblFPR.Size = new System.Drawing.Size(62, 13);
            this.lblFPR.TabIndex = 7;
            this.lblFPR.Text = "FPR Level:";
            // 
            // cmbFPR
            // 
            this.cmbFPR.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbFPR.FormattingEnabled = true;
            this.cmbFPR.Location = new System.Drawing.Point(15, 219);
            this.cmbFPR.Name = "cmbFPR";
            this.cmbFPR.Size = new System.Drawing.Size(121, 21);
            this.cmbFPR.TabIndex = 8;
            // 
            // btnCompress
            // 
            this.btnCompress.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnCompress.Location = new System.Drawing.Point(15, 255);
            this.btnCompress.Name = "btnCompress";
            this.btnCompress.Size = new System.Drawing.Size(457, 40);
            this.btnCompress.TabIndex = 9;
            this.btnCompress.Text = "COMPRESS IFC";
            this.btnCompress.UseVisualStyleBackColor = true;
            this.btnCompress.Click += new System.EventHandler(this.btnCompress_Click);
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Location = new System.Drawing.Point(12, 308);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(43, 13);
            this.lblStatus.TabIndex = 10;
            this.lblStatus.Text = "Ready.";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(484, 341); // Adjust size as needed
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.btnCompress);
            this.Controls.Add(this.cmbFPR);
            this.Controls.Add(this.lblFPR);
            this.Controls.Add(this.btnBrowseOutput);
            this.Controls.Add(this.txtOutputFile);
            this.Controls.Add(this.lblOutputFile);
            this.Controls.Add(this.btnBrowseInput);
            this.Controls.Add(this.txtInputFile);
            this.Controls.Add(this.lblInputFile);
            this.Controls.Add(this.pictureBoxLogo);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "IFC Optimizer";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();
        }
    }
}