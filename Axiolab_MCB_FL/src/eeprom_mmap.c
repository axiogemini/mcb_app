/*
 * eeprom_mmap.c
 *
 *  Created on: 2016��6��30��
 *      Author: zctiaxia
 */

#include "include.h"


const uint8_t MMAP_MCB[MEMDEP_MCB][3] = {			// descriptor-ID: 0x1000
//		entry		addr	format code
		0x01 	,	0	,	1	,
		0x02 	,	20	,	5	,
		0x03 	,	22	,	5	,
		0x04 	,	24	,	3	,
		0x05 	,	25	,	3	,
		0x06 	,	26	,	5	,
		0x30 	,	28	,	1	,
		0x32 	,	48	,	1	,
		0x35 	,	68	,	1	,
		0x42	,	88	,	1	,
		0x43 	,	108	,	16	,
		0x44 	,	112	,	1	,
		0x46 	,	132	,	3

};

const uint8_t MMAP_XYZ[MEMDEP_XYZ][3] = {			// descriptor-ID: 0x1100
		0x01 	,	256	,	1	,
		0x02 	,	276	,	5	,
		0x03 	,	278	,	5	,
		0x04 	,	280	,	3	,
		0x05 	,	281	,	3	,
		0x06 	,	282	,	5	,
		0x12 	,	284	,	6	,
		0x30 	,	288	,	6	,
		0x33 	,	292	,	6	,
		0x41 	,	296	,	6	,
		0x42 	,	300	,	6	,
		0x43 	,	304	,	6	,
		0x44 	,	308	,	6	,
		0x45 	,	312	,	6	,
		0x46 	,	316	,	6	,
		0x91 	,	320	,	6	,
		0x92 	,	324	,	6	,
		0x93 	,	328	,	6	,
		0x94 	,	332	,	6	,
		0x95 	,	336	,	6	,
		0x96 	,	340	,	6	,
		0xA1 	,	344	,	6	,
		0xA2 	,	348	,	6	,
		0xA3 	,	352	,	6	,
		0xA4 	,	356	,	6	,
		0xA5 	,	360	,	6	,
		0xA6 	,	364	,	6
};

const uint8_t MMAP_LIGHTCTRL[MEMDEP_LIGHTCTRL][3] = {		// descriptor-ID: 0x1200
		0x01 	,	512	,	1	,
		0x02 	,	532	,	5	,
		0x03 	,	534	,	5	,
		0x04 	,	536	,	3	,
		0x05 	,	537	,	3	,
		0x06 	,	538	,	5	,
		0x10 	,	540	,	1	,
		0x11 	,	560	,	3	,
		0x12 	,	561	,	2	,
		0x13 	,	562	,	2	,
		0x14 	,	563	,	5	,
		0x2A 	,	565	,	5	,
		0x2C 	,	567	,	5
};

const uint8_t MMAP_RFLM[MEMDEP_RFLM][3] = {					// descriptor-ID: 0x1201
		0x01 	,	768	,	1	,
		0x02 	,	788	,	5	,
		0x03 	,	790	,	5	,
		0x04 	,	792	,	3	,
		0x05 	,	793	,	3	,
		0x06 	,	794	,	5	,
		0x10 	,	796	,	5	,
		0x11 	,	798	,	5	,
		0x12 	,	800	,	5	,
		0x13 	,	802	,	5	,
		0x14 	,	804	,	5	,
		0x15 	,	806	,	5	,
		0x20 	,	808	,	5	,
		0x21 	,	810	,	5	,
		0x22 	,	812	,	5	,
		0x23 	,	814	,	5	,
		0x24 	,	816	,	5	,
		0x25 	,	818	,	5	,
		0x30 	,	820	,	5	,
		0x31 	,	822	,	5	,
		0x32 	,	824	,	5	,
		0x33 	,	826	,	5	,
		0x34 	,	828	,	5	,
		0x35 	,	830	,	5	,
		0x40 	,	832	,	5	,
		0x41 	,	834	,	5	,
		0x42 	,	836	,	5	,
		0x43 	,	838	,	5	,
		0x44 	,	840	,	5	,
		0x45 	,	842	,	5	,
		0x50	,	844	,	5	,
		0x51	,	846	,	5	,
		0x52 	,	848	,	5	,
		0x53 	,	850	,	5	,
		0x54 	,	852	,	5	,
		0x55 	,	854	,	5	,
		0x60 	,	856	,	5	,
		0x61 	,	858	,	5	,
		0x62 	,	860	,	5	,
		0x63 	,	862	,	5	,
		0x64 	,	864	,	5	,
		0x65 	,	866	,	5	,
		0x70 	,	868	,	5	,
		0x71 	,	870	,	5	,
		0x72 	,	872	,	5	,
		0x73 	,	874	,	5	,
		0x74 	,	876	,	5	,
		0x75 	,	878	,	5	,
		0xFD 	,	868	,	5	,
		0xFE 	,	870	,	5
};

