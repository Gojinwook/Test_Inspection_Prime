//Window cordinations...
#ifndef DEFINE_HEADER
#define DEFINE_HEADER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define INSPECTION_MODE

#define LR_MARGIN									8
#define STATUS_HEIGHT								38

#define PANNEL1_TOP									106
#define PANNEL2_TOP									700
#define PANNEL3_TOP									569

#define TEACH_ZOOM_FAC								0.25

#define PANNEL_WIDTH_INSPECTION						512
#define PANNEL_WIDTH								345

#define PANNEL1_LEFT								LR_MARGIN
#define PANNEL1_WIDTH								900-100
#define PANNEL1_HEIGHT								PANNEL2_TOP - PANNEL1_TOP

#define PANNEL2_LEFT								PANNEL1_WIDTH + (LR_MARGIN * 2) + 100
#define PANNEL2_HEIGHT								1080 - PANNEL2_TOP - STATUS_HEIGHT
#define PANNEL2_WIDTH								362
#define PANNEL_FULL_HEIGHT							PANNEL1_HEIGHT + PANNEL2_HEIGHT

#define UM_ULTIMATE_CLICKED							WM_USER+11111	// feel

#define USER_MODE_OPERATOR		0
#define USER_MODE_ENGINEER		1
#define USER_MODE_ADMIN			2

//////////////////////////////////////////////////////////////////////////
//	Ultimate Grid View Type

#define UGVT_SCALEPOSITION	0

//////////////////////////////////////////////////////////////////////////


typedef struct {
	int comport;
	int baudrate;
	int bytesize;
	int parity;
	int stopbits;
	int xonoff;
	int status_scan_time;
} COM_PORT;

typedef struct {
	double	move_rapid;
	double	move_normal;
	double	move_slow;
	double	move_accel;
	double	jog_rapid;
	double	jog_normal;
	double	jog_slow;
	double	jog_accel;
	int		direction;
	double	home_velocity;
	double	home_accel;
	double	home_offset;
	double	home_InitPos;
	double	gearratio_gear;
	double	gearratio_motor;
	double	gearratio_ball_pitch;
	int		home_direction;
	int		current_jog_speed;	//0:slow, 1:Normal, 1:Rapid
	double  current_position;
	double	swlimit_l;
	double	swlimit_u;
} CONVEYOR_PARAM;

//Setup-Motion parameters
typedef struct _MOTION_PARAM {
	double	move_rapid;
	double	move_normal;
	double	move_slow;
	double	move_accel;
	double	jog_rapid;
	double	jog_normal;
	double	jog_slow;
	double	jog_accel;
	double	home_normal;
	double	home_slow;
	double	home_accel;
	double	softlimit_pos;
	double	softlimit_neg;
	int		level_motor_direction;
	int		level_limit_pos;
	int		level_limit_neg;
	int		level_home;
	int		level_amp_fault;
	int		level_amp_enable;
	int		level_amp_reset;
	int		level_inposition;
	int		pulse_out;
	int		encoder_mode;
	double	unit_pulse;
	double	ready_position;	//Added 2005.11.21
	double  inspect_position; // Added by Eddy 2009.04.10
} MOTION_PARAM;

typedef struct {
	unsigned	b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1;
}MY_BITS;

typedef union {
	BYTE bByte;
	MY_BITS bit;
}MY_BYTE_BITS;

////////////////////

//Setup-Grab Parameters
typedef struct {
	double	head_position0;
	double	head_position1;
	double	scan_velocity0;	//Scan velocity
	double	scan_velocity1;	//Move velocity
	double	scan_accel0;	//Scan acc
	double	scan_accel1;	//Move acc
	double	rlines[2];
	double	init_phase[2];
	double	min_grid_pitch[2];
	double  max_grid_pitch[2];
	double  grid_angle[2];
	double	calibration_step;
	double	calibration_level;
	double	long_wavelength_light;
	double  height_resolution;
	int		beating_compensation;
	double	resolution;
	int		s_resolution_index;
	int		motion_s_resolution_index;
	double	trigger_dir_offset_coef;
	double	trigger_dir_offset_vel;
	double	trigger_dir_offset_vel2;
	int		trigger_time;
	int		trigger_offset;
	int		trigger_level;
	int		n2DScanLigthType;
	int		inspection_thread;
	int		mirror_delay;
	BOOL	fpga_use;
	BOOL	mirror_use;
	BOOL	trigger_dir_offset_use;
	int		m_resolution_index;
	double	exposure_time_20;
	double	camera_gain_20;
	double	reverse_scan_trigger_offset_20;
	int		scan_velocity_20;
	double	exposure_time_40;
	double	camera_gain_40;
	int		scan_velocity_40;
	double	reverse_scan_trigger_offset_40;
	BOOL	beating_use;
	BOOL	height_correction_use;
	double	z_stage_coeff;
	double	ProjectionAngle;
	BOOL	bPhaseUnwrapRepair;
	BOOL	bUseTakeBackScan;

	double dFrameInspectMargin;
	double  dTeachingScanOverlap;
	int nTeachingScanOverlapPixel;
	double dTeachingScanOverlapMachine;

	BOOL	bHeadAlignmentCheckMode;
	BOOL	bSaveTemperatureHeight;
	int		nProbeSettingType;
	double	dLargeBottom;
	double	dLargeTop;
	double	dMiddleBottom;
	double	dMiddleTop;
	double	dFlexibleBottom;
	double	dFlexibleTop;
	int		nTestModeDelayTime;
	BOOL	bUseProjectorHeightOffsetCal;
	int	    nFrameProcessingNumber;
	int		nProjectorHeightOffset;
	BOOL	bUseGammaCorrection;
	BOOL	bSubOptimalRotation;
	int		nMinLZR;
	BOOL	bWarpTest;
	BOOL	bUseWNCal;
	int		nFixedWN;
	int		fpga_camera_gain_ch1;
	int		fpga_camera_gain_ch2;
	int		fpga_camera_gain_ch3;
	int		fpga_camera_gain_offset_ch1;
	int		fpga_camera_gain_offset_ch2;
	int		fpga_camera_gain_offset_ch3;
	double  dScanFreamOffset;
	double  dPickUpOffsetX;
	double  dPickUpOffsetY;


} GRAB_PARAM;


