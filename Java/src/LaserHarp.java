import java.io.*;
import java.util.Scanner;

// MIDI stuff
import javax.sound.midi.*;



public class LaserHarp
{
	//private static int volume = 127; // between 0 et 127
	private static String[] asNotes = new String[13];
	private static int[] anNotes = new int[13];



	public static void main(String[] args) throws IOException, InterruptedException, FileNotFoundException, IllegalArgumentException, MidiUnavailableException
	{
		File midiFile = new File("/Users/jordi/Desktop/Current Projects/Laser Harp/Java/resources/midi.dat"); // The file that is decoded to midi
		MidiOut midiOut = new MidiOut(); // Create a new midi synth

		try
		{
			while (true)
			{
				Scanner terminalInput = new Scanner(System.in);
				FileReader midiIn = new FileReader(midiFile); // Reset to the beginning of the file
				char chCharacter = (char)midiIn.read(); // Read the first character in the file

				if (chCharacter == '1') // Check if we're ready to update
				{
					midiIn.skip(1); // Skip the newline

					String sInstrument = new String(""); // Initialize instrument string
					while ((chCharacter = (char)midiIn.read()) != '\n') {
						sInstrument += chCharacter;
					}

					//midiIn.skip(1); // Skip the newline

					for (int i = 0; i < 13; i++)
					{
						asNotes[i] = "";

						while ((chCharacter = (char)midiIn.read()) != ' ') {
							asNotes[i] += chCharacter;
						}

						anNotes[i] = Integer.parseInt(asNotes[i]);
					}

					midiOut.update(anNotes, Integer.parseInt(sInstrument));
				}
				else if (chCharacter == '0')
				{
					midiOut.update(anNotes);

					/*
					if (terminalInput.hasNextInt())
					{
						int nInstrument = terminalInput.nextInt();
						midiOut.update(anNotes, nInstrument);
					}
					else {
						midiOut.update(anNotes);
					}
					*/
				}

				midiIn.close();
				terminalInput.close();
			}
		}
		finally {
			//midiIn.close();
			midiOut.close();
		}
	}
}

class MidiOut
{
	private static Synthesizer synth;
	private static MidiChannel[] aChannels;
	private static int[] anNotes = new int[13];
	private static int nChannel = 0; // 0 is a piano, 9 is percussion, other channels are for other instruments


	public MidiOut() throws MidiUnavailableException
	{
		open();
	}

	public static void open() throws MidiUnavailableException
	{
		try
		{
			synth = MidiSystem.getSynthesizer();
			synth.open();
			aChannels = synth.getChannels();

			Soundbank soundfont = MidiSystem.getSoundbank(new File("/Users/jordi/Desktop/Current Projects/Laser Harp/Java/resources/Arachno.sf2"));
			if (synth.isSoundbankSupported(soundfont))
			{
				synth.unloadAllInstruments(synth.getDefaultSoundbank());
				synth.loadAllInstruments(soundfont);

				Instrument[] instruments = soundfont.getInstruments();
				for (int i = 0; i < instruments.length; i++) {
					System.out.println(Integer.toString(i) + ": " + instruments[i].getName());
				}
			}
			else {
				System.out.println("Could not load soundbank");
			}
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void update(int[] anNotesNew) throws InterruptedException, MidiUnavailableException
	{
		update(anNotesNew, aChannels[nChannel].getProgram());
	}

	public static void update(int[] anNotesNew, int nInstrument) throws InterruptedException, MidiUnavailableException
	{
		if (nInstrument != aChannels[nChannel].getProgram())
		{
			for (int i = 0; i < 13; i++)
			{
				aChannels[nChannel].noteOff(i + 60);

				anNotes[i] = 0;
			}
		}

		setInstrument(nInstrument);

		for (int i = 0; i < 13; i++)
		{
			if (anNotesNew[i] != anNotes[i])
			{
				aChannels[nChannel].noteOn(i + 60, anNotesNew[i]);

				anNotes[i] = anNotesNew[i];
			}
		}
	}

	public static void setChannel(int nChannelNew)
	{
		nChannel = nChannelNew;
	}

	public static void setInstrument(int nInstrument)
	{
		aChannels[nChannel].programChange(nInstrument);
	}

	public static void close() throws MidiUnavailableException
	{
		synth.close();
	}
}