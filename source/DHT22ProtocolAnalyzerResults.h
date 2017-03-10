#ifndef DHT22PROTOCOL_ANALYZER_RESULTS
#define DHT22PROTOCOL_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class DHT22ProtocolAnalyzer;
class DHT22ProtocolAnalyzerSettings;

class DHT22ProtocolAnalyzerResults : public AnalyzerResults
{
public:
	DHT22ProtocolAnalyzerResults( DHT22ProtocolAnalyzer* analyzer, DHT22ProtocolAnalyzerSettings* settings );
	virtual ~DHT22ProtocolAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions
	virtual std::string GetLongDecodedString(U64 frame_index, DisplayBase display_base);
    virtual std::string GetMedDecodedString(U64 frame_index, DisplayBase display_base);
    virtual S16 DecodeTemperature(U64 inTemp);

protected:  //vars
	DHT22ProtocolAnalyzerSettings* mSettings;
	DHT22ProtocolAnalyzer* mAnalyzer;
};

#endif //DHT22PROTOCOL_ANALYZER_RESULTS
