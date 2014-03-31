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

    /// <summary>
    /// Filter out silence or noise from start and end of wave file. 
    /// </summary>
    /// <param name="strPath">Source wave file</param>
    /// <param name="noiceLevel">Absolute value for noice threshold</param>
    /// <returns>True/False</returns>
    public bool StripSilence(string strPath, int noiceLevel)
    {
        if ((strPath == null) || (strPath == ""))
            return false;

        // Read from file 
        wavProcessor wain = new wavProcessor();
        if (!wain.WaveHeaderIN(@strPath)) return false;
        byte[] arrfile = GetWAVEData(strPath);

        int startpos = 0;
        int endpos = arrfile.Length - 1;

        // Check for silence at start
        for (int j = 0; isSilence(arrfile, j, noiceLevel); j += 20)
            startpos = j;

        // Allow room for tone-in buffer
        int buffer = wain.SampleRate * (wain.BitsPerSample / 8) / 32; // 1/32 seconds lead time
        startpos = startpos - buffer;
        if (startpos < 0)
            startpos = 0;

        // Check for silence at end. No need to check tone out buffer
        for (int k = arrfile.Length - buffer; (k >= 0) && (isSilence(arrfile, k, noiceLevel)); k -= 20)
            endpos = k;

        // Allow room for tone-out buffer
        endpos = endpos + buffer;
        if (endpos > arrfile.Length)
            endpos = arrfile.Length - 2;

        if (startpos >= endpos)
            return false;

        byte[] newarr = new byte[endpos - startpos];
        for (int ni = 0, m = startpos; ni < newarr.Length; m++, ni++)
            newarr[ni] = arrfile[m];

        // write file back
        WavFileWriter writer = new WavFileWriter(@strPath, wain.SampleRate, wain.BitsPerSample, wain.Channels);
        writer.Write(newarr, newarr.Length);
        writer.Close();

        return true;
    }

    // Helper function that checks if the next 10 samples is silence
    private bool isSilence(byte[] buff, int index, int noiceLevel)
    {
        if (buff.Length <= (index + 20))
            return false;

        int totalSnd = 0;
        for (int i = 0; i < 20; i += 2)
        {
            short snd = ComplementToSigned(ref buff, i + index);
            if (snd < 0)
                snd = (short)(snd * -1);
            totalSnd += snd;
        }
        return (totalSnd < (10 * noiceLevel));
    }

    /// <summary>
    /// Tone in wav file
    /// </summary>
    /// <param name="strPath">Source wave</param>
    /// <returns>True/False</returns>
    public bool ToneIn(string strPath)
    {
        if ((strPath == null) || (strPath == ""))
            return false;

        // Read from file
        wavProcessor wain = new wavProcessor();
        if (!wain.WaveHeaderIN(@strPath)) return false;
        byte[] arrfile = GetWAVEData(strPath);

        // Calculate constants
        int start = 0;
        int end = wain.SampleRate * (wain.BitsPerSample / 8) / 16; // 1/16 seconds
        int span = end - start;

        //change volume
        for (int j = start; j < end; j += 2)
        {
            short snd = ComplementToSigned(ref arrfile, j);
            snd = Convert.ToInt16(snd * (j / span));
            byte[] newval = SignedToComplement(snd);
            arrfile[j] = newval[0];
            arrfile[j + 1] = newval[1];
        }
        // write file back
        WavFileWriter writer = new WavFileWriter(@strPath, wain.SampleRate, wain.BitsPerSample, wain.Channels);
        writer.Write(arrfile, arrfile.Length);
        writer.Close();

        return true;
    }
    /// <summary>
    /// Tone out wav file
    /// </summary>
    /// <param name="strPath">Source wave</param>
    /// <returns>True/False</returns>
    public bool ToneOut(string strPath)
    {
        if ((strPath == null) || (strPath == ""))
            return false;

        // Read from file
        wavProcessor wain = new wavProcessor();
        if (!wain.WaveHeaderIN(@strPath)) return false;
        byte[] arrfile = GetWAVEData(strPath);

        // Calculate constants
        int end = wain.Length;
        int start = end - (wain.SampleRate * (wain.BitsPerSample / 8) / 16); // 1/16 seconds from end
        int span = end - start;

        //change volume
        for (int j = start; j < arrfile.Length; j += 2)
        {
            short snd = ComplementToSigned(ref arrfile, j);
            snd = Convert.ToInt16(snd * (end - j) / span);

            byte[] newval = SignedToComplement(snd);
            arrfile[j] = newval[0];
            arrfile[j + 1] = newval[1];
        }
        // write file back
        WavFileWriter writer = new WavFileWriter(@strPath, wain.SampleRate, wain.BitsPerSample, wain.Channels);
        writer.Write(arrfile, arrfile.Length);
        writer.Close();

        return true;
    }
    /// <summary>
    /// Speed up wav file to mimic Donald Duck
    /// </summary>
    /// <param name="strPath">Source wave</param>
    /// <param name="speed">Speed between 0 and 19 </param>
    /// <returns>True/False</returns>
    public bool SpeedUp(string strPath, int speed)
    {
        if ((strPath == null) || (strPath == ""))
            return false;

        if ((speed < 0) || (speed > 19))
            return false;

        // Read from file
        wavProcessor wain = new wavProcessor();
        if (!wain.WaveHeaderIN(@strPath)) return false;
        byte[] arrfile = GetWAVEData(strPath);
        byte[] newfile = new byte[arrfile.Length];

        int skip = 21-speed;
        int j = 0;
        for (int i = 0; i < arrfile.Length; i += 2)
        {
            if (skip > 20 || (((i/2) % skip) != 0))
            {
                newfile[j] = arrfile[i];
                newfile[j + 1] = arrfile[i + 1];
                j += 2;
            }
        }
        // write file back
        WavFileWriter writer = new WavFileWriter(@strPath, wain.SampleRate, wain.BitsPerSample, wain.Channels);
        writer.Write(newfile, j);
        writer.Close();
        return true;
    }

    /// <summary>
    /// Read the wave file header and store the key values in public variable.
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
        if (intPos >= bytArr.Length) return 0;
        if (snd != 0)
            snd = Convert.ToInt16((~snd | 1));
        return snd;
    }
    /// <summary>
    /// Convert signed sample value back to 2's complement value equivalent to Stereo. This method is used 
    /// by other public methods to equilibrate wave formats of different files.
    /// </summary>
    /// <param name="shtVal">The mono signed value as short</param>
    /// <returns>Stereo 2's complement value as byte array</returns>
    private byte[] SignedToComplement(short shtVal) //Convert to 2's complement and return as byte array of 2 bytes
    {
        byte[] bt = new byte[2];
        shtVal = Convert.ToInt16((~shtVal | 1));
        bt = BitConverter.GetBytes(shtVal);
        return bt;
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
