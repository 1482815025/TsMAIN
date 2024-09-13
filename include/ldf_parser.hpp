//
//  ldf_parser.hpp
//  LIN_Database_Encode_Decode_Tool
//
//  Created by Yifan Wang on 6/14/23.
//

#ifndef ldf_parser_hpp
#define ldf_parser_hpp

#include "frame.hpp"
#include "payloads.hpp"
#include "scheduleTable.hpp"

class PARSER_API LdfParser : public dbParser {

public:

	std::map<std::string, ScheduleTable> getScheduleTableLibrary() const { return scheduleTableLibrary; }
	std::map<unsigned int, std::string> getScheduleTableIdToName() const { return scheduleTableIdToName; }
	std::map<int, Frame> getFramesLibrary() const { return framesLibrary; }
	double getLinSpeed() const { return LinSpeed.value(); }
	void payloadsGenerator(std::string Node, Payloads_LIN& encodedPayloads);

	// Construct using either a File or a Stream of a LDF-File
	// A bool is used to indicate whether parsing succeeds or not
	bool parse(const std::string& filePath) override;
	// Encode
	int encode(
		int const frameId,
		std::vector<std::pair<std::string, double> >& signalsToEncode,
		unsigned char encodedPayload[MAX_FRAME_LEN]
	);
	// Decode std::make_tuple(decodedPhysicalValue, unit, sigValueType);
	std::map<std::string, std::tuple<double, std::string, LinSigEncodingValueType> >
		decode(
			int const frameId,
			int const frmSize,
			unsigned char payLoad[MAX_FRAME_LEN]
		);
	// Print LDF info
	friend std::ostream& operator<<(std::ostream& os, const LdfParser& ldfFile);
	void updateSignalValue(unsigned long MessageId, std::string signalName, double newValue, unsigned char *updatePayload);

private:

	std::optional<std::string> LinProtocolVersion;
	std::optional<double> LinSpeed;
	typedef std::map<int, Frame>::iterator framesLib_itr;
	typedef std::map<std::string, Signal_LIN>::iterator signalsLib_itr;
	typedef std::map<std::string, SignalEncodingType>::iterator sigEncodingTypeLib_itr;
	typedef std::map<std::string, ScheduleTable>::iterator scheduleTableLib_itr;
	bool isEmptyLibrary = true;
	bool isEmptyFramesLibrary = true;
	bool isEmptySignalsLibrary = true;
	bool isEmptySigEncodingTypeLibrary = true;
	// A hash table that stores all info of frames. <Frame id, Frame name>
	std::vector<std::pair<unsigned int, std::string>> frameTable{};
	// A hash table that stores all info of frames. <Frame id, Frame object>
	std::map<int, Frame> framesLibrary{};
	// A hash table that stores all info of signals. <Signal name, Signal object>
	std::map<std::string, Signal_LIN> signalsLibrary{};
	// A hash table that stores all info of signal encoding types. <encoding type name, Signal Encoding type object>
	std::map<std::string, SignalEncodingType> sigEncodingTypeLibrary{};
	// A hash table that stores all info of schedule tables. <schedule table name, schedule table object>
	std::map<std::string, ScheduleTable> scheduleTableLibrary{};
	// <schedule table id, schedule table name>
	std::map<unsigned int, std::string> scheduleTableIdToName{};
	// Function used to parse LDF file
	void resetParsedContent();
	void consistencyCheck();
	void loadAndParseFromFile(std::istream& in);
	std::vector<std::pair<unsigned long,std::vector<std::pair<std::string, double>>>> NodeSignals;

};

#endif /* ldf_parser_hpp */