//#define MAX_PANEL_SIZE_X					600
//#define MAX_PANEL_SIZE_Y					500


#define GERBER_WIDTH						73
#define GERBER_HEIGHT						150


#define PIXEL_SIZE_MM_X						preference_param.CameraResolutionX_Pixel
#define PIXEL_SIZE_MM_Y						preference_param.GrabPageY_Pixel
//#define PIXEL_SIZE_MM_AVG					0.001

//#define PIXEL_SIZE_PMODE					0.001
//#define PIXEL_SIZE_SMODE					0.004

#define CAMERA_PIXEL_RESOLUTION				preference_param.CameraPixelResolution_mm

//#define CAMERA_RESOLUTION_X					16384
//#define GRAB_PAGE_Y							1000

#define MAX_FRAME_BUFFER					6//6				//Requires more than 40GB System Memory (3.9 x 10)
#define MAX_NUM_OF_CALLBACK_BUFFERS			468				// Number of CallBack buffer (17824 * 1024) count to fill to a Frame (3.9G), 222,208 Lines, 155.54 mm MAX

//#define MAX_AF_FRAME_BUFFER					11
#define MAX_AF_FRAME_BUFFER					21
#define MAX_AL_FRAME_BUFFER					5*5
//#define AF_SCAN_LENGTH						128

//#define CALLBACK_BUFFER_BYTES				INT64(CAMERA_RESOLUTION_X * GRAB_PAGE_Y)

#define ONE_CALLBACK_BUFFER_LENGTH_MM	1 // 1000 * 0.001
#define SATURATION_BUFFER_LENGTH_MM ONE_CALLBACK_BUFFER_LENGTH_MM

#define GRAB_LINE_MARGIN					6

#define UNIT_INSPECT_MARGIN				0.15

#define X_RES_20						0
#define X_RES_40						1

#define Y_RES_20						0
#define Y_RES_40						1

#define PATTERN							0
#define VIA								1
#define LINE							2
#define GROUND							3
#define CIRCLE							4
#define CORE							5
#define DONTCARE						6
#define ADD								7
#define LINEWIDTH						8
#define GQFN							9

#define MAX_ZONE_NUMBER					50

#define SCAN_DIRECTION_FORWARD			0
#define SCAN_DIRECTION_REVERSE			1

typedef struct
{
	int	iXCameraPixel;
	double 	dYmmPerPixel;
} RESOLUTION_DATA;

typedef struct
{
	double dUpwardScaleFactor;
	double dUpwardHeight;
	double dDownwardScaleFactor;
	double dDownwardHeight;
} CAL_COMP;

typedef struct
{
	int iArrayIdx;
	int iPackageIdx;
	int iPadIdx;
} REVIEW_PAD_INDEX;
////////////////////////

//////////////////////////////////////////////////////////////////////////

#define INSPECTMODE_20SCAN	0
#define INSPECTMODE_40SCAN	1
#define INSPECTMODE_40TEACH_20SCAN	2

//////////////////////////////////////////////////////////////////////////

#define REFERENCE_METHOD_LOCALREF	0
#define REFERENCE_METHOD_3REF	1

//////////////////////////////////////////////////////////////////////////

#define UM_SHEET_CLOSED WM_USER+300
#define UM_3DCOMBINED	WM_USER+101

#define UM_2DFRAME_READY WM_USER+102
#define UM_LIST_SELECT WM_USER+103
#define UM_LIST_DELETE WM_USER+105

//////////////////////////////////////////////////////////////////////////
//	Reference Method

#define RM_3REF				0
#define RM_THRESHOLD		1
#define RM_FLEXIBLE			2

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	Threshold Reference Peak Method

#define TRPM_MAX_FREQUENCY	0
#define TRPM_BOUNDARY		1
#define TRPM_FIRST_PEAK		2

