/**
* @file
* @brief   The main file of PE-sieve built as an EXE
*/

#include <windows.h>
#include <psapi.h>
#include <sstream>
#include <fstream>

#include "pe_sieve.h"
#include "params.h"

#include "utils/process_privilege.h"
#include "params_info/pe_sieve_params_info.h"
#include "utils/process_reflection.h"
#include "utils/console_color.h"
#include "color_scheme.h"


using namespace pesieve;
using namespace pesieve::util;


void print_report(const pesieve::ReportEx& report, const t_params args)
{
	if (!report.scan_report) return;

	std::string report_str;
	if (args.json_output) {
		report_str = report_to_json(report, pesieve::REPORT_ALL, ProcessScanReport::REPORT_SUSPICIOUS_AND_ERRORS, args.json_lvl);
	} else {
		report_str = scan_report_to_string(*report.scan_report);
	}
	//summary:
	std::cout << report_str;
	if (!args.json_output) {
		std::cout << "---" << std::endl;
	}
}

void free_params(t_params &args)
{
	free_strparam(args.modules_ignored);
}

std::string version_to_str(DWORD version)
{
	BYTE* chunks = (BYTE*)&version;
	std::stringstream stream;
	stream << std::hex <<
		(int)chunks[3] << "." <<
		(int)chunks[2] << "." <<
		(int)chunks[1] << "." <<
		(int)chunks[0];

	return stream.str();
}

int main(int argc, char *argv[])
{
	bool info_req = false;
	t_params args = { 0 };

	PEsieveParams uParams(version_to_str(PESIEVE_VERSION_ID));
	if (argc < 2) {
		uParams.printBanner();
		uParams.printBriefInfo();
		system("pause");
		return PESIEVE_INFO;
	}
	if (!uParams.parse(argc, argv)) {
		return PESIEVE_INFO;
	}
	uParams.fillStruct(args);
	//---
	// if scanning of inaccessible pages was requested, auto-enable reflection mode:
	if (args.data == pesieve::PE_DATA_SCAN_INACCESSIBLE || args.data == pesieve::PE_DATA_SCAN_INACCESSIBLE_ONLY) {
		if (!args.make_reflection) {
			args.make_reflection = true;
			if (!args.quiet) {
				paramkit::print_in_color(paramkit::WARNING_COLOR, "[WARNING] Scanning of inaccessible pages requested: auto-enabled reflection mode!\n");
			}
		}
	}
	//print info about current settings:
	if (!args.quiet) {
		std::cout << "PID: " << args.pid << std::endl;
		std::cout << "Output filter: " << translate_out_filter(args.out_filter) << std::endl;
		std::cout << "Dump mode: " << translate_dump_mode(args.dump_mode) << std::endl;
	}

	pesieve::ReportEx* report = pesieve::scan_and_dump(args);
	t_pesieve_res res = PESIEVE_ERROR;
	if (report != nullptr) {
		print_report(*report, args);

		pesieve::t_report summary = report->scan_report->generateSummary();
		if (summary.scanned > 0) {
			res = (summary.suspicious > 0) ? PESIEVE_DETECTED : PESIEVE_NOT_DETECTED;
		}
		delete report;
		report = nullptr;
	}

	free_params(args);
#ifdef _DEBUG
	system("pause");
#endif
	return res;
}