const uint8_t MMAP_TMLM[MEMDEP_TMLM][3] = {				// descriptor-ID: 0x1202
		0x01 	,	1024	,	1	,
		0x02 	,	1044	,	5	,
		0x03 	,	1046	,	5	,
		0x04 	,	1048	,	3	,
		0x05 	,	1049	,	3	,
		0x06 	,	1050	,	5	,
		0x10 	,	1052	,	5	,
		0x11 	,	1054	,	5	,
		0x12 	,	1056	,	5	,
		0x13 	,	1058	,	5	,
		0x14 	,	1060	,	5	,
		0x15 	,	1062	,	5	,
		0xFD 	,	1064	,	5	,
		0xFE 	,	1066	,	5
};

const uint8_t MMAP_USRSETTING[MEMDEP_USRSETTING][3] = {			// descriptor-ID: 0x1300
		0x01 	,	1280	,	1	,
		0x02 	,	1300	,	5	,
		0x03 	,	1302	,	5	,
		0x04 	,	1304	,	3	,
		0x05 	,	1305	,	3	,
		0x06 	,	1306	,	5	,
		0x10	,	1308	,	1	,
		0x11	,	1328	,	1	,
		0x12	,	1348	,	1	,
		0x13	,	1368	,	1	,
		0x14	,	1388	,	1	,
		0x15	,	1408	,	1	,
		0x16	,	1428	,	1	,
		0x90	,	1448	,	3	,
		0x91	,	1449	,	3	,
		0x92	,	1450	,	3	,
		0x93	,	1451	,	3	,
		0x94	,	1452	,	3	,
		0x95	,	1453	,	3	,
		0x96	,	1454	,	3	,
		0xA0	,	1455	,	3	,
		0xA1	,	1456	,	3	,
		0xA2	,	1457	,	3	,
		0xA3	,	1458	,	3	,
		0xA4	,	1459	,	3	,
		0xA5	,	1460	,	3	,
		0xA6	,	1461	,	3	,
		0xAB	,	1462	,	3	,
		0xAC	,	1463	,	3	,
		0xAD	,	1464	,	3	,
		0xAE	,	1465	,	3
};

const uint8_t MMAP_ELEMENTS_NP[MEMDEP_ELEMENTS_NP][3] = {			// descriptor-ID: 0x1410
		0x01 	,	1792	,	1	,
		0x02 	,	1813	,	5	,
		0x03 	,	1815	,	5	,
		0x04 	,	1817	,	3	,
		0x05 	,	1818	,	3	,
		0x06 	,	1819	,	5	,
		0x10	,	1821	,	1	,
		0x11	,	1882	,	1	,
		0x12	,	1903	,	16	,
		0x13	,	1907	,	16	,
		0x14	,	1911	,	1	,
		0x15	,	1942	,	1	,
		0x16	,	2043	,	5	,
		0x17	,	2045	,	1	,
		0x18	,	2076	,	1	,
		0x19	,	2107	,	1	,
		0x1A	,	2128	,	16	,
		0x1B	,	2132	,	16	,
		0x1C	,	2136	,	4	,
		0x1D	,	2138	,	4	,
		0x1E	,	2140	,	7	,
		0x1F	,	2144	,	7	,
		0x20	,	2148	,	1	,
		0x21	,	2209	,	1	,
		0x22	,	2230	,	16	,
		0x23	,	2234	,	16	,
		0x24	,	2238	,	1	,
		0x25	,	2269	,	1	,
		0x26	,	2370	,	5	,
		0x27	,	2372	,	1	,
		0x28	,	2403	,	1	,
		0x29	,	2434	,	1	,
		0x2A	,	2455	,	16	,
		0x2B	,	2459	,	16	,
		0x2C	,	2463	,	4	,
		0x2D	,	2465	,	4	,
		0x2E	,	2467	,	7	,
		0x2F	,	2471	,	7	,
		0x30	,	2475	,	1	,
		0x31	,	2536	,	1	,
		0x32	,	2557	,	16	,
		0x33	,	2561	,	16	,
		0x34	,	2565	,	1	,
		0x35	,	2596	,	1	,
		0x36	,	2697	,	5	,
		0x37	,	2699	,	1	,
		0x38	,	2730	,	1	,
		0x39	,	2761	,	1	,
		0x3A	,	2782	,	16	,
		0x3B	,	2786	,	16	,
		0x3C	,	2790	,	4	,
		0x3D	,	2792	,	4	,
		0x3E	,	2794	,	7	,
		0x3F	,	2798	,	7	,
		0x40	,	2802	,	1	,
		0x41	,	2863	,	1	,
		0x42	,	2884	,	16	,
		0x43	,	2888	,	16	,
		0x44	,	2892	,	1	,
		0x45	,	2923	,	1	,
		0x46	,	3024	,	5	,
		0x47	,	3026	,	1	,
		0x48	,	3057	,	1	,
		0x49	,	3088	,	1	,
		0x4A	,	3109	,	16	,
		0x4B	,	3113	,	16	,
		0x4C	,	3117	,	4	,
		0x4D	,	3119	,	4	,
		0x4E	,	3121	,	7	,
		0x4F	,	3125	,	7	,
		0x50	,	3129	,	1	,
		0x51	,	3190	,	1	,
		0x52	,	3211	,	16	,
		0x53	,	3215	,	16	,
		0x54	,	3219	,	1	,
		0x55	,	3250	,	1	,
		0x56	,	3351	,	5	,
		0x57	,	3353	,	1	,
		0x58	,	3384	,	1	,
		0x59	,	3415	,	1	,
		0x5A	,	3436	,	16	,
		0x5B	,	3440	,	16	,
		0x5C	,	3444	,	4	,
		0x5D	,	3446	,	4	,
		0x5E	,	3448	,	7	,
		0x5F	,	3452	,	7	,
		0x60	,	3456	,	1	,
		0x61	,	3517	,	1	,
		0x62	,	3538	,	16	,
		0x63	,	3542	,	16	,
		0x64	,	3546	,	1	,
		0x65	,	3577	,	1	,
		0x66	,	3678	,	5	,
		0x67	,	3680	,	1	,
		0x68	,	3711	,	1	,
		0x69	,	3742	,	1	,
		0x6A	,	3763	,	16	,
		0x6B	,	3767	,	16	,
		0x6C	,	3771	,	4	,
		0x6D	,	3773	,	4	,
		0x6E	,	3775	,	7	,
		0x6F	,	3779	,	7	
};