//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////
//	Algorithm
#define MAX_TERMINAL_NUMBER	100

///////////////////////////////////////////////
//	Strip Align Mark Matching
#define SAM_SEARCH_MARGIN_X	200
#define SAM_SEARCH_MARGIN_Y	200
#define SHAPE_MODEL_SCALE_MIN	0.98
#define SHAPE_MODEL_SCALE_MAX	1.02

#define USER_LEVEL_ADMIN	0
#define USER_LEVEL_ENG	1
#define USER_LEVEL_OP	2

#define IMAGE_INSPECTION_MARGIN	100

///////////////////////////////////////////////
//	SPC Terminal Type
#define PATTERN_TYPE_NOT_DEFINED	-1
#define PATTERN_TYPE_TERMINAL	0
#define PATTERN_TYPE_DAP			1
#define PATTERN_TYPE_TRACE		2
#define PATTERN_TYPE_MOLD		3
///////////////////////////////////////////////
//	SPC Terminal Type
#define TERMINAL_TYPE_INNER	0
#define TERMINAL_TYPE_OUTER	1
#define TERMINAL_TYPE_CORNER	2
#define TERMINAL_TYPE_DAP	3

///////////////////////////////////////////////
//	Under Etch Defect Detection Type
#define DDT_UNDER_ETCH_GOOD		0
#define DDT_UNDER_ETCH_CONNECT	1
#define DDT_UNDER_ETCH_SPACING	2
#define DDT_UNDER_ETCH_AREA			3

///////////////////////////////////////////////
//	Over Etch Defect Detection Type
#define DDT_OVER_ETCH_GOOD					0
#define DDT_OVER_ETCH_AREA					1
#define DDT_OVER_ETCH_OPEN					2

///////////////////////////////////////////////
//	Inspection Mode
#define PRODUCTION_MODE	0
#define ACCURATE_MODE		1

///////////////////////////////////////////////
//	Review Strip Color
#define RSC_NOT_FILLED				RGB(0,0,0)
#define RSC_GOOD					RGB(0,255,0)
#define RSC_NG						RGB(255,0,0)
#define RSC_FM_ERROR				RGB(255,128,0)
#define RSC_STRIP_REVIEW_END		RGB(255,255,0)

#define MAX_DEFECT_NUMBER			15
#define MAX_SPEC_LINE				17
///////////////////////////////////////////////
//	Inspection Parameter Tab
#define IPT_DIMENSION			1
#define IPT_ETCHING				2
#define IPT_TERMINAL_CONTAM		3
#define IPT_TERMINAL_SCRATCH	4
#define IPT_TRACE_CONTAM		5
#define IPT_MOLDNG_CONTAM		6

#define IPT_CAD_IMAGE_ALIGN			0			// CAD/영상 매칭
#define IPT_INSPECT_AREA			1			// 검사영역 구분 티칭 (A, B, C)
#define IPT_DONTCARE_ROI			2			// 비검사 영역 티칭 (RECT, CIRCLE, POLYGON)
#define IPT_AUTO_TEACHING			3			// 비검사 영역 티칭 (RECT, CIRCLE, POLYGON)
#define IPT_PATTERN_PARAM			4			// 비검사 영역 티칭 (RECT, CIRCLE, POLYGON)
#define IPT_MEASUREMENT				5			// Line Pattern Inspection
#define IPT_TEST_1					6			// TEST 1

static CString g_strNameIPT[] =
{
	_T("Local Align"),
	_T("Inspect Area"),
	_T("Non-Inspect Area"),
	_T("Auto Teaching"),
	_T("Inspect Spec."),
	_T("Line Pattern"),
	_T("GV Difference")
};

#define MAX_WIDTH_HISTOGRAM_BIN		101

#define DCB_COPPER			0
#define DCB_DIMPLE			1
#define DCB_GROOVE_LINE		2
#define DCB_HOLE			3
#define DCB_PSR				4
#define DCB_CERAMIC			5
#define DCB_DONTCARE		6

#define IMAGE_FILTER_NOT_USED	0
#define IMAGE_FILTER_MEDIAN		1
#define IMAGE_FILTER_GAUSS		2
#define IMAGE_FILTER_OPEN		3
#define IMAGE_FILTER_CLOSE		4

#define MIN_DEFECT_LENGTH_PXL	8
#define DEFAULT_MIN_DEFECT_SPEC_UM	150

#define MAX_DEF			(999999999)

#define MAX_DEFECT_TYPE		16
// Defect Type
#define DT_GOOD	0
#define DT_PATTERN_MISSING	1
#define DT_SCRATCH			2
#define DT_PINHOLE			3
#define DT_DCB_CONTAM		4
#define DT_PSR				5
#define DT_DIMPLE			6
#define DT_GROOVE_LINE		7
#define DT_BRIDGE			8
#define DT_DCB_BUR			9
#define DT_DCB_PATTERN		10
#define DT_CERAMIC_BUR		11
#define DT_CERAMIC_BROKEN	12
#define DT_CERAMIC_CHIPPING	13
#define DT_CERAMIC_CONTAM	14
#define DT_PROTRUSION		15

