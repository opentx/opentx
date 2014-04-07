using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;

using WaveLib;

namespace AudioRecorder
{
    class Program
    {
        WavFileWriter filewriter;
        byte[] tmparray;

        void Go()
        {
            int samplerate = 16000;
            int bits = 16;  // 8 or 16
            int channels = 1;  // 1 or 2

            filewriter = new WavFileWriter("out.wav", samplerate, bits, channels);

            WaveFormat fmt = new WaveFormat(samplerate, bits, channels);

            // devicenumber, wavformat, buffersize, callback
            int buffersize = 16384;
            WaveInRecorder rec = new WaveInRecorder(-1, fmt, buffersize, this.DataArrived);
            tmparray = new byte[buffersize];

            Console.WriteLine("Recording - press Enter to end");
            Console.ReadLine();
            rec.Close();

            filewriter.Close();

            Console.WriteLine("Bye");
        }

        private void DataArrived(IntPtr data, int size)
        {
            Console.WriteLine("DataArrived {0} bytes", size);
            Marshal.Copy(data, tmparray, 0, size);
            filewriter.Write(tmparray);
        }

        static void Main(string[] args)
        {
            new Program().Go();
        }
    }


    class WavFileWriter
    {
        BinaryWriter filewriter;
        long audiobyteswritten;

        public WavFileWriter(string filename, int samplerate, int bits, int channels)
        {
            filewriter = new BinaryWriter(File.Open(filename, FileMode.Create));

            filewriter.Write((uint)0x46464952); // "RIFF"
            filewriter.Write((uint)0); // chunkisize (filled in when we close)
            filewriter.Write((uint)0x45564157); // "WAVE"

            // subchunk1
            filewriter.Write((uint)0x20746d66); // "fmt "
            filewriter.Write((uint)16);
            filewriter.Write((UInt16)1);
            filewriter.Write((UInt16)channels);
            filewriter.Write((uint)samplerate);
            filewriter.Write((uint)(channels*samplerate*bits/8));
            filewriter.Write((UInt16)(channels*bits/8));
            filewriter.Write((UInt16)bits);

            // subchunk2
            filewriter.Write((uint)0x61746164); // "data"
            filewriter.Write((uint)0); // chunkisize (filled in when we close)
        }

        public void Write(byte[] data)
        {
            filewriter.Write(data);
            audiobyteswritten += data.Length;
        }

        public void Close()
        {
            filewriter.Seek(4, SeekOrigin.Begin);
            filewriter.Write((uint)(audiobyteswritten + 36)); // 36 = total size of chunk headers

            filewriter.Seek(40, SeekOrigin.Begin);
            filewriter.Write((uint)audiobyteswritten);
            
            filewriter.Close();
        }
    }
}
