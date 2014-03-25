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
using System.Collections.ObjectModel;
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
        Environment env;
        SentenceTables tables = new SentenceTables();
        public Sentences sentences { get; set; }
        public Languages languages { get; set; }

        WavFileWriter filewriter;
        WaveInRecorder recorder;
        byte[] recordingBuffer;
        static int recSamplerate = 16000;  // 8000, 16000 or 32000
        static int recBits = 16;           // 8 or 16
        static int recChannels = 1;        // 1 or 2
        static int recBuffersize = 1024 * 2;
        public bool isRecording { get; set; }


        public MainWindow()
        {
            this.DataContext = this;
            sentences = new Sentences();
            languages = new Languages();
            recordingBuffer = new byte[recBuffersize];

            InitializeComponent();

            // Start by displaying Splash Screen
            SplashScreen splash = new SplashScreen("recorder_logo.png");
            splash.Show(true);
            Thread.Sleep(1500);

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

            env = new Environment(languages[0]);

            cbLanguages.SelectedIndex = 0; // Note: Sets current langugage -> triggers loadlanguage() 
        }

        private void loadLanguage()
        {
            string[] system_strings;
            string[] other_strings;

            try
            {
                system_strings = System.IO.File.ReadAllLines(env.systemSounds);
                other_strings = System.IO.File.ReadAllLines(env.otherSounds);
            }
            catch (IOException)
            {
                system_strings = tables.default_system_strings[tables.toInt(env.lang.sName)];
                other_strings = tables.default_other_strings[tables.toInt(env.lang.sName)];
            }
            sentences.Clear();

            foreach (string str in system_strings)
                sentences.Add(str, env.sysDir);

            sentences.Add(@";^ System Sounds ^;", "");

            foreach (string str in other_strings)
                sentences.Add(str, env.baseDir);
        }

        private void saveLanguage()
        {
            System.IO.Directory.CreateDirectory(env.sysDir);
            StreamWriter sw = File.CreateText(env.systemSounds);
            int i;
            for (i = 0; sentences[i].fileName != ""; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
            }
            sw.Close();
            sw = File.CreateText(env.otherSounds);
            for (i++; i < sentences.Count; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
            }
            sw.Close();
        }

        private void switchLanguage(object sender, SelectionChangedEventArgs e)
        {
            env = new Environment(((Language)e.AddedItems[0]));  // AddedItems is a strange name. It contains the last selection
            loadLanguage();
        }

        private void about(object sender, MouseButtonEventArgs e)
        {
            AboutWindow aboutWindow = new AboutWindow();
            aboutWindow.ShowDialog();
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
            sentences.Add(new Sentence(newFile + ";New Description;New Voice Message", env.baseDir));

            // Extremely ugly - direct access to the listview to scroll down and select the new object
            this.lvSentences.SelectedIndex = this.lvSentences.Items.Count - 1;
            this.lvSentences.ScrollIntoView(this.lvSentences.SelectedItem);
        }

        private void play(object sender, EventArgs e)
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
                    SoundPlayer player = new SoundPlayer(sentence.fullPath);
                    player.Play();
                }
                catch (Exception) { };  // Catch and disregard all media exceptions
            }
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
                string path = sentence.fullPath;
                Directory.CreateDirectory(Path.GetDirectoryName(path));  // Create directory if it doesn't exist
                System.IO.File.WriteAllText(path, "");                   // Create and flush the file

                filewriter = new WavFileWriter(path, recSamplerate, recBits, recChannels);
                WaveFormat fmt = new WaveFormat(recSamplerate, recBits, recChannels);
                recorder = new WaveInRecorder(-1, fmt, recBuffersize, this.DataArrived);
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

            wavProcessor processor = new wavProcessor();
            int noiceLevel = (int)this.noiceLevelSlider.Value;
            processor.StripSilence(sentence.fullPath, noiceLevel);
            processor.ToneIn(sentence.fullPath);
            processor.ToneOut(sentence.fullPath);
            processor.SpeedUp(sentence.fullPath, (int)this.speedSlider.Value);
        }

        private void DataArrived(IntPtr data, int size)
        {
            Marshal.Copy(data, recordingBuffer, 0, size);
            filewriter.Write(recordingBuffer, size);
        }
    }

    public class Environment
    {
        public Language lang { get; set; }
        public string baseDir { get { return @"SOUNDS\" + lang.sName + @"\"; } }
        public string sysDir { get { return @"SOUNDS\" + lang.sName + @"\SYSTEM\"; } }
        public string otherSounds { get { return baseDir + "other_sounds.txt"; } }
        public string systemSounds { get { return sysDir + "system_sounds.txt"; } }

        public bool fileExists(string fName)
        {
            if (File.Exists(System.AppDomain.CurrentDomain.BaseDirectory + baseDir + fName + ".wav"))
                return true;
            if (File.Exists(System.AppDomain.CurrentDomain.BaseDirectory + sysDir + fName + ".wav"))
                return true;
            return false;
        }

        public Environment(Language language)
        {
            lang = language;
        }
    }
    
    // Data container classes

    public class Languages : ObservableCollection<Language>
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

    public class Sentences : ObservableCollection<Sentence>
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
            fileExists = File.Exists(fullPath);
        }

        public string fullPath
        {
            get { return System.AppDomain.CurrentDomain.BaseDirectory + path + fileName + ".wav"; }
        }

        public string toRaw()
        {
            return "\"" + fileName + ";" + description + ";" + voiceString + "\",";
        }
    }

}