///////////////////////////////////////////////
//	Smart Defect Type
#define DT_GOOD								0
#define DT_PATTERN_PINHOLE					1
#define DT_PATTERN_MOUSEBITE				2
#define DT_PATTERN_MOUSEBITE_NONFWM			3
#define DT_PATTERN_MOUSEBITE_SURFACE		4
#define DT_PATTERN_SPACE					5
#define DT_PATTERN_ISLAND					6
#define DT_PATTERN_PROTRUSION				7
#define DT_PATTERN_PROTRUSION_NONFWM		8
#define DT_PATTERN_PROTRUSION_SURFACE		9
#define DT_PATTERN_OPEN						10
#define DT_PATTERN_OPEN_FWM					11
#define DT_PATTERN_OPEN_SURFACE				12
#define DT_PATTERN_SHORT					13
#define DT_PATTERN_SHORT_SURFACE			14
#define DT_PATTERN_NFWM_PAD_MBSP			15
#define DT_PATTERN_NFWM_PAD_SP				16
#define DT_PATTERN_NFWM_PAD_MB				17
#define DT_PATTERN_NFWM_PAD_BIG				18
#define DT_PATTERN_NFWM_PAD_HOLES			19

#define DT_PATTERN_SURFACE_SCRATCH			20
#define DT_PATTERN_SURFACE_CONTAMIN			21
#define DT_PATTERN_SURFACE_LOW_GRAY			22
#define DT_PATTERN_DUMMY1					23
#define DT_PATTERN_DUMMY2					24
#define DT_PATTERN_DUMMY3					25 
#define DT_PATTERN_DUMMY4					26
#define DT_PATTERN_DUMMY5					27
#define DT_PATTERN_DUMMY6					28
#define DT_PATTERN_DUMMY7					29
#define DT_PATTERN_DUMMY8					30
#define DT_ALIGN_FAIL						31
#define DT_INSP_ERROR						32

static CString g_strNameDefectType[] =
{
	_T("Good"),							//0
	_T("PinHole"),						//1
	_T("Mousebite-FWM"),				//2
	_T("Mousebite-NonFWM"),				//3
	_T("Mousebite-SURFACE"),			//4
	_T("Space"),						//5
	_T("Island"),						//6
	_T("Protrusion-FWM"),				//7
	_T("Protrusion-NonFWM"),			//8
	_T("Protrusion-SURFACE"),			//9
	_T("Open"),							//10
	_T("Open-FWM"),						//11
	_T("Open-SURFACE"),					//12
	_T("Short"),						//13
	_T("Short-SURFACE"),				//14
	_T("NFWM-Pad-MBSP"),				//15
	_T("NFWM-Pad-SP"),					//16
	_T("NFWM-Pad-MB"),					//17
	_T("NFWM-Pad-BIG"),					//18
	_T("NFWM-Pad-HOLE"),				//19
	_T("Surface-Scratch"),				//20
	_T("Surface-Contamin"),				//21
	_T("Surface-LowGray"),				//22
	_T("Dummy"),						//23
	_T("Dummy"),						//24
	_T("Dummy"),						//25
	_T("Dummy"),						//26
	_T("Dummy"),						//27
	_T("Dummy"),						//28
	_T("Dummy"),						//29
	_T("Dummy")							//30
};
static CString g_strNameDefectCode[] =
{
	_T("G"),							//0
	_T("H"),							//1
	_T("M"),							//2
	_T("M"),							//3
	_T("M"),							//4
	_T("P"),							//5
	_T("I"),							//6
	_T("B"),							//7
	_T("B"),							//8
	_T("B"),							//9
	_T("O"),							//10
	_T("O"),							//11
	_T("O"),							//12
	_T("S"),							//13
	_T("S"),							//14
	_T("P"),							//15
	_T("P"),							//16
	_T("P"),							//17
	_T("P"),							//18
	_T("P"),							//19
	_T("P"),							//20
	_T("P"),							//21
	_T("P"),							//22
	_T("G"),							//23
	_T("G"),							//24
	_T("G"),							//25
	_T("G"),							//26
	_T("G"),							//27
	_T("G"),							//28
	_T("G"),							//29
	_T("G")								//30
};

///////////////////////////////////////////////
//	Smart IC Layer Type	
#define SIC_LAYER_ALL						-1
#define SIC_LAYER_CONTACT					0
#define SIC_LAYER_BONDING					1
#define SIC_LAYER_PPG						2
#define SIC_LAYER_NG_HOLE					3
#define SIC_LAYER_DONTCONTACT				4			// 2018.11.03 NDH 비 영역 추가. 비중요 영역은 검사를 안하거나 일부 검사항목만 진행 할 수 있다
#define SIC_LAYER_COVERLAY					5			// 2020.03.12 HCH Coverlay 영역 추가
#define SIC_LAYER_ARRAY						6

