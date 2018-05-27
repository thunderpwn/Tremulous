static const char* radixSort32KernelsDX11= \
"/*\n"
"		2011 Takahiro Harada\n"
"*/\n"
"\n"
"typedef uint u32;\n"
"\n"
"#define GET_GROUP_IDX groupIdx.x\n"
"#define GET_LOCAL_IDX localIdx.x\n"
"#define GET_GLOBAL_IDX globalIdx.x\n"
"#define GROUP_LDS_BARRIER GroupMemoryBarrierWithGroupSync()\n"
"#define GROUP_MEM_FENCE\n"
"#define DEFAULT_ARGS uint3 globalIdx : SV_DispatchThreadID, uint3 localIdx : SV_GroupThreadID, uint3 groupIdx : SV_GroupID\n"
"#define AtomInc(x) InterlockedAdd(x, 1)\n"
"#define AtomInc1(x, out) InterlockedAdd(x, 1, out)\n"
"#define AtomAdd(x, inc) InterlockedAdd(x, inc)\n"
"\n"
"#define make_uint4 uint4\n"
"#define make_uint2 uint2\n"
"\n"
"uint4 SELECT_UINT4(uint4 b,uint4 a,uint4 condition ){ return  make_uint4( ((condition).x)?a.x:b.x, ((condition).y)?a.y:b.y, ((condition).z)?a.z:b.z, ((condition).w)?a.w:b.w ); }\n"
"\n"
"\n"
"#define WG_SIZE 64\n"
"#define ELEMENTS_PER_WORK_ITEM (256/WG_SIZE)\n"
"#define BITS_PER_PASS 4\n"
"#define NUM_BUCKET (1<<BITS_PER_PASS)\n"
"\n"
"//	this isn't optimization for VLIW. But just reducing writes. \n"
"#define USE_2LEVEL_REDUCE 1\n"
"\n"
"//#define CHECK_BOUNDARY 1\n"
"\n"
"//#define NV_GPU 1\n"
"\n"
"//	Cypress\n"
"#define nPerWI 16\n"
"//	Cayman\n"
"//#define nPerWI 20\n"
"\n"
"\n"
"#define GET_GROUP_SIZE WG_SIZE\n"
"\n"
"\n"
"cbuffer SortCB : register( b0 )\n"
"{\n"
"	int m_n;\n"
"	int m_nWGs;\n"
"	int m_startBit;\n"
"	int m_nBlocksPerWG;\n"
"};\n"
"\n"
"\n"
"StructuredBuffer<u32> gSrc : register( t0 );\n"
"StructuredBuffer<u32> gSrcVal : register( t1 );\n"
"StructuredBuffer<u32> rHistogram : register( t1 );\n"
"StructuredBuffer<u32> rHistogram2 : register( t2 );\n"
"RWStructuredBuffer<u32> histogramOut : register( u0 );\n"
"RWStructuredBuffer<u32> wHistogram1 : register( u0 );\n"
"RWStructuredBuffer<u32> gDst : register( u0 );\n"
"RWStructuredBuffer<u32> gDstVal : register( u1 );\n"
"\n"
"groupshared u32 localHistogramMat[NUM_BUCKET*WG_SIZE];\n"
"#define MY_HISTOGRAM(idx) localHistogramMat[(idx)*WG_SIZE+lIdx]\n"
"\n"
"\n"
"[numthreads(WG_SIZE, 1, 1)]\n"
"void StreamCountKernel( DEFAULT_ARGS )\n"
"{\n"
"	u32 gIdx = GET_GLOBAL_IDX;\n"
"	u32 lIdx = GET_LOCAL_IDX;\n"
"	u32 wgIdx = GET_GROUP_IDX;\n"
"	u32 wgSize = GET_GROUP_SIZE;\n"
"	const int startBit = m_startBit;\n"
"\n"
"	const int n = m_n;\n"
"	const int nWGs = m_nWGs;\n"
"	const int nBlocksPerWG = m_nBlocksPerWG;\n"
"\n"
"	for(int i=0; i<NUM_BUCKET; i++)\n"
"	{\n"
"		MY_HISTOGRAM(i) = 0;\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	const int blockSize = ELEMENTS_PER_WORK_ITEM*WG_SIZE;\n"
"	u32 localKey;\n"
"\n"
"	int nBlocks = (n)/blockSize - nBlocksPerWG*wgIdx;\n"
"\n"
"	int addr = blockSize*nBlocksPerWG*wgIdx + ELEMENTS_PER_WORK_ITEM*lIdx;\n"
"\n"
"	for(int iblock=0; iblock<min(nBlocksPerWG, nBlocks); iblock++, addr+=blockSize)\n"
"	{\n"
"		//	MY_HISTOGRAM( localKeys.x ) ++ is much expensive than atomic add as it requires read and write while atomics can just add on AMD\n"
"		//	Using registers didn't perform well. It seems like use localKeys to address requires a lot of alu ops\n"
"		//	AMD: AtomInc performs better while NV prefers ++\n"
"		for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"		{\n"
"#if defined(CHECK_BOUNDARY)\n"
"			if( addr+i < n )\n"
"#endif\n"
"			{\n"
"				localKey = (gSrc[addr+i]>>startBit) & 0xf;\n"
"#if defined(NV_GPU)\n"
"				MY_HISTOGRAM( localKey )++;\n"
"#else\n"
"				AtomInc( MY_HISTOGRAM( localKey ) );\n"
"#endif\n"
"			}\n"
"		}\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"	\n"
"	if( lIdx < NUM_BUCKET )\n"
"	{\n"
"		u32 sum = 0;\n"
"		for(int i=0; i<GET_GROUP_SIZE; i++)\n"
"		{\n"
"			sum += localHistogramMat[lIdx*WG_SIZE+(i+lIdx)%GET_GROUP_SIZE];\n"
"		}\n"
"		histogramOut[lIdx*nWGs+wgIdx] = sum;\n"
"	}\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"uint prefixScanVectorEx( inout uint4 data )\n"
"{\n"
"	u32 sum = 0;\n"
"	u32 tmp = data.x;\n"
"	data.x = sum;\n"
"	sum += tmp;\n"
"	tmp = data.y;\n"
"	data.y = sum;\n"
"	sum += tmp;\n"
"	tmp = data.z;\n"
"	data.z = sum;\n"
"	sum += tmp;\n"
"	tmp = data.w;\n"
"	data.w = sum;\n"
"	sum += tmp;\n"
"	return sum;\n"
"}\n"
"\n"
"\n"
"groupshared u32 ldsSortData[WG_SIZE*ELEMENTS_PER_WORK_ITEM+16];\n"
"//groupshared u32 ldsSortData1[128*2];\n"
"\n"
"u32 localPrefixSum( u32 pData, uint lIdx, inout uint totalSum, int wgSize /*64 or 128*/ )\n"
"{\n"
"	{	//	Set data\n"
"		ldsSortData[lIdx] = 0;\n"
"		ldsSortData[lIdx+wgSize] = pData;\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	{	//	Prefix sum\n"
"		int idx = 2*lIdx + (wgSize+1);\n"
"#if defined(USE_2LEVEL_REDUCE)\n"
"		if( lIdx < 64 )\n"
"		{\n"
"			u32 u0, u1, u2;\n"
"			u0 = ldsSortData[idx-3];\n"
"			u1 = ldsSortData[idx-2];\n"
"			u2 = ldsSortData[idx-1];\n"
"			AtomAdd( ldsSortData[idx], u0+u1+u2 );			\n"
"			GROUP_MEM_FENCE;\n"
"\n"
"			u0 = ldsSortData[idx-12];\n"
"			u1 = ldsSortData[idx-8];\n"
"			u2 = ldsSortData[idx-4];\n"
"			AtomAdd( ldsSortData[idx], u0+u1+u2 );			\n"
"			GROUP_MEM_FENCE;\n"
"\n"
"			u0 = ldsSortData[idx-48];\n"
"			u1 = ldsSortData[idx-32];\n"
"			u2 = ldsSortData[idx-16];\n"
"			AtomAdd( ldsSortData[idx], u0+u1+u2 );			\n"
"			GROUP_MEM_FENCE;\n"
"			if( wgSize > 64 )\n"
"			{\n"
"				ldsSortData[idx] += ldsSortData[idx-64];\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"\n"
"			ldsSortData[idx-1] += ldsSortData[idx-2];\n"
"			GROUP_MEM_FENCE;\n"
"		}\n"
"#else\n"
"		if( lIdx < 64 )\n"
"		{\n"
"			ldsSortData[idx] += ldsSortData[idx-1];\n"
"			GROUP_MEM_FENCE;\n"
"			ldsSortData[idx] += ldsSortData[idx-2];			\n"
"			GROUP_MEM_FENCE;\n"
"			ldsSortData[idx] += ldsSortData[idx-4];\n"
"			GROUP_MEM_FENCE;\n"
"			ldsSortData[idx] += ldsSortData[idx-8];\n"
"			GROUP_MEM_FENCE;\n"
"			ldsSortData[idx] += ldsSortData[idx-16];\n"
"			GROUP_MEM_FENCE;\n"
"			ldsSortData[idx] += ldsSortData[idx-32];\n"
"			GROUP_MEM_FENCE;\n"
"			if( wgSize > 64 )\n"
"			{\n"
"				ldsSortData[idx] += ldsSortData[idx-64];\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"\n"
"			ldsSortData[idx-1] += ldsSortData[idx-2];\n"
"			GROUP_MEM_FENCE;\n"
"		}\n"
"#endif\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	totalSum = ldsSortData[wgSize*2-1];\n"
"	u32 addValue = ldsSortData[lIdx+wgSize-1];\n"
"	return addValue;\n"
"}\n"
"\n"
"//__attribute__((reqd_work_group_size(128,1,1)))\n"
"uint4 localPrefixSum128V( uint4 pData, uint lIdx, inout uint totalSum )\n"
"{\n"
"	u32 s4 = prefixScanVectorEx( pData );\n"
"	u32 rank = localPrefixSum( s4, lIdx, totalSum, 128 );\n"
"	return pData + make_uint4( rank, rank, rank, rank );\n"
"}\n"
"\n"
"//__attribute__((reqd_work_group_size(64,1,1)))\n"
"uint4 localPrefixSum64V( uint4 pData, uint lIdx, inout uint totalSum )\n"
"{\n"
"	u32 s4 = prefixScanVectorEx( pData );\n"
"	u32 rank = localPrefixSum( s4, lIdx, totalSum, 64 );\n"
"	return pData + make_uint4( rank, rank, rank, rank );\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"#define nPerLane (nPerWI/4)\n"
"\n"
"//	NUM_BUCKET*nWGs < 128*nPerWI\n"
"[numthreads(128, 1, 1)]\n"
"void PrefixScanKernel( DEFAULT_ARGS )\n"
"{\n"
"	u32 lIdx = GET_LOCAL_IDX;\n"
"	u32 wgIdx = GET_GROUP_IDX;\n"
"	const int nWGs = m_nWGs;\n"
"\n"
"	u32 data[nPerWI];\n"
"	for(int i=0; i<nPerWI; i++)\n"
"	{\n"
"		data[i] = 0;\n"
"		if( (nPerWI*lIdx+i) < NUM_BUCKET*nWGs )\n"
"			data[i] = wHistogram1[nPerWI*lIdx+i];\n"
"	}\n"
"\n"
"	uint4 myData = make_uint4(0,0,0,0);\n"
"\n"
"	for(int i=0; i<nPerLane; i++)\n"
"	{\n"
"		myData.x += data[nPerLane*0+i];\n"
"		myData.y += data[nPerLane*1+i];\n"
"		myData.z += data[nPerLane*2+i];\n"
"		myData.w += data[nPerLane*3+i];\n"
"	}\n"
"\n"
"	uint totalSum;\n"
"	uint4 scanned = localPrefixSum128V( myData, lIdx, totalSum );\n"
"\n"
"//	for(int j=0; j<4; j++) //	somehow it introduces a lot of branches\n"
"	{	int j = 0;\n"
"		u32 sum = 0;\n"
"		for(int i=0; i<nPerLane; i++)\n"
"		{\n"
"			u32 tmp = data[nPerLane*j+i];\n"
"			data[nPerLane*j+i] = sum;\n"
"			sum += tmp;\n"
"		}\n"
"	}\n"
"	{	int j = 1;\n"
"		u32 sum = 0;\n"
"		for(int i=0; i<nPerLane; i++)\n"
"		{\n"
"			u32 tmp = data[nPerLane*j+i];\n"
"			data[nPerLane*j+i] = sum;\n"
"			sum += tmp;\n"
"		}\n"
"	}\n"
"	{	int j = 2;\n"
"		u32 sum = 0;\n"
"		for(int i=0; i<nPerLane; i++)\n"
"		{\n"
"			u32 tmp = data[nPerLane*j+i];\n"
"			data[nPerLane*j+i] = sum;\n"
"			sum += tmp;\n"
"		}\n"
"	}\n"
"	{	int j = 3;\n"
"		u32 sum = 0;\n"
"		for(int i=0; i<nPerLane; i++)\n"
"		{\n"
"			u32 tmp = data[nPerLane*j+i];\n"
"			data[nPerLane*j+i] = sum;\n"
"			sum += tmp;\n"
"		}\n"
"	}\n"
"\n"
"	for(int i=0; i<nPerLane; i++)\n"
"	{\n"
"		data[nPerLane*0+i] += scanned.x;\n"
"		data[nPerLane*1+i] += scanned.y;\n"
"		data[nPerLane*2+i] += scanned.z;\n"
"		data[nPerLane*3+i] += scanned.w;\n"
"	}\n"
"\n"
"	for(int i=0; i<nPerWI; i++)\n"
"	{\n"
"		wHistogram1[nPerWI*lIdx+i] = data[i];\n"
"	}\n"
"}\n"
"\n"
"\n"
"\n"
"u32 unpack4Key( u32 key, int keyIdx ){ return (key>>(keyIdx*8)) & 0xff;}\n"
"\n"
"u32 bit8Scan(u32 v)\n"
"{\n"
"	return (v<<8) + (v<<16) + (v<<24);\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"void sort4Bits1(inout u32 sortData[4], int startBit, int lIdx)\n"
"{\n"
"/*\n"
"	for(uint ibit=0; ibit<BITS_PER_PASS; ibit+=2)\n"
"	{\n"
"		uint4 b = make_uint4((sortData[0]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[1]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[2]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[3]>>(startBit+ibit)) & 0x3);\n"
"\n"
"		u32 key4;\n"
"		u32 sKeyPacked[4] = { 0, 0, 0, 0 };\n"
"		{\n"
"			sKeyPacked[0] |= 1<<(8*b.x);\n"
"			sKeyPacked[1] |= 1<<(8*b.y);\n"
"			sKeyPacked[2] |= 1<<(8*b.z);\n"
"			sKeyPacked[3] |= 1<<(8*b.w);\n"
"\n"
"			key4 = sKeyPacked[0] + sKeyPacked[1] + sKeyPacked[2] + sKeyPacked[3];\n"
"		}\n"
"\n"
"		u32 rankPacked;\n"
"		u32 sumPacked;\n"
"		{\n"
"			rankPacked = localPrefixSum64VSingle( key4, lIdx, sumPacked );\n"
"		}\n"
"\n"
"//		GROUP_LDS_BARRIER;\n"
"\n"
"		u32 sum[4] = { unpack4Key( sumPacked,0 ), unpack4Key( sumPacked,1 ), unpack4Key( sumPacked,2 ), unpack4Key( sumPacked,3 ) };\n"
"\n"
"		{\n"
"			u32 sum4 = 0;\n"
"			for(int ie=0; ie<4; ie++)\n"
"			{\n"
"				u32 tmp = sum[ie];\n"
"				sum[ie] = sum4;\n"
"				sum4 += tmp;\n"
"			}\n"
"		}\n"
"\n"
"		u32 newOffset[4] = { 0,0,0,0 };\n"
"\n"
"		for(int ie=0; ie<4; ie++)\n"
"		{\n"
"			uint4 key = extractKeys( b, ie );\n"
"			uint4 scannedKey = key;\n"
"			prefixScanVectorEx( scannedKey );\n"
"			uint offset = sum[ie] + unpack4Key( rankPacked, ie );\n"
"			uint4 dstAddress = make_uint4( offset, offset, offset, offset ) + scannedKey;\n"
"\n"
"			newOffset[0] += dstAddress.x*key.x;\n"
"			newOffset[1] += dstAddress.y*key.y;\n"
"			newOffset[2] += dstAddress.z*key.z;\n"
"			newOffset[3] += dstAddress.w*key.w;\n"
"		}\n"
"\n"
"\n"
"\n"
"		{\n"
"			ldsSortData[newOffset[0]] = sortData[0];\n"
"			ldsSortData[newOffset[1]] = sortData[1];\n"
"			ldsSortData[newOffset[2]] = sortData[2];\n"
"			ldsSortData[newOffset[3]] = sortData[3];\n"
"\n"
"//			GROUP_LDS_BARRIER;\n"
"\n"
"			sortData[0] = ldsSortData[lIdx*4+0];\n"
"			sortData[1] = ldsSortData[lIdx*4+1];\n"
"			sortData[2] = ldsSortData[lIdx*4+2];\n"
"			sortData[3] = ldsSortData[lIdx*4+3];\n"
"\n"
"//			GROUP_LDS_BARRIER;\n"
"		}\n"
"	}\n"
"*/\n"
"	for(uint ibit=0; ibit<BITS_PER_PASS; ibit+=2)\n"
"	{\n"
"		uint4 b = make_uint4((sortData[0]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[1]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[2]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[3]>>(startBit+ibit)) & 0x3);\n"
"\n"
"		u32 key4;\n"
"		u32 sKeyPacked[4] = { 0, 0, 0, 0 };\n"
"		{\n"
"			sKeyPacked[0] |= 1<<(8*b.x);\n"
"			sKeyPacked[1] |= 1<<(8*b.y);\n"
"			sKeyPacked[2] |= 1<<(8*b.z);\n"
"			sKeyPacked[3] |= 1<<(8*b.w);\n"
"\n"
"			key4 = sKeyPacked[0] + sKeyPacked[1] + sKeyPacked[2] + sKeyPacked[3];\n"
"		}\n"
"\n"
"		u32 rankPacked;\n"
"		u32 sumPacked;\n"
"		{\n"
"			rankPacked = localPrefixSum( key4, lIdx, sumPacked, WG_SIZE );\n"
"		}\n"
"\n"
"		GROUP_LDS_BARRIER;\n"
"\n"
"		u32 newOffset[4] = { 0,0,0,0 };\n"
"		{\n"
"			u32 sumScanned = bit8Scan( sumPacked );\n"
"\n"
"			u32 scannedKeys[4];\n"
"			scannedKeys[0] = 1<<(8*b.x);\n"
"			scannedKeys[1] = 1<<(8*b.y);\n"
"			scannedKeys[2] = 1<<(8*b.z);\n"
"			scannedKeys[3] = 1<<(8*b.w);\n"
"			{	//	4 scans at once\n"
"				u32 sum4 = 0;\n"
"				for(int ie=0; ie<4; ie++)\n"
"				{\n"
"					u32 tmp = scannedKeys[ie];\n"
"					scannedKeys[ie] = sum4;\n"
"					sum4 += tmp;\n"
"				}\n"
"			}\n"
"\n"
"			{\n"
"				u32 sumPlusRank = sumScanned + rankPacked;\n"
"				{	u32 ie = b.x;\n"
"					scannedKeys[0] += sumPlusRank;\n"
"					newOffset[0] = unpack4Key( scannedKeys[0], ie );\n"
"				}\n"
"				{	u32 ie = b.y;\n"
"					scannedKeys[1] += sumPlusRank;\n"
"					newOffset[1] = unpack4Key( scannedKeys[1], ie );\n"
"				}\n"
"				{	u32 ie = b.z;\n"
"					scannedKeys[2] += sumPlusRank;\n"
"					newOffset[2] = unpack4Key( scannedKeys[2], ie );\n"
"				}\n"
"				{	u32 ie = b.w;\n"
"					scannedKeys[3] += sumPlusRank;\n"
"					newOffset[3] = unpack4Key( scannedKeys[3], ie );\n"
"				}\n"
"			}\n"
"		}\n"
"\n"
"\n"
"		GROUP_LDS_BARRIER;\n"
"\n"
"		{\n"
"			ldsSortData[newOffset[0]] = sortData[0];\n"
"			ldsSortData[newOffset[1]] = sortData[1];\n"
"			ldsSortData[newOffset[2]] = sortData[2];\n"
"			ldsSortData[newOffset[3]] = sortData[3];\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			u32 dstAddr = 4*lIdx;\n"
"			sortData[0] = ldsSortData[dstAddr+0];\n"
"			sortData[1] = ldsSortData[dstAddr+1];\n"
"			sortData[2] = ldsSortData[dstAddr+2];\n"
"			sortData[3] = ldsSortData[dstAddr+3];\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"		}\n"
"	}\n"
"}\n"
"\n"
"\n"
"groupshared u32 localHistogramToCarry[NUM_BUCKET];\n"
"groupshared u32 localHistogram[NUM_BUCKET*2];\n"
"#define SET_HISTOGRAM(setIdx, key) ldsSortData[(setIdx)*NUM_BUCKET+key]\n"
"\n"
"\n"
"[numthreads(WG_SIZE, 1, 1)]\n"
"void SortAndScatterKernel( DEFAULT_ARGS )\n"
"{\n"
"	u32 gIdx = GET_GLOBAL_IDX;\n"
"	u32 lIdx = GET_LOCAL_IDX;\n"
"	u32 wgIdx = GET_GROUP_IDX;\n"
"	u32 wgSize = GET_GROUP_SIZE;\n"
"\n"
"	const int n = m_n;\n"
"	const int nWGs = m_nWGs;\n"
"	const int startBit = m_startBit;\n"
"	const int nBlocksPerWG = m_nBlocksPerWG;\n"
"\n"
"	if( lIdx < (NUM_BUCKET) )\n"
"	{\n"
"		localHistogramToCarry[lIdx] = rHistogram[lIdx*nWGs + wgIdx];\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	const int blockSize = ELEMENTS_PER_WORK_ITEM*WG_SIZE;\n"
"\n"
"	int nBlocks = n/blockSize - nBlocksPerWG*wgIdx;\n"
"\n"
"	int addr = blockSize*nBlocksPerWG*wgIdx + ELEMENTS_PER_WORK_ITEM*lIdx;\n"
"\n"
"	for(int iblock=0; iblock<min(nBlocksPerWG, nBlocks); iblock++, addr+=blockSize)\n"
"	{\n"
"		u32 myHistogram = 0;\n"
"\n"
"		u32 sortData[ELEMENTS_PER_WORK_ITEM];\n"
"		for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"#if defined(CHECK_BOUNDARY)\n"
"			sortData[i] = ( addr+i < n )? gSrc[ addr+i ] : 0xffffffff;\n"
"#else\n"
"			sortData[i] = gSrc[ addr+i ];\n"
"#endif\n"
"\n"
"		sort4Bits1(sortData, startBit, lIdx);\n"
"\n"
"		u32 keys[ELEMENTS_PER_WORK_ITEM];\n"
"		for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"			keys[i] = (sortData[i]>>startBit) & 0xf;\n"
"\n"
"		{	//	create histogram\n"
"			u32 setIdx = lIdx/16;\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[lIdx] = 0;\n"
"			}\n"
"			ldsSortData[lIdx] = 0;\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"#if defined(CHECK_BOUNDARY)\n"
"				if( addr+i < n )\n"
"#endif\n"
"				AtomInc( SET_HISTOGRAM( setIdx, keys[i] ) );\n"
"			\n"
"			GROUP_LDS_BARRIER;\n"
"			\n"
"			uint hIdx = NUM_BUCKET+lIdx;\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				u32 sum = 0;\n"
"				for(int i=0; i<WG_SIZE/16; i++)\n"
"				{\n"
"					sum += SET_HISTOGRAM( i, lIdx );\n"
"				}\n"
"				myHistogram = sum;\n"
"				localHistogram[hIdx] = sum;\n"
"			}\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"#if defined(USE_2LEVEL_REDUCE)\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[hIdx] = localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"\n"
"				u32 u0, u1, u2;\n"
"				u0 = localHistogram[hIdx-3];\n"
"				u1 = localHistogram[hIdx-2];\n"
"				u2 = localHistogram[hIdx-1];\n"
"				AtomAdd( localHistogram[hIdx], u0 + u1 + u2 );\n"
"				GROUP_MEM_FENCE;\n"
"				u0 = localHistogram[hIdx-12];\n"
"				u1 = localHistogram[hIdx-8];\n"
"				u2 = localHistogram[hIdx-4];\n"
"				AtomAdd( localHistogram[hIdx], u0 + u1 + u2 );\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"#else\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[hIdx] = localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-2];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-4];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-8];\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"#endif\n"
"			GROUP_LDS_BARRIER;\n"
"		}\n"
"\n"
"		{\n"
"			for(int ie=0; ie<ELEMENTS_PER_WORK_ITEM; ie++)\n"
"			{\n"
"				int dataIdx = ELEMENTS_PER_WORK_ITEM*lIdx+ie;\n"
"				int binIdx = keys[ie];\n"
"				int groupOffset = localHistogramToCarry[binIdx];\n"
"				int myIdx = dataIdx - localHistogram[NUM_BUCKET+binIdx];\n"
"#if defined(CHECK_BOUNDARY)\n"
"				if( addr+ie < n )\n"
"#endif\n"
"				gDst[ groupOffset + myIdx ] = sortData[ie];\n"
"			}\n"
"		}\n"
"\n"
"		GROUP_LDS_BARRIER;\n"
"\n"
"		if( lIdx < NUM_BUCKET )\n"
"		{\n"
"			localHistogramToCarry[lIdx] += myHistogram;\n"
"		}\n"
"		GROUP_LDS_BARRIER;\n"
"	}\n"
"/*\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	const int blockSize = ELEMENTS_PER_WORK_ITEM*WG_SIZE;\n"
"//	for(int addr=ELEMENTS_PER_WORK_ITEM*gIdx; addr<n; addr+=stride)\n"
"	[loop]\n"
"	for(int addr = blockSize*nBlocksPerWG*wgIdx+ELEMENTS_PER_WORK_ITEM*lIdx; \n"
"		addr<min(blockSize*nBlocksPerWG*(wgIdx+1), n); \n"
"		addr+=blockSize )\n"
"	{\n"
"		u32 myHistogram = 0;\n"
"\n"
"		u32 sortData[ELEMENTS_PER_WORK_ITEM];\n"
"		{\n"
"			for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"				sortData[i] = gSrc[ addr+i ];\n"
"		}\n"
"\n"
"		sort4Bits1(sortData, startBit, lIdx);\n"
"\n"
"		u32 keys[4];\n"
"		for(int i=0; i<4; i++)\n"
"			keys[i] = (sortData[i]>>startBit) & 0xf;\n"
"\n"
"		{	//	create histogram\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[lIdx] = 0;\n"
"				localHistogram[NUM_BUCKET+lIdx] = 0;\n"
"			}\n"
"//			GROUP_LDS_BARRIER;\n"
"\n"
"			AtomInc( localHistogram[NUM_BUCKET+keys[0]] );\n"
"			AtomInc( localHistogram[NUM_BUCKET+keys[1]] );\n"
"			AtomInc( localHistogram[NUM_BUCKET+keys[2]] );\n"
"			AtomInc( localHistogram[NUM_BUCKET+keys[3]] );\n"
"			\n"
"//			GROUP_LDS_BARRIER;\n"
"			\n"
"			uint hIdx = NUM_BUCKET+lIdx;\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				myHistogram = localHistogram[hIdx];\n"
"			}\n"
"//			GROUP_LDS_BARRIER;\n"
"\n"
"#if defined(USE_2LEVEL_REDUCE)\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[hIdx] = localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"\n"
"				u32 u0, u1, u2;\n"
"				u0 = localHistogram[hIdx-3];\n"
"				u1 = localHistogram[hIdx-2];\n"
"				u2 = localHistogram[hIdx-1];\n"
"				AtomAdd( localHistogram[hIdx], u0 + u1 + u2 );\n"
"				GROUP_MEM_FENCE;\n"
"				u0 = localHistogram[hIdx-12];\n"
"				u1 = localHistogram[hIdx-8];\n"
"				u2 = localHistogram[hIdx-4];\n"
"				AtomAdd( localHistogram[hIdx], u0 + u1 + u2 );\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"#else\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[hIdx] = localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-2];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-4];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-8];\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"#endif\n"
"\n"
"//			GROUP_LDS_BARRIER;\n"
"		}\n"
"\n"
"		{\n"
"			for(int ie=0; ie<ELEMENTS_PER_WORK_ITEM; ie++)\n"
"			{\n"
"				int dataIdx = 4*lIdx+ie;\n"
"				int binIdx = keys[ie];\n"
"				int groupOffset = localHistogramToCarry[binIdx];\n"
"				int myIdx = dataIdx - localHistogram[NUM_BUCKET+binIdx];\n"
"				gDst[ groupOffset + myIdx ] = sortData[ie];\n"
"			}\n"
"		}\n"
"\n"
"//		GROUP_LDS_BARRIER;\n"
"\n"
"		if( lIdx < NUM_BUCKET )\n"
"		{\n"
"			localHistogramToCarry[lIdx] += myHistogram;\n"
"		}\n"
"//		GROUP_LDS_BARRIER;\n"
"\n"
"	}\n"
"*/\n"
"}\n"
"\n"
"groupshared int ldsSortVal[WG_SIZE*ELEMENTS_PER_WORK_ITEM+16];\n"
"\n"
"void sort4Bits1KeyValue(inout u32 sortData[4], inout int sortVal[4], int startBit, int lIdx)\n"
"{\n"
"	for(uint ibit=0; ibit<BITS_PER_PASS; ibit+=2)\n"
"	{\n"
"		uint4 b = make_uint4((sortData[0]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[1]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[2]>>(startBit+ibit)) & 0x3, \n"
"			(sortData[3]>>(startBit+ibit)) & 0x3);\n"
"\n"
"		u32 key4;\n"
"		u32 sKeyPacked[4] = { 0, 0, 0, 0 };\n"
"		{\n"
"			sKeyPacked[0] |= 1<<(8*b.x);\n"
"			sKeyPacked[1] |= 1<<(8*b.y);\n"
"			sKeyPacked[2] |= 1<<(8*b.z);\n"
"			sKeyPacked[3] |= 1<<(8*b.w);\n"
"\n"
"			key4 = sKeyPacked[0] + sKeyPacked[1] + sKeyPacked[2] + sKeyPacked[3];\n"
"		}\n"
"\n"
"		u32 rankPacked;\n"
"		u32 sumPacked;\n"
"		{\n"
"			rankPacked = localPrefixSum( key4, lIdx, sumPacked, WG_SIZE );\n"
"		}\n"
"\n"
"		GROUP_LDS_BARRIER;\n"
"\n"
"		u32 newOffset[4] = { 0,0,0,0 };\n"
"		{\n"
"			u32 sumScanned = bit8Scan( sumPacked );\n"
"\n"
"			u32 scannedKeys[4];\n"
"			scannedKeys[0] = 1<<(8*b.x);\n"
"			scannedKeys[1] = 1<<(8*b.y);\n"
"			scannedKeys[2] = 1<<(8*b.z);\n"
"			scannedKeys[3] = 1<<(8*b.w);\n"
"			{	//	4 scans at once\n"
"				u32 sum4 = 0;\n"
"				for(int ie=0; ie<4; ie++)\n"
"				{\n"
"					u32 tmp = scannedKeys[ie];\n"
"					scannedKeys[ie] = sum4;\n"
"					sum4 += tmp;\n"
"				}\n"
"			}\n"
"\n"
"			{\n"
"				u32 sumPlusRank = sumScanned + rankPacked;\n"
"				{	u32 ie = b.x;\n"
"					scannedKeys[0] += sumPlusRank;\n"
"					newOffset[0] = unpack4Key( scannedKeys[0], ie );\n"
"				}\n"
"				{	u32 ie = b.y;\n"
"					scannedKeys[1] += sumPlusRank;\n"
"					newOffset[1] = unpack4Key( scannedKeys[1], ie );\n"
"				}\n"
"				{	u32 ie = b.z;\n"
"					scannedKeys[2] += sumPlusRank;\n"
"					newOffset[2] = unpack4Key( scannedKeys[2], ie );\n"
"				}\n"
"				{	u32 ie = b.w;\n"
"					scannedKeys[3] += sumPlusRank;\n"
"					newOffset[3] = unpack4Key( scannedKeys[3], ie );\n"
"				}\n"
"			}\n"
"		}\n"
"\n"
"\n"
"		GROUP_LDS_BARRIER;\n"
"\n"
"		{\n"
"			ldsSortData[newOffset[0]] = sortData[0];\n"
"			ldsSortData[newOffset[1]] = sortData[1];\n"
"			ldsSortData[newOffset[2]] = sortData[2];\n"
"			ldsSortData[newOffset[3]] = sortData[3];\n"
"\n"
"			ldsSortVal[newOffset[0]] = sortVal[0];\n"
"			ldsSortVal[newOffset[1]] = sortVal[1];\n"
"			ldsSortVal[newOffset[2]] = sortVal[2];\n"
"			ldsSortVal[newOffset[3]] = sortVal[3];\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			u32 dstAddr = 4*lIdx;\n"
"			sortData[0] = ldsSortData[dstAddr+0];\n"
"			sortData[1] = ldsSortData[dstAddr+1];\n"
"			sortData[2] = ldsSortData[dstAddr+2];\n"
"			sortData[3] = ldsSortData[dstAddr+3];\n"
"\n"
"			sortVal[0] = ldsSortVal[dstAddr+0];\n"
"			sortVal[1] = ldsSortVal[dstAddr+1];\n"
"			sortVal[2] = ldsSortVal[dstAddr+2];\n"
"			sortVal[3] = ldsSortVal[dstAddr+3];\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"		}\n"
"	}\n"
"}\n"
"\n"
"\n"
"\n"
"[numthreads(WG_SIZE, 1, 1)]\n"
"void SortAndScatterKeyValueKernel( DEFAULT_ARGS )\n"
"{\n"
"	u32 gIdx = GET_GLOBAL_IDX;\n"
"	u32 lIdx = GET_LOCAL_IDX;\n"
"	u32 wgIdx = GET_GROUP_IDX;\n"
"	u32 wgSize = GET_GROUP_SIZE;\n"
"\n"
"	const int n = m_n;\n"
"	const int nWGs = m_nWGs;\n"
"	const int startBit = m_startBit;\n"
"	const int nBlocksPerWG = m_nBlocksPerWG;\n"
"\n"
"	if( lIdx < (NUM_BUCKET) )\n"
"	{\n"
"		localHistogramToCarry[lIdx] = rHistogram2[lIdx*nWGs + wgIdx];\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	const int blockSize = ELEMENTS_PER_WORK_ITEM*WG_SIZE;\n"
"\n"
"	int nBlocks = n/blockSize - nBlocksPerWG*wgIdx;\n"
"\n"
"	int addr = blockSize*nBlocksPerWG*wgIdx + ELEMENTS_PER_WORK_ITEM*lIdx;\n"
"\n"
"	for(int iblock=0; iblock<min(nBlocksPerWG, nBlocks); iblock++, addr+=blockSize)\n"
"	{\n"
"\n"
"		u32 myHistogram = 0;\n"
"\n"
"		u32 sortData[ELEMENTS_PER_WORK_ITEM];\n"
"		int sortVal[ELEMENTS_PER_WORK_ITEM];\n"
"\n"
"		for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"#if defined(CHECK_BOUNDARY)\n"
"		{\n"
"			sortData[i] = ( addr+i < n )? gSrc[ addr+i ] : 0xffffffff;\n"
"			sortVal[i] = ( addr+i < n )? gSrcVal[ addr+i ] : 0xffffffff;\n"
"		}\n"
"#else\n"
"		{\n"
"			sortData[i] = gSrc[ addr+i ];\n"
"			sortVal[i] = gSrcVal[ addr+i ];\n"
"		}\n"
"#endif\n"
"		sort4Bits1KeyValue(sortData, sortVal, startBit, lIdx);\n"
"\n"
"		u32 keys[ELEMENTS_PER_WORK_ITEM];\n"
"		for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"			keys[i] = (sortData[i]>>startBit) & 0xf;\n"
"\n"
"		{	//	create histogram\n"
"			u32 setIdx = lIdx/16;\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[lIdx] = 0;\n"
"			}\n"
"			ldsSortData[lIdx] = 0;\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			for(int i=0; i<ELEMENTS_PER_WORK_ITEM; i++)\n"
"#if defined(CHECK_BOUNDARY)\n"
"				if( addr+i < n )\n"
"#endif\n"
"				AtomInc( SET_HISTOGRAM( setIdx, keys[i] ) );\n"
"			\n"
"			GROUP_LDS_BARRIER;\n"
"			\n"
"			uint hIdx = NUM_BUCKET+lIdx;\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				u32 sum = 0;\n"
"				for(int i=0; i<WG_SIZE/16; i++)\n"
"				{\n"
"					sum += SET_HISTOGRAM( i, lIdx );\n"
"				}\n"
"				myHistogram = sum;\n"
"				localHistogram[hIdx] = sum;\n"
"			}\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"#if defined(USE_2LEVEL_REDUCE)\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[hIdx] = localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"\n"
"				u32 u0, u1, u2;\n"
"				u0 = localHistogram[hIdx-3];\n"
"				u1 = localHistogram[hIdx-2];\n"
"				u2 = localHistogram[hIdx-1];\n"
"				AtomAdd( localHistogram[hIdx], u0 + u1 + u2 );\n"
"				GROUP_MEM_FENCE;\n"
"				u0 = localHistogram[hIdx-12];\n"
"				u1 = localHistogram[hIdx-8];\n"
"				u2 = localHistogram[hIdx-4];\n"
"				AtomAdd( localHistogram[hIdx], u0 + u1 + u2 );\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"#else\n"
"			if( lIdx < NUM_BUCKET )\n"
"			{\n"
"				localHistogram[hIdx] = localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-1];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-2];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-4];\n"
"				GROUP_MEM_FENCE;\n"
"				localHistogram[hIdx] += localHistogram[hIdx-8];\n"
"				GROUP_MEM_FENCE;\n"
"			}\n"
"#endif\n"
"			GROUP_LDS_BARRIER;\n"
"		}\n"
"\n"
"		{\n"
"			for(int ie=0; ie<ELEMENTS_PER_WORK_ITEM; ie++)\n"
"			{\n"
"				int dataIdx = ELEMENTS_PER_WORK_ITEM*lIdx+ie;\n"
"				int binIdx = keys[ie];\n"
"				int groupOffset = localHistogramToCarry[binIdx];\n"
"				int myIdx = dataIdx - localHistogram[NUM_BUCKET+binIdx];\n"
"#if defined(CHECK_BOUNDARY)\n"
"				if( addr+ie < n )\n"
"				{\n"
"					gDst[groupOffset + myIdx ] = sortData[ie];\n"
"					gDstVal[ groupOffset + myIdx ] = sortVal[ie];\n"
"				}\n"
"#else\n"
"				gDst[ groupOffset + myIdx ] = sortData[ie];\n"
"				gDstVal[ groupOffset + myIdx ] = sortVal[ie];		\n"
"#endif\n"
"			}\n"
"		}\n"
"\n"
"		GROUP_LDS_BARRIER;\n"
"\n"
"		if( lIdx < NUM_BUCKET )\n"
"		{\n"
"			localHistogramToCarry[lIdx] += myHistogram;\n"
"		}\n"
"		GROUP_LDS_BARRIER;\n"
"	}\n"
"}\n"
"\n"
"[numthreads(WG_SIZE, 1, 1)]\n"
"void SortAndScatterSortDataKernel( DEFAULT_ARGS )\n"
"{\n"
"\n"
"}\n"
"\n"
"[numthreads(WG_SIZE, 1, 1)]\n"
"void StreamCountSortDataKernel( DEFAULT_ARGS )\n"
"{\n"
"}\n"
;
