#include "DHT22ProtocolAnalyzer.h"
#include "DHT22ProtocolAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

DHT22ProtocolAnalyzer::DHT22ProtocolAnalyzer()
:	Analyzer(),  
	mSettings( new DHT22ProtocolAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

DHT22ProtocolAnalyzer::~DHT22ProtocolAnalyzer()
{
	KillThread();
}

void DHT22ProtocolAnalyzer::WorkerThread()
{
	mResults.reset( new DHT22ProtocolAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );

	mSampleRateHz = GetSampleRate();
	mOneWire = GetAnalyzerChannelData( mSettings->mInputChannel );

	U64 samples_per_20us = mSampleRateHz * 20e-6;
	U64 samples_per_50us = mSampleRateHz * 50e-6;
	U64 samples_per_80us = mSampleRateHz * 80e-6;
	U64 samples_per_100us = mSampleRateHz * 100e-6;
	U64 samples_per_1ms = mSampleRateHz * 1e-3;
	U64 samples_per_sensor_ack = mSampleRateHz * 80e-6; // could be from 40us-80us
	U64 samples_per_one = mSampleRateHz * 70e-6; // could be +- 10%
	U64 samples_per_zero = mSampleRateHz * 27e-6; // could be +- 10%
	
	// 
	struct PulseData {
		PulseData(U64 inStart, U64 inEnd, U16 inValue)
		: pulse_start(inStart), pulse_end(inEnd), value(inValue)
		{}
		U64 pulse_start;
		U64 pulse_end;
		U16 value;
	};	
	std::vector<PulseData> pulse_widths; // location, width	

	// Start on BIT_LOW
	
	if (mOneWire->GetBitState() == BIT_HIGH)
		mOneWire->AdvanceToNextEdge();
	U64 next_sample = mOneWire->GetSampleNumber();	

	for (;;)
	{
		U64 prev_sample = next_sample;
		mOneWire->AdvanceToNextEdge(); // get BIT_HIGH
		//falling edge -- begin at host pull low for > 1ms									   
		next_sample = mOneWire->GetSampleNumber();

		// Host init is when BIT_LOW for > 1 ms
		if (mOneWire->GetBitState() == BIT_HIGH && (next_sample - prev_sample) > samples_per_1ms)
		{
			mResults->AddMarker((next_sample + prev_sample) / 2, AnalyzerResults::DownArrow, mSettings->mInputChannel);

			// Now get 2 Host and 2 device transitions
			prev_sample = next_sample;
			mOneWire->AdvanceToNextEdge(); // get BIT_LOW
			next_sample = mOneWire->GetSampleNumber();
			mResults->AddMarker((next_sample + prev_sample) / 2, AnalyzerResults::UpArrow, mSettings->mInputChannel);

			prev_sample = next_sample;
			mOneWire->AdvanceToNextEdge(); // get BIT_HIGH
			next_sample = mOneWire->GetSampleNumber();
			mResults->AddMarker((next_sample + prev_sample) / 2, AnalyzerResults::DownArrow, mSettings->mInputChannel);

			prev_sample = next_sample;
			mOneWire->AdvanceToNextEdge(); // get BIT_LOW
			next_sample = mOneWire->GetSampleNumber();
			mResults->AddMarker((next_sample + prev_sample) / 2, AnalyzerResults::UpArrow, mSettings->mInputChannel);

			// Start Data			
			mOneWire->AdvanceToNextEdge(); // get BIT_HIGH
			prev_sample = mOneWire->GetSampleNumber();
			mOneWire->AdvanceToNextEdge(); // get BIT_LOW
			next_sample = mOneWire->GetSampleNumber();


			while (mOneWire->GetBitState() == BIT_LOW
				&& (next_sample - prev_sample) < samples_per_1ms)
			{
				mResults->AddMarker((next_sample + prev_sample) / 2, AnalyzerResults::Dot, mSettings->mInputChannel);
				pulse_widths.push_back(PulseData(prev_sample, next_sample, (next_sample - prev_sample) > samples_per_50us));

				// Detected Host pull high ~80us
				// Collect rising (prev_sample) and falling edge (next_sample) of first bit				
				mOneWire->AdvanceToNextEdge(); // get BIT_HIGH
				prev_sample = mOneWire->GetSampleNumber();
				mOneWire->AdvanceToNextEdge(); // get BIT_LOW
				next_sample = mOneWire->GetSampleNumber();

				ReportProgress(next_sample);
			}

			U64 value = 0;
			U32 bit_pos = 0;
			Frame frame;
			// Get Relative Humidity (16 bits)
			for (; bit_pos < 16; bit_pos++)
			{
				value += (pulse_widths[bit_pos].value) << (15 - bit_pos);
			}
			//we have a byte to save. 			
			frame.mData1 = value;
			frame.mData2 = value;
			frame.mFlags = 0;
			frame.mStartingSampleInclusive = pulse_widths[0].pulse_start;
			frame.mEndingSampleInclusive = pulse_widths[15].pulse_end;

			mResults->AddFrame(frame);
			mResults->CommitResults();
			ReportProgress(frame.mEndingSampleInclusive);

			value = 0;
			// Get Temperature (16 bits)
			for (bit_pos = 0; bit_pos < 16; bit_pos++)
			{
				value += (pulse_widths[bit_pos + 16].value & 0x1) << (15 - bit_pos);
			}
			//we have a byte to save. 			
			frame.mData1 = value;
			frame.mData2 = value;
			frame.mFlags = 0;
			frame.mStartingSampleInclusive = pulse_widths[16].pulse_start;
			frame.mEndingSampleInclusive = pulse_widths[bit_pos + 15].pulse_end;
			mResults->AddFrame(frame);
			mResults->CommitResults();
			ReportProgress(frame.mEndingSampleInclusive);

			value = 0;
			// Get Checksum (8 bits)
			for (bit_pos = 0; bit_pos < 8; bit_pos++)
			{
				value += (pulse_widths[bit_pos + 32].value & 0x1) << (7 - bit_pos);
			}
			//we have a byte to save. 			
			frame.mData1 = value;
			frame.mData2 = value;
			frame.mFlags = 0;
			frame.mStartingSampleInclusive = pulse_widths[32].pulse_start;
			frame.mEndingSampleInclusive = pulse_widths[bit_pos + 31].pulse_end;
			mResults->AddFrame(frame);
			mResults->CommitResults();
			ReportProgress(frame.mEndingSampleInclusive);

			pulse_widths.clear();
		}
	}
}

bool DHT22ProtocolAnalyzer::NeedsRerun()
{
	return false;
}

U32 DHT22ProtocolAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 DHT22ProtocolAnalyzer::GetMinimumSampleRateHz()
{
	return U32(1 / 1e-6);
}

const char* DHT22ProtocolAnalyzer::GetAnalyzerName() const
{
	return "DHT2x 1-Wire";
}

const char* GetAnalyzerName()
{
	return "DHT2x 1-Wire";
}

Analyzer* CreateAnalyzer()
{
	return new DHT22ProtocolAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}