enum eCadGeometryOption
{
	CAD_ROTATE_CLOCK,
	CAD_ROTATE_CLOCK_R,
	CAD_SCALE,
	CAD_TRANSLATE_MM,
	CAD_TRANSLATE_PIXEL,
	CAD_MIRROR_H,
	CAD_MIRROR_V,

};

enum eCadGeometryMoveOption
{
	CAD_MOVE_MM,
	CAD_MOVE_PIXEL
};

///////////////////////////////////////////////
//	Smart IC Pattern Type
#define SIC_PATTERN_CONTACT					0
#define SIC_PATTERN_BONDING					1
#define SIC_PATTERN_NG_HOLE					2
#define SIC_PATTERN_SPROCKET_HOLE			3
#define SIC_PATTERN_BONDING_HOLE			4
#define SIC_PATTERN_CHIP_HOLE				5
#define SIC_PATTERN_PPG						6
#define SIC_PATTERN_DONTCARE				7
#define SIC_PATTERN_UNIT_RECT				8
#define SIC_PATTERN_INSPECT_A				9
#define SIC_PATTERN_INSPECT_B				10
#define SIC_PATTERN_INSPECT_C				11
#define SIC_PATTERN_INSPECT_D				12
#define SIC_PATTERN_INSPECT_E				13
#define SIC_PATTERN_INSPECT_DIFF_HOR		14
#define SIC_PATTERN_INSPECT_DIFF_VER		15
#define SIC_PATTERN_INSPECT_LIGHT			16
#define SIC_PATTERN_WIDTH					17
#define SIC_PATTERN_INSPECT_OPEN			29
#define SIC_PATTERN_INSPECT_SHORT			30

#define SIC_PATTERN_DONTCARE_STAIN			31
#define SIC_PATTERN_DONTCARE_FM				32
#define SIC_PATTERN_DONTCARE_DC				33
#define SIC_PATTERN_UNIT_RECT_GROUP			34
#define SIC_PATTERN_DONTCONTACT				35			// 2018.11.03 NDH 비 영역 추가. 비중요 영역은 검사를 안하거나 일부 검사항목만 진행 할 수 있다
#define SIC_PATTERN_COVERLAY				36

#define SIC_PATTERN_INNERHOLE1				37 //InnerHole 왼쪽		Coverlay 오른쪽 위치
#define SIC_PATTERN_INNERHOLE2				38 //InnerHole 오른쪽	Coverlay 왼쪽 위치
#define SIC_PATTERN_INNERHOLE3				39 //InnerHole 위		Coverlay 아래 위치
#define SIC_PATTERN_INNERHOLE4				40 //InnerHole 아래		Coverlay 위 위치

#define SIC_PATTERN_INSPECT_A2				41
#define SIC_PATTERN_INSPECT_B2				42
#define SIC_PATTERN_INSPECT_C2				43
#define SIC_PATTERN_INSPECT_D2				44
#define SIC_PATTERN_INSPECT_E2				45

#define SIC_PATTERN_ARRAY					46
#define SIC_PATTERN_CONTACT_TRACE			47
#define SIC_PATTERN_CONTACT_FLASH			48
#define SIC_PATTERN_CONTACT_POLYGON			49
///////////////////////////////////////////////
//	Image Type
#define IMAGE_TYPE_COLOR	0
#define IMAGE_TYPE_R		1
#define IMAGE_TYPE_G		2
#define IMAGE_TYPE_B		3
#define IMAGE_TYPE_H		4
#define IMAGE_TYPE_S		5
#define IMAGE_TYPE_I		6
#define IMAGE_NUM			7

#define CHANNEL_1ST			0	//1st CCD Sensor
#define CHANNEL_2ND			1	//2nd CCD Sensor
#define CHANNEL_3RD			2	//3rd CCD Sensor
#define CHANNEL_NUM			3

#define VISION_TYPE_TOP			0
#define VISION_TYPE_BOTTOM		1

#define VISION_PC_AOI_NUM		3
#define VISION_PROGRAM_1		0
#define VISION_PROGRAM_2		1

#define VISION_PC_TOP_AOI_1		8
#define VISION_PC_TOP_AOI_2		9
#define VISION_PC_TOP_AOI_3		10
#define VISION_PC_BOTTOM_AOI_1  11
#define VISION_PC_BOTTOM_AOI_2  12
#define VISION_PC_BOTTOM_AOI_3  13
///////////////////////////////////////////////
//	Inspection Side
#define INSPECTION_SIDE_CONTACT			0
#define INSPECTION_SIDE_BONDING			1
#define INSPECTION_SIDE_DONTCONTACT		2


