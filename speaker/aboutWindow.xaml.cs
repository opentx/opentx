using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace OpenTXspeak
{
    public partial class AboutWindow : Window
    {
        public AboutWindow()
        {
            InitializeComponent();
            Assembly app = Assembly.GetExecutingAssembly();

            AssemblyTitleAttribute title = (AssemblyTitleAttribute)app.GetCustomAttributes(typeof(AssemblyTitleAttribute), false)[0];
            AssemblyProductAttribute product = (AssemblyProductAttribute)app.GetCustomAttributes(typeof(AssemblyProductAttribute), false)[0];
            AssemblyCopyrightAttribute copyright = (AssemblyCopyrightAttribute)app.GetCustomAttributes(typeof(AssemblyCopyrightAttribute), false)[0];
            AssemblyCompanyAttribute company = (AssemblyCompanyAttribute)app.GetCustomAttributes(typeof(AssemblyCompanyAttribute), false)[0];
            AssemblyDescriptionAttribute description = (AssemblyDescriptionAttribute)app.GetCustomAttributes(typeof(AssemblyDescriptionAttribute), false)[0];
            Version version = app.GetName().Version;
           
            this.Title = String.Format("About {0}", title.Title);
            this.lblProductName.Content = product.Product;
            this.lblVersion.Content = String.Format("Version {0}", version.ToString());
            this.lblCopyright.Content = copyright.Copyright.ToString();
            this.lblCompanyName.Content = company.Company;
            this.description.Text = description.Description;
        }
    }
}
