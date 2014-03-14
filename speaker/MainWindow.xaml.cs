using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Speech.Synthesis;
using System.Speech.AudioFormat;

namespace OpenTXspeak
{
    public partial class MainWindow : Window
    {
        SentenceTables tables = new SentenceTables();
        Languages languages = new Languages();
        Language curLang;
        Sentences sentences = new Sentences();
        SpeechSynthesizer synth = new SpeechSynthesizer();
        int added_files = 0;

        public MainWindow()
        {
            InitializeComponent();
            lvSentences.ItemsSource = sentences;
            cbLanguages.ItemsSource = languages;

            foreach (InstalledVoice voice in synth.GetInstalledVoices())
            {
                cbVoices.Items.Add(voice.VoiceInfo.Name);
            }
            cbVoices.SelectedIndex = 0;

            languages.Add("English", "en");
            languages.Add("Italian", "it");
            languages.Add("Swedish", "se");
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
                sentences.Add(str);

            sentences.Add(@";^ System Sounds ^;");

            foreach (string str in other_strings)
                sentences.Add(str);
            
            lvSentences.Items.Refresh(); // Workaround - Two way binding is better
        }

        private void cbLanguages_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            curLang = (Language)e.AddedItems[0];
            loadLanguage();
        }

        private void lvSentences_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            synth.SetOutputToDefaultAudioDevice();
            synth.Rate = (int)this.voiceRateSlider.Value;

            if (this.lvSentences.SelectedItems.Count < 1)
                return;

            Sentence sentence = (Sentence)this.lvSentences.SelectedItem;
            synth.SelectVoice(cbVoices.Text);
            try { synth.Speak(sentence.voiceString); }
            catch (Exception){}  // Workaround - Some TTS engines crashes on destruction
        }

        private void openTXLogo_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            AboutWindow aboutWindow = new AboutWindow();
            aboutWindow.ShowDialog();
        }

        private void buttonPlay_Click(object sender, RoutedEventArgs e)
        {
            lvSentences_MouseDoubleClick(sender, null);
        }

        private void buttonAddItem_Click(object sender, RoutedEventArgs e)
        {
            added_files++;
            sentences.Add(new Sentence("new_file_" + added_files.ToString() + ";New Description;New Voice Message"));
            this.lvSentences.Items.Refresh();
            this.lvSentences.SelectedIndex = this.lvSentences.Items.Count - 1;
            this.lvSentences.ScrollIntoView(this.lvSentences.SelectedItem);
        }

        private void buttonSaveFiles_Click(object sender, RoutedEventArgs e)
        {
            Cursor oldCursor = this.Cursor;
            this.Cursor = Cursors.Wait;

            synth.Rate = (int)this.fileRateSlider.Value;

            int sampleRate = 32000;
            if ((bool)this.rb16khz.IsChecked)
                sampleRate = 16000;
            else if ((bool)this.rb8khz.IsChecked)
                sampleRate = 8000;

            SpeechAudioFormatInfo audioFormat = new SpeechAudioFormatInfo(sampleRate, AudioBitsPerSample.Sixteen, AudioChannel.Mono);

            System.IO.Directory.CreateDirectory(@"SOUNDS\" + curLang.sName + @"\SYSTEM");
            StreamWriter sw = File.CreateText(@"SOUNDS\" + curLang.sName + @"\SYSTEM\system_sounds.txt");
            int i;
            for (i = 0; sentences[i].fileName != ""; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
                synth.SetOutputToWaveFile(@"SOUNDS\" + curLang.sName + @"\SYSTEM\" + sentences[i].fileName + ".wav", audioFormat);
                try { synth.Speak(sentences[i].voiceString); }
                catch (Exception){}  // Workaround - Some TTS engines crashes on destruction
            }
            sw.Close();
            sw = File.CreateText(@"SOUNDS\" + curLang.sName + @"\other_sounds.txt");
            for (i++; i < sentences.Count; i++)
            {
                sw.WriteLine(sentences[i].toRaw());
                synth.SetOutputToWaveFile(@"SOUNDS\" + curLang.sName + @"\" + sentences[i].fileName + ".wav", audioFormat);
                try { synth.Speak(sentences[i].voiceString); }
                catch (Exception){}  // Workaround - Some TTS engines crashes on destruction
            }
            sw.Close();
            synth.SetOutputToNull(); // Workaround - The synth does not release the sound file like it should 
            this.Cursor = oldCursor;
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

        public Sentence(string rawString)
        {
            string[] words = rawString.Split(';');
            fileName = words[0];
            description = words[1];
            voiceString = words[2];
        }

        public string toRaw()
        {
            return fileName + ";" + description + ";" + voiceString;
        }
    }

    public class Sentences : List<Sentence>
    {
        public void Add(string rawString)
        {
            string[] words = rawString.Split(';');
            this.Add(new Sentence(rawString));
        }
    }
}