#define INSPECTION_NONCOVERLAY			0
#define INSPECTION_COVERLAY				1
///////////////////////////////////////////////
//	Inspection Type
#define IT_SH_SPHOLE_DETECTION			0
#define IT_BH_PLATING_TYPE				1
#define IT_BH_CONTAM_STRONG				2
#define IT_BH_BONDING_HOLE_DETECTION	3
#define IT_BH_STAIN_NORMAL				4
#define IT_BH_INCOMPLETE_PLATING		5
#define IT_BH_CONTAM					6
#define IT_BH_ADHESIVE_FLOW				7
#define IT_PT_PLATING_TYPE				8
#define IT_PT_INCOMPLETE_PLATING		9
#define IT_PT_CONTAM_STRONG				10
#define IT_PT_DISCOLORATION				11
#define IT_PT_SCRATCH					12
#define IT_PT_STAIN						13
#define IT_PT_CONTAM					14
#define IT_PT_FOREIGN_MATERIAL			15
#define IT_TH_THROUGH_HOLE_DETECTION	16
#define IT_PPG_DISCOLORATION			17
#define IT_PPG_SCRATCH					18
#define IT_PPG_FOREIGN_MATERIAL			19
#define IT_PPG_DELAMINATION				20
#define MAX_INSPECTION_TYPE				21

///////////////////////////////////////////////
//	Algorithm Type
#define AT_REF_GRAY_CHECK		0
#define AT_BLOB_ANALYSIS		1
#define AT_EDGE_DETECTION		2
#define AT_HS_CHECK				3

///////////////////////////////////////////////
//	S-IC Algorithm Test 
#define ALGORITHM_TEST_SPHOLE					100
#define ALGORITHM_TEST_COLOR_DEVIATION			101
#define ALGORITHM_TEST_PATTERN_INNER			102
#define ALGORITHM_TEST_PATTERN_OUTER			103
//	P-TCP Algorithm Test 
#define ALGORITHM_TEST_COVERLAY					104
#define ALGORITHM_TEST_LEADTIP					105

///////////////////////////////////////////////
//	S-IC Default Spec
#define SIC_DEFAULT_SPEC_AREA		100
#define SIC_DEFAULT_SPEC_LENGTH		0.2
#define SIC_DEFAULT_SPEC_WIDTH		0.04

///////////////////////////////////////////////
//	S-IC Defect Spec
#define DEFECT_SPEC_PATTERN_REDUCTION			0
#define DEFECT_SPEC_PATTERN_PINHOLE				1
#define DEFECT_SPEC_PATTERN_BURR				2
#define DEFECT_SPEC_PATTERN_METAL_RESIDUE		3
#define DEFECT_SPEC_THROUGH_HOLE				4
#define DEFECT_SPEC_PLATING_DISCOLORATION		5
#define DEFECT_SPEC_PLATING_SCRATCH				6
#define DEFECT_SPEC_PLATING_INCOMPLETE			7
#define DEFECT_SPEC_PLATING_CONTAM				8
#define DEFECT_SPEC_PLATING_STAIN				9
#define DEFECT_SPEC_PLATING_FOREIGN_MATERIAL	10
#define DEFECT_SPEC_PPG_SCRATCH					11
#define DEFECT_SPEC_PPG_FOREIGN_MATERIAL		12
#define DEFECT_SPEC_BH_ADHESIVE_FLOW			13
#define DEFECT_SPEC_BH_BURR						14
#define DEFECT_SPEC_BH_INCOMPLETE				15
#define DEFECT_SPEC_BH_CONTAM					16
#define DEFECT_SPEC_BH_STAIN					17
#define DEFECT_SPEC_CH_ADHESIVE_FLOW			18
#define DEFECT_SPEC_CH_BURR						19
#define DEFECT_SPEC_CH_INCOMPLETE				20
#define DEFECT_SPEC_CH_CONTAM					21
#define DEFECT_SPEC_CH_STAIN					22
#define DEFECT_SPEC_PPG_BIG_DELAMINATION		23
#define DEFECT_SPEC_PPG_SMALL_DELAMINATION		24
#define DEFECT_SPEC_PATTERN_DIFF_VER			25
#define DEFECT_SPEC_PATTERN_DIFF_HOR			26
#define DEFECT_SPEC_BIG_PLATING_INCOMPLETE		27

#define DEFECT_SPEC_NUMBER						28

#define	GRAB_MODE_NONE							0
#define	GRAB_MODE_INSPECT						1
#define	GRAB_MODE_TEACHING						2

#define MAX_FRAME_COUNT							10

#define UDP_HANDLER_IP										"192.168.10.1"

static CString g_strTopVisionIP[] =
{
	_T("192.168.10.131"),
	_T("192.168.10.141"),
	_T("192.168.10.151")
};

static CString g_strBottomVisionIP[] =
{
	_T("192.168.10.231"),
	_T("192.168.10.241"),
	_T("192.168.10.251")
};

static int g_iTopSendVisionPort[] =
{
	8531,
	8541,
	8551
};

static int g_iBottomSendVisionPort[] =
{
	8631,
	8641,
	8651
};

static int g_iTopReceiveVisionPort[] =
{
	8131,
	8141,
	8151
};

static int g_iBottomReceiveVisionPort[] =
{
	8231,
	8241,
	8251
};

#define SPEC_TYPE_GLOBAL									0
#define	SPEC_TYPE_SECTION									1

