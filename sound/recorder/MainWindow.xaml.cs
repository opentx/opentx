/* This file is part of OpenTX Recorder.
 * OpenTX Recorder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * OpenTX Recorder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Copyright 2014 Kjell Kernen  */


using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Media;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Navigation;
using System.Runtime.InteropServices;

namespace OpenTXrecorder
{
    public partial class MainWindow : Window
    {
        [DllImport("winmm.dll", EntryPoint = "mciSendStringA", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        private static extern int mciSendString(string lpstrCommand, string lpstrReturnString, int uReturnLength, int hwndCallback);

        SentenceTables tables = new SentenceTables();
        Languages languages = new Languages();
        Language curLang;
        Sentences sentences = new Sentences();
        int added_files = 0;
        bool isRecording = false;

        public MainWindow()
        {
            SplashScreen splash = new SplashScreen("recorder_logo.png");
            splash.Show(true);
            Thread.Sleep(1500);

            InitializeComponent();

            lvSentences.ItemsSource = sentences;
            cbLanguages.ItemsSource = languages;
            languages.Add("English", "en");
            languages.Add("Czech", "cz");
            languages.Add("German", "de");
            languages.Add("French", "fr");
            languages.Add("Italian", "it");
            languages.Add("Polish", "pl");
            languages.Add("Portuguese", "pt");
            languages.Add("Swedish", "se");
            languages.Add("Slovak", "sk");
            languages.Add("Spanish", "es");
            curLang = languages[0];
            cbLanguages.SelectedIndex = 0; // Note: Sets current langugage -> triggers loadlanguage() 
        }

        private void loadLanguage()
        {
            string[] system_strings;
            string[] other_strings;

            try
            {
                system_strings = System.IO.File.ReadAllLines(@"SOUNDS\" + curLang.sName + @"\SYSTEM\system_sounds.txt");
                other_strings = System.IO.File.ReadAllLines(@"SOUNDS\" + curLang.sName + @"\other_sounds.txt");
            }
            catch (IOException)
            {
                system_strings = tables.default_system_strings[tables.toInt(curLang.sName)];
                other_strings = tables.default_other_strings[tables.toInt(curLang.sName)];
            }
            sentences.RemoveRange(0, sentences.Count);

            foreach (string str in system_strings)
                sentences.Add(str, @"SOUNDS\" + curLang.sName + @"\SYSTEM\");

            sentences.Add(@";^ System Sounds ^;", "");

            foreach (string str in other_strings)
                sentences.Add(str, @"SOUNDS\" + curLang.sName + @"\");

            lvSentences.Items.Refresh(); // Workaround - Two way binding is better
        }

        private void saveLanguage()
        {
            System.IO.Directory.CreateDirectory(@"SOUNDS\" + curLang.sName + @"\SYSTEM");
            StreamWriter sw = File.CreateText(@"SOUNDS\" + curLang.sName + @"\SYSTEM\system_sounds.txt");
            int i;
            for (i = 0; sentences[i].fileName != ""; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
            }
            sw.Close();
            sw = File.CreateText(@"SOUNDS\" + curLang.sName + @"\other_sounds.txt");
            for (i++; i < sentences.Count; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
            }
            sw.Close();
        }

        private void cbLanguages_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            curLang = (Language)e.AddedItems[0];
            loadLanguage();
        }

        private void lvSentences_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (this.lvSentences.SelectedItems.Count < 1) return;

            Sentence sentence = (Sentence)this.lvSentences.SelectedItem;
            if (sentence.fileExists)
            {
                try
                {
                    SoundPlayer player = new SoundPlayer(sentence.fullPath());
                    player.Play();
                }
                catch (Exception) { };  // Catch and disregard all media exceptions
            }
        }

        private void openTXLogo_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            AboutWindow aboutWindow = new AboutWindow();
            aboutWindow.ShowDialog();
        }

        private void buttonAddItem_Click(object sender, RoutedEventArgs e)
        {
            added_files++;
            sentences.Add(new Sentence("new_file_" + added_files.ToString() + ";New Description;New Voice Message", @"SOUNDS\" + curLang.sName + @"\"));
            this.lvSentences.Items.Refresh();
            this.lvSentences.SelectedIndex = this.lvSentences.Items.Count - 1;
            this.lvSentences.ScrollIntoView(this.lvSentences.SelectedItem);
        }

        private void btnRecord_Click(object sender, RoutedEventArgs e)
        {
            if (this.lvSentences.SelectedItems.Count < 1) return;

            if (isRecording)
                btnStop_Click(sender, e);
            else
            {
                saveLanguage();
                isRecording = true;
                lblRecording.Visibility = System.Windows.Visibility.Visible;
                Sentence sentence = (Sentence)this.lvSentences.SelectedItem;
                string path = sentence.fullPath();
                Directory.CreateDirectory(Path.GetDirectoryName(path));  // Create directory if it doesn't exist
                System.IO.File.WriteAllText(path, "");                  // Creates and flushes a file if it does not exist

                mciSendString("open new Type waveaudio Alias recsound", "", 0, 0);
                mciSendString("record recsound", "", 0, 0);
            }
        }

        private void btnStop_Click(object sender, RoutedEventArgs e)
        {
            if (this.lvSentences.SelectedItems.Count < 1) return;

            int index = this.lvSentences.SelectedIndex;
            lblRecording.Visibility = System.Windows.Visibility.Hidden;
            isRecording = false;
            Sentence sentence = (Sentence)this.lvSentences.SelectedItem;
            mciSendString("save recsound " + sentence.fullPath(), "", 0, 0);
            mciSendString("close recsound ", "", 0, 0);

            loadLanguage();            // Called to refresh the sentence data of the current langugae
            this.lvSentences.SelectedIndex = index;
        }

        private void btnPlay_Click(object sender, RoutedEventArgs e)
        {
            lblRecording.Visibility = System.Windows.Visibility.Hidden;
            lvSentences_MouseDoubleClick(sender, null);
        }
    }

    public class Language
    {
        public string lName { get; set; }
        public string sName { get; set; }
    }

    public class Languages : List<Language>
    {
        public void Add(string longer, string shorter)
        {
            this.Add(new Language { lName = longer, sName = shorter });
        }
    }

    public class Sentence
    {
        public string fileName { get; set; }
        public string description { get; set; }
        public string voiceString { get; set; }
        public string path { get; set; }
        public bool fileExists { get; set; }

        public Sentence(string rawString, string dirPath)
        {
            string[] words = rawString.Split(';');
            fileName = words[0].TrimStart(' ', '\"');
            description = words[1];
            voiceString = words[2].TrimEnd('\"', ',', ' ');
            path = dirPath;
            fileExists = File.Exists(fullPath());
        }

        public string fullPath()
        {
            return System.AppDomain.CurrentDomain.BaseDirectory + path + fileName + ".wav";
        }

        public string toRaw()
        {
            return "\"" + fileName + ";" + description + ";" + voiceString + "\",";
        }
    }

    public class Sentences : List<Sentence>
    {
        public void Add(string rawString, string dirPath)
        {
            this.Add(new Sentence(rawString, dirPath));
        }
    }
}
