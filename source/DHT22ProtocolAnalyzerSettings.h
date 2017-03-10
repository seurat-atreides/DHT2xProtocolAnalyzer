#ifndef DHT22PROTOCOL_ANALYZER_SETTINGS
#define DHT22PROTOCOL_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class DHT22ProtocolAnalyzerSettings : public AnalyzerSettings
{
public:
	DHT22ProtocolAnalyzerSettings();
	virtual ~DHT22ProtocolAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mLongPulse_ms;
	U32 mShortPulse_ms;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mLongPulseInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mShortPulseInterface;
};

#endif //DHT22PROTOCOL_ANALYZER_SETTINGS