enum eUnitIndex
{
	UNIT_INDEX_1ST,
	UNIT_INDEX_2ND,
	UNIT_INDEX_3RD,
	UNIT_INDEX_4TH,
	UNIT_INDEX_5TH,
	UNIT_INDEX_6TH,
	UNIT_INDEX_7TH,
	UNIT_INDEX_8TH,
	UNIT_INDEX_9TH,
	UNIT_INDEX_10TH,
	UNIT_INDEX_11TH,
	UNIT_INDEX_12TH,
	UNIT_INDEX_13TH,
	UNIT_INDEX_14TH,
	UNIT_INDEX_COUNT
};

static CString g_strUnitFileExt[] =
{
	_T("A"),				//  UNIT_INDEX_1ST
	_T("B"),				//  UNIT_INDEX_2ST
	_T("C"),				//  UNIT_INDEX_3ST
	_T("D"),				//  UNIT_INDEX_4ST
	_T("E"),				//  UNIT_INDEX_5ST
	_T("F"),				//  UNIT_INDEX_6ST
	_T("G"),				//  UNIT_INDEX_7ST
	_T("H"),				//  UNIT_INDEX_8ST
	_T("I"),				//  UNIT_INDEX_9ST
	_T("J"),				//  UNIT_INDEX_10ST
	_T("K"),				//  UNIT_INDEX_11ST
	_T("L"),				//  UNIT_INDEX_12ST
	_T("N"),				//  UNIT_INDEX_13ST
	_T("M"),				//  UNIT_INDEX_13ST
};



#define INSP_PART_UNIT										0
#define INSP_PART_PIECE										10

#define MAX_ALIGN_MARK_COUNT								10


#define	INSP_MODE_DEFAULT									0
#define	INSP_MODE_INSP										1
#define	INSP_MODE_TEACHING									2

class GTRegion;


typedef struct _INSP_THREAD_PARAM {
	_INSP_THREAD_PARAM(int iThreadIdx) { this->iThreadIdx = iThreadIdx; }

	int iThreadIdx;

} INSP_THREAD_PARAM;

#endif

