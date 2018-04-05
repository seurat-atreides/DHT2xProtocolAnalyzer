#include "DHT22ProtocolSimulationDataGenerator.h"
#include "DHT22ProtocolAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

DHT22ProtocolSimulationDataGenerator::DHT22ProtocolSimulationDataGenerator()
: mData(),
  mStringIndex( 0 )
{
    // Static Simulation Data
    U16 relh = 396;
    U16 temp = 224;
    U16 csum = GenerateCheckSum(relh, temp);
    csum &= 0xFF;
    mData.push_back(relh);
    mData.push_back(temp);
    mData.push_back(csum);
}

DHT22ProtocolSimulationDataGenerator::~DHT22ProtocolSimulationDataGenerator() {
}

void DHT22ProtocolSimulationDataGenerator::Initialize( U32 simulation_sample_rate, DHT22ProtocolAnalyzerSettings* settings ) {
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;
	mDHT2xSimulationData.SetChannel( mSettings->mInputChannel );
	mDHT2xSimulationData.SetSampleRate( simulation_sample_rate );
	mDHT2xSimulationData.SetInitialBitState( BIT_HIGH );
}

U32 DHT22ProtocolSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel ) {
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mDHT2xSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested ) {
        CreateDHT2xFrame();
	}

	*simulation_channel = &mDHT2xSimulationData;
	return 1;
}

U8 DHT22ProtocolSimulationDataGenerator::GenerateCheckSum(U16 inRelH, U16 inTemp) {
    U16 csum = ((inRelH & 0xFF00) >> 8) + ((inRelH & 0xFF))
                + ((inTemp & 0xFF00) >> 8) + ((inTemp & 0xFF));
    csum &= 0xFF;
    return csum;
}

void DHT22ProtocolSimulationDataGenerator::CreateDHT2xFrame() {
    U32 samples_per_host1 = mSimulationSampleRateHz * 2e-3;
    U32 samples_per_host2 = mSimulationSampleRateHz * 30e-6;
    U32 samples_per_sens1 = mSimulationSampleRateHz * 80e-6;
    U32 samples_per_sens2 = samples_per_sens1;
    U32 samples_per_start_tx = mSimulationSampleRateHz * 50e-6;
    U32 samples_per_zero = mSimulationSampleRateHz * 25e-6;
    U32 samples_per_one  = mSimulationSampleRateHz * 72e-6;
    U32 samples_per_wait = mSimulationSampleRateHz * 2;

    U16 relh = mData[0];
    U16 temp = mData[1];
    U16 csum = mData[2];

    mDHT2xSimulationData.TransitionIfNeeded(BIT_HIGH);
	//we're currenty high
	//let's move forward a little
	mDHT2xSimulationData.Advance(samples_per_host1 * 100);

    mDHT2xSimulationData.Transition();  //low-going edge 
    mDHT2xSimulationData.Advance(samples_per_host1);  //add host start bit time
    mDHT2xSimulationData.Transition();  //high-going edge 
    mDHT2xSimulationData.Advance(samples_per_host2);  //add host bit time
    mDHT2xSimulationData.Transition();  //low-going edge 
    mDHT2xSimulationData.Advance(samples_per_sens1);  //add sensor start bit time
    mDHT2xSimulationData.Transition();  //high-going edge 
    mDHT2xSimulationData.Advance(samples_per_sens2);  //add sensor bit time

    mDHT2xSimulationData.Transition();  //low-going edge 
    mDHT2xSimulationData.Advance(samples_per_start_tx);  //add sensor start bit time

    U16 mask = 0x1 << 15;
    for (U16 i = 0; i<16; i++) {
        mDHT2xSimulationData.Transition();  //high-going edge 
        if ((relh & mask) != 0) {
            mDHT2xSimulationData.Advance(samples_per_one);  //add sensor bit time
        }
        else {
            mDHT2xSimulationData.Advance(samples_per_zero);  //add sensor bit time
        }
        mDHT2xSimulationData.Transition();  //low-going edge 
        mDHT2xSimulationData.Advance(samples_per_start_tx);  //add sensor start bit time
        mask = mask >> 1;
    }

    mask = 0x1 << 15;
    for (U16 i = 0; i<16; i++) {
        mDHT2xSimulationData.Transition();  //high-going edge 
        if ((temp & mask) != 0) {
            mDHT2xSimulationData.Advance(samples_per_one);  //add sensor bit time
        }
        else {
            mDHT2xSimulationData.Advance(samples_per_zero);  //add sensor bit time
        }
        mDHT2xSimulationData.Transition();  //low-going edge 
        mDHT2xSimulationData.Advance(samples_per_start_tx);  //add sensor start bit time
        mask = mask >> 1;
    }

    mask = 0x1 << 7;
    for (U16 i = 0; i<8; i++) {
        mDHT2xSimulationData.Transition();  //high-going edge 
        if ((csum & mask) != 0) {
            mDHT2xSimulationData.Advance(samples_per_one);  //add sensor bit time
        }
        else {
            mDHT2xSimulationData.Advance(samples_per_zero);  //add sensor bit time
        }
        mDHT2xSimulationData.Transition();  //low-going edge 
        mDHT2xSimulationData.Advance(samples_per_start_tx);  //add sensor start bit time
        mask = mask >> 1;
    }

    mDHT2xSimulationData.Transition();  //high-going edge 
    mDHT2xSimulationData.Advance(samples_per_start_tx);  //add sensor start bit time

	//lets pad the end a bit for the stop bit:
	mDHT2xSimulationData.Advance(samples_per_wait);
}
