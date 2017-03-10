#include "DHT22ProtocolAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


DHT22ProtocolAnalyzerSettings::DHT22ProtocolAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mLongPulse_ms( 70 ),
	mShortPulse_ms( 25 )
{
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "Serial", "Standard DHT2x 1-Wire" );
	mInputChannelInterface->SetChannel( mInputChannel );

	mLongPulseInterface.reset(new AnalyzerSettingInterfaceInteger());
	mLongPulseInterface->SetTitleAndTooltip("Long Pulse Duration (miliseconds)", "Specify the duration of a long pulse in (ms).");
	mLongPulseInterface->SetMax(1000000);
	mLongPulseInterface->SetMin(1);
	mLongPulseInterface->SetInteger(mLongPulse_ms);

	mShortPulseInterface.reset(new AnalyzerSettingInterfaceInteger());
	mShortPulseInterface->SetTitleAndTooltip("Short Pulse Duration (miliseconds)", "Specify the duration of a short pulse in (ms).");
	mShortPulseInterface->SetMax(1000000);
	mShortPulseInterface->SetMin(1);
	mShortPulseInterface->SetInteger(mShortPulse_ms);

	AddInterface(mInputChannelInterface.get());
	AddInterface(mLongPulseInterface.get());
	AddInterface(mShortPulseInterface.get());

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "A", false );
}

DHT22ProtocolAnalyzerSettings::~DHT22ProtocolAnalyzerSettings()
{
}

bool DHT22ProtocolAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface->GetChannel();
	mLongPulse_ms = mLongPulseInterface->GetInteger();
	mShortPulse_ms = mShortPulseInterface->GetInteger();

	ClearChannels();
	AddChannel( mInputChannel, "B", true );

	return true;
}

void DHT22ProtocolAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface->SetChannel(mInputChannel);
	mLongPulseInterface->SetInteger(mLongPulse_ms);
	mShortPulseInterface->SetInteger(mShortPulse_ms);
}

void DHT22ProtocolAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mLongPulse_ms;
	text_archive >> mShortPulse_ms;

	ClearChannels();
	AddChannel( mInputChannel, "C", true );

	UpdateInterfacesFromSettings();
}

const char* DHT22ProtocolAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mLongPulse_ms;
	text_archive << mShortPulse_ms;

	return SetReturnString( text_archive.GetString() );
}