const uint8_t MMAP_ELEMENTS_REF[MEMDEP_ELEMENTS_REF][3] = {			// 0x1501 ... 0x1506
		0x01	,	0		,	1	,
		0x02	,	101		,	5	,
		0x03	,	103		,	5	,
		0x04	,	105		,	3	,
		0x05	,	106		,	3	,
		0x06	,	107		,	5	,
		0x10	,	108		,	1	,
		0x11	,	149		,	1	,
		0x12	,	180		,	1	,
		0x13	,	221		,	1	,
		0x14	,	242		,	1	,
		0x15	,	263		,	1	,
		0x16	,	284		,	1	,
		0x17	,	305		,	3	,
		0x1e	,	306		,	7	,
		0x1f	,	310		,	7	,
		0x20	,	314		,	16	,
		0x21	,	318		,	1	,
		0x22	,	359		,	4	,
		0x23	,	361		,	4	,
		0x24	,	363		,	2	,
		0x25	,	364		,	2	,
		0x26	,	365		,	3	,
		0x27	,	366		,	4	,
		0x28	,	368		,	4	,
		0x29	,	370		,	3	,
		0x2a	,	371		,	4	,
		0x2b	,	373		,	4	,
		0x30	,	375		,	5	,
		0x31	,	377		,	5	,
		0x32	,	379		,	5	,
		0x33	,	381		,	5	,
		0x34	,	383		,	5	,
		0x40	,	385		,	5	,
		0x41	,	387		,	5	,
		0x42	,	389		,	5	,
		0x43	,	391		,	5	,
		0x44	,	393		,	5	,
		0x50	,	395		,	5	,
		0x51	,	397		,	5	,
		0x52	,	399		,	5	,
		0x53	,	401		,	5	,
		0x54	,	403		,	5	,
		0x60	,	405		,	5	,
		0x61	,	407		,	5	,
		0x62	,	409		,	5	,
		0x63	,	411		,	5	,
		0x64	,	413		,	5	,
		0x65	,	415		,	3	,
		0x70	,	416		,	5	,
		0x71	,	418		,	2
};



const Uint16 *
BinarySearch(const Uint16 arr[][3],Uint16 len,Uint16 key)
{
	Uint16 low,high;
	high = len - 1;        // Assume the array is ranked from low to high on papre
	low = 0;
	Uint16 midle = len >> 2;

	if (key > arr[high][0]) // search value lower than LUT range
		return NULL;
	else if (key < arr[low][0])
		return NULL;

	while(high >= low) {
		if ((high - low) == 1) {
			if (arr[high][0] == key)
				return &arr[high][0];
			if (arr[low][0] == key)
				return arr[low];
			else return NULL;
		}
		midle = (high + low) >> 1;

		if(arr[midle][0] == key)
			return *(arr+midle);
		if(arr[midle][0] > key)
			high = midle ;     // Precondition is that the array is ranked from high to low
		else if(arr[midle][0] < key )
			low = midle;
	}
	return NULL;
}
