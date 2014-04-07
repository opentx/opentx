/* This file is part of OpenTX Recorder.
 * OpenTX Recorder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with OpenTX Recorder.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Copyright 2014 Tomas Andersson  */

using System;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;

namespace WaveLib
{
	public delegate void BufferDoneEventHandler(IntPtr data, int size);

	internal class WaveInBuffer
	{
        private ManualResetEvent m_RecordEvent = new ManualResetEvent(false);
		private IntPtr m_WaveIn;

		private WaveNative.WaveHdr m_Header;
		private byte[] m_HeaderData;
		private GCHandle m_HeaderHandle;
		private GCHandle m_HeaderDataHandle;

		internal static void WaveInProc(IntPtr hdrvr, int uMsg, int dwUser, ref WaveNative.WaveHdr wavhdr, int dwParam2)
		{
			if (uMsg == WaveNative.MM_WIM_DATA)
			{
				GCHandle h = (GCHandle)wavhdr.dwUser;
				WaveInBuffer buf = (WaveInBuffer)h.Target;
				buf.OnCompleted();
			}
		}

		public WaveInBuffer(IntPtr waveInHandle, int size)
		{
			m_WaveIn = waveInHandle;

			m_HeaderHandle = GCHandle.Alloc(m_Header, GCHandleType.Pinned);
			m_Header.dwUser = (IntPtr)GCHandle.Alloc(this);
			m_HeaderData = new byte[size];
			m_HeaderDataHandle = GCHandle.Alloc(m_HeaderData, GCHandleType.Pinned);
			m_Header.lpData = m_HeaderDataHandle.AddrOfPinnedObject();
			m_Header.dwBufferLength = size;
			WaveInRecorder.ThrowOnError(WaveNative.waveInPrepareHeader(m_WaveIn, ref m_Header, Marshal.SizeOf(m_Header)));
		}

		public void Close()
		{
            WaveInRecorder.ThrowOnError(WaveNative.waveInUnprepareHeader(m_WaveIn, ref m_Header, Marshal.SizeOf(m_Header)));
			m_HeaderHandle.Free();
			m_Header.lpData = IntPtr.Zero;
			m_HeaderDataHandle.Free();
		}

		public int Size
		{
			get { return m_Header.dwBufferLength; }
		}

		public IntPtr Data
		{
			get { return m_Header.lpData; }
		}

		public void Record()
		{
			m_RecordEvent.Reset();
            WaveInRecorder.ThrowOnError(WaveNative.waveInAddBuffer(m_WaveIn, ref m_Header, Marshal.SizeOf(m_Header)));
		}

		public void WaitFor()
		{
			m_RecordEvent.WaitOne();
		}

		private void OnCompleted()
		{
			m_RecordEvent.Set();
		}
	}

    public class WavFileWriter
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
            filewriter.Write((uint)(channels * samplerate * bits / 8));
            filewriter.Write((UInt16)(channels * bits / 8));
            filewriter.Write((UInt16)bits);

            // subchunk2
            filewriter.Write((uint)0x61746164); // "data"
            filewriter.Write((uint)0); // chunkisize (filled in when we close)
        }

        public void Write(byte[] data, int size)
        {
            filewriter.Write(data, 0, size);
            audiobyteswritten += size;
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

	public class WaveInRecorder
	{
		private IntPtr m_WaveIn;
		private WaveInBuffer buffer1, buffer2;
		private WaveInBuffer m_CurrentBuffer;
		private Thread m_Thread;
		private BufferDoneEventHandler m_DoneProc;
		private bool m_Finished;

		private WaveNative.WaveDelegate m_BufferProc = new WaveNative.WaveDelegate(WaveInBuffer.WaveInProc);

		public static int DeviceCount
		{
			get { return WaveNative.waveInGetNumDevs(); }
		}

		public WaveInRecorder(int device, WaveFormat format, int bufferSize, BufferDoneEventHandler doneProc)
		{
			m_DoneProc = doneProc;
            WaveInRecorder.ThrowOnError(WaveNative.waveInOpen(out m_WaveIn, device, format, m_BufferProc, 0, WaveNative.CALLBACK_FUNCTION));

            buffer1 = new WaveInBuffer(m_WaveIn, bufferSize);
            buffer2 = new WaveInBuffer(m_WaveIn, bufferSize);

            buffer1.Record();
            buffer2.Record();

            m_CurrentBuffer = buffer1;

            WaveInRecorder.ThrowOnError(WaveNative.waveInStart(m_WaveIn));
			m_Thread = new Thread(new ThreadStart(ThreadProc));
			m_Thread.Start();
		}

        public void Close()
		{
            m_Finished = true;
            WaveInRecorder.ThrowOnError(WaveNative.waveInReset(m_WaveIn));

            m_Thread.Join();
            m_Thread = null;
            m_DoneProc = null;

            buffer1.WaitFor();
            buffer2.WaitFor();

            buffer1.Close();
            buffer2.Close();

            WaveInRecorder.ThrowOnError(WaveNative.waveInClose(m_WaveIn));
			m_WaveIn = IntPtr.Zero;
		}

        private void ThreadProc()
		{
			while (!m_Finished)
			{
                m_CurrentBuffer = (m_CurrentBuffer == buffer1) ? buffer2 : buffer1;

                m_CurrentBuffer.WaitFor();
				m_DoneProc(m_CurrentBuffer.Data, m_CurrentBuffer.Size);

                if (m_Finished) break;

                m_CurrentBuffer.Record();
			}
		}

        public static void ThrowOnError(int err)
        {
            if (err != WaveNative.MMSYSERR_NOERROR) throw new Exception(err.ToString());
        }
    
    }
}
