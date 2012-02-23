#include "ReplayCodecManager.h"

using namespace std;
using namespace replay;

ReplayCodecManager* ReplayCodecManager::pInstance = NULL;

ReplayCodecManager* ReplayCodecManager::inst()
{
	if (pInstance == NULL) {
		pInstance = new ReplayCodecManager();
	}
	return pInstance;
}

ReplayCodecManager::ReplayCodecManager()
{
	int i=0;

	// --- ACORN FORMAT BLOCK ---
	M_VIDEOFORMATS[i++] = "<< No video >>";
	M_VIDEOFORMATS[i++] = "Moving Lines";
	M_VIDEOFORMATS[i++] = "16bpp Uncompressed";	// ??? RGB?
	M_VIDEOFORMATS[i++] = "10bpp YUV 4:2:2";	// 10bpp chroma subsampled by 2 horizontally
	M_VIDEOFORMATS[i++] = "8bpp uncompressed";
	M_VIDEOFORMATS[i++] = "8bpp YUV 4:2:0?";	// chroma H/V subsampled by 2
	M_VIDEOFORMATS[i++] = "8bpp YUV 4:1:0???";	// chroma H/V subsampled by 4
	M_VIDEOFORMATS[i++] = "Moving Blocks";
	M_VIDEOFORMATS[i++] = "24bpp uncompressed";	// ??? RGB?
	M_VIDEOFORMATS[i++] = "16bpp YUV 4:2:2?";	// chroma horiz subsampled by 2 (YUV)
	M_VIDEOFORMATS[i++] = "12bpp YUV 4:2:0?";	// chroma H/V subsampled by 2 (YUV)
	M_VIDEOFORMATS[i++] = "9bpp YUV 4:1:0???";	// chroma H/V subsampled by 4 (6Y5UV)
	M_VIDEOFORMATS[i++] = "Pointer to MPEG movie";
	M_VIDEOFORMATS[i++] = "MPEG video data stream in ARMovie file";
	M_VIDEOFORMATS[i++] = "IBM UltiMotion";
	M_VIDEOFORMATS[i++] = "Indirect video";
	M_VIDEOFORMATS[i++] = "12bpp YUV 4:2:0?";	// chroma H/V subsampled by 2 (6Y5UV)
	M_VIDEOFORMATS[i++] = "Moving Blocks HQ (YUV)";
	M_VIDEOFORMATS[i++] = "H.263 (6Y5UV)";
	M_VIDEOFORMATS[i++] = "Super Moving Blocks (6Y5UV)";
	M_VIDEOFORMATS[i++] = "Moving Blocks Beta (6Y6UV)";
	M_VIDEOFORMATS[i++] = "16bpp YUV Chroma horiz subsampled by 2 (6Y5UV)";
	M_VIDEOFORMATS[i++] = "12bpp YY8UVd4 chroma horiz subsampled by 2 (6Y5UV)";
	M_VIDEOFORMATS[i++] = "11bpp 6Y6Y5U5V chroma horiz subsampled by 2 (6Y5UV)";
	M_VIDEOFORMATS[i++] = "8.25vpp 6Y5UV chroma H/V subsampled by 2 (6Y5UV)";
	M_VIDEOFORMATS[i++] = "6bpp YYYYd4UVd4 chroma H/V subsampled by 2 (6Y6UV)";

	// --- THIRD PARTY BLOCKS ---
	// TODO: Fill in the ones we know about (HBP's decoders for instance)
	for (i=100; i<200; i++)
		M_VIDEOFORMATS[i] = "Unknown / EIDOS";
	for (i=200; i<300; i++)
		M_VIDEOFORMATS[i] = "Unknown / Irlam Instruments";
	for (i=300; i<400; i++)
		M_VIDEOFORMATS[i] = "Unknown / Wild Vision";
	for (i=400; i<500; i++)
		M_VIDEOFORMATS[i] = "Unknown / Aspex Software";
	for (i=500; i<600; i++)
		M_VIDEOFORMATS[i] = "Unknown / Iota Software";
	for (i=600; i<700; i++)
		M_VIDEOFORMATS[i] = "Unknown / Warm Silence Software";
	for (i=800; i<809; i++)
		M_VIDEOFORMATS[i] = "Unknown / Small_Users_Block: Henrik Bjerregard Pedersen";
	for (i=900; i<1000; i++)
		M_VIDEOFORMATS[i] = "Unknown / Innovative Media Solutions";
}

const string ReplayCodecManager::getVideoCodecName(int id) const
{
	if (M_VIDEOFORMATS.count(id) != 0) {
		return M_VIDEOFORMATS.find(id)->second;
	} else {
		return "";
	}
}