static BOOL CreateDirectoryAllParent(LPCTSTR lpszPath)
{
	TCHAR szPathBuffer[MAX_PATH];

	size_t len = _tcslen(lpszPath);
	for (size_t i = 0; i < len; i++)
	{
		szPathBuffer[i] = *(lpszPath + i);

		if (szPathBuffer[i] == _T('\\') || szPathBuffer[i] == _T('/'))
		{
			szPathBuffer[i + 1] = NULL;
			if (!PathFileExists(szPathBuffer))
			{
				if (!::CreateDirectory(szPathBuffer, NULL))
				{
					if (GetLastError() != ERROR_ALREADY_EXISTS)
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}

static BOOL CheckSpaceDriveD(int iCutGbyte)
{
	CString sDir = "D:\\";
	double dFreeBytes, iFree;
	ULARGE_INTEGER ulUserFree, ulTotal, ulRealFree;
	if (GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree))
	{
		//TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart*(double)4294967296; 
		//fTotal = (float)(TotalBytes/1024/1024/1024); // Convert (GB)     
		dFreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart*(double)4294967296;
		iFree = (int)(dFreeBytes / 1024 / 1024 / 1024); // Convert (GB) 
		//fUsed = fTotal - fFree;

		if (iFree > iCutGbyte)
			return TRUE;
	}

	return FALSE;

}

#define WINDOWS_VERSION_10		1
#define WINDOWS_VERSION_7		2


enum eDefSpec
{
	USE_SPEC_MAIN,
	USE_SPEC_A,
	USE_SPEC_B,
	USE_SPEC_C,
	USE_SPEC_D,
	USE_SPEC_E,
	USE_SPEC_A2,
	USE_SPEC_B2,
	USE_SPEC_C2,
	USE_SPEC_D2,
	USE_SPEC_E2,
	USE_SPEC_COUNT
};

//Get RGB ==  GetRValue, GetGValue, GetBValue
static COLORREF g_clrSpecColor[USE_SPEC_COUNT] =
{
	RGB(255	, 0		, 0)	,			// RED
	RGB(0	, 255	, 0)	,			// GREEN
	RGB(0	, 0		, 255)	,			// BLUe
	RGB(0	, 255	, 255)	,			// WHITE BLUE, cyan
	RGB(255	, 0		, 255)	,			// purple , magenta
	RGB(128	, 0		, 0)	,			// RED
	RGB(0	, 128	, 0)	,			// GREEN
	RGB(0	, 0		, 128)	,			// BLUe
	RGB(0	, 128	, 128)	,			// WHITE BLUE, cyan
	RGB(128	, 0		, 128)				// purple , magenta

};
enum eDefChanell
{
	ILLUM_CH1,
	ILLUM_CH2,
	ILLUM_CH3,
	ILLUM_CH4,
	ILLUM_CH5,
	ILLUM_CH6,
	ILLUM_CH7,
	ILLUM_CH8,
	ILLUM_CH_COUNT

};

#define		REMOVE_FOLDER_FROM_PATHNAME(x)	( (x).Right( (x).GetLength() - (x).ReverseFind('\\') - 1 ) )
#define		REMOVE_EXT_FROM_FILENAME(x)		( (x).Left ( (x).ReverseFind('.') ) )
#define		GET_EXTENSION_FROM_PATHNAME(x)	( (x).Right( ( (x).GetLength() - (x).ReverseFind('.') ) - 1) )

#define	UNIT_TOTAL_COUNT											14

////////////////////////////////  COF AFVI

////////////////////////////////  Local Align
#define LOCAL_ALIGN_BY_PATTERN			0
#define LOCAL_ALIGN_BY_SKELETON			1

////////////////////////////////  Pipeline
#ifdef INLINE_MODE
	#define MAX_INSPECTION_BUFFER_COUNT		64//60
#else
	#ifdef USE_DUAL_CPU
		#define MAX_INSPECTION_BUFFER_COUNT		40
	#else
		#ifdef USE_CUDA
			#define MAX_INSPECTION_BUFFER_COUNT			40  // 12
		#else
			#define MAX_INSPECTION_BUFFER_COUNT			4
		#endif
	#endif
#endif

#define NO_MAX_INSPECT_THREAD				100

#define MAX_PROCESS_WAIT_TIMEOUT			10000

enum eProcessUnit
{
	PROCESS_GLOBAL_ALIGN,
	PROCESS_INSPECT_PINHOLE_ISLAND,
	PROCESS_INSPECT_OPEN,
	PROCESS_INSPECT_SHORT,
	PROCESS_BUILD_IRB_CL,
	PROCESS_BUILD_IRBS,
	PROCESS_INSPECT_FWM,
	PROCESS_INSPECT_NONFWM,
	PROCESS_INSPECT_NONFWM_SURFACE,
	PROCESS_RESULT_SAVE,
	TOTAL_PROCESS_NUMBER		// 12
};

static CString g_strProcessUnitName[] =
{
	_T("GlobalAlign"),
	_T("InspectPinholeIsland"),
	_T("InspectOpen"),
	_T("InspectShort"),
	_T("BuildIRBCL"),
	_T("BuildIRBS"),
	_T("InspectFwm"),
	_T("InspectNonFwm"),
	_T("InspectNonFwmSurface"),
	_T("ResultSave")
};

#define VISION_STATUS_STOP				0
#define VISION_STATUS_READY				1

#define DUAL_CPU_NUMBER_OFFSET			40
#define CPU0_START_NUMBER				2

#define GRAB_PROCESS_INDEX				0
#define CROP_PROCESS_INDEX				40

#define CPU_0	0
#define CPU_1	1

#ifdef USE_CUDA
#ifdef INLINE_MODE
#define MAX_FWM_THREAD_NUMBER				2
#define MAX_NONFWM_THREAD_NUMBER			2
#else
#define MAX_FWM_THREAD_NUMBER				1
#define MAX_NONFWM_THREAD_NUMBER			1
#endif
#else
#define MAX_FWM_THREAD_NUMBER					MAX_INSPECTION_BUFFER_COUNT
#define MAX_NONFWM_THREAD_NUMBER				MAX_INSPECTION_BUFFER_COUNT
#endif

#define GPU_1	0
#define GPU_2	1
#define GPU_3	2
#define GPU_4	3

#define MAX_UNIT_NUMBER		1500

#define IMAGE_CROP_POS_COMP_PERIOD			5

#define MAX_INSPECTION_CROP_IMAGE_NUMBER	4
#define PACKET_STRING_SIZE 60
#define DEEP_MODEL_NUM 8
#define MAX_DEFECT_CNT 100
#define PC_VISION_NUMBER 8
#define SUA_IMG_SIZE 256

//TCP Communication [ADJ]
//#define INSPECTION_THREAD_NUM 3 //-> PC_VISION_NUMBER(기존에사용하던 변수)로 대체
#define UM_ADJ_SEND_IMAGE		WM_USER+9999
#define MAX_ADJ_CONNECT_NUM 4
#define ADJ_BUFFER_NUM 50
#define RCV_DATA_SIZE 25
#define ADJ_SEND_TIMEOUT 500   
#define ADJ_EACH_IMAGE_SEND_TIMEOUT 500
#define RCV_TIMEOUT -1
#define RCV_WAIT -2
#define RCV_ERROR -3
#define WAIT_FAILED_ERROR -4
#define WAIT_ABANDONED_ERROR -5
#define UNEXCEPTED_ERROR -6
#define IMAGE_LOAD_ERROR -7
#define DEEP_MODEL_NUM 16
#define MAX_DEFECT_CNT 100
#define PATH_LIST_NUM 32
#define MAX_INDEX_NUM 100
#define DEEP_LEARNING_ENABEL 1
//TCP Communication end

#define PC_NUMBER			3
#define PC_TYPE_1			0
#define PC_TYPE_2			1
#define PC_TYPE_3			2

#define LINE_SCAN			0
#define	AREA_SCAN			1


// FHM