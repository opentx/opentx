using System;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;
using WaveLib;


public class wavProcessor
{
    public int Length;
    public short Channels;
    public int SampleRate;
    public int DataLength;
    public short BitsPerSample;

    /// Filter out silence or noise from start and end of wave file. 
    public bool StripSilence(string strPath, int noiceLevel)
    {
        if (strPath == null) strPath = "";
        if (strPath == "") return false;

        wavProcessor wain = new wavProcessor();
        wavProcessor waout = new wavProcessor();

        waout.DataLength = waout.Length = 0;

        if (!wain.WaveHeaderIN(@strPath)) return false;

        waout.DataLength = wain.DataLength;
        waout.Length = wain.Length;

        waout.BitsPerSample = wain.BitsPerSample;
        waout.Channels = wain.Channels;
        waout.SampleRate = wain.SampleRate;

        byte[] arrfile = GetWAVEData(strPath);

        //check for silence
        int startpos = 0;
        int endpos = arrfile.Length - 1;
        //At start
        try
        {
            for (int j = 0; j < arrfile.Length; j += 2)
            {
                short snd = ComplementToSigned(ref arrfile, j);
                if (snd > (-1 * noiceLevel) && snd < noiceLevel) startpos = j;
                else
                    break;
            }
        }
        catch (Exception ex)
        {
            Console.Write(ex.Message);
        }

        //At end
        for (int k = arrfile.Length - 1; k >= 0; k -= 2)
        {
            short snd = ComplementToSigned(ref arrfile, k - 1);
            if (snd > (-1 * noiceLevel) && snd < noiceLevel)
                endpos = k;
            else
                break;
        }

        if (startpos == endpos) return false;
        if ((endpos - startpos) < 1) return false;

        byte[] newarr = new byte[(endpos - startpos) + 1];

        for (int ni = 0, m = startpos; m <= endpos; m++, ni++)
            newarr[ni] = arrfile[m];

        //write file
        waout.DataLength = newarr.Length;

        WavFileWriter wfWriter = new WavFileWriter(@strPath, wain.SampleRate, wain.BitsPerSample, wain.Channels);
        wfWriter.Write(newarr);
        wfWriter.Close();

        return true;
    }

    /// <summary>
    /// Read the wave file header and store the key values in public variable.
    /// Adapted from - Concatenation Wave Files using C# 2005 by By Ehab Mohamed Essa
    /// URL - http://www.codeproject.com/useritems/Concatenation_Wave_Files.asp
    /// </summary>
    /// <param name="strPath">The physical path of wave file incl. file name for reading</param>
    /// <returns>True/False</returns>
    private bool WaveHeaderIN(string strPath)
    {
        if (strPath == null) strPath = "";
        if (strPath == "") return false;

        FileStream fs = new FileStream(strPath, FileMode.Open, FileAccess.Read);

        BinaryReader br = new BinaryReader(fs);
        try
        {
            Length = (int)fs.Length - 8;
            fs.Position = 22;
            Channels = br.ReadInt16(); //1
            fs.Position = 24;
            SampleRate = br.ReadInt32(); //16000
            fs.Position = 34;
            BitsPerSample = br.ReadInt16(); //16
            DataLength = (int)fs.Length - 44;
            byte[] arrfile = new byte[fs.Length - 44];
            fs.Position = 44;
            fs.Read(arrfile, 0, arrfile.Length);
        }
        catch
        {
            return false;
        }
        finally
        {
            br.Close();
            fs.Close();
        }
        return true;
    }

    /// <summary>
    /// In stereo wave format, samples are stored in 2's complement. For Mono, it's necessary to 
    /// convert those samples to their equivalent signed value. This method is used 
    /// by other public methods to equilibrate wave formats of different files.
    /// </summary>
    /// <param name="bytArr">Sample data in array</param>
    /// <param name="intPos">Array offset</param>
    /// <returns>Mono value as signed short</returns>
    private short ComplementToSigned(ref byte[] bytArr, int intPos) // 2's complement to normal signed value
    {
        short snd = BitConverter.ToInt16(bytArr, intPos);
        if (snd != 0)
            snd = Convert.ToInt16((~snd | 1));
        return snd;
    }

    /// <summary>
    /// Read the WAVE file then position to DADA segment and return the chunk as byte array 
    /// </summary>
    /// <param name="strWAVEPath">Path of WAVE file</param>
    /// <returns>byte array</returns>
    private byte[] GetWAVEData(string strWAVEPath)
    {
        try
        {
            FileStream fs = new FileStream(@strWAVEPath, FileMode.Open, FileAccess.Read);
            byte[] arrfile = new byte[fs.Length - 44];
            fs.Position = 44;
            fs.Read(arrfile, 0, arrfile.Length);
            fs.Close();
            return arrfile;
        }
        catch (IOException ioex)
        {
            throw ioex;
        }
    }
} // End of clsWaveProcessor class
