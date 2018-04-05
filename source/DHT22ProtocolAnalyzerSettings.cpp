#include "DHT22ProtocolAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


DHT22ProtocolAnalyzerSettings::DHT22ProtocolAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mPulseWidthOne_us( 70 ),
	mPulseWidthZero_us( 26 ) {
    
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "Data Channel:", "DHT2x 1-Wire data channel" );
	mInputChannelInterface->SetChannel( mInputChannel );

	mLongPulseInterface.reset(new AnalyzerSettingInterfaceInteger());
	mLongPulseInterface->SetTitleAndTooltip("'1' Pulse Duration [microsec]:", "Specify the duration of a '1' pulse in microseconds.");
	mLongPulseInterface->SetMax(75);
	mLongPulseInterface->SetMin(68);
	mLongPulseInterface->SetInteger(mPulseWidthOne_us);

	mShortPulseInterface.reset(new AnalyzerSettingInterfaceInteger());
	mShortPulseInterface->SetTitleAndTooltip("'0' Pulse Duration [microsec]:", "Specify the duration of a '0' pulse in microseconds.");
	mShortPulseInterface->SetMax(30);
	mShortPulseInterface->SetMin(22);
	mShortPulseInterface->SetInteger(mPulseWidthZero_us);

	AddInterface(mInputChannelInterface.get());
	AddInterface(mLongPulseInterface.get());
	AddInterface(mShortPulseInterface.get());

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "Data", false );
}

DHT22ProtocolAnalyzerSettings::~DHT22ProtocolAnalyzerSettings(){
}

bool DHT22ProtocolAnalyzerSettings::SetSettingsFromInterfaces(){
	mInputChannel = mInputChannelInterface->GetChannel();
	mPulseWidthOne_us = mLongPulseInterface->GetInteger();
	mPulseWidthZero_us = mShortPulseInterface->GetInteger();

	ClearChannels();
	AddChannel( mInputChannel, "Data", true );

	return true;
}

void DHT22ProtocolAnalyzerSettings::UpdateInterfacesFromSettings(){
	mInputChannelInterface->SetChannel(mInputChannel);
	mLongPulseInterface->SetInteger(mPulseWidthOne_us);
	mShortPulseInterface->SetInteger(mPulseWidthZero_us);
}

void DHT22ProtocolAnalyzerSettings::LoadSettings( const char* settings ){
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mPulseWidthOne_us;
	text_archive >> mPulseWidthZero_us;

	ClearChannels();
	AddChannel( mInputChannel, "Data", true );

	UpdateInterfacesFromSettings();
}

const char* DHT22ProtocolAnalyzerSettings::SaveSettings(){
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mPulseWidthOne_us;
	text_archive << mPulseWidthZero_us;

	return SetReturnString( text_archive.GetString() );
}
