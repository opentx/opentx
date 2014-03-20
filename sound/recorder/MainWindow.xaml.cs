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
 * along with OpenTX Recorder.  If not, see <http://www.gnu.org/licenses/>. 
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
using WaveLib;
using System.Net;


namespace OpenTXrecorder
{
    public partial class MainWindow : Window
    {
        SentenceTables tables = new SentenceTables();
        Sentences sentences = new Sentences();
        Languages languages = new Languages();
        Environment env;

        WavFileWriter filewriter;
        WaveInRecorder recorder;
        byte[] tmparray;
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
            env = new Environment(languages[0].sName);
            cbLanguages.SelectedIndex = 0; // Note: Sets current langugage -> triggers loadlanguage() 
        }

        private void loadLanguage()
        {
            string[] system_strings;
            string[] other_strings;

            try
            {
                system_strings = System.IO.File.ReadAllLines(env.systemSounds());
                other_strings = System.IO.File.ReadAllLines(env.otherSounds());
            }
            catch (IOException)
            {
                system_strings = tables.default_system_strings[tables.toInt(env.shortLanguage)];
                other_strings = tables.default_other_strings[tables.toInt(env.shortLanguage)];
            }
            sentences.Clear();

            foreach (string str in system_strings)
                sentences.Add(str, env.sysDir());

            sentences.Add(@";^ System Sounds ^;", "");

            foreach (string str in other_strings)
                sentences.Add(str, env.baseDir());

            lvSentences.Items.Refresh(); // Workaround - Two way binding is better
        }

        private void saveLanguage()
        {
            System.IO.Directory.CreateDirectory(env.sysDir());
            StreamWriter sw = File.CreateText(env.systemSounds());
            int i;
            for (i = 0; sentences[i].fileName != ""; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
            }
            sw.Close();
            sw = File.CreateText(env.otherSounds());
            for (i++; i < sentences.Count; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
            }
            sw.Close();
        }

        private void switchLanguage(object sender, SelectionChangedEventArgs e)
        {
            env = new Environment(((Language)e.AddedItems[0]).sName);
            loadLanguage();
        }


        private void about(object sender, MouseButtonEventArgs e)
        {
            AboutWindow aboutWindow = new AboutWindow();
            aboutWindow.ShowDialog();
        }

        private void sentenceRowDblClk(object sender, MouseButtonEventArgs e)
        {
            if (this.lvSentences.SelectedItems.Count < 1)
                return;

            if (isRecording)
                stop(null, null);

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

        private void play(object sender, RoutedEventArgs e)
        {
            sentenceRowDblClk(null, null);
        }

        private void addSentence(object sender, RoutedEventArgs e)
        {
            int i = 0;
            string newFile;
            do
            {
                newFile = "new_file_" + i.ToString();
                i++;
            }
            while (env.fileExists(newFile));
            sentences.Add(new Sentence(newFile + ";New Description;New Voice Message", env.baseDir()));
            this.lvSentences.Items.Refresh();
            this.lvSentences.SelectedIndex = this.lvSentences.Items.Count - 1;
            this.lvSentences.ScrollIntoView(this.lvSentences.SelectedItem);
        }

        private void record(object sender, RoutedEventArgs e)
        {
            if (this.lvSentences.SelectedItems.Count < 1) return;

            if (isRecording)
                stop(sender, e);
            else
            {
                saveLanguage();
                isRecording = true;
                lblRecording.Visibility = System.Windows.Visibility.Visible;
                Sentence sentence = (Sentence)this.lvSentences.SelectedItem;
                string path = sentence.fullPath();
                Directory.CreateDirectory(Path.GetDirectoryName(path));  // Create directory if it doesn't exist
                System.IO.File.WriteAllText(path, "");                  // Creates and flushes a file if it does not exist

                int samplerate = 16000;
                int bits = 16;      // 8 or 16
                int channels = 1;   // 1 or 2

                filewriter = new WavFileWriter(path, samplerate, bits, channels);
                WaveFormat fmt = new WaveFormat(samplerate, bits, channels);

                // devicenumber, wavformat, buffersize, callback
                int buffersize = 1024 * 2;
                tmparray = new byte[buffersize];
                recorder = new WaveInRecorder(-1, fmt, buffersize, this.DataArrived);
            }
        }

        private void stop(object sender, RoutedEventArgs e)
        {
            if (!isRecording || this.lvSentences.SelectedItems.Count < 1)
                return;

            isRecording = false;
            recorder.Close();
            filewriter.Close();

            int index = this.lvSentences.SelectedIndex;
            lblRecording.Visibility = System.Windows.Visibility.Hidden;
            Sentence sentence = (Sentence)this.lvSentences.SelectedItem;
            loadLanguage();            // Called to refresh the sentence data of the current langugae
            this.lvSentences.SelectedIndex = index;
        }

        private void DataArrived(IntPtr data, int size)
        {
            if (!isRecording)
                return;

            Marshal.Copy(data, tmparray, 0, size);
            filewriter.Write(tmparray);
        }
    }

    public class Environment
    {
        public string shortLanguage { get; set; }
        public string baseDir() { return @"SOUNDS\" + shortLanguage + @"\"; }
        public string sysDir() { return @"SOUNDS\" + shortLanguage + @"\SYSTEM\"; }
        public string otherSounds() { return baseDir() + "other_sounds.txt"; }
        public string systemSounds() { return sysDir() + "system_sounds.txt"; }

        public bool fileExists(string fName)
        {
            if (File.Exists(System.AppDomain.CurrentDomain.BaseDirectory + baseDir() + fName + ".wav"))
                return true;
            if (File.Exists(System.AppDomain.CurrentDomain.BaseDirectory + sysDir() + fName + ".wav"))
                return true;
            return false;
        }

        public Environment(string str)
        {
            shortLanguage = str;
        }
    }

    /* 
     * Data container classes
     */
    public class Languages : List<Language>
    {
        public void Add(string longer, string shorter)
        {
            this.Add(new Language { lName = longer, sName = shorter });
        }
    }

    public class Language
    {
        public string lName { get; set; }
        public string sName { get; set; }
    }

    public class Sentences : List<Sentence>
    {
        public void Add(string rawString, string dirPath)
        {
            this.Add(new Sentence(rawString, dirPath));
        }
    }

    public class Sentence
    {
        public bool fileExists { get; set; }
        public string fileName { get; set; }
        public string description { get; set; }
        public string voiceString { get; set; }
        public string path { get; set; }

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

}
